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

#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include <QVariant>
#include "GraphicsScene.h"
#include "CoordMarker.h"
#include "CoordinateModel.h"

class AddCommand : public QUndoCommand
{
	
public:
	AddCommand(GraphicsScene *graphicsScene, const QModelIndex &index, const QVariant &value, CoordinateModel *model, QUndoCommand *parent);
	~AddCommand();
	void undo();
	void redo();
	
private:
	CoordMarker *marker;
	GraphicsScene *scene;
	QVariant initialPosition;
	CoordinateModel *dataModel;
	int row, column;
};



/*
class AddCommand : public QUndoCommand
{
	
public:
	AddCommand(GraphicsScene *graphicsScene, QPointF position, QUndoCommand *parent = 0);
//	AddCommand(GraphicsScene *graphicsScene, const QModelIndex &index, const QVariant &value, CoordinateModel *m, QUndoCommand *parent);
	~AddCommand();
	void undo();
	void redo();
	
private:
	CoordMarker *marker;
	GraphicsScene *scene;
	QPointF initialPosition;
//	CoordinateModel *model;
};


class MoveCommand : public QUndoCommand
{
	
public:
	enum { Id = 1234 };
	
	MoveCommand(CoordMarker *item, const QPointF &oldPosition, QUndoCommand *parent = 0);
	void undo();
	void redo();
	bool mergeWith(const QUndoCommand *command);
	int id() const { return Id; }
	
private:
	CoordMarker *marker;
	QPointF oldPos;
	QPointF newPos;
//	CoordinateModel *model;
};
*/

#endif