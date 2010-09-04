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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>

#include "ui_PinpointWCS.h"
#include "FitsImage.h"
#include "EpoImage.h"
#include "GraphicsScene.h"
#include "CoordMarker.h"
#include "WcsInfoPanel.h"
#include "CoordinatePanel.h"
#include "FitsToolbar.h"
#include "CoordinateModel.h"
#include "CoordinateDelegate.h"
#include "ComputeWCS.h"
#include "ExportWCS.h"

 
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	// Constructor and Deconstructor
	MainWindow();
	~MainWindow();

private:
	// User Interface Attributes
	Ui::MainWindow ui;
	GraphicsScene *fitsScene;
	GraphicsScene *epoScene;
	WcsInfoPanel *fitsWcsInfoPanel;
	WcsInfoPanel *epoWcsInfoPanel;
	CoordinatePanel *fitsCoordPanel;
	CoordinatePanel *epoCoordPanel;
	FitsToolbar *fitsToolbar;
	
	// Image Attributes
	FitsImage *fitsImage;
	EpoImage *epoImage;
	
	// Undo and Redo Attributes
	QUndoStack *undoStack;
	QAction *undoAction;
	QAction *redoAction;
	
	// Model, View, and Delegate Attributes
	CoordinateModel *dataModel;
	QTableView *tableView;
	CoordinateDelegate *tableDelegate;
	
	// Compute and Export Attributes
	ComputeWCS *computewcs;
	ExportWCS *exportwcs;
	
	// State Machines Attributes
	QStateMachine *WcsInfoPanelMachine;
	QState *WcsInfoPanelOn;
	QState *WcsInfoPanelOff;
	
	QStateMachine *CoordPanelMachine;
	QState *CoordPanelOn;
	QState *CoordPanelOff;
	
	QStateMachine *imageAdjustmentMachine;
	QState *imageAdjustmentPanelOn;
	QState *imageAdjustmentPanelOff;
	
	// State Machine Methods
	void buildWcsInfoPanelMachine();
	void buildCoordPanelMachine();
	void buildImageAdjustmentMachine();
	
private slots:
	bool loadImages();
	bool loadEpoImage(QString& filename);
	bool loadFitsImage(QString& filename);
	void addMarker(GraphicsScene *scene, QPointF pos);
	void updateCoordPanelProperties();
	void updateFitsCoordinates(QPointF pos);
	void updateEpoCoordinates(QPointF pos);
	void itemMoved(CoordMarker *movedItem, const QPointF &moveStartPosition);
	void enableExport();
	void computeWCS();
	
	// A slot for testing ...
	void testSlot();
};


#endif