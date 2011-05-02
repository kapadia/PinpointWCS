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

#include <QMainWindow>
#include <QStateMachine>
#include <QPropertyAnimation>
#include <QAbstractTransition>
#include <QSignalTransition>
#include <QActionGroup>
#include <QSettings>
//#include <QtNetwork>

#include "ui_PinpointWCS.h"
#include "FitsImage.h"
#include "EpoImage.h"
#include "GraphicsScene.h"
#include "CoordinateMarker.h"
#include "WcsInfoPanel.h"
#include "CoordinatePanel.h"
#include "FitsToolbar.h"
#include "CoordinateModel.h"
#include "CoordinateTableDialog.h"
#include "CoordinateDelegate.h"
#include "ComputeWCS.h"
#include "ExportWCS.h"
#include "AboutDialog.h"
#include "MessageBox.h"
#include "FITSThread.h"
#include "DS9Thread.h"
#include "HelpPanel.h"
 
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
	QActionGroup *stretchActionGroup;
	QActionGroup *wcsFormatActionGroup;
	MessageBox *msg;
	HelpPanel *helpPanel;
	
	// Image Attributes
	FitsImage *fitsImage;
	EpoImage *epoImage;
	FITSThread *fitsThread;
	
	// XPA Attributes
	DS9Thread *ds9thread;
	
	// Undo and Redo Attributes
	QAction *undoAction;
	QAction *redoAction;
	
	// Model, View, and Delegate Attributes
	CoordinateModel *dataModel;
	CoordinateTableDialog *coordinateTableDialog;
	CoordinateDelegate *tableDelegate;
	
	// Compute and Export Attributes
	ComputeWCS *computewcs;
	ExportWCS *exportwcs;
	
	// State Machines Attributes
	QStateMachine *HelpPanelMachine;
	QState *HelpPanelOn;
	QState *HelpPanelOff;
	
	QStateMachine *WcsInfoPanelMachine;
	QState *WcsInfoPanelOn;
	QState *WcsInfoPanelOff;
	
	QStateMachine *CoordPanelMachine;
	QState *CoordPanelOn;
	QState *CoordPanelOff;
	
	QStateMachine *imageAdjustmentMachine;
	QState *imageAdjustmentPanelOn;
	QState *imageAdjustmentPanelOff;
	
	// Animation Attributes
	QPropertyAnimation *WcsFitsPanelOn;
	QPropertyAnimation *WcsFitsPanelOff;
	QPropertyAnimation *WcsEpoPanelOn;
	QPropertyAnimation *WcsEpoPanelOff;
	QPropertyAnimation *WcsFitsToolbarOn;
	
	QPropertyAnimation *ImgFitsToolbarOff;
	QPropertyAnimation *ImgFitsToolbarOn; 
	QPropertyAnimation *ImgFitsPanelOn;
	QPropertyAnimation *ImgEpoPanelOn;
	
	QPropertyAnimation *CoordFitsPanelOff;
	QPropertyAnimation *CoordEpoPanelOff;
	QPropertyAnimation *CoordFitsPanelOn;
	QPropertyAnimation *CoordEpoPanelOn;
	
	QPropertyAnimation *HelpPanelAnimationOn;
	QPropertyAnimation *HelpPanelAnimationOff;
	
	// Dialogs
	AboutDialog *aboutDialog;
	
	// State Machine Methods
	void buildHelpPanelMachine();
	void buildWcsInfoPanelMachine();
	void buildCoordPanelMachine();
	void buildImageAdjustmentMachine();
	void teardownWcsInfoPanelMachine();
	void teardownCoordPanelMachine();
	void teardownImageAdjustmentMachine();
	void resizeEvent(QResizeEvent *event);
	
private slots:
	bool setupImages();
	bool setupWorkspace();
	bool teardownWorkspace();
	bool startFITSThread(QString& filename);
	bool loadFITSImage();
	bool loadEpoImage(QString& filename);
	void stretch(QAction *action);
	void updateCoordPanelProperties();
	void updateHelpPanelProperties();
	void enableExport();
	void computeWCS();
	void rotateMenuItems(GraphicsView *gv);
	void promptMessage(bool status);
	void predictEpoPoint();
	void updateWithCentroid(QPointF pos);
	void openDS9();
	void closeDS9();
	
	// A slots for testing ...
	void testSlot();
	
	// Test function(s) for debugging
	void testI();
	void testII();
	void testIII();
	
signals:
	void objectResized(QSize s);
};


#endif