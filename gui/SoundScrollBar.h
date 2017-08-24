#ifndef SOUNDSCROLLBAR_H
#define SOUNDSCROLLBAR_H

#include <QScrollBar>

class SoundScrollBar : public QScrollBar
{
	Q_OBJECT

public:
	SoundScrollBar(Qt::Orientation orientation);

public slots:
	void increment();
	void decrement();
};

#endif // SOUNDSCROLLBAR_H
