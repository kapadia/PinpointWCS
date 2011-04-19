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
#include <QPalette>

#include "WcsInfoPanel.h"

WcsInfoPanel::WcsInfoPanel(bool ref, QWidget *parent)
: QFrame(parent)
{
	// Set up user interface from the Designer file
	qDebug() <<  "Initializing WcsInfoPanel ...";
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
	
	reference = ref;
	// Specifics settings for FITS and EPO
	if (ref)
	{
		ui.misc2->clear();
	}
	else
	{
		ui.misc1->setText("Orientation:");
		ui.misc1_input2->clear();
	}
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
    ui.misc1->setFont(font);
	ui.misc2->setFont(font);
    ui.misc1_input1->setFont(font);
    ui.misc2_input1->setFont(font);
    ui.naxis2_input->setFont(font);
    ui.crval2_input->setFont(font);
    ui.crpix2_input->setFont(font);
    ui.misc1_input2->setFont(font);
    ui.misc2_input2->setFont(font);
    ui.ctype->setFont(font);
    ui.ctype1_input->setFont(font);
    ui.ctype2_input->setFont(font);
}


void WcsInfoPanel::loadWCS(struct WorldCoor* wcs, double rms_x, double rms_y)
{
	// Write some data to the WcsInfoPanels
	QString radesys = QString("%1").arg(wcs->radecsys);
	QString equinox = QString("%1").arg(wcs->equinox, 0, 'f', 1);
	QString ctype1 = QString("%1").arg(wcs->c1type);
	QString ctype2 = QString("%1").arg(wcs->c2type);
	QString naxis1 = QString("%1").arg(wcs->nxpix, 0, 'f', 2);
	QString naxis2 = QString("%1").arg(wcs->nypix, 0, 'f', 2);
	QString crval1 = QString("%1").arg(wcs->xref, 0, 'f', 2);
	QString crval2 = QString("%1").arg(wcs->yref, 0, 'f', 2);
	QString crpix1 = QString("%1").arg(wcs->xrefpix, 0, 'f', 2);
	QString crpix2 = QString("%1").arg(wcs->yrefpix, 0, 'f', 2);
	
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
	
	if (reference)
	{
		QString cd11 = QString("%1").arg(wcs->cd[0], 0, 'f', 6);
		QString cd12 = QString("%1").arg(wcs->cd[1], 0, 'f', 6);
		QString cd21 = QString("%1").arg(wcs->cd[2], 0, 'f', 6);
		QString cd22 = QString("%1").arg(wcs->cd[3], 0, 'f', 6);
		
		ui.misc1_input1->setText(cd11);
		ui.misc1_input2->setText(cd12);
		ui.misc2_input1->setText(cd21);
		ui.misc2_input2->setText(cd22);
	}
	else
	{
		QString orientation = QString("%1%2").arg((wcs->rot-360), 0, 'f', 6).arg(QChar(176));
		ui.misc1_input1->setText(orientation);
		if (rms_x != NULL)
		{
			QString rms1 = QString("%1 px").arg(rms_x, 0, 'f', 2);
			QString rms2 = QString("%1 px").arg(rms_y, 0, 'f', 2);
			ui.misc2_input1->setText(rms1);
			ui.misc2_input2->setText(rms2);
		}
	}
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
	ui.misc1_input1->setText("-");
	ui.misc2_input1->setText("-");
	ui.misc2_input2->setText("-");	
	if (reference)
		ui.misc1_input2->setText("-");

}


void WcsInfoPanel::parentResized(QSize sz)
{
	resize(sz.width(), 60);
	qDebug() << sz.width();
	float fontsize = 1./80. * (sz.width() - 390) + 7;
	QFont font;
	font.setPointSize(fontsize);
	updateFontSize(font);
}