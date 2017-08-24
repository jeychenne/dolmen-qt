/*
 * SoundView.h
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
 * Purpose:
 * Created: 01/03/2011
 */

#ifndef SOUNDVIEW_H
#define SOUNDVIEW_H

#include "lib/Sound.h"
#include "gui/View.h"
#include "gui/MetadataEditor.h"
#include "gui/blocks/SignalBlock.h"

class SoundView: public View
{
	Q_OBJECT
public:
	SoundView(QTabWidget *parent, Sound *snd);
	~SoundView();
	void leave();
	QString id() const;

private slots:
	void editMetadata();

private:
    Sound       *m_sound;
	SignalBlock *m_wave;

	void setWaveform();
};


#endif // SOUNDVIEW_H

