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
#include "Commands.h"


AddCommand::AddCommand(GraphicsScene *graphicsScene, const QVariant &value, CoordinateModel *model)
: QUndoCommand()
{
	// Initialize attributes
	scene = graphicsScene;
	initialPosition = value;
	dataModel = model;
}


AddCommand::~AddCommand()
{}

void AddCommand::redo()
{	
	// Initialize some indices
	QModelIndex index1;
	QModelIndex index2;
	QPointF coord;
	int row;
	
	// Determine the number of rows in the data model
	int numrows = dataModel->rowCount(QModelIndex());
	
	if (scene->reference)
	{
		// Insert a single row
		dataModel->insertRows(numrows, 1, QModelIndex());
		
		// Access the data
		coord = dataModel->refCoords.value(numrows);
		
		// Set the reference coordinate and indices
		coord = initialPosition.toPointF();
		index1 = dataModel->index(numrows, 0);
		index2 = dataModel->index(numrows, 1);
		row = numrows;
		
		// Update the data
		dataModel->refCoords.replace(numrows, coord);
	}
	else
	{
		// Access the data
		coord = dataModel->epoCoords.value(numrows-1);
		
		// Set the epo coordinate and indices
		coord = initialPosition.toPointF();
		index1 = dataModel->index(numrows-1, 2);
		index2 = dataModel->index(numrows-1, 3);
		row = numrows-1;
		
		// Update the data
		dataModel->epoCoords.replace(numrows-1, coord);
	}
	
	// FIXME: Checking QModelIndex values
//	qDebug() << index1 << index2;
	
	// TODO: Add a QModelIndex as an attribute to the CoordinateMarker
	// How shall this be done???
	// Need to figure out what the index will be.
	// This is a little confusing since the way the data is stored.  Two table items refer to one CoordinateMarker.
	// Determine which row the marker belongs to, then determine the column (i.e. reference or epo)
	// Store the model index with row and either 1 or 3 refering to reference or epo, respectively.
	
	// Initialize CoordinateMarker and add to GraphicScene
	marker = new CoordinateMarker(index1, scene->centralItem);
	marker->setPos(initialPosition.toPointF());
	scene->toggleClickable(true);
	scene->clearSelection();
	scene->update();
	
	// Broadcast to ComputeWCS
	dataModel->compute();
	
	// Broadcast some info
	dataModel->emitDataChanged(index1, index2);
	
	// FIXME: Checking data model
	qDebug() << dataModel->refCoords;
	qDebug() << dataModel->epoCoords;
}


void AddCommand::undo()
{
	// Initialize some indices
	QModelIndex index1;
	QModelIndex index2;
	
	// Determine the number of rows in the data model
	int numrows = dataModel->rowCount(QModelIndex());
	
	// Remove datum from model
	if (scene->reference)
	{
		// Datum coming from FITS scene; remove an entire row
		dataModel->removeRows(numrows-1, 1, QModelIndex());
		index1 = dataModel->index(numrows-1, 0);
		index2 = dataModel->index(numrows-1, 1);
		dataModel->emitDataChanged(index1, index2);
	}
	else
	{
		// Access the data
		QPointF coord = dataModel->epoCoords.value(numrows-1);
		coord = QPointF(-1, -1);
		index1 = dataModel->index(numrows-1, 0);
		index2 = dataModel->index(numrows-1, 1);
		dataModel->epoCoords.replace(numrows-1, coord);
	}
	
	// Remove CoordinateMarker from GraphicsScene
	scene->removeItem(marker);
	scene->toggleClickable(true);
	scene->update();
	
	// Broadcast to ComputeWCS
	dataModel->compute();
	
	// Broadcast some info
	dataModel->emitDataChanged(index1, index2);
	
	// FIXME: Checking data model
	qDebug() << dataModel->refCoords;
	qDebug() << dataModel->epoCoords;
}



