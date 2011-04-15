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

GraphicsScene::GraphicsScene(QPixmap *pix, bool ref, QObject *parent)
: QGraphicsScene(parent)
{
	reference = ref;
	movingItem = 0;
	setSceneRect(0, 0, pix->width(), pix->height());	
	ptr_pixmap = addPixmap(*pix);
	
	// Compute the measure of the pixmap
	measure = sqrt(pix->width() * pix->height());
	
	if (reference)
		clickable = true;
	else
		clickable = false;
	
	// Create a container for all the markers
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
}


void GraphicsScene::signalItemMoved(CoordinateMarker *m, QPointF oldPos)
{
	// Broadcast the item moved and its old position
	emit itemMoved(this, m->scenePos(), oldPos);
}


float GraphicsScene::computeRadii()
{
	// Use the measure and zoom factor to determine the radius in a
	// manner that is consistent with the full range of image dimensions
	float radius = qobject_cast<GraphicsView*> (views().at(0))->scaling();
	
	qDebug() << "Radius: " << radius;
	
	return radius;
}


void GraphicsScene::findSelectedItem()
{
	// Determine the selected item
	if (selectedItems().isEmpty())
	{
		emit clearCorrespondingSelection();
		return;
	}
	CoordinateMarker *item = qgraphicsitem_cast<CoordinateMarker *>(selectedItems()[0]);
	emit currentSelection(item->index->row());
}


void GraphicsScene::matchSelectedItem(int row)
{
	// Check if corresponding marker is already selected
	if (!selectedItems().isEmpty())
	{
		CoordinateMarker *item = qgraphicsitem_cast<CoordinateMarker*>(selectedItems()[0]);
		if (item->index->row() == row)
			return;
	}
	
	// Clear selection
	clearSelection();
	
	// Search for the corresponding marker
	if (items().isEmpty())
		return;
	QList<QGraphicsItem*> markers = centralItem->childItems();
	for (int i = 0; i < markers.size(); ++i)
	{
		CoordinateMarker *marker = qgraphicsitem_cast<CoordinateMarker*>(markers.at(i));
		if (marker->index->row() == row)
			marker->setSelected(true);
	}
}


void GraphicsScene::selectedItemPos()
{	
	// Get selected item if any
	if (selectedItems().isEmpty())
		return;
	CoordinateMarker *item = qgraphicsitem_cast<CoordinateMarker*>(selectedItems()[0]);
	emit itemPos(item->pos());
}