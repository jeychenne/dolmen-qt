/*
 * soundio.h
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
 * Purpose: low-level sound handling
 * Created: 16/04/2011
 */

#ifndef SOUNDIO_H
#define SOUNDIO_H

#include <QObject>
#include <QVector>
#include <QDebug>

#ifdef Q_OS_WIN
#define ENABLE_SNDFILE_WINDOWS_PROTOTYPES
#endif

#include <sndfile.hh>
#include <speex/speex_resampler.h>
#include <audio/RtAudio.h>


//FIXME: with a buffer size of 1024, there are glitches on my 2012 MacBook Pro (8,3)
#ifdef Q_OS_MAC
const int BUFFER_SIZE = 4096;
#else
const int BUFFER_SIZE = 2048;
#endif
/*
On Mac OS X, default output is stereo 44100Hz and no automatic resampling is performed.
We use Speex to resample the audio when necessary.
*/
#ifdef Q_OS_WIN
	#define DM_SOUND_API RtAudio::WINDOWS_DS
#elif defined(Q_OS_MAC)
    #define DM_SOUND_API RtAudio::MACOSX_CORE
	#define MAC_SAMPLE_RATE 44100.0
#else
    #define DM_SOUND_API RtAudio::LINUX_ALSA
#endif

#define MIN(a,b) ((a) < (b) ? (a) : (b))
//#define MAX(a,b) ((a) > (b) ? (a) : (b))

// sample size used when displaying a sound file
typedef short dm_sample_t;



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// The callback data is used to pass information from the Sound object to the
// playback function.
class CallbackData : public QObject
{
	Q_OBJECT

public:
    CallbackData(SndfileHandle &fh, sf_count_t startf, sf_count_t endf);
    SndfileHandle	sndfile;
    RtAudio *stream = nullptr;
	double			outputrate;
	int				nchannels; // output parameters
	sf_count_t		start_frame, end_frame, position, length;
    float           *buffer;
	bool paused;

    void update(); // send signal

    bool needsResampling() const;

    SpeexResamplerState *resampler;
	double ratio;

	void stop(); // emits finished();

    int inputRate() const;

signals:
	void finished();
	void currentTime(double t);
};


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int playback(void *outputBuffer, void *inputBuffer, unsigned int nframes,
		double streamTime, RtAudioStreamStatus status, void *userData);



#endif // SOUNDIO_H
