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
#include "CoordinateModel.h"
#include "GraphicsScene.h"
#include <QDoubleSpinBox>
#include <QDebug>


CoordinateDelegate::CoordinateDelegate(GraphicsScene *s1, GraphicsScene *s2, QObject *parent)
: QItemDelegate(parent)
{
	fitsScene = s1;
	epoScene = s2;
}


QWidget* CoordinateDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QDoubleSpinBox *editor = new QDoubleSpinBox(parent);

	double maxValue;
	switch (index.column()) {
		case 0:
			maxValue = fitsScene->width();
			break;
		case 1:
			maxValue = fitsScene->height();
			break;
		case 2:
			maxValue = epoScene->width();
			break;
		case 3:
			maxValue = epoScene->height();
			break;
	}
	editor->setRange(0, maxValue);
	return editor;
}

void CoordinateDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QItemDelegate::paint(painter, option, index);
}

void CoordinateDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	// Cast the model as a CoordinateModel
	const CoordinateModel *model = qobject_cast<const CoordinateModel*> (index.model());
	QVariant var = model->data(index, Qt::DisplayRole);
	double value = var.toDouble();
	
	// Cast the editor as a double spin box
	QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(editor);
	spinBox->setValue(value);
}

void CoordinateDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{	
	
	// Boolean to determine corresponding scene of data
	bool scene;
	bool broadcast = true;
	
	// Cast the editor as a double spin box
	QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(editor);
	spinBox->interpretText();

	// Get the value from the editor
	double value = spinBox->value();
	
	// Cast the model as a CoordinateModel and set the data
	CoordinateModel *m = qobject_cast<CoordinateModel*>(model);
	
	// Use the QModelIndex to locate the corresponding QPointF in the data model
	QList<QPointF> refCoords = m->refCoords;
	QList<QPointF> epoCoords = m->epoCoords;

	QPointF oldPosition;
	QPointF newPosition;
	
	switch (index.column()) {
		case 0:
			oldPosition = refCoords.at(index.row());
			newPosition = QPointF(oldPosition);
			newPosition.setX(value);
			scene = true;
			break;
		case 1:
			oldPosition = refCoords.at(index.row());
			newPosition = QPointF(oldPosition);
			newPosition.setY(value);
			scene = true;
			break;
		case 2:
			oldPosition = epoCoords.at(index.row());
			if (oldPosition == QPointF(-1, -1))
				broadcast = false;
			newPosition = QPointF(oldPosition);
			newPosition.setX(value);
			scene = false;
			break;
		case 3:
			oldPosition = epoCoords.at(index.row());
			if (oldPosition == QPointF(-1, -1))
				broadcast = false;
			newPosition = QPointF(oldPosition);
			newPosition.setY(value);
			scene = false;
			break;
	}
	
	// Broadcast change of data
	if (broadcast)
	{
		if (scene == true)
			emit itemMoved(fitsScene, newPosition, oldPosition);
		else
			emit itemMoved(epoScene, newPosition, oldPosition);
	}
}

QSize CoordinateDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QItemDelegate::sizeHint(option, index);
}