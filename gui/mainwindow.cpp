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
	QLabel *msg = new QLabel(QString("PinpointWCS is ready!"));
	ui.statusbar->addWidget(msg);
	
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
		fitsCoordinatePanel->show();
		epoCoordinatePanel->show();
		buildCoordinatePanelMachine();
		i
		// Connect some signals
		connect(ui.graphicsView_1, SIGNAL(objectResized(QSize)), this, SLOT(updateCoordinatePanelStates()));
		connect(ui.graphicsView_2, SIGNAL(objectResized(QSize)), this, SLOT(updateCoordinatePanelStates()));
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
	FitsImage fits(filename);

	std::cout << "Image is " << fits.image->isNull() << "\n";
	QPixmap *pixmap = new QPixmap(QPixmap::fromImage(*(fits.image), Qt::DiffuseDither));
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
	QRect r1 = ui.graphicsView_1->geometry();
	QRect r2 = ui.graphicsView_2->geometry();
	
	// Properties for the on state
	coordinatePanelOn->assignProperty(fitsCoordinatePanel, "geometry", QRectF(r1.x()+1, r1.y()+1, r1.width(), 100));
	coordinatePanelOn->assignProperty(epoCoordinatePanel, "geometry", QRectF(r2.x()+1, r2.y()+1, r2.width(), 100));
	
	// Properties for the off state
	coordinatePanelOff->assignProperty(fitsCoordinatePanel, "geometry", QRectF(r1.x()+1, r1.y()-100, r1.width(), 100));
	coordinatePanelOff->assignProperty(epoCoordinatePanel, "geometry", QRectF(r2.x()+1, r2.y()-100, r1.height(), 100));
	
	// Set transition from on state to off state
	QAbstractTransition *t1 = coordinatePanelOn->addTransition(ui.actionCoordinates, SIGNAL(triggered()), coordinatePanelOff);
	t1->addAnimation(new QPropertyAnimation(fitsCoordinatePanel, "geometry"));
	t1->addAnimation(new QPropertyAnimation(epoCoordinatePanel, "geometry"));
	
	// Set transition from off state to on state
	QAbstractTransition *t2 = coordinatePanelOff->addTransition(ui.actionCoordinates, SIGNAL(triggered()), coordinatePanelOn);
	t2->addAnimation(new QPropertyAnimation(fitsCoordinatePanel, "geometry"));
	t2->addAnimation(new QPropertyAnimation(epoCoordinatePanel, "geometry"));	
	
	// Start the machine
	coordinatePanelMachine->start();
}


void MainWindow::updateCoordinatePanelStates()
{
	// Get the position of the QStackedWidgets
	QRect r1 = ui.graphicsView_1->geometry();
	QRect r2 = ui.graphicsView_2->geometry();
	
	// Properties for the on state
	coordinatePanelOn->assignProperty(fitsCoordinatePanel, "geometry", QRectF(r1.x()+1, r1.y()+1, r1.width(), 100));
	coordinatePanelOn->assignProperty(epoCoordinatePanel, "geometry", QRectF(r2.x()+1, r2.y()+1, r2.width(), 100));
	
	// Properties for the off state
	coordinatePanelOff->assignProperty(fitsCoordinatePanel, "geometry", QRectF(r1.x()+1, r1.y()-100, r1.width(), 100));
	coordinatePanelOff->assignProperty(epoCoordinatePanel, "geometry", QRectF(r2.x()+1, r2.y()-100, r2.width(), 100));
}
