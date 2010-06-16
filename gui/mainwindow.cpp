/*
 *  PinpointWCS is developed by the Chandra X-ray Center
 *  Education and Public Outreach Group
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <iostream>
#include <string>
#include <QtOpenGL>

#include "mainwindow.h"
#include "FitsImage.h"

MainWindow::MainWindow()
{
	// Set up user interface from the Designer file
    ui.setupUi(this);
	
	// Set null to the CoordinatePanels
	fitsCoordinatePanel = new CoordinatePanel(ui.graphicsView_1);
	epoCoordinatePanel = new CoordinatePanel(ui.graphicsView_2);
	fitsCoordinatePanel->setWindowOpacity(qreal(0.40));
	epoCoordinatePanel->setWindowOpacity(qreal(0.40));
	fitsCoordinatePanel->hide();
	epoCoordinatePanel->hide();
	
	// Set up the DropSites to accept the correct extensions
	ui.dropLabel_1->setFileExtensions(false);
	ui.dropLabel_2->setFileExtensions(true);
	
	// Create a tool bar
	dockwidget = new QDockWidget(this);
	dockwidget->setObjectName(QString::fromUtf8("dockwidget"));
	dockwidget->setAllowedAreas(Qt::BottomDockWidgetArea);
	QWidget* blah = new QWidget(this);
	dockwidget->setTitleBarWidget(blah);
	 
	// Add widgets layout to tool bar
	ui_dockwidget.setupUi(dockwidget);
	
	// Add the tool bar to the main window
	addDockWidget(Qt::BottomDockWidgetArea, dockwidget, Qt::Horizontal);
	
	// Testing message on status bar
//	QLabel *msg = new QLabel(QString("PinpointWCS is ready!"));
//	ui.statusbar->addWidget(msg);
	
	// Connect signals and slots
	connect(ui.dropLabel_1, SIGNAL(readyForImport()), this, SLOT(loadImages()));
	connect(ui.dropLabel_2, SIGNAL(readyForImport()), this, SLOT(loadImages()));
}


MainWindow::~MainWindow() {}

bool MainWindow::loadImages()
{
	std::cout << "Attempting to load files to workspace ... \n";
	if (ui.dropLabel_1->ready and ui.dropLabel_2->ready) {
		std::cout << "Both DropAreas are ready!\n";
		
		// Call loadEpoImage and loadFitsImage
		if (!loadEpoImage(ui.dropLabel_2->filepath))
		{
			std::cout << "Loading of EPO image failed ...\n";
			return false;
		}
		if (!loadFitsImage(ui.dropLabel_1->filepath))
		{
			std::cout << "Loading of FITS image failed ...\n";
			return false;
		}
		
		// Disconnect dropLabels from signal
		
		// Flip the stacked widgets
		ui.stackedWidget_1->setCurrentIndex(1);
		ui.stackedWidget_2->setCurrentIndex(1);
		
		// Set up the CoordinatePanels for each image
		setCoordinatePanelSize();
		fitsCoordinatePanel->show();
		epoCoordinatePanel->show();
		buildCoordinatePanelMachine();
		
		// Write some data to the coordinate panel
		QString equinox;
		QString naxis1;
		QString naxis2;
		QString crval1;
		QString crval2;
		QString crpix1;
		QString crpix2;
		QString cd11;
		QString cd12;
		QString cd21;
		QString cd22;
		
		equinox.sprintf("%.1f", fits->wcs->equinox);
		naxis1.sprintf("%d", fits->image->width());
		naxis2.sprintf("%d", fits->image->height());
		crval1.sprintf("%.2f", fits->wcs->crval[0]);
		crval2.sprintf("%.2f", fits->wcs->crval[1]);
		crpix1.sprintf("%.2f", fits->wcs->crpix[0]);
		crpix2.sprintf("%.2f", fits->wcs->crpix[1]);
		cd11.sprintf("%.6f", fits->wcs->cd[0]);
		cd12.sprintf("%.6f", fits->wcs->cd[1]);
		cd21.sprintf("%.6f", fits->wcs->cd[2]);
		cd22.sprintf("%.6f", fits->wcs->cd[3]);
		
		fitsCoordinatePanel->ui.radesys_input->setText(fits->wcs->radesys);
		fitsCoordinatePanel->ui.equinox_input->setText(equinox);
		fitsCoordinatePanel->ui.ctype1_input->setText(fits->wcs->ctype[0]);
		fitsCoordinatePanel->ui.ctype2_input->setText(fits->wcs->ctype[1]);
		fitsCoordinatePanel->ui.naxis1_input->setText(naxis1);
		fitsCoordinatePanel->ui.naxis2_input->setText(naxis2);
		fitsCoordinatePanel->ui.crval1_input->setText(crval1);
		fitsCoordinatePanel->ui.crval2_input->setText(crval2);
		fitsCoordinatePanel->ui.crpix1_input->setText(crpix1);
		fitsCoordinatePanel->ui.crpix2_input->setText(crpix2);
		fitsCoordinatePanel->ui.cd_11_input->setText(cd11);
		fitsCoordinatePanel->ui.cd_12_input->setText(cd12);
		fitsCoordinatePanel->ui.cd_21_input->setText(cd21);
		fitsCoordinatePanel->ui.cd_22_input->setText(cd22);

		
		

		
		std::cout << fits->wcs->crpix[0] << "\n";
		
		// Connect some signals
		connect(ui.graphicsView_1, SIGNAL(objectResized(QSize)), this, SLOT(setCoordinatePanelSize()));
		connect(ui.graphicsView_2, SIGNAL(objectResized(QSize)), this, SLOT(setCoordinatePanelSize()));
		return true;
	}
	return false;
}

bool MainWindow::loadEpoImage(QString& filename)
{
	std::cout << "Loading EPO image ...\n";
	ui.graphicsView_2->setup(QPixmap(filename), false);
	return true;
}


bool MainWindow::loadFitsImage(QString& filename)
{
	std::cout << "Loading FITS image ... \n";
	fits = new FitsImage(filename);

	std::cout << "Image is " << fits->image->isNull() << "\n";
	QPixmap *pixmap = new QPixmap(QPixmap::fromImage(*(fits->image), Qt::DiffuseDither));
	ui.graphicsView_1->setup(*pixmap, true);	
	return true;
}


void MainWindow::buildCoordinatePanelMachine()
{
	// Intialize machine and states
	coordinatePanelMachine = new QStateMachine;
	coordinatePanelOn = new QState(coordinatePanelMachine);
	coordinatePanelOff = new QState(coordinatePanelMachine);
	
	// Set the initial state of the machine
	coordinatePanelMachine->setInitialState(coordinatePanelOn);
	
	// Get the position of the QStackedWidgets
	QRect p1 = ui.graphicsView_1->geometry();
	QRect p2 = ui.graphicsView_2->geometry();
	
	// Properties for the on state
	coordinatePanelOn->assignProperty(fitsCoordinatePanel, "pos", QPointF(0, 0));
	coordinatePanelOn->assignProperty(epoCoordinatePanel, "pos", QPointF(0, 0));
	
	// Properties for the off state
	coordinatePanelOff->assignProperty(fitsCoordinatePanel, "pos", QPointF(0, -55));
	coordinatePanelOff->assignProperty(epoCoordinatePanel, "pos", QPointF(0, -55));
	
	// Set transition from on state to off state
	QAbstractTransition *t1 = coordinatePanelOn->addTransition(ui.actionCoordinates, SIGNAL(triggered()), coordinatePanelOff);
	t1->addAnimation(new QPropertyAnimation(fitsCoordinatePanel, "pos"));
	t1->addAnimation(new QPropertyAnimation(epoCoordinatePanel, "pos"));
	
	// Set transition from off state to on state
	QAbstractTransition *t2 = coordinatePanelOff->addTransition(ui.actionCoordinates, SIGNAL(triggered()), coordinatePanelOn);
	t2->addAnimation(new QPropertyAnimation(fitsCoordinatePanel, "pos"));
	t2->addAnimation(new QPropertyAnimation(epoCoordinatePanel, "pos"));
	
	// Start the machine
	coordinatePanelMachine->start();
}

void MainWindow::setCoordinatePanelSize()
{
	// This function sets the size of the coordinate panels
	// relative to the parent (i.e. GraphicsViews)
	fitsCoordinatePanel->resize(QSize(ui.graphicsView_1->width(), 55));
	epoCoordinatePanel->resize(QSize(ui.graphicsView_2->width(), 55));
}