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

#ifndef COORDINATE_PANEL_H
#define COORDINATE_PANEL_H

#include <QFrame>
#include <QTextStream>
#include <QSettings>
#include "PPWcsImage.h"
#include "ui_CoordinatePanel.h"

class CoordinatePanel : public QFrame
	{
		Q_OBJECT
		
	public:
		CoordinatePanel(PPWcsImage *im, QWidget *parent = 0);
		~CoordinatePanel();
		Ui::CoordinatePanel ui;
		
	public slots:
		void parentResized(QSize sz);
		void setWcsFormat(bool format);
		void updateCoordinates(QPointF pos);
		
	private:
		// Attributes
		PPWcsImage *image;
		bool wcsFormat;
		
		// Methods
		void updateFontSize(QFont font);
	};

#endif