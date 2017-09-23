/*
 * SoundPlayer.cpp
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
 * Purpose: audio output
 * Created: 16/04/2011
 */

#include "SoundPlayer.h"

/* from 0 = bad to 10 = pro. 3 is said to be fine for desktop*/
#define DM_SOUND_QUALITY 4

SoundPlayer::SoundPlayer(QObject *parent, CallbackData *data): QThread(parent)
{
//	qDebug("init output thread");
	this->data = data;
	m_stream = new RtAudio(DM_SOUND_API);
    data->stream = m_stream;
	m_params = new RtAudio::StreamParameters;
	m_params->deviceId = m_stream->getDefaultOutputDevice();
#ifdef Q_OS_MAC
	m_params->nChannels = 2;
#else
    m_params->nChannels = data->sndfile.channels();
#endif
	m_params->firstChannel = 0;
	m_options = new RtAudio::StreamOptions;
#ifdef Q_OS_LINUX
	m_options->flags = RTAUDIO_ALSA_USE_DEFAULT;
#elif defined(Q_OS_MAC)
    if (data->sndfile.channels() == 1)
        m_options->flags = RTAUDIO_NONINTERLEAVED | RTAUDIO_SCHEDULE_REALTIME;
	else
		m_options->flags = 0;
#else
    m_options->flags = RTAUDIO_SCHEDULE_REALTIME; //0;
#endif

//    moveToThread(this);

    //connect(data, SIGNAL(finished()), this, SLOT(stop()));
	connect(data, SIGNAL(currentTime(double)), this, SIGNAL(currentTime(double)));
}

void SoundPlayer::run()
{
//    qDebug("starting sound thread...");
    unsigned int bufferFrames = BUFFER_SIZE;

	m_stream->openStream(m_params,
						 NULL,
                         RTAUDIO_FLOAT32,
                         data->outputrate,
                         &bufferFrames,
                         &playback,
						 data,
						 m_options);
//    qDebug("Stream opened...");

    //if (data->inputRate() != m_stream->getStreamSampleRate())
    {
        this->initializeResampling();
    }

//    qDebug("getting device info");
//    RtAudio::DeviceInfo info = m_stream->getDeviceInfo(m_stream->getDefaultOutputDevice());
//    for (int i = 0; i < info.sampleRates.size(); ++i)
//    {
//        qDebug("Supported rate: %d", info.sampleRates[i]);
//    }
//    qDebug("starting stream");
	m_stream->startStream();

    qDebug("Output rate: %d", m_stream->getStreamSampleRate());
    qDebug("Input rate: %d", data->sndfile.samplerate());

    while (m_stream->isStreamRunning())
        QThread::msleep(2); // wait 2 ms

//    qDebug("stopping stream");
//    this->stop();

//    qDebug("Start event handling");
//    exec();
}

void SoundPlayer::stop()
{
//	qDebug("stopping sound thread");

	this->finalizeResampling();
	this->quit();
}

void SoundPlayer::interrupt()
{
//	qDebug("interrupting sound thread");
	// FIXME: abortStream() seems to freeze on Windows when the stream is closed afterwards
	//m_stream->abortStream();
	m_stream->closeStream();
	this->quit();
}

SoundPlayer::~SoundPlayer()
{
    //qDebug("deleting sound thread");
	if (this->isRunning())
	{
		qDebug("WARNING: sound output thread was still running: stopping");
		this->stop();
	}

	delete m_stream; // automatically closed
//	delete data;
	delete m_params;
	delete m_options;
}

void SoundPlayer::initializeResampling()
{
	int error = 0;
    spx_uint32_t input_rate = (spx_uint32_t) data->sndfile.samplerate();
    spx_uint32_t output_rate = (spx_uint32_t) m_stream->getStreamSampleRate();

    data->resampler = speex_resampler_init(data->sndfile.channels(), input_rate, output_rate, DM_SOUND_QUALITY, &error);

	if (error != 0)
	{
        qDebug("Problem initializing resampling");
        data->resampler = NULL;
	}
}

void SoundPlayer::finalizeResampling()
{
	//FIXME: crashes (invalid pointer)

}

void SoundPlayer::pause()
{
	data->paused = true;
}

bool SoundPlayer::isPaused()
{
	return data->paused;
}

void SoundPlayer::resume()
{
	data->paused = false;
}


