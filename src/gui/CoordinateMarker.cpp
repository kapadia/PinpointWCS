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
	setOpacity(100.0);
	setSelected(true);
	setFocus();
	setEnabled(true);
	setRadius();
	index = new QPersistentModelIndex(idx);
	
	// Set some flags
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIsMovable, true);
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

// Don't think we need these methods overridden
/*
void CoordinateMarker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mousePressEvent(event);
}


void CoordinateMarker::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseMoveEvent(event);
}


void CoordinateMarker::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{	
	QGraphicsItem::mouseReleaseEvent(event);
}
*/

void CoordinateMarker::wheelEvent(QGraphicsSceneWheelEvent *event)
{
	// TODO: Call prepareGeometryChange() when ready to implement this dynamic resizing feature.
//	prepareGeometryChange();
	QGraphicsItem::wheelEvent(event);
}

void CoordinateMarker::keyPressEvent(QKeyEvent *event)
{
	qDebug() << "keyPressEvent";
	setFlag(QGraphicsItem::ItemIsMovable, false);
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	QGraphicsItem::keyPressEvent(event);
}

void CoordinateMarker::keyReleaseEvent(QKeyEvent *event)
{
	qDebug() << "keyReleaseEvent";
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, true);
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
	return QGraphicsItem::itemChange(change, value);
}

void CoordinateMarker::setRadius()
{
	/*
	measure = qobject_cast<GraphicsScene*> (scene())->measure;
	scale = qobject_cast<GraphicsView*> (scene()->views().at(0))->scaling();
	float a, b, c, xmin, xmax, rmin, rmax;
	
	rmin = 0.03*measure;
	rmax = 0.3*measure;
	xmin = measure/MAXZOOM;
	xmax = measure/MINZOOM;
	c = xmax-xmin;
	a = (rmax-rmin)/c;
	b = (rmin*xmax-rmax*xmin)/c;
	if (scale > 2)
		return a*(measure/2)+b;
	return a*(measure/scale)+b;
	 */
	radius = 30;
}

float CoordinateMarker::setPenWidth()
{
//	qDebug() << exp(0.001*radius);
//	float s = std::max(0.04*measure/scale, 0.04*measure);
	return 0.03*radius;
}

