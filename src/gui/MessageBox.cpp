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
#include "MessageBox.h"


MessageBox::MessageBox(const QString &title, bool status, QWidget *parent, Qt::WindowFlags f)
: QMessageBox(QMessageBox::NoIcon, title, "", QMessageBox::Ok, parent, f)
{
	icon = new QPixmap;
	if (status)
	{
		setText("Cool stuff just happened.  Celestial Coordinates Exported Successfully!");
		icon->load(":/gui/images/good.png");
	}
	else 
	{
		setText("Uh Oh! Spaghetti O's.  Check that:<p><ul><li>The image is in the same directory as when imported</li><li>The image is not locked</li><li>The image has not been deleted</li><li>The astronomical object hasn't disappeared into the nether regions of the universe.</li></p>");
		icon->load(":/gui/images/bad.png");	
	}
	setIconPixmap(*icon);
	setWindowModality(Qt::ApplicationModal);
	qDebug() << "ButtonRole:\t" << buttonRole(buttons()[0]);
}

MessageBox::~MessageBox()
{}

void MessageBox::closeEvent(QCloseEvent *event)
{
	qDebug() << "closeEvent";
	this->~MessageBox();
}