/*
 * Sound.h
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
 * Purpose: implement sound files
 * Created: 10/09/2010
 */

#ifndef SOUND_H
#define SOUND_H

#include <math.h>

#include "lib/DFile.h"
#include "audio/SoundPlayer.h"
#include "utils/routines.h"

typedef QVector<dm_sample_t> FrameVector;

class Sound : public DFile
{
	Q_OBJECT
public:
	Sound(const QString &path);
	~Sound();

	void save();
    void open();
	QString metadataAsHtml(bool withDescription = true) const;
	void writeToTxt(QString path);
	void play(double start = 0.0, double end = 0.0);
	void pause();
	bool isPlaying() const;
	bool isPaused() const;
	int nchannels();
	double length();
	bool isMono();
	bool isStereo();
	FrameVector rawFramesMono();
	QPair<FrameVector,FrameVector> rawFramesStereo();

	size_t time2frame(double t) const;
	double frame2time(size_t f) const;
	int samplerate() const;
    size_t nframes() const;

signals:
	void finished();
	void startLoadingData(int);
	void bufferLoaded(int);
	void currentTime(double); // indicate current time while playing

public slots:
	void stop();

private slots:
	void cleanupThread();

private:
    SndfileHandle *m_sound_file;
    CallbackData *m_callback_data;
	SoundPlayer *m_player;

	void setFileHandle();
	void readFromDmm();
	void writeMetadata(QString path); // write to sound file
	void writeToDmf(QString path);
	void initializeCallbackData(double start, double end);

};

#endif // SOUND_H
