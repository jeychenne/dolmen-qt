#include "SoundScrollBar.h"

SoundScrollBar::SoundScrollBar(Qt::Orientation orientation) :
	QScrollBar(orientation)
{
	this->setFocusPolicy(Qt::StrongFocus);
}

void SoundScrollBar::increment()
{
	if (value() != maximum())
		emit valueChanged(value() + singleStep());
}

void SoundScrollBar::decrement()
{
	if (value() != 0)
		emit valueChanged(value() - singleStep());
}
