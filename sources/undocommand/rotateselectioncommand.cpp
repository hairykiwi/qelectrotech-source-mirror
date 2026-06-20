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
#include "rotateselectioncommand.h"

#include "../QPropertyUndoCommand/qpropertyundocommand.h"
#include "../diagram.h"
#include "../qet.h"
#include "../qetgraphicsitem/conductor.h"
#include "../qetgraphicsitem/conductortextitem.h"
#include "../qetgraphicsitem/diagramimageitem.h"
#include "../qetgraphicsitem/dynamicelementtextitem.h"
#include "../qetgraphicsitem/element.h"
#include "../qetgraphicsitem/elementtextitemgroup.h"
#include "../qetgraphicsitem/independenttextitem.h"
#include "rotationpivot.h"

#include <QGraphicsItem>

RotateSelectionCommand::RotateSelectionCommand(Diagram *diagram, qreal angle, QUndoCommand *parent) :
QUndoCommand(parent),
m_diagram(diagram)
{
	setText(QObject::tr("Pivoter la selection"));
	
	if(!m_diagram->isReadOnly())
	{
			//Directly-selected text/group rotation+pos commands are collected here
			//and kept synchronous (below) so the redo() readability correction
			//reads the settled rotation, not a mid-animation value.
		QList<QPropertyUndoCommand*> sync_undo;
		for (QGraphicsItem *item : m_diagram->selectedItems())
		{
			switch (item->type())
			{
				case Element::Type:
					m_undo << new QPropertyUndoCommand(item->toGraphicsObject(), "rotation", QVariant(item->rotation()), QVariant(item->rotation()+angle), this);
					break;
				case ConductorTextItem::Type:
				{
					m_cond_text << static_cast<ConductorTextItem *>(item);
					m_undo << new QPropertyUndoCommand(item->toGraphicsObject(), "rotation", QVariant(item->rotation()), QVariant(item->rotation()+angle), this);
				}
					break;
				case IndependentTextItem::Type:
					m_undo << new QPropertyUndoCommand(item->toGraphicsObject(), "rotation", QVariant(item->rotation()), QVariant(item->rotation()+angle), this);
					break;
				case DynamicElementTextItem::Type:
				{
					if(item->parentItem() && !item->parentItem()->isSelected())
					{
						QPropertyUndoCommand *rot = new QPropertyUndoCommand(item->toGraphicsObject(), "rotation", QVariant(item->rotation()), QVariant(item->rotation()+angle), this);
							//Compensate position so the text pivots about its own
							//bounding-rect center, matching the readability correction's
							//pivot (see centerPivotEndPos); undo reverses via the stored
							//old value.
						QPropertyUndoCommand *pos = new QPropertyUndoCommand(item->toGraphicsObject(), "pos", QVariant(item->pos()), QVariant(centerPivotEndPos(item, item->rotation()+angle)), this);
						m_undo << rot << pos;
						sync_undo << rot << pos;
						m_corr_texts << static_cast<DynamicElementTextItem *>(item);
					}
				}
					break;
				case QGraphicsItemGroup::Type:
				{
					if(ElementTextItemGroup *grp = dynamic_cast<ElementTextItemGroup *>(item))
						if(grp->parentElement() && !grp->parentElement()->isSelected())
						{
							QPropertyUndoCommand *rot = new QPropertyUndoCommand(grp, "rotation", QVariant(item->rotation()), QVariant(item->rotation()+angle), this);
							QPropertyUndoCommand *pos = new QPropertyUndoCommand(grp, "pos", QVariant(grp->pos()), QVariant(centerPivotEndPos(grp, grp->rotation()+angle)), this);
							m_undo << rot << pos;
							sync_undo << rot << pos;
							m_corr_groups << grp;
						}
				}
					break;
				case DiagramImageItem::Type:
					m_undo << new QPropertyUndoCommand(item->toGraphicsObject(), "rotation", QVariant(item->rotation()), QVariant(item->rotation()+angle), this);
					break;
				default:
					break;
			}
		}
		
		for (QPropertyUndoCommand *undo : m_undo)
			undo->setAnimated(true, false);
			//Override: directly-selected text/group commands stay synchronous so
			//redo (initial AND re-redo) applies the rotation before the readability
			//correction reads it; element/conductor/image branches keep animation.
		for (QPropertyUndoCommand *undo : sync_undo)
			undo->setAnimated(false);
	}
}

/**
	@brief RotateSelectionCommand::undo
*/
void RotateSelectionCommand::undo()
{
	m_diagram->showMe();
	QUndoCommand::undo();
	
	for(const QPointer<ConductorTextItem>& cti : m_cond_text)
	{
		cti->forceRotateByUser(m_rotate_by_user.value(cti.data()));
		if(!cti->wasRotatedByUser())
			cti->parentConductor()->calculateTextItemPosition();
	}

	correctSelectedTexts();
}

/**
	@brief RotateSelectionCommand::redo
*/
void RotateSelectionCommand::redo()
{
	m_diagram->showMe();
	QUndoCommand::redo();
	
		for(const QPointer<ConductorTextItem>& cti : m_cond_text)
		{
			m_rotate_by_user.insert(cti, cti->wasRotatedByUser());
			cti->forceRotateByUser(true);
		}

	correctSelectedTexts();
}

/**
	@brief RotateSelectionCommand::correctSelectedTexts
	Re-apply the readability correction (and, under mirror/flip, the reflection
	compensation) to each directly-selected text/group. Own-rotation change does
	not otherwise re-fire it. Called on BOTH redo and undo: compensate sets an
	absolute item transform that the rotation/pos reversal does not restore, so
	undo must recompute it for the reverted rotation.
*/
void RotateSelectionCommand::correctSelectedTexts()
{
	for(const QPointer<DynamicElementTextItem>& deti : m_corr_texts)
		if(deti && deti->parentElement())
			deti->parentElement()->correctTextReadability(deti);
	for(const QPointer<ElementTextItemGroup>& grp : m_corr_groups)
		if(grp && grp->parentElement())
			grp->parentElement()->correctTextReadability(grp);
}

/**
	@brief RotateSelectionCommand::isValid
	@return true if this command rotate a least one item.
*/
bool RotateSelectionCommand::isValid()
{
	if(childCount())
		return true;
	else
		return false;
}
