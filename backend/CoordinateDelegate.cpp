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

#include "CoordinateDelegate.h"
#include <QDoubleSpinBox>
#include "CoordinateModel.h"
#include "GraphicsScene.h"
#include <QDebug>

CoordinateDelegate::CoordinateDelegate(QObject *parent)
: QItemDelegate(parent) {}


QWidget* CoordinateDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
	/*
	const CoordinateModel2 *model = qobject_cast<const CoordinateModel2*> (index.model());
	QList< QPair<CoordinateMarker*, CoordinateMarker*> > l = model->listOfMarkerPairs;
	QPair<CoordinateMarker*, CoordinateMarker*> p = l.at(index.row());
	CoordinateMarker *marker;
	GraphicsScene *s;
	double maxValue;
	
	if (index.column() == 0)
	{
		marker = p.first;
//		maxValue = marker->scene()->width();
	}
	else if (index.column() == 1)
	{
		marker = p.first;
//		s = qobject_cast<GraphicsScene*> (marker->scene());
//		maxValue = s->height();
	}
	else if (index.column() == 2)
	{
		marker = p.second;
//		s = qobject_cast<GraphicsScene*> (marker->scene());
//		maxValue = s->width();
	}
	else
	{
		marker = p.second;
//		s = qobject_cast<GraphicsScene*> (marker->scene());
//		maxValue = s->height();
	}
	 */
	editor->setRange(0, 10000);
	return editor;
}

void CoordinateDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QItemDelegate::paint(painter, option, index);
}

void CoordinateDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{

	// Cast the model as a CoordinateModel
	const CoordinateModel2 *model = qobject_cast<const CoordinateModel2*> (index.model());
	QVariant var = model->data(index, Qt::DisplayRole);
	double value = var.toDouble();
	
	// Cast the editor as a double spin box
	QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(editor);
	spinBox->setValue(value);
}

void CoordinateDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{		
	
	// Cast the editor as a double spin box
	QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(editor);
	spinBox->interpretText();

	// Get the value from the editor
	double value = spinBox->value();
	
	// Cast the model as a CoordinateModel and set the data
	CoordinateModel2 *m = qobject_cast<CoordinateModel2*>(model);
	
	// FIXME: Testing the index variable
	qDebug() << index.row() << "\t" << index.column();
	
	// Use the QModelIndex to locate the corresponding CoordinateMarker in the data model
	QList< QPair<CoordinateMarker*, CoordinateMarker*> > l = m->listOfMarkerPairs;
	QPair<CoordinateMarker*, CoordinateMarker*> p = l.at(index.row());
	CoordinateMarker *marker;
	QPointF oldPosition;
	QPointF newPosition;
	
	switch (index.column()) {
		case 0:
			marker = p.first;
			oldPosition = marker->pos();
			newPosition = QPointF(oldPosition);
			newPosition.setX(value);
			break;
		case 1:
			marker = p.first;
			oldPosition = marker->pos();
			newPosition = QPointF(oldPosition);
			newPosition.setY(value);
			break;
		case 2:
			marker = p.second;
			oldPosition = marker->pos();
			newPosition = QPointF(oldPosition);
			newPosition.setX(value);
			break;
		case 3:
			marker = p.second;
			oldPosition = marker->pos();
			newPosition = QPointF(oldPosition);
			newPosition.setY(value);
			break;
	}
	marker->setPos(newPosition);
//	emit itemMoved(qgraphicsitem_cast<CoordinateMarker *>(movingItem), oldPos);
	emit itemMoved(marker, oldPosition);
}

QSize CoordinateDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QItemDelegate::sizeHint(option, index);
}