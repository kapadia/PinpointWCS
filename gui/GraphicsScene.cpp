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
#include "CoordMarker.h";

GraphicsScene::GraphicsScene(QPixmap pix, bool ref, QObject *parent)
: QGraphicsScene(parent)
{
	reference = ref;
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
	mousePositionChanged(event->scenePos());
	QGraphicsScene::mouseMoveEvent(event);
}


void GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (clickable)
	{
		// Get scene position
		QPointF pos = event->scenePos();
		// Toggle the boolean
		clickable = !clickable;
		CoordMarker *marker = new CoordMarker(markerRadius);
		marker->setPos(pos);
		addItem(marker);
		// Emit signal
		emit coordinateMarked();
	}
}


void GraphicsScene::makeClickable()
{
	clickable = !clickable;
}
