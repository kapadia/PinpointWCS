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
	
	// Set up the DropSites to accept the correct extensions
	ui.dropLabel_1->setup(false);
	ui.dropLabel_2->setup(true);
	
	// Testing message on status bar
//	QLabel *msg = new QLabel(QString("PinpointWCS from the Chandra X-ray Observatory"));
//	ui.statusbar->addWidget(msg);
	
	// Create a QActionGroup for the stretch menu items
	stretchActionGroup = new QActionGroup(this);
	stretchActionGroup->addAction(ui.actionLinear_Stretch);
	stretchActionGroup->addAction(ui.actionLogarithm_Stretch);
	stretchActionGroup->addAction(ui.actionSquare_Root_Stretch);
	stretchActionGroup->addAction(ui.actionInverseHyperbolic_Sine_Stretch);
	stretchActionGroup->addAction(ui.actionPower_Stretch);
	stretchActionGroup->setExclusive(true);
	
	// Create a QActionGroup for the WCS format options
	wcsFormatActionGroup = new QActionGroup(this);
	wcsFormatActionGroup->addAction(ui.actionDegrees);
	wcsFormatActionGroup->addAction(ui.actionSexagesimal);
	wcsFormatActionGroup->setExclusive(true);
	
	// Initialize dialogs
	aboutDialog = new AboutDialog(this);
	helpPanel = new HelpPanel(this);
	buildHelpPanelMachine();
	
	// Connect signals and slots
	connect(ui.dropLabel_1, SIGNAL(readyForImport()), this, SLOT(setupImages()));
	connect(ui.dropLabel_2, SIGNAL(readyForImport()), this, SLOT(setupImages()));
	connect(ui.actionAbout_PinpointWCS, SIGNAL(triggered(bool)), aboutDialog, SLOT(exec()));
	connect(this, SIGNAL(objectResized(QSize)), helpPanel, SLOT(parentResized(QSize)));
	connect(this, SIGNAL(objectResized(QSize)), this, SLOT(updateHelpPanelProperties()));
	
	// Teardown the workspace
	ui.actionNew_Workspace->setEnabled(false);
	connect(ui.actionNew_Workspace, SIGNAL(triggered(bool)), this, SLOT(teardownWorkspace()));
}


MainWindow::~MainWindow() {}

