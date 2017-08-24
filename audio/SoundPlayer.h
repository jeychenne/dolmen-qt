/*
 * SoundPlayer.h
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
 * Purpose: audio output
 * Created: 16/04/2011
 */

#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <QThread>
#include "audio/soundio.h"

class SoundPlayer: public QThread
{
	Q_OBJECT
public:
	SoundPlayer(QObject *parent, CallbackData *data);
	~SoundPlayer();
	void run();
	bool isPaused();

signals:
	void currentTime(double); // indicate time while playing

public slots:
	void interrupt();
	void pause();
	void resume();

private slots:
	void stop();

private:
	CallbackData *data;
	RtAudio::StreamParameters *m_params;
	RtAudio::StreamOptions *m_options;
	RtAudio *m_stream;

	void initializeResampling();
	void finalizeResampling();
};

#endif // SOUNDPLAYER_H
