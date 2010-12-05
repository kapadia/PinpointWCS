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

#include "WcsInfoPanel.h"

WcsInfoPanel::WcsInfoPanel(QWidget *parent)
: QFrame(parent)
{
	// Set up user interface from the Designer file
	std::cout << "Initializing WcsInfoPanel ...\n";
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
	font.setPointSize(11);
	updateFontSize(font);
}

WcsInfoPanel::~WcsInfoPanel() {}

void WcsInfoPanel::updateFontSize(QFont font)
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


void WcsInfoPanel::loadWCS(struct WorldCoor wcs)
{
	// Write some data to the WcsInfoPanels
	QString radesys;
	QString equinox;
	QString ctype1;
	QString ctype2;
	QString naxis1;
	QString naxis2;
	QString crval1;
	QString crval2;
	QString crpix1;
	QString crpix2;
	QString cd11;
	QString cd12;
	QString cd21;
	QString cd22;
	
	radesys.sprintf("%s", wcs.radecsys);
	equinox.sprintf("%.1f", wcs.equinox);
	ctype1.sprintf("%s", wcs.c1type);
	ctype2.sprintf("%s", wcs.c2type);
	naxis1.sprintf("%.2f", wcs.nxpix);
	naxis2.sprintf("%.2f", wcs.nypix);
	crval1.sprintf("%.2f", wcs.xref);
	crval2.sprintf("%.2f", wcs.yref);
	crpix1.sprintf("%.2f", wcs.xrefpix);
	crpix2.sprintf("%.2f", wcs.yrefpix);
	cd11.sprintf("%.6f", wcs.cd[0]);
	cd12.sprintf("%.6f", wcs.cd[1]);
	cd21.sprintf("%.6f", wcs.cd[2]);
	cd22.sprintf("%.6f", wcs.cd[3]);
	
	ui.radesys_input->setText(radesys);
	ui.equinox_input->setText(equinox);
	ui.ctype1_input->setText(ctype1);
	ui.ctype2_input->setText(ctype2);
	ui.naxis1_input->setText(naxis1);
	ui.naxis2_input->setText(naxis2);
	ui.crval1_input->setText(crval1);
	ui.crval2_input->setText(crval2);
	ui.crpix1_input->setText(crpix1);
	ui.crpix2_input->setText(crpix2);
	ui.cd_11_input->setText(cd11);
	ui.cd_12_input->setText(cd12);
	ui.cd_21_input->setText(cd21);
	ui.cd_22_input->setText(cd22);
}


void WcsInfoPanel::clear()
{
	ui.radesys_input->setText("-");
	ui.equinox_input->setText("-");
	ui.ctype1_input->setText("-");
	ui.ctype2_input->setText("-");
	ui.naxis1_input->setText("-");
	ui.naxis2_input->setText("-");
	ui.crval1_input->setText("-");
	ui.crval2_input->setText("-");
	ui.crpix1_input->setText("-");
	ui.crpix2_input->setText("-");
	ui.cd_11_input->setText("-");
	ui.cd_12_input->setText("-");
	ui.cd_21_input->setText("-");
	ui.cd_22_input->setText("-");
}


void WcsInfoPanel::parentResized(QSize sz)
{
	resize(sz.width(), 55);
	qDebug() << sz.width();
	float fontsize = 1./80. * (sz.width() - 390) + 7;
	QFont font;
	font.setPointSize(fontsize);
	updateFontSize(font);
}