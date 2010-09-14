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

#include <QtGui>
#include "Commands.h"

/*
AddCommand::AddCommand(GraphicsScene *graphicsScene, const QVariant &value, CoordinateModel *model)
: QUndoCommand()
{
	// Initialize attributes
	scene = graphicsScene;
	initialPosition = value;
	dataModel = model;
	
	// Instantiate a CoordinateMarker object
	marker = new CoordinateMarker(scene->markerRadius);
}


AddCommand::~AddCommand()
{}

void AddCommand::undo()
{
	// Determine the number of rows in the data model
	int numrows = dataModel->rowCount(QModelIndex());
	
	// Initialize some indices
	QModelIndex index1;
	QModelIndex index2;
	
	// Remove datum from model
	if (scene->reference)
	{
		// Datum coming from FITS scene, so remove a row
		dataModel->removeRows(numrows-1, 1, QModelIndex());
		index1 = dataModel->index(numrows-1, 0);
		index2 = dataModel->index(numrows-1, 1);
		dataModel->emitDataChanged(index1, index2);
		
		// Call function that sends signal to ComputeWCS object
		dataModel->computeMapping();
	}
	else
	{
		// Clear only the index (0, 1)
		QPair<QPointF, QPointF> p = dataModel->listOfCoordinatePairs.value(numrows-1);
		p.second = QPointF(-1, -1);
		index1 = dataModel->index(numrows-1, 0);
		index2 = dataModel->index(numrows-1, 1);
		dataModel->listOfCoordinatePairs.replace(numrows-1, p);
	}
	
	dataModel->emitDataChanged(index1, index2);
	dataModel->computeMapping();
	
	// Remove marker from scene and adjust some parameters
	scene->removeItem(marker);
	scene->toggleClickable(true);
	scene->update();
}

void AddCommand::redo()
{	
	// Determine the number of rows in the data model
	int numrows = dataModel->rowCount(QModelIndex());
	
	// Initialize some indices
	QModelIndex index1;
	QModelIndex index2;
	
	if (scene->reference)
	{
		// Insert a row
		dataModel->insertRows(numrows, 1, QModelIndex());
		
		// Access the data (remember AddCommand is a friend of CoordinateModel)
		QPair<QPointF, QPointF> p = dataModel->listOfCoordinatePairs.value(numrows);
		
		p.first = initialPosition.toPointF();
		index1 = dataModel->index(numrows, 0);
		index2 = dataModel->index(numrows, 1);
		
		dataModel->listOfCoordinatePairs.replace(numrows, p);
		marker->row = numrows;
	}
	else
	{
		// Access the data (remember AddCommand is a friend of CoordinateModel)
		QPair<QPointF, QPointF> p = dataModel->listOfCoordinatePairs.value(numrows-1);
		
		p.second = initialPosition.toPointF();
		index1 = dataModel->index(numrows-1, 0);
		index2 = dataModel->index(numrows-1, 1);
		dataModel->listOfCoordinatePairs.replace(numrows-1, p);
		marker->row = numrows-1;
	}
	
	dataModel->emitDataChanged(index1, index2);
	dataModel->computeMapping();
	
	// Add marker to scene and adjust some parameters
	scene->addItem(marker);
	marker->setPos(initialPosition.toPointF());
	scene->toggleClickable(true);
	scene->clearSelection();
	scene->update();
}


MoveCommand::MoveCommand(CoordinateMarker *item, const QVariant &value, CoordinateModel *model)
: QUndoCommand()
{
	marker = item;
	newPos = marker->scenePos();
	oldPos = value;
	dataModel = model;
}

bool MoveCommand::mergeWith(const QUndoCommand *command)
{
	const MoveCommand *moveCommand = static_cast<const MoveCommand *>(command);
	CoordinateMarker *item = moveCommand->marker;
	
	if (marker != item)
		return false;
	
	newPos = item->scenePos();
	return true;
}

void MoveCommand::undo()
{
	// Get the scene
	GraphicsScene *scene = qobject_cast<GraphicsScene*> (marker->scene());
	int row = marker->row;
	
	// Initialize some indices
	QModelIndex index1;
	QModelIndex index2;
	
	// Get data from model
	QPair<QPointF, QPointF> p = dataModel->listOfCoordinatePairs.value(row);
	
	if (scene->reference)
	{
		p.first = oldPos.toPointF();
		dataModel->listOfCoordinatePairs.replace(row, p);
		index1 = dataModel->index(row, 0);
		index2 = dataModel->index(row, 1);
	}
	else
	{
		p.second = oldPos.toPointF();
		dataModel->listOfCoordinatePairs.replace(row, p);
		index1 = dataModel->index(row, 2);
		index2 = dataModel->index(row, 3);
	}
	
	dataModel->emitDataChanged(index1, index2);
	dataModel->computeMapping();
	
	// Move marker to old position
	marker->setPos(oldPos.toPointF());
	marker->scene()->update();
}

void MoveCommand::redo()
{
	// Get the scene
	GraphicsScene *scene = qobject_cast<GraphicsScene*> (marker->scene());
	int row = marker->row;
	
	// Initialize some indices
	QModelIndex index1;
	QModelIndex index2;
	
	// Get data from model
	QPair<QPointF, QPointF> p = dataModel->listOfCoordinatePairs.value(row);
	
	if (scene->reference)
	{
		p.first = newPos.toPointF();
		dataModel->listOfCoordinatePairs.replace(row, p);
		index1 = dataModel->index(row, 0);
		index2 = dataModel->index(row, 1);
	}
	else
	{
		p.second = newPos.toPointF();
		dataModel->listOfCoordinatePairs.replace(row, p);
		index1 = dataModel->index(row, 2);
		index2 = dataModel->index(row, 3);
	}
	
	dataModel->emitDataChanged(index1, index2);
	dataModel->computeMapping();
	
	// Move the marker to the new position
	marker->setPos(newPos.toPointF());
}

*/

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/


