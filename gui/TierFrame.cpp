/*
 * TierFrame.cpp
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
 * Purpose: Base tier frame to avoid mutual dependencies between
 * TierView and ItemView.
 * Created: 12/08/2011
 */

#include "TierFrame.h"

TierFrame::TierFrame(Tier *tier, int index) :
	QFrame()
{
	m_tier   = tier;
	m_index  = index;

	/* draw backgroun in black so that it appears as separators */
	//FIXME: the following doesn't do anything
//	this->setAutoFillBackground(true);
//	QPalette palette = this->palette();
//	palette.setColor(this->backgroundRole(), Qt::black);
//	this->setPalette(palette);

}

int TierFrame::index() const
{
	return m_index;
}
