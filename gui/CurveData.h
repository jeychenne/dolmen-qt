/*
 * CurveData.h
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
 * Created: 01/04/2011
 */

#ifndef CURVEDATA_H
#define CURVEDATA_H

#include <QVector>


#include <qwt/qwt_series_data.h>

#include "lib/Sound.h"
#include "gui/SignalPlot.h"
#include "dolmen.h"

class SignalPlot;



enum DMChannel
{
	MonoChannel,
	LeftChannel,
	RightChannel
};

typedef enum DMChannel dm_channel_t;

class WaveData : public QwtSeriesData<QPointF>
{
public:
	WaveData();
	WaveData(Sound *snd, SignalPlot *plot, dm_channel_t channel = MonoChannel,
			 double start = 0., double end = DEFAULT_WINDOW_LENGTH);
	~WaveData();
    WaveData & operator=(const WaveData &wd);
	size_t size() const;
    QwtSeriesData<QPointF> * copy() const;
    QPointF sample( size_t i ) const;
    QRectF boundingRect() const;
	double start() const;
	double end() const;
    double duration() const;
	dm_channel_t channel() const;
	void setWindow(double start, double end);
	FrameVector data() const;
	Sound * soundFile() const;
	void setSoundFile(Sound *s);
	SignalPlot * plot() const;
	void setPlot(SignalPlot *p);

	// the data is not loaded in the constructor but must be
	// loaded explicitly by the caller
	void setData(); // read from Sound object
	void setData(const FrameVector &data); // copy from existing QVector

	inline double windowSize() { return window_end - window_start; }

private:
    FrameVector   m_data;
    dm_sample_t  *ptr_data;
    dm_channel_t  m_channel;
    Sound        *m_sound;
    SignalPlot   *m_plot;
    double        window_start, window_end; // window, in seconds

	size_t framesPerPixel() const;
};


#endif // CURVEDATA_H
