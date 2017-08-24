#include "SignalBlock.h"
#include <QFrame>

const int TIME_UNIT = 10000; // tenth of millisecond

SignalBlock::SignalBlock(QWidget *parent, Sound *snd, double start, double end) :
    QVBoxLayout(parent)
{
	m_sound = snd;
	m_scrollbar = NULL;
    wdata = NULL;
	dm_channel_t channel;
    current_selection = new DMSelection(0.0, 0.0);
	this->setMargin(5);

	if (m_sound)
	{
		connect(m_sound, SIGNAL(startLoadingData(int)), this, SLOT(displayProgressBar(int)));
		connect(m_sound, SIGNAL(bufferLoaded(int)), this, SLOT(updateProgressBar(int)));
		connect(m_sound, SIGNAL(currentTime(double)), this, SLOT(updateTimeMark(double)));
		connect(m_sound, SIGNAL(finished()), this, SLOT(setPlayIcon()));
        connect(m_sound, SIGNAL(finished()), this, SLOT(hideTimeMark()));
	}
	// TODO: handle more than mono files and left channel of stereo files
	channel = m_sound && m_sound->isMono() ? MonoChannel : LeftChannel;

    // must create plot before WaveData
    wave_plot = new SignalPlot;

	if (end)
    {
        wdata = new WaveData(m_sound, wave_plot, channel, start, end);
    }
	else
    {
        wdata = new WaveData(m_sound, wave_plot, channel);
    }
    wdata->setData(); // read whole file into memory

    // now that the data is initialized, setup UI
    wave_plot->setWaveData(wdata);
    this->setupUi();
}


