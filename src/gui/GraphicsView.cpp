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
#include "GraphicsView.h"
#include "GraphicsScene.h"
#include "math.h"
//#include <QGLWidget>

GraphicsView::GraphicsView(QWidget *parent)
: QGraphicsView(parent)
{	
	// Set up view port to use OpenGL
//	setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
//	setViewportUpdateMode(FullViewportUpdate);
	
	// Set attributes from QGraphicsView
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setRenderHint(QPainter::Antialiasing);
//	setDragMode(NoDrag);
	// Testing: Testing panning by default
	setDragMode(ScrollHandDrag);
	setTransformationAnchor(AnchorViewCenter);
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	
	viewport()->setMouseTracking(true);
	viewport()->setCursor(QCursor(Qt::ArrowCursor));
	
	// Adjust the background color
	setBackgroundBrush(QBrush(QColor(30, 30, 30)));
	
	// Set initial conditions
	rotateFactor = 0;
}

GraphicsView::~GraphicsView() {}

void GraphicsView::resizeEvent(QResizeEvent *event)
{
	// Emit signal with size information
	QSize s = size();
	emit objectResized(s);
	
	// Call parent function
	QWidget::resizeEvent(event);
}


void GraphicsView::keyPressEvent(QKeyEvent *event)
{
	// Testing: Testing panning by default
	/*
	if (event->key() == Qt::Key_Space)
		setDragMode(ScrollHandDrag);
	 */
	/*
	else if(event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_R)
	{
		rotate(5);
		rotateFactor = (rotateFactor + 5) % 360;
	}
	else if(event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_R)
	{
		rotate(-5);
		rotateFactor = (rotateFactor - 5) % 360;
	}
	 */
	
	QGraphicsView::keyPressEvent(event);
}

void GraphicsView::keyReleaseEvent(QKeyEvent *event)
{
	// Testing: Testing panning by default
	/*
	if (event->key() == Qt::Key_Space)
		setDragMode(NoDrag);
	 */
	QGraphicsView::keyReleaseEvent(event);
}


void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	QGraphicsView::mouseReleaseEvent(event);
	viewport()->setCursor(QCursor(Qt::ArrowCursor));
}


void GraphicsView::enterEvent(QEvent *event)
{
	// Reset some graphics view settings
	// Testing: Testing panning by default
//	setDragMode(QGraphicsView::NoDrag);
	setFocus();
	
	// Signal a mouseEnterEvent
	emit mouseEnterEvent(this);
}

void GraphicsView::leaveEvent(QEvent *event)
{
	// Testing: Testing panning by default
//	setDragMode(QGraphicsView::NoDrag);
	
	// Reset some graphics item settings
	GraphicsScene *s = qobject_cast<GraphicsScene*> (scene());
	QList<QGraphicsItem*> list = s->centralItem->childItems();
	for (int i=0; i<list.size(); i++)
	{
		list.at(i)->setFlag(QGraphicsItem::ItemIsMovable, true);
		list.at(i)->setFlag(QGraphicsItem::ItemIsSelectable, true);
	}
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
	scaleView(pow((double)2, event->delta() / 240.0));
}

void GraphicsView::scaleView(qreal scaleFactor)
{
	qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	
	if (factor < MINZOOM || factor > MAXZOOM)
		return;	
	scale(scaleFactor, scaleFactor);
}

float GraphicsView::scaling()
{
	// Get transform matrix
	QTransform t = transform();
	
	// Undo any rotations
	t.rotate(-rotateFactor);
	
	return t.m11();
}

void GraphicsView::rotateCW()
{
	rotate(5);
	rotateFactor = (rotateFactor + 5) % 360;
}

void GraphicsView::rotateCCW()
{
	rotate(-5);
	rotateFactor = (rotateFactor - 5) % 360;
}