AddCommand2::AddCommand2(GraphicsScene *graphicsScene, const QVariant &value, CoordinateModel2 *model)
: QUndoCommand()
{
	// Initialize attributes
	scene = graphicsScene;
	initialPosition = value;
	dataModel = model;
	
	// Instantiate a CoordinateMarker object
	marker = new CoordinateMarker(scene->markerRadius);
}


AddCommand2::~AddCommand2()
{}

void AddCommand2::undo()
{
	// Determine the number of rows in the data model
	int numrows = dataModel->rowCount(QModelIndex());
	
	// Initialize some indices
	QModelIndex index1;
	QModelIndex index2;
	
	// Remove datum from model
	if (scene->reference)
	{
		// Datum coming from FITS scene, so remove a row
		dataModel->removeRows(numrows-1, 1, QModelIndex());
		index1 = dataModel->index(numrows-1, 0);
		index2 = dataModel->index(numrows-1, 1);
		dataModel->emitDataChanged(index1, index2);
		
		// Call function that sends signal to ComputeWCS object
		dataModel->computeMapping();
	}
	else
	{
		// Clear only the index (0, 1)
		QPair<CoordinateMarker*, CoordinateMarker*> p = dataModel->listOfMarkerPairs.value(numrows-1);
		p.second = new CoordinateMarker();
		index1 = dataModel->index(numrows-1, 0);
		index2 = dataModel->index(numrows-1, 1);
		dataModel->listOfMarkerPairs.replace(numrows-1, p);
	}
	
	dataModel->emitDataChanged(index1, index2);
	dataModel->computeMapping();
	
	// Remove marker from scene and adjust some parameters
	scene->removeItem(marker);
	scene->toggleClickable(true);
	scene->update();
}

