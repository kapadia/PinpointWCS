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

#include <QDebug>
#include "RemoteData.h"


RemoteData::RemoteData()
{}

RemoteData::~RemoteData()
{}

void RemoteData::pingSimbad()
{
	qDebug() << "Pinging SIMBAD ...";
	
	// Generate URL
	QUrl q("http://simbad.u-strasbg.fr/simbad/sim-coo");
	q.addQueryItem("Coord", "210.801871+54.348181");
	q.addQueryItem("Radius", "1536.5808");
	q.addQueryItem("Radius.unit", "arcsec");
	q.addQueryItem("CooFrame", "ICRS");
	q.addQueryItem("CooEpoch", "2000");
	q.addQueryItem("CooEqui", "2000");
	q.addQueryItem("output.format", "VOTable");	
	q.addQueryItem("output.max", "2");
	q.addQueryItem("obj.cooN", "off");
	q.addQueryItem("list.cooN", "off");
	q.addQueryItem("obj.pmsel", "off");
	q.addQueryItem("obj.plxsel", "off");
	q.addQueryItem("obj.rvsel", "off");
	q.addQueryItem("obj.spsel", "off");
	q.addQueryItem("list.spsel", "off");
	q.addQueryItem("obj.mtsel", "off");
	q.addQueryItem("obj.sizesel", "off");
	q.addQueryItem("obj.fluxsel", "off");
	q.addQueryItem("list.fluxsel", "off");
	q.addQueryItem("list.idsel", "on");
	q.addQueryItem("obj.bibsel", "off");
	q.addQueryItem("list.bibsel", "off");
	q.addQueryItem("obj.messel", "off");
	q.addQueryItem("list.messel", "off");
	q.addQueryItem("obj.notesel", "off");
	q.addQueryItem("list.notesel", "off");
	
	qDebug() << q;
	manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(retreiveSIMBAD(QNetworkReply*)));
	manager->get(QNetworkRequest(q));
}

void RemoteData::retreiveSIMBAD(QNetworkReply* reply)
{
	qDebug() << "Retreiving from SIMBAD ...";
	QByteArray response(reply->readAll());
	qDebug() << response;
}