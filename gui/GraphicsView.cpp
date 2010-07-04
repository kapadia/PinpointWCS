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

#include "GraphicsView.h"
#include "GraphicsScene.h"
#include <QtGui>
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
	setDragMode(NoDrag);
	setTransformationAnchor(AnchorViewCenter);
	viewport()->setMouseTracking(true);
	
	// Adjust the background color
	setBackgroundBrush(QBrush(QColor(30, 30, 30)));
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
	if (event->key() == Qt::Key_Space)
		setDragMode(ScrollHandDrag);
}

void GraphicsView::keyReleaseEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Space)
		setDragMode(NoDrag);
}

void GraphicsView::enterEvent(QEvent *event)
{
	setDragMode(QGraphicsView::NoDrag);
	setFocus();
}

void GraphicsView::leaveEvent(QEvent *event)
{
	setDragMode(QGraphicsView::NoDrag);
}
