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

CoordinateDelegate::CoordinateDelegate(QObject *parent)
: QItemDelegate(parent) {}


QWidget* CoordinateDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
	editor->setRange(0, 10000);
	return editor;
	/*
	if (index.column() <= 3)
	{
		QDoubleSpinBox *spinbox = new QDoubleSpinBox;
		spinbox->setRange(0, 20000);
		spinbox->setSingleStep(1);
		spinbox->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
		return spinbox;
	}
	return QItemDelegate::createEditor(parent, option, index);
	 */
}

void CoordinateDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QItemDelegate::paint(painter, option, index);
}

void CoordinateDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{

	// Cast the model as a CoordinateModel
	const CoordinateModel *model = qobject_cast<const CoordinateModel*> (index.model());
	double value = model->data(index, Qt::DisplayRole).toDouble();
	
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
	//const CoordinateModel *m = qobject_cast<const CoordinateModel*> (index.model());
	// FIXME: Need to call setData() from the data model without knowledge of the parent graphics scene
	//m->setData();
}

QSize CoordinateDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QItemDelegate::sizeHint(option, index);
}