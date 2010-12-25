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

#ifndef COORDINATE_TABLE
#define COORDINATE_TABLE

#include <QDialog>
#include "ui_CoordinateTableDialog.h"

class CoordinateTableDialog : public QDialog
{
	
	Q_OBJECT
	
public:
	CoordinateTableDialog(QWidget *parent = 0);
	~CoordinateTableDialog();
	Ui::CoordinateTableDialog ui;
};

#endif