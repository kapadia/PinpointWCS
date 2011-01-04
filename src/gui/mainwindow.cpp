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

#include <QDebug>
#include "mainwindow.h"

MainWindow::MainWindow()
{
	// Set up the user interface
    ui.setupUi(this);
	
	// Initialize the data model and set up the undostack
	dataModel = new CoordinateModel();
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
	
	// Initialize the FitsToolbar
	fitsToolbar = new FitsToolbar(ui.graphicsView_1);
	fitsToolbar->hide();
		
	// Set up the DropSites to accept the correct extensions
	ui.dropLabel_1->setFileExtensions(false);
	ui.dropLabel_2->setFileExtensions(true);
	
	// Testing message on status bar
//	QLabel *msg = new QLabel(QString("PinpointWCS from the Chandra X-ray Observatory"));
//	ui.statusbar->addWidget(msg);
	
	// Initialize dialogs
	aboutDialog = new AboutDialog(this);
	
	// Connect signals and slots
	connect(ui.dropLabel_1, SIGNAL(readyForImport()), this, SLOT(setupWorkspace()));
	connect(ui.dropLabel_2, SIGNAL(readyForImport()), this, SLOT(setupWorkspace()));
	connect(ui.actionAbout_PinpointWCS, SIGNAL(triggered(bool)), aboutDialog, SLOT(exec()));
}


MainWindow::~MainWindow() {}

