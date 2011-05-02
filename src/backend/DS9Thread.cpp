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

#include "DS9Thread.h"
#include <QDebug>
#include <QtCore>

DS9Thread::DS9Thread(QString s1, QString s2)
{
	orig = s1;
	exported = s2;
}

DS9Thread::~DS9Thread()
{
	delete process;
}

void DS9Thread::run()
{
	// Start DS9
	process = new QProcess();
//	qRegisterMetaType<QProcess::ProcessState>("QProcess::ProcessState");
//	qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");	
//	connect(process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(blah()));
//	connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(blah()));
	
	// First check if the location of DS9 is saved in the preference file
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	QString ds9path;
#if defined(Q_OS_MAC)
	ds9path = settings.value("ds9path", "/Applications/SAOImage DS9.app/Contents/MacOS/ds9").toString() + "/Contents/MacOS/ds9";
#endif
	
	qDebug() << ds9path;
	
	process->start("\""+ds9path+"\"");
	bool started = process->waitForStarted();
	
	if (started)
	{
		// Initialize some objects
		int got;
		char *names[NXPA];
		char *msgs[NXPA];
		
		// Initialize and open an XPA persistent connection
		XPA xpa;
		xpa = XPAOpen(NULL);
		
		// Determine if the XPA server is open
		int attempt = 0;
		while (attempt < 3)
		{
			if (XPAAccess(xpa, "ds9", "s", NULL, names, msgs, NXPA))
				break;
			qDebug() << "Sleeping ...";
			attempt++;
			this->sleep(3);
		}
		
		// Format the commands to be send to DS9
		QString s1 = "file " + orig;
		QString s2 = "file " + exported;
		
		char *orig, *exported;
		orig = new char[s1.toStdString().size()+1];
		exported = new char[s2.toStdString().size()+1];
		
		strcpy(orig, s1.toStdString().c_str());
		strcpy(exported, s2.toStdString().c_str());
		
		// Control DS9 baby!!!
		got = XPASet(xpa, "ds9", orig, "", NULL, 0, names, msgs, NXPA);
        got = XPASet(xpa, "ds9", "frame new", "", NULL, 0, names, msgs, NXPA);
        got = XPASet(xpa, "ds9", exported, "", NULL, 0, names, msgs, NXPA);
        got = XPASet(xpa, "ds9", "tile", "", NULL, 0, names, msgs, NXPA);
        got = XPASet(xpa, "ds9", "match frames wcs", "", NULL, 0, names, msgs, NXPA);
        got = XPASet(xpa, "ds9", "mode crosshair", "", NULL, 0, names, msgs, NXPA);
        got = XPASet(xpa, "ds9", "lock crosshair wcs", "", NULL, 0, names, msgs, NXPA);
		
		// Close the XPA persistent connection
		XPAClose(xpa);
		free(orig);
		free(exported);
	}
	else
	{
		// DS9 has not started, provide user with feedback
		qDebug() << "whoops something happened ...";
	}
}

void DS9Thread::blah()
{
	qDebug() << "blah blah blah";
}
