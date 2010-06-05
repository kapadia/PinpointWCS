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
	
	// Set up the DropSites to accept the correct extensions
	ui.label->setFileExtensions(false);
	ui.label_2->setFileExtensions(true);
	
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
	connect(ui.label, SIGNAL(readyForImport()), this, SLOT(loadImages()));
	connect(ui.label_2, SIGNAL(readyForImport()), this, SLOT(loadImages()));
	
}


bool MainWindow::loadImages()
{
	std::cout << "Attempting to load files to workspace ... \n";
	if (ui.label->ready and ui.label_2->ready) {
		std::cout << "Both DropAreas are ready!\n";
		
		// Call loadEpoImage and loadFitsImage
		if (!loadEpoImage(ui.label_2->filepath))
		{
			std::cout << "Loading of EPO image failed ...\n";
			return false;
		}
		if (!loadFitsImage(ui.label->filepath))
		{
			std::cout << "Loading of FITS image failed ...\n";
			return false;
		}
		
		// Flip the stacked widgets
		ui.stackedWidget_1->setCurrentIndex(1);
		ui.stackedWidget_2->setCurrentIndex(1);
		
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
	ui.graphicsView->setup(*pixmap, true);
	
	return true;
}