bool MainWindow::setupWorkspace()
{
	qDebug() << "Attempting to setup workspace ...";
	if (ui.dropLabel_1->ready and ui.dropLabel_2->ready) {
		
		// Call loadEpoImage and loadFitsImage
		if (!loadEpoImage(ui.dropLabel_2->filepath))
			return false;
		if (!loadFitsImage(ui.dropLabel_1->filepath))
			return false;
		
		// Disconnect dropLabels from signal
		disconnect(ui.dropLabel_1, SIGNAL(readyForImport()), this, SLOT(setupWorkspace()));
		disconnect(ui.dropLabel_2, SIGNAL(readyForImport()), this, SLOT(setupWorkspace()));
		
		// Initialize the CoordinatePanels
		fitsCoordPanel = new CoordinatePanel(fitsImage, ui.graphicsView_1);
		epoCoordPanel = new CoordinatePanel(epoImage, ui.graphicsView_2);
		
		// Initialize the CoordinateTableDialog
		tableDelegate = new CoordinateDelegate(fitsScene, epoScene, this);
		coordinateTableDialog = new CoordinateTableDialog(this);
		coordinateTableDialog->ui.coordinateTable->setModel(dataModel);
		coordinateTableDialog->ui.coordinateTable->setItemDelegate(tableDelegate);
		
		// Initialize the ComputeWCS and ExportWCS objects
		computewcs = new ComputeWCS(&(dataModel->refCoords), &(dataModel->epoCoords), fitsImage->wcs, epoImage->pixmap->width(), epoImage->pixmap->height());
		exportwcs = new ExportWCS(&(ui.dropLabel_2->filepath), epoImage->pixmap, computewcs);
		
		// Flip the stacked widgets
		ui.stackedWidget_1->setCurrentIndex(1);
		ui.stackedWidget_2->setCurrentIndex(1);
		
		// Enable View Menu items
		ui.actionInfo->setEnabled(true);
		ui.actionCoordinates->setEnabled(true);
		ui.actionImageAdjustments->setEnabled(true);
		ui.actionCoordinate_Table->setEnabled(true);
		
		// Enable Image Menu items
		ui.actionLinear_Stretch->setEnabled(true);
		ui.actionLogarithm_Stretch->setEnabled(true);
		ui.actionSquare_Root_Stretch->setEnabled(true);
		ui.actionHyperbolic_Sine_Stretch->setEnabled(true);
		ui.actionPower_Stretch->setEnabled(true);
		ui.actionInvert->setEnabled(true);
		ui.actionRotate_Clockwise->setEnabled(true);
		ui.actionRotate_Counterclockwise->setEnabled(true);
		
		// Create a QActionGroup from the stretch menu items
		QActionGroup *stretchActionGroup = new QActionGroup(this);
		stretchActionGroup->addAction(ui.actionLinear_Stretch);
		stretchActionGroup->addAction(ui.actionLogarithm_Stretch);
		stretchActionGroup->addAction(ui.actionSquare_Root_Stretch);
		stretchActionGroup->addAction(ui.actionHyperbolic_Sine_Stretch);
		stretchActionGroup->addAction(ui.actionPower_Stretch);
		stretchActionGroup->setExclusive(true);
		
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
		connect(fitsScene, SIGNAL(mousePositionChanged(QPointF)), fitsCoordPanel, SLOT(updateCoordinates(QPointF)));
		connect(epoScene, SIGNAL(mousePositionChanged(QPointF)), epoCoordPanel, SLOT(updateCoordinates(QPointF)));
		
		// Connect yet more signals -- para comunicación entre los GraphicsScenes
		connect(fitsScene, SIGNAL(sceneDoubleClicked(GraphicsScene*, QPointF)), dataModel, SLOT(setData(GraphicsScene*, QPointF)));
		connect(epoScene, SIGNAL(sceneDoubleClicked(GraphicsScene*, QPointF)), dataModel, SLOT(setData(GraphicsScene*, QPointF)));
		
		connect(fitsScene, SIGNAL(toggleNeighborScene(bool)), epoScene, SLOT(toggleClickable(bool)));
		connect(epoScene, SIGNAL(toggleNeighborScene(bool)), fitsScene, SLOT(toggleClickable(bool)));
		
		connect(fitsScene, SIGNAL(itemMoved(GraphicsScene*, QPointF, QPointF)), dataModel, SLOT(updateData(GraphicsScene*, QPointF, QPointF)));
		connect(epoScene, SIGNAL(itemMoved(GraphicsScene*, QPointF, QPointF)), dataModel, SLOT(updateData(GraphicsScene*, QPointF, QPointF)));
		connect(tableDelegate, SIGNAL(itemMoved(GraphicsScene*, QPointF, QPointF, QModelIndex*)), dataModel, SLOT(updateData(GraphicsScene*, QPointF, QPointF, QModelIndex*)));
		
		// Connect even more signals -- Menu items and Sliders for FitsImage and GraphicsScene
		connect(stretchActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(stretch(QAction*)));
		connect(ui.actionInvert, SIGNAL(triggered(bool)), fitsImage, SLOT(invert()));
		connect(fitsToolbar, SIGNAL(updateVmin(float)), fitsImage, SLOT(setVmin(float)));
		connect(fitsToolbar, SIGNAL(updateVmax(float)), fitsImage, SLOT(setVmax(float)));
		connect(fitsImage, SIGNAL(pixmapChanged(QPixmap*)), fitsScene, SLOT(updatePixmap(QPixmap*)));
		
		// Connect more signals -- communicate between data model and ComputeWCS object
		connect(dataModel, SIGNAL(compute()), computewcs, SLOT(computeTargetWCS()));
		connect(computewcs, SIGNAL(wcs()), this, SLOT(enableExport()));
		connect(computewcs, SIGNAL(nowcs()), this, SLOT(enableExport()));

		// Connect signals -- export options
		connect(ui.actionFITS_Image, SIGNAL(triggered(bool)), exportwcs, SLOT(exportFITS()));
		connect(ui.actionAstronomy_Visualization_Metadata, SIGNAL(triggered(bool)), exportwcs, SLOT(exportAVM()));
		
		// And more signals ...
		connect(ui.actionCoordinate_Table, SIGNAL(triggered(bool)), coordinateTableDialog, SLOT(show()));
		
		// Mouse dependent rotation menu items
		rotateMenuItems(ui.graphicsView_1); // Set a default
		// TODO: What was I doing here!?!?!
//		connect(ui.graphicsView_1, SIGNAL(mouseEnterEvent(GraphicsView*)), this, SLOT());
		connect(ui.graphicsView_2, SIGNAL(mouseEnterEvent(GraphicsView*)), this, SLOT(rotateMenuItems(GraphicsView*)));
		
		return true;
	}
	return false;
}