MoveCommand::MoveCommand(GraphicsScene *s, const QVariant &newValue, const QVariant &oldValue, CoordinateModel *model, QModelIndex *index)
: QUndoCommand()
{
	qDebug() << "MoveCommand";
	newPos = newValue.toPointF();
	oldPos = oldValue.toPointF();
	scene = s;
	marker = qgraphicsitem_cast<CoordinateMarker*>(scene->itemAt(newPos));
	// FIXME: Memory error if a marker is not referenced.
	// FIXME: This occurs when handling data via the table view
	if (marker == 0)
	{
		QList<QGraphicsItem*> markers = scene->selectedItems();
		if (markers.isEmpty())
		{
			// Loop through the CoordinateMarkers, checking the indices
			QList<QGraphicsItem*> markers = scene->items();
			for (int i = 0; i < markers.size(); ++i)
			{
				marker = qgraphicsitem_cast<CoordinateMarker*>(markers.at(i));
				if (marker->index->row() == index->row())
					break;
			}
		}
		else {
			marker = qgraphicsitem_cast<CoordinateMarker*>(scene->selectedItems()[0]);
		}
	}
	qDebug() << "Marker: " << marker;
	dataModel = model;
}

bool MoveCommand::mergeWith(const QUndoCommand *command)
{
	qDebug() << "mergeWith";
	const MoveCommand *moveCommand = static_cast<const MoveCommand*>(command);
	CoordinateMarker *item = moveCommand->marker;
	
	if (marker != item)
		return false;
	
	newPos = item->scenePos();
	return true;
}

void MoveCommand::undo()
{
	qDebug() << "MoveCommand undo()";
	// Initialize some indices
	QModelIndex index1;
	QModelIndex index2;
	
	// Get the row of the data
	int row = marker->index->row();
	
	if (scene->reference)
	{		
		dataModel->refCoords.replace(row, oldPos);
		index1 = dataModel->index(row, 0);
		index2 = dataModel->index(row, 1);
	}
	else
	{
		dataModel->epoCoords.replace(row, oldPos);
		index1 = dataModel->index(row, 2);
		index2 = dataModel->index(row, 3);
	}
	
	// Move marker to old position
	marker->setPos(oldPos);
	marker->show();
	scene->update();
	
	// Broadcast to ComputeWCS
	dataModel->compute();
	
	// Broadcast some info
	dataModel->emitDataChanged(index1, index2);
	
	// FIXME: Checking data model
	qDebug() << dataModel->refCoords;
	qDebug() << dataModel->epoCoords;
}

void MoveCommand::redo()
{
	qDebug() << "MoveCommand redo()";
	// Initialize some variables
	QModelIndex index1;
	QModelIndex index2;
	qDebug() << "one";
	// If the marker is destroyed by AddCommand::undo(), this code will find the marker using coordinates (shoddy...)
	if (scene->itemAt(oldPos) != 0)
	{
		if (scene->itemAt(oldPos)->pos() != QPointF(0, 0))
			marker = qgraphicsitem_cast<CoordinateMarker*>(scene->itemAt(oldPos));
	} else if(scene->selectedItems()[0]->pos() != QPointF(0, 0))
		marker = qgraphicsitem_cast<CoordinateMarker*>(scene->selectedItems()[0]);
	
	int row = marker->index->row();
	
	if (scene->reference)
	{       
		dataModel->refCoords.replace(row, newPos);
		index1 = dataModel->index(row, 0);
		index2 = dataModel->index(row, 1);
	}
	else
	{
		dataModel->epoCoords.replace(row, newPos);
		index1 = dataModel->index(row, 2);
		index2 = dataModel->index(row, 3);
	}
	
	// Move the marker to the new position
	marker->setPos(newPos);
	marker->update();
	scene->update();
	
	// Broadcast to ComputeWCS
	dataModel->compute();
	
	// Broadcast some info
	dataModel->emitDataChanged(index1, index2);
	
	// FIXME: Checking data model
	qDebug() << dataModel->refCoords;
	qDebug() << dataModel->epoCoords;
}
