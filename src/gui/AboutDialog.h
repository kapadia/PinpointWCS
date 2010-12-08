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

#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include <QDialog>
#include "ui_AboutDialog.h"

QT_BEGIN_NAMESPACE
class QString;
class QPixmap;
QT_END_NAMESPACE

class AboutDialog : public QDialog
{
	Q_OBJECT
	
public:
	AboutDialog(QWidget *parent = 0);
	~AboutDialog();
	Ui::AboutDialog ui;
	
private:
	QString *version;
	QPixmap *logo;
	
};

#endif