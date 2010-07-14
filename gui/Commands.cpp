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

AddCommand::AddCommand(GraphicsScene *graphicsScene, QPointF position, QUndoCommand *parent)
: QUndoCommand(parent)
{
	scene = graphicsScene;
	marker = new CoordMarker(scene->markerRadius);
	initialPosition = position;
}

AddCommand::~AddCommand()
{}

void AddCommand::undo()
{
	scene->removeItem(marker);
	scene->toggleClickable(true);
	scene->update();
}

void AddCommand::redo()
{
	scene->addItem(marker);
	marker->setPos(initialPosition);
	scene->toggleClickable(true);
	scene->clearSelection();
	scene->update();
}

MoveCommand::MoveCommand(CoordMarker *item, const QPointF &oldPosition, QUndoCommand *parent)
: QUndoCommand(parent)
{
	marker = item;
	newPos = marker->scenePos();
	oldPos = oldPosition;
}

bool MoveCommand::mergeWith(const QUndoCommand *command)
{
	const MoveCommand *moveCommand = static_cast<const MoveCommand *>(command);
	CoordMarker *item = moveCommand->marker;
	
	if (marker != item)
		return false;
	
	newPos = item->scenePos();
	return true;
}

void MoveCommand::undo()
{
	marker->setPos(oldPos);
	marker->scene()->update();
}

void MoveCommand::redo()
{
	marker->setPos(newPos);
}