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
	ui.radesys->setFont(font);
    ui.radesys_input->setFont(font);
    ui.equinox->setFont(font);
    ui.equinox_input->setFont(font);
    ui.naxis->setFont(font);
    ui.naxis1_input->setFont(font);
    ui.crval->setFont(font);
    ui.crval1_input->setFont(font);
    ui.crpix->setFont(font);
    ui.crpix1_input->setFont(font);
    ui.cd->setFont(font);
    ui.cd_11_input->setFont(font);
    ui.cd_21_input->setFont(font);
    ui.naxis2_input->setFont(font);
    ui.crval2_input->setFont(font);
    ui.crpix2_input->setFont(font);
    ui.cd_12_input->setFont(font);
    ui.cd_22_input->setFont(font);
    ui.ctype->setFont(font);
    ui.ctype1_input->setFont(font);
    ui.ctype2_input->setFont(font);
}

void CoordinatePanel::parentResized(QSize sz)
{
	setGeometry(0, 0.75*sz.height(), sz.width(), sz.height()-0.75*sz.height());
}