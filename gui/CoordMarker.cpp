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

#include <iostream>
#include <QtGui>
#include <CoordMarker.h>


CoordMarker::CoordMarker()
{
	qDebug() << "Initializing CoordMarker object ...";
	setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable|QGraphicsItem::ItemIsFocusable);
	setZValue(2);
	setOpacity(100.0);
	setSelected(true);
	setFocus();
	setFlag(ItemSendsScenePositionChanges, true);
	setFlag(ItemSendsGeometryChanges, true);
}

CoordMarker::~CoordMarker() {}

QRectF CoordMarker::boundingRect() const
{
	qreal penWidth = 1;
	return QRectF(-10 - penWidth / 2, -10 - penWidth / 2, 20 + penWidth, 20 + penWidth);
}

void CoordMarker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{	
	// Set up pen and brush
	QPen pen;
	pen.setWidthF(1);
	painter->setPen(pen);	
	
	// Draw an slightly transparent circle and crosshair
	painter->setBrush(Qt::red);
//	painter->setOpacity(0.4);
	painter->drawEllipse(-10, -10, 20, 20);
	
//	painter->setOpacity(1.0);
	painter->setBrush(Qt::black);
	painter->drawLine(0, -18, 0, 18);
	painter->drawLine(-18, 0, 18, 0);
	
}