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
#include "GraphicsScene.h"
#include "CoordinateMarker.h";

GraphicsScene::GraphicsScene(QPixmap pix, bool ref, QObject *parent)
: QGraphicsScene(parent)
{
	reference = ref;
	movingItem = 0;
	pixmap = addPixmap(pix);
	
	if (reference)
		clickable = true;
	else
		clickable = false;
	
	// Determine marker radius
	if (width() > height())
		markerRadius = width()*0.025;
	else
	{
		markerRadius = height()*0.025;
	}
	qDebug() << "Finished Initializing Scene";
}

GraphicsScene::~GraphicsScene() {}


void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	emit mousePositionChanged(event->scenePos());
	QGraphicsScene::mouseMoveEvent(event);
}

 
void GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (clickable)
	{
		// Get scene position
		QPointF pos = event->scenePos();
		
		// Broadcast the event for AddCommand and the other GraphicsScene
		emit sceneDoubleClicked(this, pos);
	}
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
            emit itemMoved(qgraphicsitem_cast<CoordinateMarker *>(movingItem), oldPos);
        movingItem = 0;
    }
	
    QGraphicsScene::mouseReleaseEvent(event);
}

void GraphicsScene::toggleClickable(bool sendSignal)
{
	clickable = !clickable;
	if (sendSignal)
		emit toggleNeighborScene(false);
}

void GraphicsScene::updatePixmap(QPixmap *pm)
{
	pixmap->setPixmap(*pm);
}

/*
void GraphicsScene::itemChange(CoordinateMarker *m)
{
	emit markerChange(m);
}
*/