void MainWindow::stretch(QAction *action)
{
	if (action == ui.actionLinear_Stretch)
	{
		if (fitsImage->stretch != LINEAR_STRETCH)
			fitsImage->setStretch(LINEAR_STRETCH);
	}
	else if (action == ui.actionLogarithm_Stretch)
	{
		if (fitsImage->stretch != LOG_STRETCH)
			fitsImage->setStretch(LOG_STRETCH);
	}
	else if (action == ui.actionSquare_Root_Stretch)
	{
		if (fitsImage->stretch != SQRT_STRETCH)
			fitsImage->setStretch(SQRT_STRETCH);
	}
	else if (action == ui.actionHyperbolic_Sine_Stretch)
	{
		if (fitsImage->stretch != ARCSINH_STRETCH)
			fitsImage->setStretch(ARCSINH_STRETCH);
	}
	else if (action == ui.actionPower_Stretch)
	{
		if (fitsImage->stretch != POWER_STRETCH)
			fitsImage->setStretch(POWER_STRETCH);
	}
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
	fitsScene = new GraphicsScene((fitsImage->pixmap), true);
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


void MainWindow::enableExport()
{
	if (computewcs->epoWCS)
	{
		// Enable export menu items
		ui.actionAstronomy_Visualization_Metadata->setEnabled(true);
		ui.actionFITS_Image->setEnabled(true);
		
		// Create EPO WCS object and load WCS to panel and export object
		epoImage->wcs = computewcs->initTargetWCS();
		// TODO: Change method argument type
		epoWcsInfoPanel->loadWCS(*(epoImage->wcs));
		exportwcs->setWCS(epoImage->wcs);
	}
	else
	{
		// Disable export menu items
		ui.actionAstronomy_Visualization_Metadata->setEnabled(false);
		ui.actionFITS_Image->setEnabled(false);
		
		// Destroy EPO WCS object and clear WCS from panel
		epoImage->wcs = NULL;
		epoWcsInfoPanel->clear();
		exportwcs->clearWCS();
	}
}


void MainWindow::computeWCS()
{
	computewcs->computeTargetWCS();
	if (computewcs->epoWCS)
	{
		epoImage->wcs = computewcs->initTargetWCS();
		epoWcsInfoPanel->loadWCS(*(epoImage->wcs));
	}
	else
	{
		epoWcsInfoPanel->clear();
	}
}


void MainWindow::rotateMenuItems(GraphicsView *gv)
{
	if (gv == ui.graphicsView_1)
	{
		ui.actionRotate_Clockwise->setText("Rotate FITS Clockwise");
		ui.actionRotate_Counterclockwise->setText("Rotate FITS Counterclockwise");
		
		// Disconnect slots
		disconnect(ui.actionRotate_Clockwise, SIGNAL(triggered()), ui.graphicsView_2, SLOT(rotateCW()));
		disconnect(ui.actionRotate_Counterclockwise, SIGNAL(triggered()), ui.graphicsView_2, SLOT(rotateCCW()));
		
		// Connect slots
		connect(ui.actionRotate_Clockwise, SIGNAL(triggered()), ui.graphicsView_1, SLOT(rotateCW()));
		connect(ui.actionRotate_Counterclockwise, SIGNAL(triggered()), ui.graphicsView_1, SLOT(rotateCCW()));
	}
	else
	{
		ui.actionRotate_Clockwise->setText("Rotate EPO Clockwise");
		ui.actionRotate_Counterclockwise->setText("Rotate EPO Counterclockwise");
		
		// Disconnect slots
		disconnect(ui.actionRotate_Clockwise, SIGNAL(triggered()), ui.graphicsView_1, SLOT(rotateCW()));
		disconnect(ui.actionRotate_Counterclockwise, SIGNAL(triggered()), ui.graphicsView_1, SLOT(rotateCCW()));
		
		// Connect slots
		connect(ui.actionRotate_Clockwise, SIGNAL(triggered()), ui.graphicsView_2, SLOT(rotateCW()));
		connect(ui.actionRotate_Counterclockwise, SIGNAL(triggered()), ui.graphicsView_2, SLOT(rotateCCW()));
	}
}

void MainWindow::testSlot()
{
	qDebug() << "Test Slot";
}