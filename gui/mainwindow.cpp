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
#include "Commands.h"

MainWindow::MainWindow()
{
	// Set up user interface from the Designer file
    ui.setupUi(this);
	
	// Initialize the data model
	dataModel = new CoordinateModel();
	
	// Initialize the undo stack
	undoAction = dataModel->undoStack->createUndoAction(this, tr("&Undo"));
	redoAction = dataModel->undoStack->createRedoAction(this, tr("&Redo"));
	undoAction->setShortcut(QKeySequence::Undo);
	redoAction->setShortcut(QKeySequence::Redo);
	ui.menuEdit->addAction(undoAction);
	ui.menuEdit->addAction(redoAction);
	
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
//	QLabel *msg = new QLabel(QString("PinpointWCS from the Chandra X-ray Observatory"));
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
		disconnect(ui.dropLabel_1, SIGNAL(readyForImport()), this, SLOT(loadImages()));
		disconnect(ui.dropLabel_2, SIGNAL(readyForImport()), this, SLOT(loadImages()));
		
		// Set up the table view
		tableView = new QTableView;
		tableView->setModel(dataModel);
		tableView->show();
		
		// Flip the stacked widgets
		ui.stackedWidget_1->setCurrentIndex(1);
		ui.stackedWidget_2->setCurrentIndex(1);
		
		// Enable some menu items
		ui.actionInfo->setEnabled(true);
		ui.actionCoordinates->setEnabled(true);
		ui.actionImageAdjustments->setEnabled(true);
		
		// Set up the WcsInfoPanel for each image
		fitsWcsInfoPanel->parentResized(ui.graphicsView_1->size());
		epoWcsInfoPanel->parentResized(ui.graphicsView_2->size());
		fitsWcsInfoPanel->show();
		epoWcsInfoPanel->show();
		buildWcsInfoPanelMachine();
		
		// Set up the Coordinate Panels for each image
		fitsCoordPanel->parentResized(ui.graphicsView_1->size());
		epoCoordPanel->parentResized(ui.graphicsView_2->size());
		fitsCoordPanel->show();
		epoCoordPanel->show();
		buildCoordPanelMachine();
		fitsWcsInfoPanel->loadWCS(*(fitsImage->wcs));
		
		// Set up the FitsToolbar, set range and value for sliders
		fitsToolbar->setExtremals(fitsImage->lowerLimit, fitsImage->upperLimit);
		fitsToolbar->setSliderValues(fitsImage->vmin, fitsImage->vmax);
		fitsToolbar->parentResized(ui.graphicsView_1->size());
		buildImageAdjustmentMachine();
		fitsToolbar->show();
		
		// Connect some signals -- used for resizing panels
		connect(ui.graphicsView_1, SIGNAL(objectResized(QSize)), fitsWcsInfoPanel, SLOT(parentResized(QSize)));
		connect(ui.graphicsView_2, SIGNAL(objectResized(QSize)), epoWcsInfoPanel, SLOT(parentResized(QSize)));		
		connect(ui.graphicsView_1, SIGNAL(objectResized(QSize)), fitsCoordPanel, SLOT(parentResized(QSize)));
		connect(ui.graphicsView_2, SIGNAL(objectResized(QSize)), epoCoordPanel, SLOT(parentResized(QSize)));
		connect(ui.graphicsView_1, SIGNAL(objectResized(QSize)), this, SLOT(updateCoordPanelProperties()));
		connect(ui.graphicsView_2, SIGNAL(objectResized(QSize)), this, SLOT(updateCoordPanelProperties()));
		
		connect(ui.graphicsView_1, SIGNAL(objectResized(QSize)), fitsToolbar, SLOT(parentResized(QSize)));
		
		// Connect more signals -- used for updating info on panels
		connect(fitsScene, SIGNAL(mousePositionChanged(QPointF)), this, SLOT(updateFitsCoordinates(QPointF)));
		connect(epoScene, SIGNAL(mousePositionChanged(QPointF)), this, SLOT(updateEpoCoordinates(QPointF)));
		
		// Connect yet more signals -- para comunicación entre los GraphicsScenes
		connect(fitsScene, SIGNAL(sceneDoubleClicked(GraphicsScene*, QPointF)), this, SLOT(addMarker(GraphicsScene*, QPointF)));
		connect(epoScene, SIGNAL(sceneDoubleClicked(GraphicsScene*, QPointF)), this, SLOT(addMarker(GraphicsScene*, QPointF)));
		
		connect(fitsScene, SIGNAL(toggleNeighborScene(bool)), epoScene, SLOT(toggleClickable(bool)));
		connect(epoScene, SIGNAL(toggleNeighborScene(bool)), fitsScene, SLOT(toggleClickable(bool)));
		connect(fitsScene, SIGNAL(itemMoved(CoordMarker*, QPointF)), this, SLOT(itemMoved(CoordMarker*, QPointF)));
		connect(epoScene, SIGNAL(itemMoved(CoordMarker*, QPointF)), this, SLOT(itemMoved(CoordMarker*, QPointF)));
		
		// Connect even more signals -- ComboBox and Sliders for FitsImage and GraphicsScene
		connect(fitsToolbar->ui.stretchComboBox, SIGNAL(currentIndexChanged(int)), fitsImage, SLOT(setStretch(int)));
		connect(fitsToolbar, SIGNAL(updateVmin(float)), fitsImage, SLOT(setVmin(float)));
		connect(fitsToolbar, SIGNAL(updateVmax(float)), fitsImage, SLOT(setVmax(float)));
		connect(fitsToolbar->ui.invertCheckBox, SIGNAL(stateChanged(int)), fitsImage, SLOT(invert()));
		connect(fitsImage, SIGNAL(pixmapChanged(QPixmap*)), fitsScene, SLOT(updatePixmap(QPixmap*)));
		
		return true;
	}
	return false;
}

