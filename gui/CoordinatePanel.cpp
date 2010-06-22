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
#include <QPalette>
#include "CoordinatePanel.h"

CoordinatePanel::CoordinatePanel(QWidget *parent)
: QFrame(parent)
{
	// Set up user interface from the Designer file
	std::cout << "Initializing CoordinatePanel ...\n";
    ui.setupUi(this);
	
	// Set up frame style
	setFrameStyle(QFrame::Panel | QFrame::StyledPanel);
	
	// Set up colors
	setAutoFillBackground(true);
	setBackgroundRole(QPalette::Window);
	QPalette palette;
	QColor bgcolor = QColor(0, 0, 0, 100);
	QColor textcolor = QColor(230, 230, 230);
//	QColor textcolor = QColor(237, 28, 36);
	palette.setColor(QPalette::Background, bgcolor);
	palette.setColor(QPalette::WindowText, textcolor);
	setPalette(palette);
	
	// Resize font
	QFont font;
	font.setPointSize(10);
	updateFontSize(font);
}

CoordinatePanel::~CoordinatePanel() {}

void CoordinatePanel::updateFontSize(QFont font)
{
	ui.x->setFont(font);
	ui.x_value->setFont(font);
	ui.y->setFont(font);
	ui.y_value->setFont(font);
	ui.ra->setFont(font);
	ui.ra_value->setFont(font);
	ui.dec->setFont(font);
	ui.dec_value->setFont(font);
}

void CoordinatePanel::parentResized(QSize sz)
{
	resize(sz.width(), 30);
}

void CoordinatePanel::updateCoordinates(QPointF pos)
{
	QString x;
	QString y;
	
	x.sprintf("%.2f", pos.x());
	y.sprintf("%.2f", pos.y());
	ui.x_value->setText(x);
	ui.y_value->setText(y);
}