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

#ifndef COORDINATE_MODEL_H
#define COORDINATE_MODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QPointF>
#include <QUndoStack>
#include <QVariant>
#include "GraphicsScene.h"
#include "Commands.h"
#include "CoordinateMarker.h"
#include "CoordinateDelegate.h"
#include "ComputeWCS.h"


class CoordinateModel : public QAbstractTableModel
{
	
	Q_OBJECT
	
	// These classes have access to private members
	friend class AddCommand;
	friend class MoveCommand;
	
public:	
	CoordinateModel(QObject *parent=0);
	CoordinateModel(QList<QPointF> r, QList<QPointF> e, QObject *parent=0);
	~CoordinateModel();
	
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
	bool setData(GraphicsScene *scene, const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
//	bool updateData(CoordinateMarker *marker, const QVariant &oldValue, int role=Qt::EditRole);
	bool updateData(GraphicsScene *scene, const QVariant &newValue, const QVariant &oldValue, int role=Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
	
	// Attributes
	QUndoStack *undoStack;
	QList<QPointF> refCoords;
	QList<QPointF> epoCoords;

public slots:
	void setData(GraphicsScene *s, QPointF coord);
//	void updateData(CoordinateMarker *m, QPointF coord);
	void updateData(GraphicsScene *scene, QPointF newCoord, QPointF oldCoord);
	
protected:
	void emitDataChanged(const QModelIndex &index1, const QModelIndex &index2);
	void computeMapping();
	QPointF *p;
	
signals:
	void compute();

};


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/


class CoordinateModel2 : public QAbstractTableModel
{
	
	Q_OBJECT
	
	friend class AddCommand2;
	friend class MoveCommand2;
	friend class CoordinateDelegate;
	
public:	
	CoordinateModel2(QObject *parent=0);
	CoordinateModel2(QList< QPair<CoordinateMarker*, CoordinateMarker*> > markerPairs, QObject *parent=0);
	~CoordinateModel2();
	
	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool setData(GraphicsScene *scene, const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
	bool updateData(CoordinateMarker *item, const QVariant &value, int role=Qt::EditRole);
	bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
	bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());
	QList< QPair<CoordinateMarker*, CoordinateMarker*> > getList();
	QUndoStack *undoStack;
	QList< QPair<CoordinateMarker*, CoordinateMarker*> > listOfMarkerPairs;
	
protected:
	void emitDataChanged(const QModelIndex &index1, const QModelIndex &index2);
	void computeMapping();
	CoordinateMarker *defaultMarker;
	
signals:
	void compute();
	
};



#endif