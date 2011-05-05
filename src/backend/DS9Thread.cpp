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
	
	// First check if the location of DS9 is saved in the preference file
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	QString ds9path;
#if defined(Q_OS_MAC)
	ds9path = settings.value("ds9path", "/Applications/SAOImage DS9.app/Contents/MacOS/ds9").toString() + "/Contents/MacOS/ds9";
#endif
	
	qDebug() << ds9path;
	
	// Determine if any XPA connections to DS9 are open
	char **classes;
	char **names;
	char **methods;
	char **infos;
	XPA xpa;
	xpa = XPAOpen(NULL);
	int xpaconns = XPANSLookup(xpa, "ds9*", "s", &classes, &names, &methods, &infos);
	
	// Free some memory
	for (int i=0; i<xpaconns; i++)
	{
		free(classes[i]);
		free(names[i]);
		free(methods[i]);
		free(infos[i]);
	}
	
	// Start up own instance of DS9
	process->start("\""+ds9path+"\"");
	bool started = process->waitForStarted();
	
	if (started)
	{
		// Determine the number of available XPA connections to DS9
		int n = XPANSLookup(xpa, "ds9*", "s", &classes, &names, &methods, &infos);
		
		// Determine if the new XPA connection is open
		int attempt = 0;
		while (attempt < 3)
		{
			n = XPANSLookup(xpa, "ds9*", "s", &classes, &names, &methods, &infos);
			if (n > xpaconns)
				break;
			qDebug() << "Sleeping ...";
			attempt++;
			this->sleep(3);
		}
		
		if (attempt == 3)
		{
			qDebug() << "whoops something happened ...";
			return;
		}
		
		// Get the name of the template before freeing the memory
		char *method;
		method = new char[14];
		strcpy(method, methods[n-1]);
		
		// Free some memory
		for (int i=0; i<n; i++)
		{
			free(classes[i]);
			free(names[i]);
			free(methods[i]);
			free(infos[i]);
		}
		free(classes);
		free(names);
		free(methods);
		free(infos);
		
		// There exists at least one connection
		int got;
		char *nombres[NXPA];
		char *msgs[NXPA];
		
		// Format the commands to be send to DS9
		QString s1 = "file " + orig;
		QString s2 = "file " + exported;
		
		char *orig, *exported;
		orig = new char[s1.toStdString().size()+1];
		exported = new char[s2.toStdString().size()+1];
		
		strcpy(orig, s1.toStdString().c_str());
		strcpy(exported, s2.toStdString().c_str());
		
		// Control DS9 baby!!!
		got = XPASet(xpa, method, orig, "", NULL, 0, nombres, msgs, NXPA);
        got = XPASet(xpa, method, "frame new", "", NULL, 0, nombres, msgs, NXPA);
        got = XPASet(xpa, method, exported, "", NULL, 0, nombres, msgs, NXPA);
        got = XPASet(xpa, method, "tile", "", NULL, 0, nombres, msgs, NXPA);
        got = XPASet(xpa, method, "match frames wcs", "", NULL, 0, nombres, msgs, NXPA);
        got = XPASet(xpa, method, "mode crosshair", "", NULL, 0, nombres, msgs, NXPA);
        got = XPASet(xpa, method, "lock crosshair wcs", "", NULL, 0, nombres, msgs, NXPA);
		
		// Close the XPA persistent connection
		XPAClose(xpa);
		free(orig);
		free(exported);
		free(method);
	}
	else
	{
		// DS9 has not started, provide user with feedback
		qDebug() << "whoops something happened ...";
	}
}