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

#include "AboutDialog.h"
#include "version.h"

AboutDialog::AboutDialog(QWidget *parent)
: QDialog(parent, Qt::CustomizeWindowHint|Qt::WindowCloseButtonHint)
{
	// Set up user interface from the Designer file
	qDebug() << "Initializing About Dialog ...";
    ui.setupUi(this);
	
	// Get version and revision info from version.h
	version = new QString;
	version->sprintf("Version %s\n(rev %s)", VERSION, REVISION);
	ui.version->setText(*version);
	
	// Set the logo
	logo = new QPixmap;
	logo->load(":/gui/images/logo.png");
	ui.logoLabel->setPixmap(*logo);
}

AboutDialog::~AboutDialog() {}