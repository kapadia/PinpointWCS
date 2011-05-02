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

#ifndef DS9THREAD_H
#define DS9THREAD_H

#include <QThread>
#include <QProcess>
#include <QSettings>
#include <QFileDialog>

#include <xpa.h>

QT_BEGIN_NAMESPACE
//class QProcess;
QT_END_NAMESPACE

#define NXPA 1

class DS9Thread : public QThread {
	
	Q_OBJECT
	
public:
	DS9Thread(QString s1, QString s2);
	~DS9Thread();
	
public slots:
	void blah();
	
protected:
	void run();
	
private:
	QProcess *process;
	QString orig;
	QString exported;
};

#endif