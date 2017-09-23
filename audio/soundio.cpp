/*
 * soundio.cpp
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

#include "soundio.h"

#define DM_MONO_CHANNEL 0
#define DM_KEEP_PLAYING 0
#define DM_STOP_PLAYING 1

CallbackData::CallbackData(SndfileHandle &fh, sf_count_t startf, sf_count_t endf) :
    sndfile(fh)
{
	start_frame	= position = startf;
	end_frame	= endf;
	length		= endf - startf;
	paused = false;
    buffer = (float *) malloc(sizeof(float) * BUFFER_SIZE * sndfile.channels());

#ifdef Q_OS_MAC
	outputrate = MAC_SAMPLE_RATE;
	nchannels  = 2;
    ratio      = outputrate / (double)fh.samplerate(); //TODO: refactor
#else
    outputrate = sndfile.samplerate();
    nchannels  = sndfile.channels();
#endif

}

// emit signal from callback (which is a function and has no knowledge of signals/slots)
void CallbackData::stop()
{
    emit finished();
}

int CallbackData::inputRate() const
{
    return sndfile.samplerate();
}

void CallbackData::update()
{
    double time = (double)position / sndfile.samplerate();
    emit currentTime(time);
}

bool CallbackData::needsResampling() const
{
    return stream->getStreamSampleRate() != (unsigned int)sndfile.samplerate();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void playSilence(float *buffer, size_t size)
{
    memset(buffer, 0, size);
}

inline void duplicateChannel(float *output, sf_count_t nframes)
{
    sf_count_t i;
    for (i = 0; i < nframes; ++i)
        output[nframes+i] = output[i];
}

#ifdef Q_OS_MAC
int playback(void *outputBuffer, void *inputBuffer, unsigned int OUTPUT_BUFFER_SIZE,
        double streamTime, RtAudioStreamStatus status, void *userData)
{
    /* OUTPUT_BUFFER_SIZE represents the total number of frames per channel */

    Q_UNUSED(inputBuffer)
    Q_UNUSED(streamTime)

    sf_count_t size_in_sf, nread_in;
    spx_uint32_t nread_out;

    CallbackData *data = (CallbackData *) userData;
    float *output = (float *) outputBuffer;

    // Perform some checks before playing
    if (data->resampler == NULL)
        return DM_STOP_PLAYING;

    if (data->paused)
    {
        size_t size = (size_t) (sizeof(float) * OUTPUT_BUFFER_SIZE * data->nchannels);
        playSilence(output, size);
        return DM_KEEP_PLAYING;
    }

    if (status)
      qDebug("Stream underflow detected!");

    // seek current file position
    data->sndfile.seek(data->position, SEEK_SET);

    size_in_sf = OUTPUT_BUFFER_SIZE / data->ratio;
    sf_count_t framesNotPlayed = data->end_frame - data->position;

    // Example: 231 frames left for a buffer of 241
    if (size_in_sf > framesNotPlayed)
        size_in_sf = framesNotPlayed;

    if (true) //data->needsResampling())
    {
        // Read data with libsndfile
        nread_in = data->sndfile.readf(data->buffer, size_in_sf);
        // Set size of the output buffer
        nread_out = nread_in * data->ratio;

        spx_uint32_t nread_in_spx = (spx_uint32_t)nread_in;
        int error = speex_resampler_process_float(data->resampler, DM_MONO_CHANNEL, data->buffer, &nread_in_spx, output, &nread_out);
        if (error)
            qDebug("Error in speex_resampler_process_int()");
    }
    else
    {
        //TODO: read from buffer instead of reading from file directly
        nread_in = data->sndfile.readf(output, size_in_sf);
    }

    data->position += nread_in;

   if (data->sndfile.channels() == 1)
       duplicateChannel((float*)outputBuffer, OUTPUT_BUFFER_SIZE);

   data->update(); // send time in the stream


   if (data->position < data->end_frame)
       return DM_KEEP_PLAYING;
   else
   {
       // zero out buffer to avoid playing it twice
//       int i;
//       for (i = 0; i < OUTPUT_BUFFER_SIZE; ++i)
//           *output++ = 0.0;

       return DM_STOP_PLAYING;
   }
}

#else

int playback(void *outputBuffer, void *inputBuffer, unsigned int nframes,
		double streamTime, RtAudioStreamStatus status, void *userData)
{
    /* nframes represents the total number of frames per channel */

	Q_UNUSED(inputBuffer)
	Q_UNUSED(streamTime)

    spx_uint32_t len, nframes_left;
	CallbackData *data = (CallbackData *) userData;
	float *output = (float *) outputBuffer;

	if (data->paused)
	{
        size_t size = (size_t) (sizeof(float) * nframes * data->nchannels);
        playSilence(output, size);
        return DM_KEEP_PLAYING;
	}

	if (status)
	  qDebug("Stream underflow detected!");


    // seek to the current file position
    data->sndfile.seek(data->position, SEEK_SET);
//    qDebug() << "pos: " << data->position;

    // how far are we from the end?
    sf_count_t leftOver = data->end_frame - data->position;
    if (nframes > leftOver)
        len = leftOver;
    else
        len = nframes;


    data->position += data->sndfile.readf(output, len);

    nframes_left = (spx_uint32_t) (data->end_frame - data->position);


    data->update();

    if (nframes_left > 0)
        return DM_KEEP_PLAYING;
    else
	{
		// zero out buffer to avoid playing it twice
        unsigned int i;
		for (i = 0; i < nframes; ++i)
			*output++ = 0.0;

//        qDebug() << "FINISHED PLAYING IN CALLBACK";
//        data->stop();
        return DM_STOP_PLAYING;
	}
}

#endif // IFDEF Q_OS_MAC
