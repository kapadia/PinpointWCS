#include <iostream>
#include <QDragEnterEvent>

#include "DropArea.h"

DropArea::DropArea(QWidget *parent)
	: QLabel(parent)
{	
	// Set attributes of DropSite
	ready = false;

	// Set attributes from QLabel
	setAcceptDrops(true);
	setFrameStyle(QFrame::Box | QFrame::Sunken);
	setLineWidth(1);
	setMidLineWidth(2);
	setAlignment(Qt::AlignCenter);
	setAutoFillBackground(true);
	setBackgroundRole(QPalette::Window);
	setTextInteractionFlags(Qt::NoTextInteraction);	
	
	// Adjust the text color via a QPalette
	QPalette palette;
	palette.setColor(QPalette::WindowText, QColor(75, 75, 75));
	setPalette(palette);
	
	// Set font attributes
	QFont font;
	font.setPixelSize(12);
	setFont(font);
}

DropArea::~DropArea() {}

void DropArea::setFileExtensions(bool exts) {
	if (exts == false)
		extList << "fits";
	else
		extList << "jpg" << "jpeg" << "tif" << "tiff" << "png";
}


void DropArea::dragEnterEvent(QDragEnterEvent *event)
{
	// Collect all the files that the user is dragging to the widget
	if (event->mimeData()->hasUrls()) {
		const QList<QUrl> files = event->mimeData()->urls();
		
		// Ignore event if more than one item
		const int listSize = files.size();
		if (listSize != 1) {
			event->ignore();
		} else {
			QUrl url = files.first();
			
			// Check for the correct file type
			QString localFile = url.toLocalFile();
			QStringList strlist = localFile.split(".");
			QString ext = strlist.last();
			
			if (extList.contains(ext, Qt::CaseInsensitive)) {
				setBackgroundRole(QPalette::Dark);
				event->acceptProposedAction();
			}
		}
	}
}

void DropArea::dropEvent(QDropEvent *event)
{
	filepath = event->mimeData()->urls().first().toLocalFile();
	setText(filepath.split("/").last());
	ready = true;
	emit readyForImport();
}

void DropArea::dragLeaveEvent(QDragLeaveEvent *event) {
	setBackgroundRole(QPalette::Window);
	event->accept();
}