void SignalBlock::setupUi()
{
	this->setSpacing(0);

	wave_plot->setAutoReplot(false);
	wave_plot->setFixedHeight(200);
	wave_plot->setCanvasBackground(QColor(Qt::white));

	waveform = new QwtPlotCurve("Waveform");

    //FIXME: requires Qwt >= 6.1
    waveform->setPaintAttribute(QwtPlotCurve::FilterPoints, true);
	waveform->attach(wave_plot);

    if (Global::UseAntialiasingInPlots)
        waveform->setRenderHint(QwtPlotItem::RenderAntialiased);

	this->setToolbar();
	// set start/end labels
	QHBoxLayout *hl = new QHBoxLayout;
    label_start = new QLabel;
    label_end   = new QLabel;
    hl->addWidget(label_start);
	hl->addStretch();
    hl->addWidget(label_end);
	this->addLayout(hl);

	this->addWidget(wave_plot);
	this->displayWindow();
	this->setScrollbar();
	this->setScrollbarRange();
	this->setSelector();

    connect(btn_play, SIGNAL(clicked()), this, SLOT(playSelection()));
    connect(btn_stop, SIGNAL(clicked()), this, SLOT(stop()));
    connect(btn_stop, SIGNAL(clicked()), this, SLOT(setPlayIcon()));
    connect(btn_zoom_in, SIGNAL(clicked()), this, SLOT(zoomIn()));
    connect(btn_zoom_out, SIGNAL(clicked()), this, SLOT(zoomOut()));
    connect(btn_zoom_selection, SIGNAL(clicked()), this, SLOT(zoomToSelection()));
	connect(wave_plot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
	/* Connection to update and redraw the selection in the plot */
	connect(this, SIGNAL(updateSelection(int,int)), wave_plot, SLOT(drawRubberband(int,int)));
	connect(wave_plot, SIGNAL(selection(double,double)), this, SLOT(setSelection(double,double)));

    if (m_sound)
    {
        connect(wave_plot, SIGNAL(stopSoundRequested()), this, SLOT(stop()));
        connect(wave_plot, SIGNAL(playSoundRequested()), this, SLOT(playSelection()));
    }
}


void SignalBlock::showContextMenu(const QPoint &pos)
{
	ContextMenu menu(wave_plot);

    QAction *play_sel = new QAction(tr("Play selection"), this);
    menu.addAction(play_sel);
    connect(play_sel, SIGNAL(triggered()), this, SLOT(playSelection()));

    QAction *zoom_sel = new QAction(tr("Zoom to selection"), this);
    menu.addAction(zoom_sel);
    connect(zoom_sel, SIGNAL(triggered()), this, SLOT(zoomToSelection()));

	if (! this->hasActiveSelection())
        zoom_sel->setEnabled(false);

	menu.display(pos);
}

void SignalBlock::insertTierLayout(QVBoxLayout *layout)
{
	this->insertLayout(3, layout);
}

void SignalBlock::setPlotMenu()
{
    menu_plots = new QMenu;

    action_wave = new QAction(tr("Waveform"), this);
    action_wave->setCheckable(true);
    action_wave->setChecked(true);
//    action_spectrogram = new QAction(tr("Spectrogram"), this);
//    action_spectrogram->setCheckable(true);
//    action_pitch = new QAction(tr("Pitch"), this);
//    action_pitch->setCheckable(true);

    menu_plots->addAction(action_wave);
//    menu_plots->addAction(action_spectrogram);
//    menu_plots->addAction(action_pitch);
}

void SignalBlock::setToolbar()
{
    QToolBar *toolbar = new QToolBar;

    setPlotMenu();

    btn_play = new QToolButton;
	this->setPlayIcon();

    btn_stop = new QToolButton;
    btn_stop->setIcon(QIcon(":/icons/stop.png"));
    btn_stop->setToolTip(tr("Stop"));

    btn_zoom_in = new QToolButton;
    btn_zoom_in->setIcon(QIcon(":/icons/zoom_in.png"));
    btn_zoom_in->setToolTip(tr("Zoom in"));

    btn_zoom_out = new QToolButton;
    btn_zoom_out->setIcon(QIcon(":/icons/zoom_out.png"));
    btn_zoom_out->setToolTip(tr("Zoom out"));

    btn_zoom_selection = new QToolButton;
    btn_zoom_selection->setIcon(QIcon(":/icons/collapse.png"));
    btn_zoom_selection->setToolTip(tr("Zoom to selection"));

    btn_zoom_all = new QToolButton;
    btn_zoom_all->setIcon(QIcon(":/icons/expand.png"));
    btn_zoom_all->setToolTip(tr("View whole file"));

    btn_plots = new QToolButton;
    btn_plots->setIcon(QIcon(":/icons/waveform.png"));
    btn_plots->setToolTip(tr("Configure visible plots"));
    btn_plots->setMenu(menu_plots);
    btn_plots->setPopupMode(QToolButton::InstantPopup);

    toolbar->addWidget(btn_play);
    toolbar->addWidget(btn_stop);
    toolbar->addSeparator();
    toolbar->addWidget(btn_zoom_in);
    toolbar->addWidget(btn_zoom_out);
    toolbar->addWidget(btn_zoom_selection);
    toolbar->addWidget(btn_zoom_all);
    toolbar->addSeparator();
    toolbar->addWidget(btn_plots);
    toolbar->setIconSize(QSize(25,25));

    this->addWidget(toolbar);

    connect(btn_zoom_all, SIGNAL(clicked()), this, SLOT(displayWholeSound()));
}

void SignalBlock::setSelector()
{
	// highlight user selection
	m_picker = new QwtPlotPicker(wave_plot->canvas());
//	m_picker->setSelectionFlags(QwtPlotPicker::RectSelection);
	m_picker->setRubberBand(QwtPlotPicker::VLineRubberBand);
//	m_picker->setRubberBandPen(QPen(QColor(Qt::red)));
	m_picker->setTrackerMode(QwtPicker::AlwaysOn);
	m_picker->setTrackerPen(QPen(QColor(Qt::blue)));

    m_time_mark = new DMRubberBand(QRubberBand::Line, wave_plot, Qt::red, false);
    m_time_mark->hide();

    connect(m_picker, SIGNAL(selected(QRectF)), this, SLOT(onSelection(QRectF)));
}

void SignalBlock::updateTimeMark(double t)
{
	// convert from Qwt to widget coordinates
    int pos = (int)(((t - wdata->start()) * wave_plot->width()) / windowLength());

    m_time_mark->setGeometry(pos, 0, 1, wave_plot->height());
    m_time_mark->show();
}

void SignalBlock::onSelection(QRectF sel)
{
	double x1 = (double) sel.bottomLeft().x();
	double x2 = (double) sel.bottomRight().x();

	if (x1 == x2)
		// invalid selection
        current_selection->start = current_selection->end = 0.0;
	else if (x1 < x2)
	{
        current_selection->start = x1;
        current_selection->end   = x2;
	}
	else
	{
        current_selection->start = x2;
        current_selection->end   = x1;
	}

    qDebug("Selection: %f to %f", current_selection->start, current_selection->end);
}

void SignalBlock::setSelection(double tstart, double tend)
{
    current_selection->start = tstart;
    current_selection->end   = tend;
}


void SignalBlock::removeSelection()
{
	wave_plot->hideRubberband();
}

bool SignalBlock::hasActiveSelection() const
{
    return current_selection->end != 0.0 && current_selection->duration() > 0;
}

void SignalBlock::setScrollbar()
{
	m_scrollbar = new SoundScrollBar(Qt::Horizontal);
	this->addWidget(m_scrollbar);

	connect(m_scrollbar, SIGNAL(valueChanged(int)), this, SLOT(updateWindow(int)));
    /* Allow the plot to be scrolled with the mouse wheel (or left/right arrows) */
	connect(wave_plot, SIGNAL(slideBackward()), m_scrollbar, SLOT(decrement()));
	connect(wave_plot, SIGNAL(slideForward()), m_scrollbar, SLOT(increment()));

    // Connect up and down arrows
    connect(wave_plot, SIGNAL(zoomInRequested()), this, SLOT(zoomIn()));
    connect(wave_plot, SIGNAL(zoomOutRequested()), this, SLOT(zoomOut()));
}

void SignalBlock::setScrollbarRange()
{
    m_scrollbar->setPageStep((int)(wdata->windowSize() * TIME_UNIT));

    int min = (int)(wdata->start() * TIME_UNIT);
	int max = (int)(m_sound->length() * TIME_UNIT) - m_scrollbar->pageStep();
	m_scrollbar->setRange(0, max);

//	qDebug("Max WINDOW: %f", (double)(max+m_scrollbar->pageStep())/TIME_UNIT);
//	qDebug("File length: %f", m_sound->length());

    int step = (int) ((wdata->windowSize()) * TIME_UNIT * 0.05); // shift by 5% of the window (TIME_UNIT * 0.05) ;
	m_scrollbar->setSingleStep(step);
	m_scrollbar->setValue(min);
}

void SignalBlock::playSelection()
{
	if (this->hasActiveSelection())
        this->play(current_selection->start, current_selection->end);
	else
        this->play(wdata->start(), wdata->end());
}

void SignalBlock::stop()
{
	m_sound->stop();
    m_time_mark->hide();
	wave_plot->replot();
}

void SignalBlock::play(double start, double end)
{
	if (m_sound)
	{
		if (m_sound->isPlaying())
		{
			// set icon
			if (m_sound->isPaused())
				this->setPauseIcon();

			else
				this->setPlayIcon();

			m_sound->pause();
		}
		else
		{
			this->setPauseIcon();
			m_sound->play(start, end);
		}
	}
}

void SignalBlock::zoomIn()
{
    const double step = wdata->windowSize() / 4;
    double start = wdata->start() + step;
    double end = wdata->end() - step;

	this->displayWindow(start, end);
}

void SignalBlock::zoomOut()
{
    const double step = wdata->windowSize() / 2;
    double start = wdata->start() - step;
    double end = wdata->end() + step;

	this->displayWindow(start, end);
}

void SignalBlock::zoomToSelection()
{
    qDebug() << "ZOOMING TO SEL: " << current_selection->start << current_selection->end;
    if (this->hasActiveSelection())
        this->displayWindow(current_selection);
}

void SignalBlock::displayWholeSound()
{
    this->displayWindow(0, m_sound->length());
}

void SignalBlock::updateWindow(int value)
{
	double start = (double) value / TIME_UNIT;
    double end = (double) start + wdata->windowSize();

	this->validateTime(&start);
	this->validateTime(&end);
    wdata->setWindow(start, end);
	this->displayWindow();
}

void SignalBlock::displayWindow(DMSelection *sel)
{
	this->displayWindow(sel->start, sel->end);
}

void SignalBlock::displayWindow(double start, double end)
{
	this->validateTime(&start);
	this->validateTime(&end);
    wdata->setWindow(start, end);
	this->displayWindow();
}

void SignalBlock::displayWindow()
{

	this->removeSelection();

    waveform->setData(wdata);

	wave_plot->setAxisScale(QwtPlot::yLeft, -1, 1, 1);
	wave_plot->enableAxis(QwtPlot::yLeft, false);
    wave_plot->setAxisScale(QwtPlot::xBottom, wdata->start(), wdata->end());
	wave_plot->enableAxis(QwtPlot::xBottom, false);

    wave_plot->replot();
	if (m_scrollbar)
		this->setScrollbarRange();

	QString start, end;
    start.setNum(wdata->start(), 'f', 4);
    end.setNum(wdata->end(), 'f', 4);
    label_start->setText(start);
    label_end->setText(end);
    emit currentWindow(DMSelection(wdata->start(), wdata->end()));

    if (current_selection)
        wave_plot->restoreRubberband(current_selection->start, current_selection->end);
}

// trick to re-emit signal after connection so that it's caught by
// the block tier
void SignalBlock::refresh()
{
    emit currentWindow(DMSelection(wdata->start(), wdata->end()));
}


void SignalBlock::displayProgressBar(int count)
{
	QString txt = tr("Loading sound file...");
    progress_bar = new QProgressDialog(txt, QString(), 0, count, this->parentWidget());
    progress_bar->setWindowModality(Qt::WindowModal);
}

void SignalBlock::updateProgressBar(int value)
{
    progress_bar->setValue(value);

    if (value == progress_bar->maximum())
	{
        progress_bar->close();
        delete progress_bar;
        progress_bar = NULL;
	}
}


double SignalBlock::windowLength() const
{
    return wdata->end() - wdata->start();
}

void SignalBlock::validateTime(double *t)
{
	if (*t < 0)
		*t = 0;
	else if (*t > m_sound->length())
		*t = m_sound->length();
}

void SignalBlock::setPlayIcon()
{
    btn_play->setIcon(QIcon(":/icons/start.png"));
    btn_play->setToolTip(tr("Play"));
}

void SignalBlock::setPauseIcon()
{
    btn_play->setIcon(QIcon(":/icons/pause.png"));
    btn_play->setToolTip(tr("Pause"));

}

void SignalBlock::hideTimeMark()
{
    m_time_mark->hide();
    m_time_mark->setGeometry(0,0,0,0);
}


SignalBlock::~SignalBlock()
{
    delete current_selection;
}
