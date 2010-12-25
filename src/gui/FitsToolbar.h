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

#ifndef FITS_TOOLBAR_H
#define FITS_TOOLBAR_H

#include <QFrame>
#include "ui_FitsToolbar.h"

class FitsToolbar : public QFrame
{

	Q_OBJECT
		
public:
	FitsToolbar(QWidget *parent = 0);
	~FitsToolbar();
	Ui::FitsToolbar ui;
	void setExtremals(float min, float max);
	void setSliderValues(float vmin, float vmax);
		
public slots:
	void parentResized(QSize sz);
	void vminSliderReleased();
	void vmaxSliderReleased();

signals:
	void updateVmin(float value);
	void updateVmax(float value);
	
private:
	float minimum, maximum;
};

#endif