/*
	Copyright 2006-2026 The QElectroTech Team
	This file is part of QElectroTech.

	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with QElectroTech.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef ROTATETEXTSCOMMAND_H
#define ROTATETEXTSCOMMAND_H

#include <QUndoCommand>
#include <QPointer>
#include <QHash>
#include <QList>

class ConductorTextItem;
class DynamicElementTextItem;
class ElementTextItemGroup;
class Diagram;

/**
	@brief The RotateTextsCommand class
	Open a dialog for edit the rotation of the current selected texts and texts group in diagram.
	Just instantiate this undo command and push it in a QUndoStack.
*/
class RotateTextsCommand : public QUndoCommand
{
	public:
		RotateTextsCommand(Diagram *diagram, QUndoCommand *parent=nullptr);

		void undo() override;
		void redo() override;

	private:
		void openDialog();
			//Re-apply the readability correction to each rotated text/group after
			//the rotation/position is set. Own-rotation change does not otherwise
			//re-fire it. Called synchronously from both redo() and undo().
		void correctSelectedTexts();

	private:
		QPointer<Diagram> m_diagram;
		QHash<ConductorTextItem *, bool> m_cond_texts;
		QList<QPointer<DynamicElementTextItem>> m_texts;
		QList<QPointer<ElementTextItemGroup>> m_groups;
		qreal m_rotation=0;
};

#endif // ROTATETEXTSCOMMAND_H
