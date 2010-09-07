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

#ifndef COORDINATE_MARKER_H
#define COORDINATE_MARKER_H

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QModelIndex>

class CoordinateMarker : public QGraphicsItem
{
	
public:
	CoordinateMarker(QGraphicsItem *parent = 0);
	CoordinateMarker(float r, QGraphicsItem *parent = 0);
	~CoordinateMarker();
	int row;
	
	// Required methods to implement
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
	
private:
	float radius;
	
};

#endif