void AddCommand2::redo()
{	
	// Determine the number of rows in the data model
	int numrows = dataModel->rowCount(QModelIndex());
	
	// Initialize some indices
	QModelIndex index1;
	QModelIndex index2;
	
	if (scene->reference)
	{
		// Insert a row
		dataModel->insertRows(numrows, 1, QModelIndex());
		
		// Access the data (remember AddCommand is a friend of CoordinateModel)
		QPair<CoordinateMarker*, CoordinateMarker*> p = dataModel->listOfMarkerPairs.value(numrows);
		
		p.first = new CoordinateMarker();
		p.first->setPos(initialPosition.toPointF());
		index1 = dataModel->index(numrows, 0);
		index2 = dataModel->index(numrows, 1);
		
		dataModel->listOfMarkerPairs.replace(numrows, p);
		marker->row = numrows;
	}
	else
	{
		// Access the data (remember AddCommand is a friend of CoordinateModel)
		QPair<CoordinateMarker*, CoordinateMarker*> p = dataModel->listOfMarkerPairs.value(numrows-1);
		
		p.second = new CoordinateMarker();
		p.second->setPos(initialPosition.toPointF());
		index1 = dataModel->index(numrows-1, 0);
		index2 = dataModel->index(numrows-1, 1);
		dataModel->listOfMarkerPairs.replace(numrows-1, p);
		marker->row = numrows-1;
	}
	
	dataModel->emitDataChanged(index1, index2);
	dataModel->computeMapping();
	
	// Add marker to scene and adjust some parameters
	scene->addItem(marker);
	marker->setPos(initialPosition.toPointF());
	scene->toggleClickable(true);
	scene->clearSelection();
	scene->update();
}


MoveCommand2::MoveCommand2(CoordinateMarker *item, const QVariant &value, CoordinateModel2 *model)
: QUndoCommand()
{
	qDebug() << "New MoveCommand2";
	marker = item;
	newPos = marker->scenePos();
	oldPos = value;
	dataModel = model;
}

bool MoveCommand2::mergeWith(const QUndoCommand *command)
{
	const MoveCommand2 *moveCommand = static_cast<const MoveCommand2 *>(command);
	CoordinateMarker *item = moveCommand->marker;
	
	if (marker != item)
		return false;
	
	newPos = item->scenePos();
	return true;
}

void MoveCommand2::undo()
{
	// Get the scene
	GraphicsScene *scene = qobject_cast<GraphicsScene*> (marker->scene());
	int row = marker->row;
	
	// Initialize some indices
	QModelIndex index1;
	QModelIndex index2;
	
	// Get data from model
	QPair<CoordinateMarker*, CoordinateMarker*> p = dataModel->listOfMarkerPairs.value(row);
	
	if (scene->reference)
	{
		p.first->setPos(oldPos.toPointF());
		dataModel->listOfMarkerPairs.replace(row, p);
		index1 = dataModel->index(row, 0);
		index2 = dataModel->index(row, 1);
	}
	else
	{
		p.second->setPos(oldPos.toPointF());
		dataModel->listOfMarkerPairs.replace(row, p);
		index1 = dataModel->index(row, 2);
		index2 = dataModel->index(row, 3);
	}
	
	dataModel->emitDataChanged(index1, index2);
	dataModel->computeMapping();
	
	// Move marker to old position
	marker->setPos(oldPos.toPointF());
	marker->scene()->update();
}

void MoveCommand2::redo()
{
	// Get the scene
	GraphicsScene *scene = qobject_cast<GraphicsScene*> (marker->scene());
	int row = marker->row;
	
	// Initialize some indices
	QModelIndex index1;
	QModelIndex index2;
	
	// Get data from model
	QPair<CoordinateMarker*, CoordinateMarker*> p = dataModel->listOfMarkerPairs.value(row);
	
	if (scene->reference)
	{
		p.first->setPos(newPos.toPointF());
		dataModel->listOfMarkerPairs.replace(row, p);
		index1 = dataModel->index(row, 0);
		index2 = dataModel->index(row, 1);
	}
	else
	{
		p.second->setPos(newPos.toPointF());
		dataModel->listOfMarkerPairs.replace(row, p);
		index1 = dataModel->index(row, 2);
		index2 = dataModel->index(row, 3);
	}
	
	dataModel->emitDataChanged(index1, index2);
	dataModel->computeMapping();
	
	// Move the marker to the new position
	marker->setPos(newPos.toPointF());
}