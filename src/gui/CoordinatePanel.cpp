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

#include <math.h>
#include "CoordinatePanel.h"
#include <QDebug>

CoordinatePanel::CoordinatePanel(PPWcsImage *im, QWidget *parent)
: QFrame(parent)
{
	// Setup user interface
    ui.setupUi(this);
	wcsFormat = false; // true for sexagesimal, false for degrees
	
	// Set associated image
	image = im;
	
	// Set frame style
	setFrameStyle(QFrame::Panel | QFrame::StyledPanel);
	
	// Set colors
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
	xs = new QTextStream();
	xs->setRealNumberNotation(QTextStream::FixedNotation);
	xs->setRealNumberPrecision(2);
	xs->setFieldWidth(7);
	xs->setFieldAlignment(QTextStream::AlignLeft);
	ys = new QTextStream();
	ys->setRealNumberNotation(QTextStream::FixedNotation);
	ys->setRealNumberPrecision(2);
	ys->setFieldWidth(7);
	ys->setFieldAlignment(QTextStream::AlignLeft);
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


void CoordinatePanel::setWcsFormat(bool format)
{
	wcsFormat = !format;
}

void CoordinatePanel::updateCoordinates(QPointF pos)
{
	// Initialize variables
	QString x;
	QString y;
	double *world = NULL;

	// Format and display xy coordinates
	xs->setString(&x);
	*xs << pos.x();
	ys->setString(&y);
	*ys << pos.y();
	
	ui.x_value->setText(x);
	ui.y_value->setText(y);
	
	
	// Check if WCS exists
	if (image->wcs)
	{
		world = image->pix2sky(pos);
		
		if (wcsFormat)
		{
			char rastr[32], decstr[32];
			ra2str(rastr, 31, world[0], 3);
			dec2str(decstr, 31, world[1], 2);
				
			// Set the text
			ui.ra_value->setText(rastr);
			ui.dec_value->setText(decstr);
		}
		else {
			QString rastr;
			QString decstr;
			rastr.sprintf("%.8f", world[0]);
			decstr.sprintf("%.8f", world[1]);
			
			// Set the text
			ui.ra_value->setText(rastr);
			ui.dec_value->setText(decstr);
		}
	}
	else
	{
		ui.ra_value->setText("-");
		ui.dec_value->setText("-");
	}
}