bool MainWindow::teardownWorkspace()
{
	qDebug() << "Attempting to teardown workspace ...";

	// Reconnect dropLabels to signal
	connect(ui.dropLabel_1, SIGNAL(readyForImport()), this, SLOT(setupImages()));
	connect(ui.dropLabel_2, SIGNAL(readyForImport()), this, SLOT(setupImages()));
		
	// Flip the stacked widgets
	ui.stackedWidget_1->setCurrentIndex(0);
	ui.stackedWidget_2->setCurrentIndex(0);
		
	// Disconnect some signals -- used for resizing panels
	disconnect(ui.graphicsView_1, SIGNAL(objectResized(QSize)), fitsWcsInfoPanel, SLOT(parentResized(QSize)));
	disconnect(ui.graphicsView_2, SIGNAL(objectResized(QSize)), epoWcsInfoPanel, SLOT(parentResized(QSize)));		
	disconnect(ui.graphicsView_1, SIGNAL(objectResized(QSize)), fitsCoordPanel, SLOT(parentResized(QSize)));
	disconnect(ui.graphicsView_2, SIGNAL(objectResized(QSize)), epoCoordPanel, SLOT(parentResized(QSize)));
	disconnect(ui.graphicsView_1, SIGNAL(objectResized(QSize)), this, SLOT(updateCoordPanelProperties()));
	disconnect(ui.graphicsView_2, SIGNAL(objectResized(QSize)), this, SLOT(updateCoordPanelProperties()));
	disconnect(ui.graphicsView_1, SIGNAL(objectResized(QSize)), fitsToolbar, SLOT(parentResized(QSize)));
	
	// Disconnect more signals -- used for updating info on panels
	disconnect(fitsScene, SIGNAL(mousePositionChanged(QPointF)), fitsCoordPanel, SLOT(updateCoordinates(QPointF)));
	disconnect(epoScene, SIGNAL(mousePositionChanged(QPointF)), epoCoordPanel, SLOT(updateCoordinates(QPointF)));
	
	// Disconnect yet more signals -- para comunicación entre los GraphicsScenes
	disconnect(fitsScene, SIGNAL(sceneDoubleClicked(GraphicsScene*, QPointF)), dataModel, SLOT(setData(GraphicsScene*, QPointF)));
	disconnect(epoScene, SIGNAL(sceneDoubleClicked(GraphicsScene*, QPointF)), dataModel, SLOT(setData(GraphicsScene*, QPointF)));
	
	disconnect(fitsScene, SIGNAL(toggleNeighborScene(bool)), epoScene, SLOT(toggleClickable(bool)));
	disconnect(epoScene, SIGNAL(toggleNeighborScene(bool)), fitsScene, SLOT(toggleClickable(bool)));
	
	disconnect(fitsScene, SIGNAL(itemMoved(GraphicsScene*, QPointF, QPointF)), dataModel, SLOT(updateData(GraphicsScene*, QPointF, QPointF)));
	disconnect(epoScene, SIGNAL(itemMoved(GraphicsScene*, QPointF, QPointF)), dataModel, SLOT(updateData(GraphicsScene*, QPointF, QPointF)));
	disconnect(tableDelegate, SIGNAL(itemMoved(GraphicsScene*, QPointF, QPointF, QModelIndex*)), dataModel, SLOT(updateData(GraphicsScene*, QPointF, QPointF, QModelIndex*)));
	
	// Disconnect even more signals -- Menu items and Sliders for FitsImage and GraphicsScene
	disconnect(stretchActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(stretch(QAction*)));
	disconnect(ui.actionInvert, SIGNAL(triggered(bool)), fitsImage, SLOT(invert()));
	disconnect(fitsToolbar, SIGNAL(updateVmin(float)), fitsImage, SLOT(setVmin(float)));
	disconnect(fitsToolbar, SIGNAL(updateVmax(float)), fitsImage, SLOT(setVmax(float)));
	disconnect(fitsImage, SIGNAL(pixmapChanged(QPixmap*)), fitsScene, SLOT(updatePixmap(QPixmap*)));
	disconnect(epoImage, SIGNAL(pixmapChanged(QPixmap*)), epoScene, SLOT(updatePixmap(QPixmap*)));
	
	// Disconnect señales for the WCS format
	disconnect(ui.actionDegrees, SIGNAL(toggled(bool)), fitsCoordPanel, SLOT(setWcsFormat(bool)));
	disconnect(ui.actionDegrees, SIGNAL(toggled(bool)), epoCoordPanel, SLOT(setWcsFormat(bool)));
	
	// Disconnect more signals -- communicate between data model and ComputeWCS object
	disconnect(dataModel, SIGNAL(compute()), computewcs, SLOT(computeTargetWCS()));
	disconnect(computewcs, SIGNAL(wcs()), this, SLOT(enableExport()));
	disconnect(computewcs, SIGNAL(nowcs()), this, SLOT(enableExport()));
	
	// Disconnect signals -- export options
	disconnect(ui.actionFITS_Image, SIGNAL(triggered(bool)), exportwcs, SLOT(exportFITS()));
	disconnect(ui.actionAVM, SIGNAL(triggered(bool)), exportwcs, SLOT(exportAVM()));
	
	// And more signals ...
	disconnect(ui.actionCoordinate_Table, SIGNAL(triggered(bool)), coordinateTableDialog, SLOT(toggle()));
	
	// Mouse dependent rotation menu items
	disconnect(ui.graphicsView_1, SIGNAL(mouseEnterEvent(GraphicsView*)), this, SLOT(rotateMenuItems(GraphicsView*)));
	disconnect(ui.graphicsView_2, SIGNAL(mouseEnterEvent(GraphicsView*)), this, SLOT(rotateMenuItems(GraphicsView*)));
	
	// Selecting correspofile://localhost/Users/akapadia/Documents/TestData/M101/m101.fitsnding items across scenes
	disconnect(fitsScene, SIGNAL(selectionChanged()), fitsScene, SLOT(findSelectedItem()));
	disconnect(fitsScene, SIGNAL(currentSelection(int)), epoScene, SLOT(matchSelectedItem(int)));
	disconnect(epoScene, SIGNAL(selectionChanged()), epoScene, SLOT(findSelectedItem()));
	disconnect(epoScene, SIGNAL(currentSelection(int)), fitsScene, SLOT(matchSelectedItem(int)));
	disconnect(fitsScene, SIGNAL(clearCorrespondingSelection()), epoScene, SLOT(clearSelection()));
	disconnect(epoScene, SIGNAL(clearCorrespondingSelection()), fitsScene, SLOT(clearSelection()));
	
	// Exporting signals and slots
	disconnect(exportwcs, SIGNAL(exportResults(bool)), this, SLOT(promptMessage(bool)));
	
	// Prediction and centroid signal and slots
	disconnect(ui.actionFit_Point, SIGNAL(triggered(bool)), this, SLOT(predictEpoPoint()));
	disconnect(ui.actionCentroid, SIGNAL(triggered(bool)), fitsScene, SLOT(selectedItemPos()));
	disconnect(fitsScene, SIGNAL(itemPos(QPointF)), fitsImage, SLOT(getCentroid(QPointF)));
	disconnect(fitsImage, SIGNAL(centroid(QPointF)), this, SLOT(updateWithCentroid(QPointF)));
	
	// Disable View Menu items
	ui.actionWCSInfo->setEnabled(false);
	ui.actionCoordinates->setEnabled(false);
	ui.actionImageAdjustments->setEnabled(false);
	ui.actionCoordinate_Table->setEnabled(false);
	ui.actionDegrees->setEnabled(false);
	ui.actionDegrees->setChecked(false);
	ui.actionSexagesimal->setEnabled(false);
	
	// Disable Image Menu items
	ui.actionLinear_Stretch->setEnabled(false);
	ui.actionLogarithm_Stretch->setEnabled(false);
	ui.actionSquare_Root_Stretch->setEnabled(false);
	ui.actionInverseHyperbolic_Sine_Stretch->setEnabled(false);
	ui.actionPower_Stretch->setEnabled(false);
	ui.actionInvert->setEnabled(false);
	ui.actionRotate_Clockwise->setEnabled(false);
	ui.actionRotate_Counterclockwise->setEnabled(false);
	
	// Deconstruct the undostack and data model
	ui.menuEdit->removeAction(redoAction);
	ui.menuEdit->removeAction(undoAction);
	
	// Delete some objects last
	delete undoAction;
	delete redoAction;
	delete dataModel;
	delete fitsWcsInfoPanel;
	delete epoWcsInfoPanel;
	delete fitsToolbar;
	delete epoImage;
	delete epoScene;
	delete fitsImage;
	delete fitsScene;
	delete fitsCoordPanel;
	delete epoCoordPanel;
	delete tableDelegate;
	delete coordinateTableDialog;
	delete computewcs;
	delete exportwcs;	
	delete msg;
	teardownWcsInfoPanelMachine();
	teardownImageAdjustmentMachine();
	teardownCoordPanelMachine();
	
	// Reset the drop labels
	ui.dropLabel_1->clean();
	ui.dropLabel_2->clean();
	
	// Disable from menu
	ui.actionNew_Workspace->setEnabled(false);
	
	return true;
}


