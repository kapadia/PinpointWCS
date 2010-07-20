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

#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include "CoordMarker.h"

class GraphicsScene : public QGraphicsScene
{
	Q_OBJECT
		
public:
	GraphicsScene(QPixmap pix, bool ref, QObject *parent = 0);
	~GraphicsScene();
	float markerRadius;
	bool reference;
	void itemChange(CoordMarker *m);
		
protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
	bool clickable;
	QGraphicsItem *movingItem;
	QPointF oldPos;
	QGraphicsPixmapItem *pixmap;
	
signals:
	void mousePositionChanged(QPointF pos);
	void sceneDoubleClicked(GraphicsScene *scene, QPointF pos);
	void toggleNeighborScene(bool sendSignal = false);
    void itemMoved(CoordMarker *movedItem, const QPointF &movedFromPosition);
	void markerChange(CoordMarker *m);

public slots:
	void updatePixmap(QPixmap *pm);
	void toggleClickable(bool sendSignal = true);
	
};

#endif
