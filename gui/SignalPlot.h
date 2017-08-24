/*
 * SignalPlot.h
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
 * Purpose: a canvas to display a sound file
 * Created: 06/05/2011
 */

#ifndef SIGNALPLOT_H
#define SIGNALPLOT_H

#include <QPoint>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QPaintEvent>

#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_plot_layout.h>


#include "DMRubberBand.h"
#include "CurveData.h"

class WaveData;

class SignalPlot : public QwtPlot
{
	Q_OBJECT

public:
	SignalPlot(QWidget *parent = NULL);
	void hideRubberband();
	void setWaveData(WaveData *d); // needs to be called once data is available
	void restoreRubberband(double tstart, double tend);

signals:
	void slideForward();
	void slideBackward();
    void zoomInRequested();
    void zoomOutRequested();
	void selection(double tstart, double tend);
    void selectionAvailable(); // update selection button
    void stopSoundRequested();
    void playSoundRequested();

public slots:
	void drawRubberband(int xstart, int xend);


protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
	void wheelEvent(QWheelEvent *);
	void resizeEvent(QResizeEvent *e);
	void paintEvent(QPaintEvent *e);

private:
    DMRubberBand	*rubber_band;
	QPoint			 origin;
	WaveData		*wdata;

	//TODO: merge with TierView methods
	int time2pos(double time, int width) const;
	double pos2time(int pos, int width) const;
	QRect createRect(int xstart, int xend) const;

};

#endif // SIGNALPLOT_H
