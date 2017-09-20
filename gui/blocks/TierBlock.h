/*
 * TierBlock.h
 *
 * Copyright (C) 2010-2017 Julien Eychenne
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
 * Purpose: A class a block of tiers. A tier block is bound to a signal block.
 * Created: 08/08/2011
 */

#ifndef TIERBLOCK_H
#define TIERBLOCK_H

#include <QVBoxLayout>

#include "gui/TierView.h"
#include "lib/Annotation.h"
#include "dolmen.h"

class TierBlock : public QVBoxLayout
{
    Q_OBJECT

public:
	TierBlock(Annotation *annot);
	~TierBlock();

signals:
	void soundSelection(int xstart, int xend);
	void displayText(QString);
	void hideTextEditor();
	void tiersModified();

public slots:
	void updateCurrentWindow(DMSelection);
	void selectItem(ItemView *);

private slots:
	void updateSelectedItem(ItemView *i);
	void updateSelectedItemText(const QString &);	// send out to output

private:
	DMSelection		*m_window;				// current window being displayed
	Annotation		*m_annot;
	QList<TierView*>	 m_tiers;
    ItemView			*selected_item;
    Item			*hidden_selected_item;	// selected item outside the window
    int				 tier_no;

	void updateDisplay(double start, double end);

};

#endif // TIERBLOCK_H
