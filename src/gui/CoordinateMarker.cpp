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

#include "CoordinateMarker.h"
#include "GraphicsScene.h"
#include "GraphicsView.h"
#include "math.h"

CoordinateMarker::CoordinateMarker(QModelIndex &idx, QGraphicsItem *parent)
: QGraphicsItem(parent)
{
	qDebug() << "Initializing CoordinateMarker object ...";
	setZValue(2);
	setRadius();
	setOpacity(100.0);
	
	setSelected(true);
	setFocus();
	setEnabled(true);
	
	index = new QPersistentModelIndex(idx);
	
	// Set some flags
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIsMovable, false);
	setFlag(QGraphicsItem::ItemIsFocusable, true);
	//	setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
}

CoordinateMarker::~CoordinateMarker() {}

QRectF CoordinateMarker::boundingRect() const
{
	return QRectF(-radius - penWidth, -radius - penWidth, 2*(radius+penWidth), 2*(radius+penWidth));
}

QPainterPath CoordinateMarker::shape() const
{
	QPainterPath path;
	path.addEllipse(boundingRect());
	return path;
}


void CoordinateMarker::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{	
	QPen pen;
	
	// Compute width based on zoom factor of graphics view
	setRadius();
	penWidth = setPenWidth();
	pen.setWidthF(penWidth);
	
	// Set selected state style
	if (option->state & QStyle::State_Selected)
	{
		pen.setColor(Qt::red);
		painter->setPen(pen);
		float length = radius*0.125;
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
	
	// Draw a slightly transparent circle and crosshair
	painter->drawEllipse(-radius, -radius, radius*2, radius*2);
}

void CoordinateMarker::wheelEvent(QGraphicsSceneWheelEvent *event)
{
	prepareGeometryChange();
	QGraphicsItem::wheelEvent(event);
}


void CoordinateMarker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mousePressEvent(event);
}


void CoordinateMarker::keyPressEvent(QKeyEvent *event)
{
	QGraphicsItem::keyPressEvent(event);
}


void CoordinateMarker::keyReleaseEvent(QKeyEvent *event)
{
	QGraphicsItem::keyReleaseEvent(event);
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
	if (change == QGraphicsItem::ItemSelectedChange)
		if (value == true)
			setFlag(ItemIsMovable, true);
		else
			setFlag(ItemIsMovable, false);
		
	return QGraphicsItem::itemChange(change, value);
}

void CoordinateMarker::setRadius()
{
	measure = qobject_cast<GraphicsScene*> (scene())->measure;
	radius = measure*0.018;
}

float CoordinateMarker::setPenWidth()
{
	float scale = qobject_cast<GraphicsView*> (scene()->views().at(0))->scaling();
	return (0.5 + 0.02*radius)/scale;
}

