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

#ifndef DROPAREA_H
#define DROPAREA_H

#include <QLabel>
#include <QUrl>
#include <QDragEnterEvent>


class DropArea : public QLabel
{
	Q_OBJECT
public:
	DropArea(QWidget *parent = 0);
	~DropArea();
	void setFileExtensions(bool exts);
	bool ready;
	QString filepath;

protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dragLeaveEvent(QDragLeaveEvent *event);
	void dropEvent(QDropEvent *event);

private:
	QStringList extList;

signals:
	void readyForImport();
};

#endif
