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
#ifndef ROTATESELECTIONCOMMAND_H
#define ROTATESELECTIONCOMMAND_H

#include <QUndoCommand>
#include <QPointer>
#include <QHash>

class Diagram;
class ConductorTextItem;
class DynamicElementTextItem;
class ElementTextItemGroup;
class QPropertyUndoCommand;

/**
	@brief The RotateSelectionCommand class
	Rotate the selected items in the given diagram
*/
class RotateSelectionCommand : public QUndoCommand
{
	public:
		RotateSelectionCommand(Diagram *diagram, qreal angle=90, QUndoCommand *parent=nullptr);
		void undo() override;
		void redo() override;

		bool isValid();

	private:
		void correctSelectedTexts();

		Diagram *m_diagram =nullptr;

		QList<QPointer<ConductorTextItem>> m_cond_text;
		QHash<ConductorTextItem *, bool> m_rotate_by_user;
		QList<QPropertyUndoCommand*> m_undo;

			//Directly-selected element text/groups to re-correct for readability
			//on redo (own-rotation change does not re-fire the correction).
		QList<QPointer<DynamicElementTextItem>> m_corr_texts;
		QList<QPointer<ElementTextItemGroup>> m_corr_groups;

};

#endif // ROTATESELECTIONCOMMAND_H
