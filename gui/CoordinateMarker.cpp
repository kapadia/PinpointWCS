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

#include <QtGui>
#include <CoordinateMarker.h>
#include "GraphicsScene.h"

CoordinateMarker::CoordinateMarker(QGraphicsItem *parent)
: QGraphicsItem(parent)
{
	qDebug() << "Initializing CoordinateMarker object ...";
	radius = 1;
	row = NULL;
	setZValue(2);
	setOpacity(100.0);
	setSelected(true);
	setFocus();
	setEnabled(true);
	
	// Set some flags
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsFocusable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
}


CoordinateMarker::CoordinateMarker(float r, QGraphicsItem *parent)
: QGraphicsItem(parent)
{
	qDebug() << "Initializing CoordinateMarker object ...";
	radius = r;
	row = NULL;
	setZValue(2);
	setOpacity(100.0);
	setSelected(true);
	setFocus();
	setEnabled(true);
	
	// Set some flags
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsFocusable, true);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
	
}

CoordinateMarker::~CoordinateMarker() {}

QRectF CoordinateMarker::boundingRect() const
{
	float length = radius/30. + radius;
	return QRectF(-1*length, -1*length, 2*length, 2*length);
}

void CoordinateMarker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{	
	QPen pen;
	
	// Compute width based on zoom factor of graphics view
	double x = scene()->views().at(0)->transform().determinant();
	pen.setWidthF(std::max((-0.75/4.5)*(x-1)+1.25, 0.2));
	
	// Set selected state style
	if (option->state & QStyle::State_Selected)
	{
		pen.setColor(Qt::red);
		painter->setPen(pen);
		float length = radius/30. + radius;
		painter->drawLine(0, -1*length, 0, length);
		painter->drawLine(-1*length, 0, length, 0);
	}
	// Set regular state style
	else
	{
		pen.setColor(Qt::yellow);
		painter->setPen(pen);
	}
	painter->setBrush(Qt::NoBrush);
	
	// Draw an slightly transparent circle and crosshair
	painter->drawEllipse(-radius/2, -radius/2, radius, radius);
}

void CoordinateMarker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    update();
}

void CoordinateMarker::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseMoveEvent(event);
}

void CoordinateMarker::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    update();
}

QVariant CoordinateMarker::itemChange(GraphicsItemChange change, const QVariant &value)
{
	/*
	if (change == QGraphicsItem::ItemPositionHasChanged)
	{
		GraphicsScene *s = qobject_cast<GraphicsScene*> (scene());
		s->itemChange(this);
	}
	 */
	return QGraphicsItem::itemChange(change, value);
}