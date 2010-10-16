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

#include <QApplication>
#include "mainwindow.h"

// TODO: Program crashes when undo-ing many markers.  Problem seems to be with the QList associated with refCoords of the data model.
// TODO: Pass *QPointFs not QPointFs
// TODO: Quadratic and Cubic mappings (fix the math)
// TODO: Finish XMP/AVM export.  There are some fields that are not enabled for export.


int main(int argc, char *argv[])
{
	// Initialize application
	QApplication app(argc, argv);
	app.setOrganizationName("Smithsonian Astrophysical Observatory");
	app.setApplicationName("PinpointWCS");
	
	/*
	// Set style sheet
    QFile styleSheet(":/gui/style.qss");
    if (!styleSheet.open(QIODevice::ReadOnly)) {
        qWarning("Unable to open :/gui/style.qss");
    }
    qApp->setStyleSheet(styleSheet.readAll());
	 */
	
	// Set up main window
	MainWindow mainwindow;
	mainwindow.show();
	
	return app.exec();
}