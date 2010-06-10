#include <iostream>

#include "GraphicsView.h"
#include <QtGui>
#include <QGLWidget>

GraphicsView::GraphicsView(QWidget *parent)
: QGraphicsView(parent)
{	
	// Set up view port to use OpenGL
	setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	setViewportUpdateMode(FullViewportUpdate);
	
	// Set attributes from QGraphicsView
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setRenderHint(QPainter::Antialiasing);
	setDragMode(NoDrag);
	setTransformationAnchor(AnchorViewCenter);
	setScene(new QGraphicsScene());
	
	// Adjust the background color
	setBackgroundBrush(QBrush(QColor(30, 30, 30)));
}

GraphicsView::~GraphicsView() {}

void GraphicsView::setup(QPixmap pix, bool ref)
{
	reference = ref;
	pixmap = scene()->addPixmap(pix);
	pixmap->setZValue(1);
	
	if (reference)
		clickable = true;
	else
		clickable = false;
	
	show();
	std::cout << "Finished setup\n";
	
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