bool MainWindow::loadEpoImage(QString& filename)
{
	qDebug() << "Loading EPO image ...";
	epoImage = new EpoImage(filename);
	epoScene = new GraphicsScene(*(epoImage->pixmap), false);
	ui.graphicsView_2->setScene(epoScene);
	return true;
}


bool MainWindow::loadFitsImage(QString& filename)
{
	qDebug() << "Loading FITS image ...";
	fitsImage = new FitsImage(filename);
	fitsScene = new GraphicsScene(*(fitsImage->pixmap), true);
	ui.graphicsView_1->setScene(fitsScene);
	return true;
}

void MainWindow::addMarker(GraphicsScene *scene, QPointF pos)
{
	dataModel->setData(scene, QModelIndex(), pos, Qt::EditRole);
}



void MainWindow::itemMoved(CoordMarker *movedItem, const QPointF &oldPosition)
{
	qDebug() << "itemMoved";
	dataModel->updateData(movedItem, oldPosition, Qt::EditRole);
}


void MainWindow::testSlot()
{
	qDebug() << "Test Slot";
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
	WcsInfoPanelOn->assignProperty(fitsToolbar, "pos", QPointF(0, -1*fitsToolbar->height()));
	WcsInfoPanelOn->assignProperty(ui.actionImageAdjustments, "checked", false);
	
	// Properties for the off state
	WcsInfoPanelOff->assignProperty(fitsWcsInfoPanel, "pos", QPointF(0, -1*fitsWcsInfoPanel->height()));
	WcsInfoPanelOff->assignProperty(epoWcsInfoPanel, "pos", QPointF(0, -1*epoWcsInfoPanel->height()));
	
	// Set transition from on state to off state
	QAbstractTransition *t1 = WcsInfoPanelOn->addTransition(ui.actionInfo, SIGNAL(toggled(bool)), WcsInfoPanelOff);
	t1->addAnimation(new QPropertyAnimation(fitsWcsInfoPanel, "pos"));
	t1->addAnimation(new QPropertyAnimation(epoWcsInfoPanel, "pos"));
	
	// Set transition from off state to on state
	QAbstractTransition *t2 = WcsInfoPanelOff->addTransition(ui.actionInfo, SIGNAL(toggled(bool)), WcsInfoPanelOn);
	t2->addAnimation(new QPropertyAnimation(fitsWcsInfoPanel, "pos"));
	t2->addAnimation(new QPropertyAnimation(epoWcsInfoPanel, "pos"));
	t2->addAnimation(new QPropertyAnimation(fitsToolbar, "pos"));
	
	// Start the machine
	WcsInfoPanelMachine->start();
}

void MainWindow::buildImageAdjustmentMachine()
{
	// Initialize machine and states
	imageAdjustmentMachine = new QStateMachine;
	imageAdjustmentPanelOn = new QState(imageAdjustmentMachine);
	imageAdjustmentPanelOff = new QState(imageAdjustmentMachine);
	
	// Set initial state of the machine
	imageAdjustmentMachine->setInitialState(imageAdjustmentPanelOn);
	
	// Set properties for the on state - image adjustment panel on, wcs info panel off
	imageAdjustmentPanelOn->assignProperty(fitsToolbar, "pos", QPointF(0, 0));
	imageAdjustmentPanelOn->assignProperty(fitsWcsInfoPanel, "pos", QPointF(0, -1*fitsWcsInfoPanel->height()));
	imageAdjustmentPanelOn->assignProperty(epoWcsInfoPanel, "pos", QPointF(0, -1*epoWcsInfoPanel->height()));
	imageAdjustmentPanelOn->assignProperty(ui.actionInfo, "checked", false);
	
	// Set properites for the off state - image adjustment panel off, wcs info panel off
	imageAdjustmentPanelOff->assignProperty(fitsToolbar, "pos", QPointF(0, -1*fitsToolbar->height()));
	
	// Set transition from on state to off state
	QAbstractTransition *t1 = imageAdjustmentPanelOn->addTransition(ui.actionImageAdjustments, SIGNAL(toggled(bool)), imageAdjustmentPanelOff);
	t1->addAnimation(new QPropertyAnimation(fitsToolbar, "pos"));
	
	// Set transition from off state to on state
	QAbstractTransition *t2 = imageAdjustmentPanelOff->addTransition(ui.actionImageAdjustments, SIGNAL(toggled(bool)), imageAdjustmentPanelOn);
	t2->addAnimation(new QPropertyAnimation(fitsToolbar, "pos"));
	t2->addAnimation(new QPropertyAnimation(fitsWcsInfoPanel, "pos"));
	t2->addAnimation(new QPropertyAnimation(epoWcsInfoPanel, "pos"));
	
	// Start the machine
	imageAdjustmentMachine->start();
	
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