bool MainWindow::setupImages()
{
	qDebug() << "Attempting to set up images ...";
	if (ui.dropLabel_1->ready and ui.dropLabel_2->ready)
	{
		// Call loadEPOImage and loadFITSImage
		loadEpoImage(ui.dropLabel_2->filepath);
		startFITSThread(ui.dropLabel_1->filepath);
		
		return true;
	}
	
	return false;
}

bool MainWindow::setupWorkspace()
{
	qDebug() << "Setting up workspace ...";
	
	// Disconnect dropLabels from signal
	disconnect(ui.dropLabel_1, SIGNAL(readyForImport()), this, SLOT(setupImages()));
	disconnect(ui.dropLabel_2, SIGNAL(readyForImport()), this, SLOT(setupImages()));
	
	// Initialize the data model and set up the undostack
	dataModel = new CoordinateModel();
	undoAction = dataModel->undoStack->createUndoAction(this, tr("&Undo"));
	redoAction = dataModel->undoStack->createRedoAction(this, tr("&Redo"));
	undoAction->setShortcut(QKeySequence::Undo);
	redoAction->setShortcut(QKeySequence::Redo);
	ui.menuEdit->addAction(undoAction);
	ui.menuEdit->addAction(redoAction);
	
	// Initialize the WcsInfoPanels
	fitsWcsInfoPanel = new WcsInfoPanel(true, ui.graphicsView_1);
	epoWcsInfoPanel = new WcsInfoPanel(false, ui.graphicsView_2);
	
	// Initialize the FitsToolbar
	fitsToolbar = new FitsToolbar(ui.graphicsView_1);
	
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
	ui.actionWCSInfo->setEnabled(true);
	ui.actionCoordinates->setEnabled(true);
	ui.actionCoordinates->setChecked(true);
	ui.actionImageAdjustments->setEnabled(true);
	ui.actionImageAdjustments->setChecked(true);
	ui.actionCoordinate_Table->setEnabled(true);
	ui.actionDegrees->setEnabled(true);
	ui.actionDegrees->setChecked(true);
	ui.actionSexagesimal->setEnabled(true);
	
	// Enable Image Menu items
	ui.actionLinear_Stretch->setEnabled(true);
	ui.actionLogarithm_Stretch->setEnabled(true);
	ui.actionSquare_Root_Stretch->setEnabled(true);
	ui.actionInverseHyperbolic_Sine_Stretch->setEnabled(true);
	ui.actionPower_Stretch->setEnabled(true);
	ui.actionInvert->setEnabled(true);
	ui.actionRotate_Clockwise->setEnabled(true);
	ui.actionRotate_Counterclockwise->setEnabled(true);
	ui.actionCentroid->setEnabled(true);
	
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
	fitsWcsInfoPanel->loadWCS(fitsImage->wcs);
	
	// Set up the FitsToolbar, set range and value for sliders
	fitsToolbar->setExtremals(fitsImage->lowerLimit, fitsImage->upperLimit);
	fitsToolbar->setSliderValues(fitsImage->vmin, fitsImage->vmax);
	fitsToolbar->parentResized(ui.graphicsView_1->size());
	buildImageAdjustmentMachine();
	
	// Initialize MessageBox
	msg = new MessageBox("Export Status", this);
	
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
	connect(fitsToolbar, SIGNAL(updateVmin(float)), fitsImage, SLOT(setVmin(float)));
	connect(fitsToolbar, SIGNAL(updateVmax(float)), fitsImage, SLOT(setVmax(float)));
	connect(fitsImage, SIGNAL(pixmapChanged(QPixmap*)), fitsScene, SLOT(updatePixmap(QPixmap*)));
	connect(epoImage, SIGNAL(pixmapChanged(QPixmap*)), epoScene, SLOT(updatePixmap(QPixmap*)));
	
	// Connect señales for the WCS format
	connect(ui.actionDegrees, SIGNAL(toggled(bool)), fitsCoordPanel, SLOT(setWcsFormat(bool)));
	connect(ui.actionDegrees, SIGNAL(toggled(bool)), epoCoordPanel, SLOT(setWcsFormat(bool)));
	
	// Connect more signals -- communicate between data model and ComputeWCS object
	connect(dataModel, SIGNAL(compute()), computewcs, SLOT(computeTargetWCS()));
	connect(computewcs, SIGNAL(wcs()), this, SLOT(enableExport()));
	connect(computewcs, SIGNAL(nowcs()), this, SLOT(enableExport()));

	// Connect signals -- export options
	connect(ui.actionFITS_Image, SIGNAL(triggered(bool)), exportwcs, SLOT(exportFITS()));
	connect(ui.actionAVM, SIGNAL(triggered(bool)), exportwcs, SLOT(exportAVM()));
	
	// And more signals ...
	connect(ui.actionCoordinate_Table, SIGNAL(triggered(bool)), coordinateTableDialog, SLOT(toggle()));
	
	// Mouse dependent rotation menu items
	rotateMenuItems(ui.graphicsView_1); // Set a default
	connect(ui.graphicsView_1, SIGNAL(mouseEnterEvent(GraphicsView*)), this, SLOT(rotateMenuItems(GraphicsView*)));
	connect(ui.graphicsView_2, SIGNAL(mouseEnterEvent(GraphicsView*)), this, SLOT(rotateMenuItems(GraphicsView*)));
	
	// Selecting corresponding items across scenes
	connect(fitsScene, SIGNAL(selectionChanged()), fitsScene, SLOT(findSelectedItem()));
	connect(fitsScene, SIGNAL(currentSelection(int)), epoScene, SLOT(matchSelectedItem(int)));
	connect(epoScene, SIGNAL(selectionChanged()), epoScene, SLOT(findSelectedItem()));
	connect(epoScene, SIGNAL(currentSelection(int)), fitsScene, SLOT(matchSelectedItem(int)));
	connect(fitsScene, SIGNAL(clearCorrespondingSelection()), epoScene, SLOT(clearSelection()));
	connect(epoScene, SIGNAL(clearCorrespondingSelection()), fitsScene, SLOT(clearSelection()));

	// Exporting signals and slots
	connect(exportwcs, SIGNAL(exportResults(bool)), this, SLOT(promptMessage(bool)));
	
	// Prediction, centroid and DS9 signal and slots
	connect(ui.actionFit_Point, SIGNAL(triggered(bool)), this, SLOT(predictEpoPoint()));
	connect(ui.actionCentroid, SIGNAL(triggered(bool)), fitsScene, SLOT(selectedItemPos()));
	connect(fitsScene, SIGNAL(itemPos(QPointF)), fitsImage, SLOT(getCentroid(QPointF)));
	connect(fitsImage, SIGNAL(centroid(QPointF)), this, SLOT(updateWithCentroid(QPointF)));
	connect(ui.actionOpen_in_DS9, SIGNAL(triggered()), this, SLOT(openDS9()));
	
	// Enable the teardown menu item
	ui.actionNew_Workspace->setEnabled(true);
	
	// Scan preference file to set the WCS format
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	bool wcsformat = settings.value("wcsformat", false).toBool();
	if (wcsformat) 
		ui.actionSexagesimal->trigger();
	else
		ui.actionDegrees->trigger();
	
	// TODO: Testing coordinate info panel by setting some markers for the M101 data
//	testI();
	
	return true;
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
	else if (action == ui.actionInverseHyperbolic_Sine_Stretch)
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


bool MainWindow::loadEpoImage(QString &filename)
{
	qDebug() << "Loading EPO image ...";
	epoImage = new EpoImage(filename);
	// TODO: Make same as fitsScene argument ...
	epoScene = new GraphicsScene(epoImage->pixmap, false);
	ui.graphicsView_2->setScene(epoScene);
	return true;
}


bool MainWindow::startFITSThread(QString &filename)
{
	qDebug() << "Initializing FITS thread ...";
	fitsThread = new FITSThread();
	connect(fitsThread, SIGNAL(finished()), this, SLOT(loadFITSImage()));
	fitsImage = new FitsImage(filename);
	fitsThread->setup(fitsImage);
	fitsThread->start();

	return true;
}


bool MainWindow::loadFITSImage()
{
	qDebug() << "Loading FITS Image ...";
	// Load the pixmap to the scene
	// TODO: Make epoScene the same as this ...
	fitsScene = new GraphicsScene(&(fitsImage->pixmap), true);
	ui.graphicsView_1->setScene(fitsScene);
	
	// Delete the FITS Thread
	disconnect(fitsThread, SIGNAL(finished()), this, SLOT(loadFITSImage()));
	delete fitsThread;
	
	// Finish setting up the workspace
	setupWorkspace();
	
	return true;
}

void MainWindow::buildHelpPanelMachine()
{
	// Initialize machine and states
	HelpPanelMachine = new QStateMachine;
	HelpPanelOn = new QState(HelpPanelMachine);
	HelpPanelOff = new QState(HelpPanelMachine);
	
	HelpPanelAnimationOn = new QPropertyAnimation(helpPanel, "pos");
	HelpPanelAnimationOff = new QPropertyAnimation(helpPanel, "pos");
	
	// Set the initial state of the machine
	HelpPanelMachine->setInitialState(HelpPanelOff);
	
	// Property for the on state
	HelpPanelOn->assignProperty(helpPanel, "pos", QPointF(0, 0));
	
	// Property for the off state
	HelpPanelOff->assignProperty(helpPanel, "pos", QPointF(0, -1*height()));
	
	// Set transition from the on state to the off state
	QAbstractTransition *t1 = HelpPanelOn->addTransition(ui.actionPinpointWCSHelp, SIGNAL(triggered()), HelpPanelOff);
	t1->addAnimation(HelpPanelAnimationOn);
	
	// Set transition from the off state to the one state
	QAbstractTransition *t2 = HelpPanelOff->addTransition(ui.actionPinpointWCSHelp, SIGNAL(triggered()), HelpPanelOn);
	t2->addAnimation(HelpPanelAnimationOff);
	
	// Start the machine
	HelpPanelMachine->start();
}

void MainWindow::buildWcsInfoPanelMachine()
{
	// Initialize machine and states
	WcsInfoPanelMachine = new QStateMachine;
	WcsInfoPanelOn = new QState(WcsInfoPanelMachine);
	WcsInfoPanelOff = new QState(WcsInfoPanelMachine);
	
	WcsFitsPanelOn = new QPropertyAnimation(fitsWcsInfoPanel, "pos");
	WcsEpoPanelOn = new QPropertyAnimation(epoWcsInfoPanel, "pos");
	WcsFitsPanelOff = new QPropertyAnimation(fitsWcsInfoPanel, "pos");
	WcsEpoPanelOff = new QPropertyAnimation(epoWcsInfoPanel, "pos");
	WcsFitsToolbarOn = new QPropertyAnimation(fitsToolbar, "pos");
	
	// Set the initial state of the machine
	WcsInfoPanelMachine->setInitialState(WcsInfoPanelOff);
	
	// Properties for the on state
	WcsInfoPanelOn->assignProperty(fitsWcsInfoPanel, "pos", QPointF(0, 0));
	WcsInfoPanelOn->assignProperty(epoWcsInfoPanel, "pos", QPointF(0, 0));
	WcsInfoPanelOn->assignProperty(fitsToolbar, "pos", QPointF(0, -1*fitsToolbar->height()));
	WcsInfoPanelOn->assignProperty(ui.actionImageAdjustments, "checked", false);
	
	// Properties for the off state
	WcsInfoPanelOff->assignProperty(fitsWcsInfoPanel, "pos", QPointF(0, -1*fitsWcsInfoPanel->height()));
	WcsInfoPanelOff->assignProperty(epoWcsInfoPanel, "pos", QPointF(0, -1*epoWcsInfoPanel->height()));
	
	// Set transition from on state to off state
	QAbstractTransition *t1 = WcsInfoPanelOn->addTransition(ui.actionWCSInfo, SIGNAL(toggled(bool)), WcsInfoPanelOff);
	t1->addAnimation(WcsFitsPanelOn);
	t1->addAnimation(WcsEpoPanelOn);
	
	// Set transition from off state to on state
	QAbstractTransition *t2 = WcsInfoPanelOff->addTransition(ui.actionWCSInfo, SIGNAL(toggled(bool)), WcsInfoPanelOn);
	t2->addAnimation(WcsFitsPanelOff);
	t2->addAnimation(WcsEpoPanelOff);
	t2->addAnimation(WcsFitsToolbarOn);
	
	// Start the machine
	WcsInfoPanelMachine->start();
}


void MainWindow::teardownWcsInfoPanelMachine()
{
	WcsInfoPanelMachine->stop();
	delete WcsFitsPanelOn;
	delete WcsEpoPanelOn;
	delete WcsFitsPanelOff;
	delete WcsEpoPanelOff;
	delete WcsFitsToolbarOn;
	
	delete WcsInfoPanelOn;
	delete WcsInfoPanelOff;
	delete WcsInfoPanelMachine;
}


void MainWindow::buildImageAdjustmentMachine()
{
	// Initialize machine and states
	imageAdjustmentMachine = new QStateMachine;
	imageAdjustmentPanelOn = new QState(imageAdjustmentMachine);
	imageAdjustmentPanelOff = new QState(imageAdjustmentMachine);
	
	ImgFitsToolbarOff = new QPropertyAnimation(fitsToolbar, "pos");
	ImgFitsToolbarOn = new QPropertyAnimation(fitsToolbar, "pos");
	ImgFitsPanelOn = new QPropertyAnimation(fitsWcsInfoPanel, "pos");
	ImgEpoPanelOn = new QPropertyAnimation(epoWcsInfoPanel, "pos");
	
	// Set initial state of the machine
	imageAdjustmentMachine->setInitialState(imageAdjustmentPanelOn);
	
	// Set properties for the on state - image adjustment panel on, wcs info panel off
	imageAdjustmentPanelOn->assignProperty(fitsToolbar, "pos", QPointF(0, 0));
	imageAdjustmentPanelOn->assignProperty(fitsWcsInfoPanel, "pos", QPointF(0, -1*fitsWcsInfoPanel->height()));
	imageAdjustmentPanelOn->assignProperty(epoWcsInfoPanel, "pos", QPointF(0, -1*epoWcsInfoPanel->height()));
	imageAdjustmentPanelOn->assignProperty(ui.actionWCSInfo, "checked", false);
	
	// Set properites for the off state - image adjustment panel off, wcs info panel off
	imageAdjustmentPanelOff->assignProperty(fitsToolbar, "pos", QPointF(0, -1*fitsToolbar->height()));
	
	// Set transition from on state to off state
	QAbstractTransition *t1 = imageAdjustmentPanelOn->addTransition(ui.actionImageAdjustments, SIGNAL(toggled(bool)), imageAdjustmentPanelOff);
	t1->addAnimation(ImgFitsToolbarOff);
	
	// Set transition from off state to on state
	QAbstractTransition *t2 = imageAdjustmentPanelOff->addTransition(ui.actionImageAdjustments, SIGNAL(toggled(bool)), imageAdjustmentPanelOn);
	t2->addAnimation(ImgFitsToolbarOn);
	t2->addAnimation(ImgFitsPanelOn);
	t2->addAnimation(ImgEpoPanelOn);
	
	// Start the machine
	imageAdjustmentMachine->start();
	
}


void MainWindow::teardownImageAdjustmentMachine()
{
	imageAdjustmentMachine->stop();
	delete ImgFitsToolbarOff;
	delete ImgFitsToolbarOn; 
	delete ImgFitsPanelOn;
	delete ImgEpoPanelOn;
	
	delete imageAdjustmentPanelOn;
	delete imageAdjustmentPanelOff;
	delete imageAdjustmentMachine;
}


void MainWindow::buildCoordPanelMachine()
{
	// Initialize machine and states
	CoordPanelMachine = new QStateMachine;
	CoordPanelOn = new QState(CoordPanelMachine);
	CoordPanelOff = new QState(CoordPanelMachine);
	
	CoordFitsPanelOff = new QPropertyAnimation(fitsCoordPanel, "pos");
	CoordEpoPanelOff = new QPropertyAnimation(epoCoordPanel, "pos");
	CoordFitsPanelOn = new QPropertyAnimation(fitsCoordPanel, "pos");
	CoordEpoPanelOn = new QPropertyAnimation(epoCoordPanel, "pos");
	
	// Set initial state of the machine
	CoordPanelMachine->setInitialState(CoordPanelOn);
	
	// Set properties
	updateCoordPanelProperties();
	
	// Set transition from the on state to the off state
	QAbstractTransition *t1 = CoordPanelOn->addTransition(ui.actionCoordinates, SIGNAL(toggled(bool)), CoordPanelOff);
	t1->addAnimation(CoordFitsPanelOff);
	t1->addAnimation(CoordEpoPanelOff);
	
	// Set transition from the off state to the on state
	QAbstractTransition *t2 = CoordPanelOff->addTransition(ui.actionCoordinates, SIGNAL(toggled(bool)), CoordPanelOn);
	t2->addAnimation(CoordFitsPanelOn);
	t2->addAnimation(CoordEpoPanelOn);
	
	// Start the machine
	CoordPanelMachine->start();
}


void MainWindow::teardownCoordPanelMachine()
{
	CoordPanelMachine->stop();
	
	delete CoordFitsPanelOff;
	delete CoordEpoPanelOff;
	delete CoordFitsPanelOn; 
	delete CoordEpoPanelOn;
	
	delete CoordPanelOn;
	delete CoordPanelOff;
	delete CoordPanelMachine;
}


void MainWindow::updateHelpPanelProperties()
{
	// Move panels
	helpPanel->move(0, 0);
	
	// Property for the on state
	HelpPanelOn->assignProperty(helpPanel, "pos", QPointF(0, 0));
	
	// Property for the off state
	HelpPanelOff->assignProperty(helpPanel, "pos", QPointF(0, -1*height()));
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
		ui.actionAVM->setEnabled(true);
		ui.actionFITS_Image->setEnabled(true);
		
		// Create EPO WCS object and load WCS to panel and export object
		epoImage->wcs = computewcs->initTargetWCS();
		epoWcsInfoPanel->loadWCS(epoImage->wcs, computewcs->rms_x, computewcs->rms_y);
		exportwcs->setWCS(epoImage->wcs);
	}
	else
	{
		// Disable export menu items
		ui.actionAVM->setEnabled(false);
		ui.actionFITS_Image->setEnabled(false);
		
		// Disable prediction of EPO coordinate
		ui.actionFit_Point->setEnabled(false);
		
		// Destroy EPO WCS object and clear WCS from panel
		epoImage->wcs = NULL;
		epoWcsInfoPanel->clear();
		exportwcs->clearWCS();
	}
	
	// Enable prediction of EPO coordinate if mapping exists
	if (computewcs->mappingExists && epoScene->clickable)
		ui.actionFit_Point->setEnabled(true);
	else
		ui.actionFit_Point->setEnabled(false);
		
}


