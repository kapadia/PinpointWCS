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
#include <QUndoCommand>
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
    if (!index.isValid())
        return QVariant();
    
    if (index.row() >= refCoords.size() || index.row() < 0)
        return QVariant();
	
	if (role == Qt::DisplayRole)
	{
		// Set some variables
		QString formattedData;
		QPointF refCoord = refCoords.at(index.row());
		QPointF epoCoord = epoCoords.at(index.row());
		
		if (index.column() == 0)
			return refCoord.x();
		else if (index.column() == 1)
			return refCoord.y();
		else if (index.column() == 2)
		{
			if (epoCoord.x() == -1)
				return formattedData.sprintf("%s", "-");
			else
				return epoCoord.x();
		}
		else
		{
			if (epoCoord.y() == -1)
				return formattedData.sprintf("%s", "-");
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



bool CoordinateModel::updateData(GraphicsScene *scene, const QVariant &newValue, const QVariant &oldValue, int role)
{
	if (role == Qt::EditRole)
	{
		MoveCommand *m = new MoveCommand(scene, newValue, oldValue, this);
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

void CoordinateModel::computeMapping()
{
	emit compute();
}

 
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

CoordinateModel2::CoordinateModel2(QObject *parent)
: QAbstractTableModel(parent)
{
	undoStack = new QUndoStack(this);
	defaultMarker = new CoordinateMarker();
}


CoordinateModel2::CoordinateModel2(QList< QPair<CoordinateMarker*, CoordinateMarker*> > pairs, QObject *parent)
: QAbstractTableModel(parent)
{
    listOfMarkerPairs = pairs;
}

CoordinateModel2::~CoordinateModel2()
{}

int CoordinateModel2::rowCount(const QModelIndex &parent) const
{
    return listOfMarkerPairs.size();
}

int CoordinateModel2::columnCount(const QModelIndex &parent) const
{
    return 4;
}

QVariant CoordinateModel2::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    
    if (index.row() >= listOfMarkerPairs.size() || index.row() < 0)
        return QVariant();
	
	if (role == Qt::DisplayRole)
	{
		QPair<CoordinateMarker*, CoordinateMarker*> pair = listOfMarkerPairs.at(index.row());
		QString formattedData;
		
		if (index.column() == 0)
			return pair.first->scenePos().x();
		else if (index.column() == 1)
			return pair.first->scenePos().y();
		else if (index.column() == 2)
		{
			if (pair.second->scenePos().x() == -1)
				return formattedData.sprintf("%s", "-");
			else
				return pair.second->scenePos().x();
		}
		else
		{
			if (pair.second->scenePos().y() == -1)
				return formattedData.sprintf("%s", "-");
			else
				return pair.second->scenePos().y();
		}
	}
	return QVariant();
}

QVariant CoordinateModel2::headerData(int section, Qt::Orientation orientation, int role) const
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


bool CoordinateModel2::insertRows(int position, int rows, const QModelIndex &index)
{
    beginInsertRows(QModelIndex(), position, position+rows-1);
	
    for (int row=0; row < rows; row++) {
        QPair<CoordinateMarker*, CoordinateMarker*> pair(defaultMarker, defaultMarker);
        listOfMarkerPairs.insert(position, pair);
    }
	
    endInsertRows();
    return true;
}



bool CoordinateModel2::removeRows(int position, int rows, const QModelIndex &index)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);
    
    for (int row=0; row < rows; ++row) {
        listOfMarkerPairs.removeAt(position);
    }
	
    endRemoveRows();
    return true;
}


bool CoordinateModel2::setData(GraphicsScene *scene, const QModelIndex &index, const QVariant &value, int role)
{
	if (role == Qt::EditRole)
	{
		// Call add command and push to undo stack
		AddCommand2 *a = new AddCommand2(scene, value, this);
		undoStack->push(a);
        return true;
	}
	return false;
}


bool CoordinateModel2::updateData(CoordinateMarker *item, const QVariant &value, int role)
{
	if (role == Qt::EditRole)
	{
		MoveCommand2 *m = new MoveCommand2(item, value, this);
		undoStack->push(m);
		return true;
	}
	return false;
}


Qt::ItemFlags CoordinateModel2::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}


QList< QPair<CoordinateMarker*, CoordinateMarker*> > CoordinateModel2::getList()
{
    return listOfMarkerPairs;
}


void CoordinateModel2::emitDataChanged(const QModelIndex &index1, const QModelIndex &index2)
{
    emit dataChanged(index1, index2);
}

void CoordinateModel2::computeMapping()
{
	emit compute();
}

