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
#ifndef MIRRORFLIPSELECTIONCOMMAND_H
#define MIRRORFLIPSELECTIONCOMMAND_H

#include <QUndoCommand>
#include <QPointer>
#include <QList>

class Diagram;
class Element;

/**
	@brief The MirrorFlipSelectionCommand class
	Toggle the folio-level mirror (horizontal) or flip (vertical) state of the
	selected elements in the given diagram. Toggling is its own inverse, so undo
	simply re-applies the toggle.
*/
class MirrorFlipSelectionCommand : public QUndoCommand
{
	public:
		enum Axis {Mirror, Flip};

		MirrorFlipSelectionCommand(Diagram *diagram, Axis axis, QUndoCommand *parent=nullptr);
		void undo() override;
		void redo() override;

		bool isValid() const;

	private:
		void toggle();

		Diagram *m_diagram = nullptr;
		Axis m_axis;
		QList<QPointer<Element>> m_elements;
};

#endif // MIRRORFLIPSELECTIONCOMMAND_H
