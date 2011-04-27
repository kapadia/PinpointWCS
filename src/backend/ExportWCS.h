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


#ifndef EXPORTWCS_H
#define EXPORTWCS_H

#include <QPixmap>
#include <QString>
#include <QFileDialog>
#include "wcs.h"
#include "EpoImage.h"
#include "ComputeWCS.h"

#define AVM_VERSION "1.2"

class ExportWCS : public QObject {
	
	Q_OBJECT
	
public:
	ExportWCS(QString *f, QPixmap *p, ComputeWCS *cwcs);
	~ExportWCS();
	QString saveas;
	bool fitsexport;
	
	void setWCS(struct WorldCoor *w);
	void clearWCS();
	
public slots:
	void exportFITS();
	void exportAVM();
	
signals:
	void exportResults(bool success);
	
private:
	// Attributes
	QPixmap *pixmap;
	QString *filename;
	struct WorldCoor *wcs;
	ComputeWCS *computewcs;
};

#endif

