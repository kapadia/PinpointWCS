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
	
	// Set null to the WcsInfoPanels
	fitsWcsInfoPanel = new WcsInfoPanel(ui.graphicsView_1);
	epoWcsInfoPanel = new WcsInfoPanel(ui.graphicsView_2);
	fitsWcsInfoPanel->hide();
	epoWcsInfoPanel->hide();
	
	// Set null to the CoordinatePanels
	fitsCoordPanel = new CoordinatePanel(ui.graphicsView_1);
	epoCoordPanel = new CoordinatePanel(ui.graphicsView_2);
	fitsCoordPanel->hide();
	epoCoordPanel->hide();
	
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
		
		// Set up the WcsInfoPanel for each image
		fitsWcsInfoPanel->show();
		epoWcsInfoPanel->show();
		fitsWcsInfoPanel->parentResized(ui.graphicsView_1->size());
		epoWcsInfoPanel->parentResized(ui.graphicsView_2->size());
		buildWcsInfoPanelMachine();
		
		fitsCoordPanel->show();
		epoCoordPanel->show();
		fitsCoordPanel->parentResized(ui.graphicsView_1->size());
		epoCoordPanel->parentResized(ui.graphicsView_2->size());
		buildCoordPanelMachine();
		fitsWcsInfoPanel->loadWCS(*(fits->wcs), fits->image->width(), fits->image->height());
		
		// Connect some signals
		connect(ui.graphicsView_1, SIGNAL(objectResized(QSize)), fitsWcsInfoPanel, SLOT(parentResized(QSize)));
		connect(ui.graphicsView_2, SIGNAL(objectResized(QSize)), epoWcsInfoPanel, SLOT(parentResized(QSize)));		
		connect(ui.graphicsView_1, SIGNAL(objectResized(QSize)), fitsCoordPanel, SLOT(parentResized(QSize)));
		connect(ui.graphicsView_2, SIGNAL(objectResized(QSize)), epoCoordPanel, SLOT(parentResized(QSize)));
		connect(ui.graphicsView_1, SIGNAL(objectResized(QSize)), this, SLOT(updateCoordPanelProperties()));
		connect(ui.graphicsView_2, SIGNAL(objectResized(QSize)), this, SLOT(updateCoordPanelProperties()));
//		connect(ui.graphicsView_1->scene(), SIGNAL(mousePositionChanged(QPointF)), fitsCoordPanel, SLOT(updateCoordinates(QPointF)));
//		connect(ui.graphicsView_2->scene(), SIGNAL(mousePositionChanged(QPointF)), epoCoordPanel, SLOT(updateCoordinates(QPointF)));
		
		connect(ui.graphicsView_1->scene(), SIGNAL(mousePositionChanged(QPointF)), this, SLOT(computeCoordinates(QPointF)));
//		connect(ui.graphicsView_2->scene(), SIGNAL(mousePositionChanged(QPointF)), this, SLOT(computeCoordinates(QPointF)));
		
		
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


void MainWindow::buildWcsInfoPanelMachine()
{
	// Intialize machine and states
	WcsInfoPanelMachine = new QStateMachine;
	WcsInfoPanelOn = new QState(WcsInfoPanelMachine);
	WcsInfoPanelOff = new QState(WcsInfoPanelMachine);
	
	// Set the initial state of the machine
	WcsInfoPanelMachine->setInitialState(WcsInfoPanelOff);
	
	// Get the position of the GraphicsViews
	QRect p1 = ui.graphicsView_1->geometry();
	QRect p2 = ui.graphicsView_2->geometry();
	
	// Properties for the on state
	WcsInfoPanelOn->assignProperty(fitsWcsInfoPanel, "pos", QPointF(0, 0));
	WcsInfoPanelOn->assignProperty(epoWcsInfoPanel, "pos", QPointF(0, 0));
	
	// Properties for the off state
	WcsInfoPanelOff->assignProperty(fitsWcsInfoPanel, "pos", QPointF(0, -55));
	WcsInfoPanelOff->assignProperty(epoWcsInfoPanel, "pos", QPointF(0, -55));
	
	// Set transition from on state to off state
	QAbstractTransition *t1 = WcsInfoPanelOn->addTransition(ui.actionInfo, SIGNAL(triggered()), WcsInfoPanelOff);
	t1->addAnimation(new QPropertyAnimation(fitsWcsInfoPanel, "pos"));
	t1->addAnimation(new QPropertyAnimation(epoWcsInfoPanel, "pos"));
	
	// Set transition from off state to on state
	QAbstractTransition *t2 = WcsInfoPanelOff->addTransition(ui.actionInfo, SIGNAL(triggered()), WcsInfoPanelOn);
	t2->addAnimation(new QPropertyAnimation(fitsWcsInfoPanel, "pos"));
	t2->addAnimation(new QPropertyAnimation(epoWcsInfoPanel, "pos"));
	
	// Start the machine
	WcsInfoPanelMachine->start();
}

void MainWindow::buildCoordPanelMachine()
{
	// Initialize machine and states
	CoordPanelMachine = new QStateMachine;
	CoordPanelOn = new QState(CoordPanelMachine);
	CoordPanelOff = new QState(CoordPanelMachine);
	
	// Set initial state of the machine
	CoordPanelMachine->setInitialState(CoordPanelOn);
	
	// Set properties
	updateCoordPanelProperties();
	
	// Set transition from the on state to the off state
	QAbstractTransition *t1 = CoordPanelOn->addTransition(ui.actionCoordinates, SIGNAL(triggered()), CoordPanelOff);
	t1->addAnimation(new QPropertyAnimation(fitsCoordPanel, "pos"));
	t1->addAnimation(new QPropertyAnimation(epoCoordPanel, "pos"));
	
	// Set transition from the off state to the on state
	QAbstractTransition *t2 = CoordPanelOff->addTransition(ui.actionCoordinates, SIGNAL(triggered()), CoordPanelOn);
	t2->addAnimation(new QPropertyAnimation(fitsCoordPanel, "pos"));
	t2->addAnimation(new QPropertyAnimation(epoCoordPanel, "pos"));
	
	// Start the machine
	CoordPanelMachine->start();
}


void MainWindow::updateCoordPanelProperties()
{
	
	// Get the position of the GraphicsViews
	int h1 = ui.graphicsView_1->height();
	int h2 = ui.graphicsView_2->height();

	// Move panels
	fitsCoordPanel->move(0, h1-30);
	epoCoordPanel->move(0, h2-30);
	
	// Properties for the on state
	CoordPanelOn->assignProperty(fitsCoordPanel, "pos", QPointF(0, h1-30));
	CoordPanelOn->assignProperty(epoCoordPanel, "pos", QPointF(0, h2-30));
	
	// Properties for the off state
	CoordPanelOff->assignProperty(fitsCoordPanel, "pos", QPointF(0, h1));
	CoordPanelOff->assignProperty(epoCoordPanel, "pos", QPointF(0, h2));
}

void MainWindow::computeCoordinates(QPointF pos)
{
	double *world;
	world = fits->pixelToCelestialCoordinates(pos);
	fitsCoordPanel->updateCoordinates(pos, world);
}