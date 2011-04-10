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
#include <time.h>

// TODO: Pass *QPointFs not QPointFs
// FIXME: Something unusual when marking new point, undoing, then predicting.

int main(int argc, char *argv[])
{
	/*
	time_t seconds;
	seconds = time(NULL);
	std::cout << seconds << std::endl;
	*/
	
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