/*
 * ItemView.h
 *
 * Copyright (C) 2010-2013 Julien Eychenne
 *
 * This file is part of Dolmen.
 *
 * Dolmen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License (GPL) as
 * published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * Dolmen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Dolmen. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:  Julien Eychenne
 * Purpose: classes to represent spans and points in the GUI
 * Created: 09/08/2011
 */

#ifndef UIITEM_H
#define UIITEM_H

#include <QLabel>
#include <QPair>

#include "TierFrame.h"
#include "ItemBoundary.h"


class ItemView
{
public:
	ItemView(TierFrame *tier, Item *item, SpanBoundary *sharedBoundary = NULL);
	~ItemView();

	QRect			textGeometry() const;
	void			hide();
	void			show();
	void			display(int left, int right);
	void			setSelected(bool selected);
	bool			isSelected() const;
	TierFrame*		tierView() const;
	Item*			item() const;
	QPair<int,int>	screenPos() const;
	QString			toHtml() const;
	bool			setText(const QString &); // returns whether the text has been modified
	ItemBoundary*	boundaryAt(int pos) const;
	SpanBoundary *	sharedBoundaryWithNextItem(double tstart);
	void			moveLeftBoundary(int x);
	void			moveRightBoundary(int x);



private:
	Item			*m_item;
	TierFrame		*m_tierFrame;
	SpanBoundary	*start_boundary, *end_boundary;
	PointBoundary	*timeBoundary;
	QLabel			*m_text;
	bool			m_selected, is_span, shares_start_boundary;

	void			moveBoundary(int x);



};


#endif // UIITEM_H