void MainWindow::computeWCS()
{
	computewcs->computeTargetWCS();
	if (computewcs->epoWCS)
	{
		epoImage->wcs = computewcs->initTargetWCS();
		epoWcsInfoPanel->loadWCS(computewcs->initTargetWCS());
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
		// Disconnect slots
		disconnect(ui.actionRotate_Clockwise, SIGNAL(triggered()), ui.graphicsView_2, SLOT(rotateCW()));
		disconnect(ui.actionRotate_Counterclockwise, SIGNAL(triggered()), ui.graphicsView_2, SLOT(rotateCCW()));
		disconnect(ui.actionInvert, SIGNAL(triggered(bool)), epoImage, SLOT(invert()));
		
		// Connect slots
		connect(ui.actionRotate_Clockwise, SIGNAL(triggered()), ui.graphicsView_1, SLOT(rotateCW()));
		connect(ui.actionRotate_Counterclockwise, SIGNAL(triggered()), ui.graphicsView_1, SLOT(rotateCCW()));
		connect(ui.actionInvert, SIGNAL(triggered(bool)), fitsImage, SLOT(invert()));
	}
	else
	{		
		// Disconnect slots
		disconnect(ui.actionRotate_Clockwise, SIGNAL(triggered()), ui.graphicsView_1, SLOT(rotateCW()));
		disconnect(ui.actionRotate_Counterclockwise, SIGNAL(triggered()), ui.graphicsView_1, SLOT(rotateCCW()));
		disconnect(ui.actionInvert, SIGNAL(triggered(bool)), fitsImage, SLOT(invert()));
		
		// Connect slots
		connect(ui.actionRotate_Clockwise, SIGNAL(triggered()), ui.graphicsView_2, SLOT(rotateCW()));
		connect(ui.actionRotate_Counterclockwise, SIGNAL(triggered()), ui.graphicsView_2, SLOT(rotateCCW()));
		connect(ui.actionInvert, SIGNAL(triggered(bool)), epoImage, SLOT(invert()));
	}
}


