#include "CurveData.h"

#include <limits>
using namespace std;

// on MSVC2010, causes numeric_limits<dm_sample_t>::max()) to be expanded
#undef min
#undef max

WaveData::WaveData()
{
	ptr_data	= NULL;
	m_sound		= NULL;
	m_plot		= NULL;
	m_channel	= MonoChannel;

	this->setWindow(0., DEFAULT_WINDOW_LENGTH);
}


WaveData::WaveData(Sound *snd, SignalPlot *plot, dm_channel_t channel, double start, double end)
{
	m_sound		= snd;
	ptr_data	= NULL;
	m_channel	= channel;
	m_plot		= plot;

    if (m_sound && end > m_sound->length()) {
        end = m_sound->length();
    }

	this->setWindow(start, end);
}

QwtSeriesData<QPointF> *WaveData::copy() const
{
	WaveData *cpy = new WaveData(m_sound, m_plot, this->channel(), this->start(), this->end());
	cpy->setData(this->data());
	return cpy;
}

WaveData & WaveData::operator=(const WaveData &wd)
{
	if (this != &wd)
	{
		m_data.clear();
		this->setSoundFile(wd.soundFile());
		this->setData(wd.data());
		this->setWindow(wd.start(), wd.end());
		this->setPlot(wd.plot());
		m_channel = wd.channel();
	}

	return *this;
}

SignalPlot * WaveData::plot() const
{
	return m_plot;
}

void WaveData::setPlot(SignalPlot *p)
{
	m_plot = p;
}

dm_channel_t WaveData::channel() const
{
	return m_channel;
}

Sound * WaveData::soundFile() const
{
	return m_sound;
}

void WaveData::setSoundFile(Sound *s)
{
	m_sound = s;
}


QPointF WaveData::sample(size_t i) const
{
    qreal x,y;
    size_t j = i;

    /*
    SET X

    offset window by the window's start.
    Display 2 points (min and max amplitudes) for each pixel.
    */
    if (j % 2 == 1)
        --j;

    size_t frame  = j/2 * this->framesPerPixel();
    x = static_cast<qreal>(m_sound->frame2time(frame) + window_start);

    /*
    SET Y

    A sequence of even/odd indices represents min and max values for a given point.
    We compute both when an even index (min) is asked and return/reset max
    when an odd index is asked.
    */
    static double AMP_NORM = (double)abs((long) numeric_limits<dm_sample_t>::max());
    static dm_sample_t MAX_AMP = numeric_limits<dm_sample_t>::max();
    static dm_sample_t MIN_AMP = numeric_limits<dm_sample_t>::min();

    dm_sample_t value;
    size_t chunkSize = this->framesPerPixel();

    size_t startFrame = m_sound->time2frame(window_start) + (i / 2) * chunkSize;
    size_t endFrame = startFrame + chunkSize - 1;
    size_t k;

    size_t nframes = m_sound->nframes();

    if (endFrame > nframes)
    {
        value = 0.0;
    }
    else if (i % 2 == 0) // min value
    {
        value = MAX_AMP;

        for (k = startFrame; k <= endFrame; ++k)
        {
            if (ptr_data[k] < value)
                value = ptr_data[k];
        }
    }
    else // max value
    {
        value = MIN_AMP;

        for (k = startFrame; k <= endFrame; ++k)
        {
            if (ptr_data[k] > value)
                value = ptr_data[k];
        }
    }

    y = (double) value / AMP_NORM;

    return QPointF(x,y);
}

QRectF WaveData::boundingRect() const
{
    return qwtBoundingRect(*this); //???????????????????????,,
}


size_t WaveData::size() const
{
	/* twice the number of pixels (for each pixel, display min and max amplitudes) */
	if (m_plot == NULL)
		return 0;

	return m_plot->width() * 2;
}

double WaveData::start() const
{
	return window_start;
}

double WaveData::end() const
{
	return window_end;
}

double WaveData::duration() const
{
    return m_sound->length();
}

void WaveData::setWindow(double start, double end)
{
	window_start  = start;
	window_end    = end;
}

FrameVector WaveData::data() const
{
	return m_data;
}

void WaveData::setData()
{
	if (m_sound)
	{
		if (m_channel == LeftChannel)
		{
			QPair<FrameVector,FrameVector> channels = m_sound->rawFramesStereo();
			channels.second.clear();
			this->setData(channels.first);
		}
		else
		{
			this->setData(m_sound->rawFramesMono());
		}
	}
	else
		qDebug("m_sound == NULL in WaveData::setData()");
}

void WaveData::setData(const FrameVector &data)
{
	m_data = data;
	ptr_data = m_data.data();
}


size_t WaveData::framesPerPixel() const
{
	return (size_t) floor(((window_end-window_start) * m_sound->samplerate()) / m_plot->width()) + 1;
}

WaveData::~WaveData()
{

}
