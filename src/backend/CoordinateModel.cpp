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
#include "CoordinateModel.h"


CoordinateModel::CoordinateModel(QObject *parent)
: QAbstractTableModel(parent)
{
	undoStack = new QUndoStack(this);
	p = new QPointF(-1, -1);
}


CoordinateModel::CoordinateModel(QList<QPointF> r, QList<QPointF> e, QObject *parent)
: QAbstractTableModel(parent)
{
	refCoords = r;
	epoCoords = e;
}

CoordinateModel::~CoordinateModel()
{}

int CoordinateModel::rowCount(const QModelIndex &parent) const
{
    return refCoords.size();
}

int CoordinateModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}

QVariant CoordinateModel::data(const QModelIndex &index, int role) const
{
	// Prevent an invalid QModelIndex from being processed
    if (!index.isValid())
        return QVariant();
    if (index.row() >= refCoords.size() || index.row() < 0)
        return QVariant();
	
	// Display the data correctly
	if (role == Qt::DisplayRole)
	{
		// Get the two coordinates
		QPointF refCoord = refCoords.at(index.row());
		QPointF epoCoord = epoCoords.at(index.row());
		
		if (index.column() == 0)
			return refCoord.x();
		else if (index.column() == 1)
			return refCoord.y();
		else if (index.column() == 2)
		{
			if (epoCoord.x() == -1)
				return QString("-");
			else
				return epoCoord.x();
		}
		else
		{
			if (epoCoord.y() == -1)
				return QString("-");
			else
				return epoCoord.y();
		}
	}
	
	return QVariant();
}

QVariant CoordinateModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    
    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return tr("FITS X");
            case 1:
                return tr("FITS Y");
			case 2:
				return tr("EPO X");
			case 3:
				return tr("EPO Y");
            default:
                return QVariant();
        }
    }
    return QVariant();
}


bool CoordinateModel::insertRows(int position, int rows, const QModelIndex &index)
{
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; row++) {
		refCoords.insert(position, *p);
		epoCoords.insert(position, *p);
    }

    endInsertRows();
    return true;
}



bool CoordinateModel::removeRows(int position, int rows, const QModelIndex &index)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    
    for (int row=0; row < rows; ++row) {
		refCoords.removeAt(position);
		epoCoords.removeAt(position);
    }
	
    endRemoveRows();
    return true;
}


void CoordinateModel::setData(GraphicsScene *scene, QPointF coord)
{
	setData(scene, QModelIndex(), coord, Qt::EditRole);
}


bool CoordinateModel::setData(GraphicsScene *scene, const QModelIndex &index, const QVariant &value, int role)
{
	if (role == Qt::EditRole)
	{
		// Call add command and push to undo stack
		AddCommand *a = new AddCommand(scene, value, this);
		undoStack->push(a);
        return true;
	}
	return false;
}


void CoordinateModel::updateData(GraphicsScene *scene, QPointF newCoord, QPointF oldCoord, QModelIndex *index)
{
	updateData(scene, newCoord, oldCoord, index, Qt::EditRole);
}

bool CoordinateModel::updateData(GraphicsScene *scene, const QVariant &newValue, const QVariant &oldValue, QModelIndex *index, int role)
{
	if (role == Qt::EditRole)
	{
		MoveCommand *m = new MoveCommand(scene, newValue, oldValue, this, index);
		qDebug() << m;		
		undoStack->push(m);
		return true;
	}
	return false;
}


Qt::ItemFlags CoordinateModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}


void CoordinateModel::emitDataChanged(const QModelIndex &index1, const QModelIndex &index2){
    emit dataChanged(index1, index2);
}
