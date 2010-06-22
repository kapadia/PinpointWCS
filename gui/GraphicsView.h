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

#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QPixmap>
#include <QGraphicsPixmapItem>

class GraphicsView : public QGraphicsView
{
	Q_OBJECT
	
public:
	GraphicsView(QWidget *parent = 0);
	~GraphicsView();
	void setup(QPixmap pix, bool ref);
//	void mouseDoubleClickEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
//	void mouseMoveEvent(QMouseEvent* event);
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);
	
//	void wheelEvent(QWheelEvent *event);

protected:
	// Methods
	void resizeEvent(QResizeEvent *event);
	
private:
	bool reference;
	bool clickable;
	QGraphicsPixmapItem *pixmap;
	
signals:
	void objectResized(QSize s);
};

#endif
