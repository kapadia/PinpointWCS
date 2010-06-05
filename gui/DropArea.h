#ifndef DROPAREA_H
#define DROPAREA_H

#include <QLabel>
#include <QUrl>

class DropArea : public QLabel
{
	Q_OBJECT
public:
	DropArea(QWidget *parent = 0);
	~DropArea();
	void setFileExtensions(bool exts);
	bool ready;
	QString filepath;

protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dragLeaveEvent(QDragLeaveEvent *event);
	void dropEvent(QDropEvent *event);

private:
	QStringList extList;

signals:
	void readyForImport();
};

#endif
