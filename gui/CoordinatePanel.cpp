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
#include <QPalette>
#include "CoordinatePanel.h"

CoordinatePanel::CoordinatePanel(PPWcsImage *im, QWidget *parent)
: QFrame(parent)
{
	// Setup user interface
    ui.setupUi(this);
	
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
	// Initialize variables
	QString x;
	QString y;
	double *world = NULL;

	// Format and display xy coordinates
	x.sprintf("%.2f", pos.x());
	y.sprintf("%.2f", pos.y());
	ui.x_value->setText(x);
	ui.y_value->setText(y);
	
	
	// Check if WCS exists
	if (image->wcs)
	{
		world = image->pix2sky(pos);
		
		// Initialize some variables for a conversion
//		int H1, M1, H2, M2;
//		double S1, S2;
		
		// Convert from decimal to sexagesimal
//		H1 = floor(world[0]/360 * 24);
//		M1 = floor( (world[0]/360 * 24 - H1) * 60);
//		S1 = ((world[0]/360 * 24 - H1) * 60 - floor((world[0]/360 * 24 - H1) * 60)) * 60;
		
//		H2 = floor(abs(world[1])) * world[1]/abs(world[1]);
//		M2 = abs(floor((world[1] - H2) * 60));
//		S2 = abs(((world[1] - H2) * 60 - floor((world[1] - H2) * 60)) * 60);
		
		QString ra;
		QString dec;
		ra.sprintf("%.8f", world[0]);
		dec.sprintf("%.8f", world[1]);
//		ra.sprintf("%02d::%02d::%06.3f", H1, M1, S1);
//		dec.sprintf("%02d::%02d::%06.3f", H2, M2, S2);
		ui.ra_value->setText(ra);
		ui.dec_value->setText(dec);
	}
}