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
#include <QDebug>

#include "mainwindow.h"
#include "FitsImage.h"
#include "GraphicsScene.h"

MainWindow::MainWindow()
{
	// Set up user interface from the Designer file
    ui.setupUi(this);
	
	// Initialize the WcsInfoPanels
	fitsWcsInfoPanel = new WcsInfoPanel(ui.graphicsView_1);
	epoWcsInfoPanel = new WcsInfoPanel(ui.graphicsView_2);
	fitsWcsInfoPanel->hide();
	epoWcsInfoPanel->hide();
	
	// Initialize the CoordinatePanels
	fitsCoordPanel = new CoordinatePanel(ui.graphicsView_1);
	epoCoordPanel = new CoordinatePanel(ui.graphicsView_2);
	fitsCoordPanel->hide();
	epoCoordPanel->hide();
	
	// Initialize the FitsToolbar
	fitsToolbar = new FitsToolbar(ui.graphicsView_1);
	fitsToolbar->hide();
	
	// Set up the DropSites to accept the correct extensions
	ui.dropLabel_1->setFileExtensions(false);
	ui.dropLabel_2->setFileExtensions(true);
	
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
	qDebug() << "Attempting to load files to workspace ...";
	if (ui.dropLabel_1->ready and ui.dropLabel_2->ready) {
		qDebug() << "Both DropAreas are ready!";
		
		// Call loadEpoImage and loadFitsImage
		if (!loadEpoImage(ui.dropLabel_2->filepath))
		{
			qDebug() << "Loading of EPO image failed ...";
			return false;
		}
		if (!loadFitsImage(ui.dropLabel_1->filepath))
		{
			qDebug() << "Loading of FITS image failed ...";
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
		
		// Set up the Coordinate Panels for each image
		fitsCoordPanel->show();
		epoCoordPanel->show();
		fitsCoordPanel->parentResized(ui.graphicsView_1->size());
		epoCoordPanel->parentResized(ui.graphicsView_2->size());
		buildCoordPanelMachine();
		fitsWcsInfoPanel->loadWCS(*(fitsImage->wcs));
		
		// Set up the FitsToolbar
		fitsToolbar->show();
		
		// Connect some signals -- used for resizing panels
		connect(ui.graphicsView_1, SIGNAL(objectResized(QSize)), fitsWcsInfoPanel, SLOT(parentResized(QSize)));
		connect(ui.graphicsView_2, SIGNAL(objectResized(QSize)), epoWcsInfoPanel, SLOT(parentResized(QSize)));		
		connect(ui.graphicsView_1, SIGNAL(objectResized(QSize)), fitsCoordPanel, SLOT(parentResized(QSize)));
		connect(ui.graphicsView_2, SIGNAL(objectResized(QSize)), epoCoordPanel, SLOT(parentResized(QSize)));
		connect(ui.graphicsView_1, SIGNAL(objectResized(QSize)), this, SLOT(updateCoordPanelProperties()));
		connect(ui.graphicsView_2, SIGNAL(objectResized(QSize)), this, SLOT(updateCoordPanelProperties()));
		
		// Connect more signals -- used for updating info on panels
		connect(ui.graphicsView_1->scene(), SIGNAL(mousePositionChanged(QPointF)), this, SLOT(updateFitsCoordinates(QPointF)));
		connect(ui.graphicsView_2->scene(), SIGNAL(mousePositionChanged(QPointF)), this, SLOT(updateEpoCoordinates(QPointF)));
		
		// Connect yet more signals -- para comunicación entre los GraphicsScenes
		connect(ui.graphicsView_1->scene(), SIGNAL(coordinateMarked()), ui.graphicsView_2->scene(), SLOT(makeClickable()));
		connect(ui.graphicsView_2->scene(), SIGNAL(coordinateMarked()), ui.graphicsView_1->scene(), SLOT(makeClickable()));
		
		// Connect even more signals -- Notify GraphicsScene of an update pixmap
		connect(fitsToolbar->ui.stretchComboBox, SIGNAL(currentIndexChanged(int index)), fitsImage, SLOT(magic(int)));
		connect(fitsImage, SIGNAL(pixmapChanged(QPixmap)), ui.graphicsView_1->scene(), SLOT(updatePixmap(QPixmap)));
		
		return true;
	}
	return false;
}

bool MainWindow::loadEpoImage(QString& filename)
{
	qDebug() << "Loading EPO image ...";
	epoImage = new EpoImage(filename);
	GraphicsScene *epoScene = new GraphicsScene(*(epoImage->pixmap), false);
	ui.graphicsView_2->setScene(epoScene);
	return true;
}


bool MainWindow::loadFitsImage(QString& filename)
{
	qDebug() << "Loading FITS image ...";
	fitsImage = new FitsImage(filename);
	GraphicsScene *fitsScene = new GraphicsScene(*(fitsImage->pixmap), true);
	ui.graphicsView_1->setScene(fitsScene);
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

void MainWindow::updateFitsCoordinates(QPointF pos)
{
	double *world;
	world = fitsImage->pix2sky(pos);
	fitsCoordPanel->updateCoordinates(pos, world);
}

void MainWindow::updateEpoCoordinates(QPointF pos)
{
	double *world = NULL;
	if (epoImage->wcsExists)
		world = epoImage->pix2sky(pos);
	epoCoordPanel->updateCoordinates(pos, world);
}