/*
 * SignalPlot.cpp
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
 * Purpose: a canvas to display a sound file
 * Created: 06/05/2011
 */

#include "SignalPlot.h"


SignalPlot::SignalPlot(QWidget *parent) :
    QwtPlot(parent)
{
 #ifdef Q_OS_UNIX
    rubber_band = new DMRubberBand(QRubberBand::Line, this);
    rubber_band->setOpacity(0.2);
#else
    rubber_band = new DMRubberBand(QRubberBand::Rectangle, this);
#endif
	rubber_band->hide();
	wdata = NULL;
	this->setContextMenuPolicy(Qt::CustomContextMenu);
    //this->setCanvasLineWidth(0); //TODO: disappeared in qwt 6.1?

	/* prevent margin in the canvas */
	this->plotLayout()->setCanvasMargin(0);


    //FOCUS to handle 'Esc'
    setFocusPolicy(Qt::StrongFocus);
    setFocus(Qt::OtherFocusReason);
}

void SignalPlot::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left)
    {
        emit slideBackward();
        event->accept();
    }
    else if (event->key() == Qt::Key_Right)
    {
        emit slideForward();
        event->accept();
    }
    else if (event->key() == Qt::Key_Up)
    {
        emit zoomOutRequested();
        event->accept();
    }
    else if (event->key() == Qt::Key_Down)
    {
        emit zoomInRequested();
        event->accept();
    }

    event->ignore();
}

//The user can press 'space' to play the current match and 'Esc' to stop it
void SignalPlot::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space)
    {
        emit playSoundRequested();
        event->accept();
    }
    else if (event->key() == Qt::Key_Escape)
    {
        emit stopSoundRequested();
        event->accept();
    }

    else
        event->ignore();
}


void SignalPlot::setWaveData(WaveData *d)
{
	wdata = d;
}

void SignalPlot::hideRubberband()
{
	rubber_band->hide();
}

void SignalPlot::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		origin = event->pos();

		rubber_band->setGeometry(QRect(origin, QSize()));
		rubber_band->show();
	}

	QwtPlot::mousePressEvent(event);
}

void SignalPlot::mouseMoveEvent(QMouseEvent *event)
{

	QwtPlot::mouseMoveEvent(event);

	QRect rect = QRect(origin, event->pos()).normalized();

	// fill Y axis entirely
	rect.setY(0);
	rect.setSize(QSize(rect.size().width(), this->size().height()));

	rubber_band->setGeometry(rect);

}



void SignalPlot::mouseReleaseEvent(QMouseEvent *event)
{
    // Catch the position of the rubberband to the selected time span
    QRect rect = rubber_band->geometry();
    double tstart = pos2time(rect.x(), this->width());
    double tend   = pos2time(rect.x() + rect.width(), this->width());

    // then inform signal block to update the selection
    emit selection(tstart, tend);
    emit selectionAvailable();

    QwtPlot::mouseReleaseEvent(event);
}

void SignalPlot::wheelEvent(QWheelEvent *event)
{
	if (event->delta() > 0)		// scrolling up == sliding backward
		emit slideBackward();
	else						// scrolling down == sliding forward
		emit slideForward();

	event->accept();
}


void SignalPlot::resizeEvent(QResizeEvent *event)
{
	const int oldWidth = event->oldSize().width();
	double tstart, tend;
	QRect rect;

	rect = rubber_band->geometry();
	tstart = pos2time(rect.x(), oldWidth);
	tend   = pos2time(rect.x() + rect.width(), oldWidth);

	int xstart = time2pos(tstart, this->width());
	int xend   = time2pos(tend, this->width());

//	rect.setX(xstart);
//	rect.setWidth(xend-xstart);
//	rubberband->setGeometry(rect);
//	rubberband->show();
    this->drawRubberband(xstart, xend);

	QwtPlot::resizeEvent(event);
}

void SignalPlot::paintEvent(QPaintEvent *event)
{
	qDebug("plot paint event");
	QwtPlot::paintEvent(event);
}

void SignalPlot::drawRubberband(int xstart, int xend)
{
	QRect rect = this->createRect(xstart, xend);
	rubber_band->setGeometry(rect);
	rubber_band->show();

	emit selection(pos2time(xstart, width()), pos2time(xend, width()));
}

void SignalPlot::restoreRubberband(double tstart, double tend)
{
	if (tstart != tend)
	{
		int xstart = time2pos(tstart, width());
		int xend   = time2pos(tend, width());
		QRect rect = this->createRect(xstart, xend);

		rubber_band->setGeometry(rect);
		rubber_band->show();
	}
}

int SignalPlot::time2pos(double time, int width) const
{
	return (int)(((time - wdata->start()) * width) / wdata->windowSize());
}

double SignalPlot::pos2time(int pos, int width) const
{
	return (((double)pos * wdata->windowSize()) / width) + wdata->start();
}

QRect SignalPlot::createRect(int xstart, int xend) const
{
	QRect rect = QRect(xstart, 0, xend-xstart, this->height());
	return rect;
}

