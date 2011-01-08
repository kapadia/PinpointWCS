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
#include "CoordinateMarker.h"
#include "GraphicsView.h"

class GraphicsScene : public QGraphicsScene
{
	Q_OBJECT
		
public:
	GraphicsScene(QPixmap pix, bool ref, QObject *parent = 0);
	~GraphicsScene();
	bool reference;
	float measure;
	QGraphicsRectItem *centralItem;
	bool clickable;
	void signalItemMoved(CoordinateMarker *m, QPointF oldPos);
		
protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

private:
	QPointF oldPos;
	QGraphicsItem *movingItem;
	QGraphicsPixmapItem *ptr_pixmap;
	
	float computeRadii();
	
signals:
	void mousePositionChanged(QPointF pos);
	void sceneDoubleClicked(GraphicsScene *scene, QPointF pos);
	void toggleNeighborScene(bool sendSignal = false);
//    void itemMoved(CoordinateMarker *movedItem, const QPointF &movedFromPosition);
	void itemMoved(GraphicsScene *scene, QPointF newPosition, QPointF oldPosition);
	void markerChange(CoordinateMarker *m);
	void currentSelection(int row);
	void clearCorrespondingSelection();

public slots:
	void updatePixmap(QPixmap *pm);
	void toggleClickable(bool sendSignal = true);
	void findSelectedItem();
	void matchSelectedItem(int row);
	
};

#endif
