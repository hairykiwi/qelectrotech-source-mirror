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
#ifndef ROTATIONPIVOT_H
#define ROTATIONPIVOT_H

#include <QGraphicsItem>
#include <QPointF>
#include <QTransform>

/**
	@brief centerPivotEndPos
	End position so that rotating @p item from its current rotation to
	@p target_rotation pivots about its own bounding-rect center (not the
	default (0,0) corner): keeps the bounding-rect center fixed in the parent
	frame.
	Same stateless math as Element::rotateAboutOwnCentre, for an absolute
	target: endPos = pos + R(own)*C - R(target)*C. This keeps the user
	text-rotate pivot consistent with the text-readability correction applied
	on element rotation/mirror/flip (Element::rotateAboutOwnCentre /
	Element::correctReadability), so user text-rotate no longer drifts
	position under subsequent element rotation.

	Single source of truth for the center-pivot end position, shared by the
	directly-selected text/group rotation paths (RotateTextsCommand and
	RotateSelectionCommand).
*/
inline QPointF centerPivotEndPos(QGraphicsItem *item, qreal target_rotation)
{
	const QPointF c = item->boundingRect().center();
	const qreal   own = item->rotation();
	return item->pos()
		+ QTransform().rotate(own).map(c)
		- QTransform().rotate(target_rotation).map(c);
}

#endif // ROTATIONPIVOT_H
