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

#include <QUndoCommand>
#include <QDebug>
#include "CoordinateModel.h"


CoordinateModel::CoordinateModel(QObject *parent)
: QAbstractTableModel(parent)
{
	undoStack = new QUndoStack(this);
	p1 = new QPointF(-1, -1);
	p2 = new QPointF(-1, -1);
}


CoordinateModel::CoordinateModel(QList< QPair<QPointF, QPointF> > pairs, QObject *parent)
: QAbstractTableModel(parent)
{
    listOfCoordinatePairs = pairs;
}

CoordinateModel::~CoordinateModel()
{}

int CoordinateModel::rowCount(const QModelIndex &parent) const
{
    return listOfCoordinatePairs.size();
}

int CoordinateModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant CoordinateModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    if (index.row() >= listOfCoordinatePairs.size() || index.row() < 0)
        return QVariant();
	
	if (role == Qt::DisplayRole)
	{
		QPair<QPointF, QPointF> pair = listOfCoordinatePairs.at(index.row());
		
		QString formattedData;
		
		if (index.column() == 0)
			return formattedData.sprintf("%.2f, %.2f", pair.first.x(), pair.first.y());
		else
		{
			if (pair.second.x() == -1 && pair.second.y() == -1)
				return formattedData.sprintf("%s, %s", "-", "-");
			return formattedData.sprintf("%.2f, %.2f", pair.second.x(), pair.second.y());
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
                return tr("FITS Coordinates");
                
            case 1:
                return tr("EPO Coordinates");
                
            default:
                return QVariant();
        }
    }
    return QVariant();
}

/*
QVariant CoordinateModel::data(const QModelIndex &index, int role) const
{	
    if (!index.isValid())
        return QVariant();
    
    if (index.row() >= listOfCoordinatePairs.size() || index.row() < 0)
        return QVariant();
    
    if (role == Qt::DisplayRole) {
        QPair<QPointF, QPointF> pair = listOfCoordinatePairs.at(index.row());
		
        if (index.column() == 0)
		{
            return pair.first.x();
		}
        else if (index.column() == 1)
		{
            return pair.first.y();
		}
		else if (index.column() == 2)
		{
			return pair.second.x();
		}
		else if (index.column() == 3)
		{
			return pair.second.y();
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
*/

bool CoordinateModel::insertRows(int position, int rows, const QModelIndex &index)
{
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; row++) {
        QPair<QPointF, QPointF> pair(*p1, *p2);
        listOfCoordinatePairs.insert(position, pair);
    }

    endInsertRows();
    return true;
}



bool CoordinateModel::removeRows(int position, int rows, const QModelIndex &index)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    
    for (int row=0; row < rows; ++row) {
        listOfCoordinatePairs.removeAt(position);
    }
	
    endRemoveRows();
    return true;
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

bool CoordinateModel::updateData(CoordMarker *item, const QVariant &value, int role)
{
	if (role == Qt::EditRole)
	{
		MoveCommand *m = new MoveCommand(item, value, this);
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


QList< QPair<QPointF, QPointF> > CoordinateModel::getList()
{
    return listOfCoordinatePairs;
}


void CoordinateModel::emitDataChanged(const QModelIndex &index1, const QModelIndex &index2){
	qDebug() << listOfCoordinatePairs;
    emit dataChanged(index1, index2);
}

void CoordinateModel::computeMapping()
{
	emit compute();
}