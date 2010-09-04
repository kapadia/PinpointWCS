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


QWidget* CoordinateDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem&, const QModelIndex &index) const
{
	QDoubleSpinBox *spinbox = new QDoubleSpinBox;
	spinbox->setRange(0, 20000);
	spinbox->setSingleStep(1);
	spinbox->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
	return spinbox;
	/*
	if (index.column() <= 3)
	{
		QDoubleSpinBox *spinbox = new QDoubleSpinBox;
		spinbox->setRange(0, 20000);
		spinbox->setSingleStep(1);
		spinbox->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
		return spinbox;
	}
	 */
}

void CoordinateDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QItemDelegate::paint(painter, option, index);
}

void CoordinateDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	// Cast the editor as a double spinbox
	QDoubleSpinBox *edit = qobject_cast<QDoubleSpinBox*>(editor);
	const CoordinateModel *model = qobject_cast<const CoordinateModel*> (index.model());
	double value = model->data(index, Qt::DisplayRole).toDouble();
	if (index.column() <= 3)
		edit->setValue(value);
	else
		QItemDelegate::setEditorData(editor, index);
}

void CoordinateDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{	
	const CoordinateModel *m = qobject_cast<const CoordinateModel*> (index.model());
	
	if (index.column() <= 3)
	{
		// FIXME: Find a way to call setData without having to pass the scene
//		m.setData(index, QVariant(editor.value()));
	}
	else
		QItemDelegate::setModelData(editor, model, index);
}

QSize CoordinateDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QItemDelegate::sizeHint(option, index);
}