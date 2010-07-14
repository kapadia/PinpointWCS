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
#include "WcsInfoPanel.h"
#include "CoordinatePanel.h"
#include "FitsToolbar.h"

 
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	// Methods
	MainWindow();
	~MainWindow();

private:
	// Attributes
	Ui::MainWindow ui;
	FitsImage *fitsImage;
	EpoImage *epoImage;
	GraphicsScene *fitsScene;
	GraphicsScene *epoScene;
	WcsInfoPanel *fitsWcsInfoPanel;
	WcsInfoPanel *epoWcsInfoPanel;
	CoordinatePanel *fitsCoordPanel;
	CoordinatePanel *epoCoordPanel;
	FitsToolbar *fitsToolbar;
	QUndoStack *undoStack;
	QAction *undoAction;
	QAction *redoAction;
	
	// State Machines
	QStateMachine *WcsInfoPanelMachine;
	QState *WcsInfoPanelOn;
	QState *WcsInfoPanelOff;
	
	QStateMachine *CoordPanelMachine;
	QState *CoordPanelOn;
	QState *CoordPanelOff;
	
	QStateMachine *imageAdjustmentMachine;
	QState *imageAdjustmentPanelOn;
	QState *imageAdjustmentPanelOff;
	
	// Methods
	void buildWcsInfoPanelMachine();
	void buildCoordPanelMachine();
	void buildImageAdjustmentMachine();
	
private slots:
	bool loadImages();
	bool loadEpoImage(QString& filename);
	bool loadFitsImage(QString& filename);
	void updateCoordPanelProperties();
	void updateFitsCoordinates(QPointF pos);
	void updateEpoCoordinates(QPointF pos);
	void addFitsMarker(QPointF pos);
	void addEpoMarker(QPointF pos);
	void testSlot();
};


#endif