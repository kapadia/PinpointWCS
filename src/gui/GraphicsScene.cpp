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
#include "GraphicsScene.h"
#include <math.h>

GraphicsScene::GraphicsScene(QPixmap pix, bool ref, QObject *parent)
: QGraphicsScene(parent)
{
	reference = ref;
	movingItem = 0;
	setSceneRect(0, 0, pix.width(), pix.height());	
	ptr_pixmap = addPixmap(pix);
	
	// Compute the measure of the pixmap
	measure = sqrt(pix.width() * pix.height());
	
	if (reference)
		clickable = true;
	else
		clickable = false;
	
	// TODO: Testing a central graphics item
	centralItem = new QGraphicsRectItem;
//	centralItem->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
	centralItem->setRect(sceneRect());
	addItem(centralItem);
}


GraphicsScene::~GraphicsScene() {}


void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{	
	// Broadcast the position of the mouse
	emit mousePositionChanged(event->scenePos());
	QGraphicsScene::mouseMoveEvent(event);
}

 
void GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	// Broadcast the event for the other GraphicsScene and the data model
	if (clickable)
		emit sceneDoubleClicked(this, event->scenePos());
}


void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF mousePos(event->buttonDownScenePos(Qt::LeftButton).x(),
                     event->buttonDownScenePos(Qt::LeftButton).y());
    movingItem = itemAt(mousePos.x(), mousePos.y());
	
    if (movingItem != 0 && event->button() == Qt::LeftButton) {
        oldPos = movingItem->pos();
    }

    clearSelection();    
    QGraphicsScene::mousePressEvent(event);
}


void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (movingItem != 0 && event->button() == Qt::LeftButton) {
        if (oldPos != movingItem->pos())
			emit itemMoved(this, movingItem->pos(), oldPos);
        movingItem = 0;
	}
	/*
    if (movingItem != 0 && event->button() == Qt::LeftButton) {
		QPointF newPos = movingItem->pos();
		
        if (oldPos != movingItem->pos())
		{
			if (newPos.x() < 0)
				movingItem->setX(0);
			if (newPos.x() > width())
				movingItem->setX(width());
			if (newPos.y() < 0)
				movingItem->setY(0);
			if (newPos.y() > height())
				movingItem->setY(height());
			emit itemMoved(this, movingItem->pos(), oldPos);
		}
        movingItem = 0;
    }
	 */

    QGraphicsScene::mouseReleaseEvent(event);
}


void GraphicsScene::keyPressEvent(QKeyEvent *event)
{
	QGraphicsScene::keyPressEvent(event);
}


void GraphicsScene::keyReleaseEvent(QKeyEvent *event)
{
	QGraphicsScene::keyReleaseEvent(event);
}


void GraphicsScene::toggleClickable(bool sendSignal)
{
	// Toggle the clickable variable for both GraphicsScenes
	clickable = !clickable;
	if (sendSignal)
		emit toggleNeighborScene(false);
}


void GraphicsScene::updatePixmap(QPixmap *pm)
{
	qDebug() << pm;
	// Update the pixmap
	ptr_pixmap->setPixmap(*pm);
//	delete ptr_pixmap;
//	ptr_pixmap = addPixmap(*pm);
}


void GraphicsScene::signalItemMoved(CoordinateMarker *m, QPointF oldPos)
{
	// Broadcast the item moved and its old position
//	emit itemMoved(qgraphicsitem_cast<CoordinateMarker *>(m), oldPos);
	emit itemMoved(this, m->scenePos(), oldPos);
}


float GraphicsScene::computeRadii()
{
	float radius;
	
	// Use the measure and zoom factor to determine the radius in a
	// manner that is consistent with the full range of image dimensions
	radius = qobject_cast<GraphicsView*> (views().at(0))->scaling();
	
	qDebug() << "Radius: " << radius;
	
	return radius;
}
