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
//	void wheelEvent(QWheelEvent *event);
//	void enterEvent(QEvent *event);
	
private:
	bool reference;
	bool clickable;
	QGraphicsPixmapItem *pixmap;
};

#endif