void MainWindow::promptMessage(bool status)
{
	// Enable the menu item for DS9 interation
	if (exportwcs->fitsexport)
		ui.actionOpen_in_DS9->setEnabled(true);
	
	// Set the status of the message box
	msg->setStatus(status);
	msg->show();
}


void MainWindow::predictEpoPoint()
{	
	// Get the last reference point in the data model
	QPointF refCoord = dataModel->refCoords.last();
	// Send the predicted coordinate to the data model via commands
	dataModel->setData(epoScene, computewcs->fitsToEpo(&refCoord));
	
}


void MainWindow::updateWithCentroid(QPointF pos)
{
	CoordinateMarker *item = qgraphicsitem_cast<CoordinateMarker*>(fitsScene->selectedItems()[0]);
	dataModel->updateData(fitsScene, pos, item->pos());	
}


void MainWindow::openDS9()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	if (!settings.contains("ds9path"))
	{
		QString ds9path = QFileDialog::getOpenFileName(this, "Locate DS9", "/Applications", "Executables(*.app)");
		settings.setValue("ds9path", ds9path);
	}
	
	ds9thread = new DS9Thread(fitsImage->filename, exportwcs->saveas);
	ds9thread->start();
}

void MainWindow::closeDS9()
{
	qDebug() << "Closing DS9 ...";
	delete ds9thread;
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
	// Emit signal with size information
	QSize s = size();
	emit objectResized(s);
	
	// Call parent function
	QWidget::resizeEvent(event);
}


