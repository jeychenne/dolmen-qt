/*
 * ItemView.cpp
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
 * Purpose: classes to represent spans and points in the GUI
 * Created: 09/08/2011
 */

#include "ItemView.h"

/* Boundary thickness */
const int THICKNESS = 3;

ItemView::ItemView(TierFrame *frame, Item *item, SpanBoundary *sharedBoundary)
{
	m_item				= item;
	m_tierFrame			= frame;
	m_text				= new QLabel(frame);
    shares_start_boundary	= false;
	m_selected			= false;
    is_span				= isInstance(item, DSpan);

    m_text->setWordWrap(is_span);

    if (is_span)
	{
		if (sharedBoundary)
		{
            shares_start_boundary = true;
            start_boundary = sharedBoundary;
		}
		else
		{
            start_boundary = new SpanBoundary(frame);
            start_boundary->hide();
		}

        end_boundary = new SpanBoundary(frame);
        end_boundary->hide();

		timeBoundary = NULL;
	}
	else
	{
        start_boundary = NULL;
        end_boundary = NULL;
		timeBoundary = new PointBoundary(frame);
	}
		m_text->show();
}

QRect ItemView::textGeometry() const
{
	return m_text->geometry();
}

void ItemView::hide()
{
    if (is_span)
	{
        start_boundary->hide();
        end_boundary->hide();
	}
	else
		timeBoundary->hide();

	m_text->hide();
}

void ItemView::show()
{
    if (is_span)
	{
        start_boundary->show();
        end_boundary->show();
	}
	else
		timeBoundary->show();

	m_text->show();
}

void ItemView::display(int left, int right)
{
    if (is_span)
	{
		/* offset boundaries by 1 so that their centre coincide with left and right */
        start_boundary->setGeometry(left-1, 0, THICKNESS, m_tierFrame->height());
        start_boundary->show();

        end_boundary->setGeometry(right-1, 0, THICKNESS, m_tierFrame->height());
        start_boundary->show();

		m_text->setGeometry(left+2, 1, right-left-2, m_tierFrame->height()-2);
	}
	else
	{
		timeBoundary->setGeometry(left-1, 0, THICKNESS, m_tierFrame->height());
		timeBoundary->show();
		const int mid = (int) floor((double)m_tierFrame->height()/2);
		m_text->setGeometry(left-25, mid-10, 50, 20);
	}

	this->setSelected(false);

	m_text->setAutoFillBackground(true);
	m_text->setText(m_item->text());
	m_text->setToolTip(m_item->text());
	m_text->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

TierFrame* ItemView::tierView() const
{
	return m_tierFrame;
}

Item * ItemView::item() const
{
	return m_item;
}

QPair<int,int> ItemView::screenPos() const
{
	QPair<int,int> pos;

    if (is_span)
	{
        pos.first = start_boundary->geometry().x();
        pos.second = end_boundary->geometry().x() + end_boundary->geometry().width();
	}
	else
	{
		pos = timeBoundary->xpos();
	}

	return pos;
}

void ItemView::setSelected(bool selected)
{
	m_selected = selected;

    if (is_span)
	{
        if (selected)
            m_text->setStyleSheet("QLabel { background-color: #cdc3c3; }"); //b3afaf
        else
            m_text->setStyleSheet("QLabel { background-color: #f2eeee; }");

//		if (selected)
//		{
//			m_text->setStyleSheet("QLabel { background-color: QLinearGradient("
//								  "x1: 0, y1: 0, x2: 0, y2: 1, "
//                                  "stop: 0.0 #c4b7c6, stop: 1 #cf9bd9); }");	//first color: ceadd4 | second: 9c55a9
//		}
//		else
//		{
//			m_text->setStyleSheet("QLabel { background-color: QLinearGradient("
//								  "x1: 0, y1: 0, x2: 0, y2: 1, "
//                                  "stop: 0 #F2EEEE, stop: 1 #baaeae); }"); // #ded6d6, stop: 1 #c4b7c6); }"); // first : ded6d6 | second color: ceadd4
//		}

		m_text->update();
	}
	else
	{
		timeBoundary->setSelected(selected);
	}

	if (selected)
		m_text->setText("<b>" + m_item->text() + "</b>");
	else
		m_text->setText(m_item->text());
}

bool ItemView::isSelected() const
{
	return m_selected;
}

bool ItemView::setText(const QString &text)
{
	bool modified = false;

	if (text != m_item->text())
	{
		m_item->setText(text);
		m_text->setText(text);
		modified = true;
	}

	return modified;
}

ItemBoundary * ItemView::boundaryAt(int pos) const
{
    if (is_span)
	{
        if (start_boundary->contains(pos))
            return start_boundary;
        else if (end_boundary->contains(pos))
            return end_boundary;
	}
	else if (timeBoundary->contains(pos))
		return timeBoundary;

	return NULL;
}

SpanBoundary * ItemView::sharedBoundaryWithNextItem(double tstart)
{
    if (is_span && tstart == m_item->right())
        return end_boundary;
	else
		return NULL;
}

void ItemView::moveLeftBoundary(int x)
{
    Q_UNUSED(x);
}

void ItemView::moveRightBoundary(int x)
{
    Q_UNUSED(x);
}

QString ItemView::toHtml() const
{
	QString text;

    if (is_span)
	{
		QString start, end, dur;
		start.setNum(m_item->left(), 'f', 4);
		end.setNum(m_item->right(), 'f', 4);
		dur.setNum(m_item->right() - m_item->left(), 'f', 4);

#ifdef Q_OS_MAC
		text += QString("<b>Span:</b> ") + QString::fromUtf8("⟨") + start + QString(" to ")
				+ end + QString::fromUtf8("⟩") + QString(" duration: %1 s<br/>").arg(dur);
#else
        text += QString("<b>Span:</b> ") + QString("&lt;") + start + QString(" to ")
				+ end + QString("&gt;") + QString(" (duration: %1)<br/>").arg(dur);
#endif
	}
	else
	{
		QString time;
		time.setNum(m_item->left(), 'f', 4);

#ifdef Q_OS_MAC
		text += QString("<b>Point:</b> ") + QString::fromUtf8("⟨") + time + QString::fromUtf8("⟩") + QString("<br/>");
#else
		text += QString("<b>Point:</b> ") + QString("&lt;") + time + QString("&gt;") + QString("<br/>");
#endif

	}

	text += m_item->text();
	return text;
}

ItemView::~ItemView()
{
    if (is_span)
	{
        if (!shares_start_boundary)
            delete start_boundary;

        delete end_boundary;
	}
	else
	{
		delete timeBoundary;
	}

	delete m_text;
}


