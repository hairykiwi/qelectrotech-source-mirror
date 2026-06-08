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
#include "mirrorflipselectioncommand.h"

#include "../diagram.h"
#include "../qetgraphicsitem/element.h"

#include <QGraphicsItem>

MirrorFlipSelectionCommand::MirrorFlipSelectionCommand(Diagram *diagram, Axis axis, QUndoCommand *parent) :
	QUndoCommand(parent),
	m_diagram(diagram),
	m_axis(axis)
{
	setText(axis == Mirror ? QObject::tr("Miroir de la sélection")
						   : QObject::tr("Retourner la sélection"));

	if (m_diagram && !m_diagram->isReadOnly())
	{
		for (QGraphicsItem *item : m_diagram->selectedItems())
			if (item->type() == Element::Type)
				m_elements << static_cast<Element *>(item);
	}
}

/**
	@brief MirrorFlipSelectionCommand::toggle
	Toggle the mirror/flip state on every (still alive) selected element.
*/
void MirrorFlipSelectionCommand::toggle()
{
	for (const QPointer<Element> &e : m_elements)
	{
		if (!e)
			continue;
		if (m_axis == Mirror)
			e->setMirror(!e->isMirrored());
		else
			e->setFlip(!e->isFlipped());
	}
}

/**
	@brief MirrorFlipSelectionCommand::redo
*/
void MirrorFlipSelectionCommand::redo()
{
	if (m_diagram)
		m_diagram->showMe();
	toggle();
}

/**
	@brief MirrorFlipSelectionCommand::undo
	Toggling is its own inverse.
*/
void MirrorFlipSelectionCommand::undo()
{
	if (m_diagram)
		m_diagram->showMe();
	toggle();
}

/**
	@brief MirrorFlipSelectionCommand::isValid
	@return true if at least one element will be transformed.
*/
bool MirrorFlipSelectionCommand::isValid() const
{
	return !m_elements.isEmpty();
}