void MainWindow::testSlot()
{
	qDebug() << "Test Slot ";
}

void MainWindow::testI()
{
	// Automatically add some markers for the M101 data
	dataModel->setData(fitsScene, QPointF(394, 445));
	dataModel->setData(epoScene, QPointF(826, 461));
	
	dataModel->setData(fitsScene, QPointF(432, 367));
	dataModel->setData(epoScene, QPointF(904, 544));
	
	dataModel->setData(fitsScene, QPointF(543.84, 717.53));
	dataModel->setData(epoScene, QPointF(417.55, 513.86));
	
	dataModel->setData(fitsScene, QPointF(589.43, 718.44));
	dataModel->setData(epoScene, QPointF(394.76, 569.78));
}

// TESTING: Using EPO image derived from FITS image - M101
void MainWindow::testII()
{
	// Pixel should be the usual FITS pixels to EPO pixels transformation.
	dataModel->setData(fitsScene, QPointF(394, 445));
	dataModel->setData(epoScene, QPointF(394, 445));
	
	dataModel->setData(fitsScene, QPointF(432, 367));
	dataModel->setData(epoScene, QPointF(432, 367));
	
	dataModel->setData(fitsScene, QPointF(543.84, 717.53));
	dataModel->setData(epoScene, QPointF(543.84, 717.53));
	
	dataModel->setData(fitsScene, QPointF(589.43, 718.44));
	dataModel->setData(epoScene, QPointF(589.43, 718.44));
}


// TESTING: Using EPO image derived from FITS image - Butterfly Nebula
void MainWindow::testIII()
{
	dataModel->setData(fitsScene, QPointF(4155.51, 1051.50));
	dataModel->setData(epoScene, QPointF(4155.51, 1051.50));
	
	dataModel->setData(fitsScene, QPointF(3159.51, 3354.50));
	dataModel->setData(epoScene, QPointF(3159.51, 3354.50));

	dataModel->setData(fitsScene, QPointF(787.31, 2169.28));
	dataModel->setData(epoScene, QPointF(787.31, 2169.28));

	dataModel->setData(fitsScene, QPointF(778.51, 2690.50));
	dataModel->setData(epoScene, QPointF(778.51, 2690.50));
}