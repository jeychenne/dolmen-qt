/*
 * HideableBlock.h
 *
 * Copyright (C) 2010-2011 Julien Eychenne
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
 * Purpose: Define a generic QVBoxLayout that can be hidden with an arrow button
 * Created: 01/03/2011
 */


#ifndef HIDEABLEBLOCK_H
#define HIDEABLEBLOCK_H

#include <QVBoxLayout>
#include <QToolButton>
#include <QLabel>

class HideableBlock : public QVBoxLayout
{
	Q_OBJECT

public:
    HideableBlock();

protected slots:
	void onArrowClicked();

protected:
	QToolButton *showHide_arrow;
	QLabel *header_label; // show/hide text at the top of the block

	// children implement how their widgets should be shown/hidden
	void virtual hideWidgets() = 0;
	void virtual showWidgets() = 0;
};

#endif // HIDEABLEBLOCK_H
