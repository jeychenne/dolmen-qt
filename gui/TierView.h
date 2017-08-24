/*
 * TierView.h
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
 * Purpose: GUI representation of a tier.
 * Created: 08/08/2011
 */

#ifndef TIERVIEW_H
#define TIERVIEW_H

#include <QMouseEvent>

#include "gui/TierFrame.h"
#include "gui/ItemTextEdit.h"
#include "gui/ItemView.h"
#include "lib/DMSelection.h"
#include "dolmen.h"


class TierView : public TierFrame
{
	Q_OBJECT

public:
	TierView(Tier *tier, int index);
	~TierView();

	void updateWindow(double start, double end);
	DMSelection * window() const;
	ItemView * findItemView(Item *i);
	void signalSelectedItem(ItemView *i);

signals:
	void updateSelectedItem(ItemView*);			// reset selected item on resize
	void selectItem(ItemView*);
	void soundSelection(int xstart, int xend); // show selection in sound plot
	void hideAllTextEditors();					// request all other tiers to hide their selection
	void modifySelectedItemText(const QString &);

public slots:
	void hideTextEditor();
	void redrawTextEdit(ItemView *itemView, const QString &text);	// update on redraw

private slots:
	void modifyItemText();
    void abandonItemChanges();

protected:
	void resizeEvent(QResizeEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);

private:
    QList<ItemView*>	  m_itemViews;						//TODO: points...
    DMSelection         * m_window;						// current window being displayed
    ItemTextEdit		* m_textEditor;
    int					  boundaryPosition;
    ItemBoundary		* selectedBoundary;

    void	       displayItems();
    bool	       fitsWindow(Item *) const;
    int		       time2pos(double time) const;
    double	       pos2time(int pos) const;
    ItemView     * itemAtPos(int pos) const;
    void	       editItemText(ItemView *i);
    void	       setTextEditor();
    ItemBoundary * boundaryClickedAt(int pos) const;

};

#endif // TIERVIEW_H
