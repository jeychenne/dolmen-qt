/*
 * dolmen.h
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
 * Purpose:
 * Created: //
 */

#include "SoundView.h"

SoundView::SoundView(QTabWidget *parent, Sound *snd):
		View(parent)
{
	this->m_sound = snd;
	QFileInfo info(snd->path());
	int index = parent->indexOf(this);
	parent->setTabText(index, info.fileName());

    layout->addSpacing(5);
	this->setWaveform();
	//this->setMetadata();
    layout->addStretch();

	parent->setCurrentWidget(this);
}

void SoundView::editMetadata()
{
	MetadataEditor *ed = new MetadataEditor(this, m_sound);
	ed->show();
}

void SoundView::setWaveform()
{
	m_wave = new SignalBlock(this, m_sound);
	layout->addLayout(m_wave);

    connect(m_wave, SIGNAL(editMetadata()), this, SLOT(editMetadata()));
}

QString SoundView::id() const
{
	return m_sound->path();
}

void SoundView::leave()
{
	/* clear output */
	emit displayText("");
}

SoundView::~SoundView()
{
    //delete m_wave;
}
