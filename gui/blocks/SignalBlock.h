/*
 * SignalBlock.h
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
 * Purpose: A class to display a sound (waveform, spectrogram, etc.).
 * Created: 01/03/2011
 */

#ifndef SIGNALBLOCK_H
#define SIGNALBLOCK_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressDialog>
#include <QToolBar>
#include <QToolButton>
#include <QLabel>
#include <QMenu>

#include <qwt_plot_curve.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>


#include "utils/Global.h"
#include "audio/soundio.h"
#include "lib/Sound.h"
#include "lib/DMSelection.h"
#include "gui/SignalPlot.h"
#include "gui/CurveData.h"
#include "gui/ContextMenu.h"
#include "gui/SoundScrollBar.h"

class SignalBlock: public QVBoxLayout
{
	Q_OBJECT

public:
	SignalBlock(QWidget *parent, Sound *snd, double beginning = 0.0, double end = 0.0);
	~SignalBlock();
	void refresh(); // this function is public so that it can be called after its signal is connected to a tier block
	void insertTierLayout(QVBoxLayout *layout);

signals:
	void soundLoaded();
	void currentWindow(DMSelection);
	void updateSelection(int xstart, int xend);//TODO: harmonize conversition time/pixels (should be done in plot)
    void editMetadata(); // FIXME: Needed??

private slots:
	void updateWindow(int value);
	void displayProgressBar(int count);
	void updateProgressBar(int value);
	void playSelection(); // plays selection if there is one, current window otherwise
	void zoomIn();
	void zoomOut();
	void stop();
	void zoomToSelection();
    void displayWholeSound();
    void onSelection(QRectF sel);
	void showContextMenu(const QPoint &pos);
	void setPauseIcon();
	void setPlayIcon();
	void updateTimeMark(double t);
	void hideTimeMark();
	void setSelection(double tstart, double tend);

private:
	Sound				*m_sound;
	SoundScrollBar		*m_scrollbar;
	SignalPlot			*wave_plot;
	QwtPlotCurve		*waveform;
    WaveData			*wdata;
    QProgressDialog		*progress_bar;
    QToolButton			*btn_play, *btn_stop, *btn_zoom_in, *btn_zoom_out, *btn_zoom_selection, *btn_zoom_all, *btn_plots;
	QwtPlotPicker		*m_picker;
    DMRubberBand		*m_time_mark;// mark current when a sound is playing
    DMSelection			*current_selection;
    QLabel				*label_start, *label_end; // display start and end of sound file
    QMenu               *menu_plots;
    QAction             *action_wave, *action_spectrogram, *action_pitch;

	void setupUi();
    void setPlotMenu();
	void setScrollbar();
	void setScrollbarRange();
	void displayWindow();
	void displayWindow(double, double);
	void displayWindow(DMSelection *sel);
	void setToolbar();
	void setSelector();
	void play(double start, double end);
	void validateTime(double *t);
	bool hasActiveSelection() const;
	void removeSelection();
	double windowLength() const;

};

#endif // SIGNALBLOCK_H
