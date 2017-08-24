/*
 * TierView.h
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
 * Purpose: GUI representation of a tier.
 * Created: 08/08/2011
 */

#include "TierView.h"

TierView::TierView(Tier *tier, int index) :
	TierFrame(tier, index)
{
	m_window			= new DMSelection(0.0, DEFAULT_WINDOW_LENGTH);
	m_textEditor		= NULL;
	selectedBoundary	= NULL;
	boundaryPosition	= INVALID_POSITION;

	this->setTextEditor();
	this->setFixedHeight(50);
	this->setFrameStyle(QFrame::Panel | QFrame::Raised);

	m_textEditor->hide();
}

ItemView * TierView::findItemView(Item *i)
{
	ItemView *view;

	foreach (view, m_itemViews)
	{
		if (view->item() == i)
			return view;
	}

	return NULL;
}

void TierView::resizeEvent(QResizeEvent *event)
{
	QFrame::resizeEvent(event);
	this->displayItems();
}

void TierView::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
//	if (selectedBoundary)
//	{
//		if (selectedBoundary->isPoint())
//			selectedBoundary->updatePosition(event->pos().x());
//	}
}

void TierView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		/* First save item text editor, if any */
		emit hideAllTextEditors();

		/* Clicked on a boundary? */
		int pos = event->pos().x();

		ItemBoundary *boundary = this->boundaryClickedAt(pos);
		if (boundary)
		{
			boundary->setSelected(true);
			selectedBoundary = boundary;
		}
	}

	event->accept();
}

void TierView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (selectedBoundary)
		{
			selectedBoundary->setSelected(false);
			selectedBoundary = NULL;
		}

		ItemView *itemView = this->itemAtPos(event->pos().x());
		this->signalSelectedItem(itemView);
	}

	event->accept();
}

void TierView::mouseDoubleClickEvent(QMouseEvent *event)
{
	ItemView *item = itemAtPos(event->pos().x());

	if (item)
		this->editItemText(item);
}

void TierView::signalSelectedItem(ItemView *itemView)
{
	emit selectItem(itemView);

	if (itemView)
	{
		int xstart = this->time2pos(itemView->item()->left());
		int xend   = this->time2pos(itemView->item()->right());

		emit soundSelection(xstart, xend);
	}
}

void TierView::editItemText(ItemView *itemView)
{
	// FIXME: if the editor is not recreated, it no longer displays properly after an update
	this->setTextEditor();

    static const int LABEL_WIDTH = 400;
    static const int LABEL_HEIGHT = 40;

	QRect rect = itemView->textGeometry();
	//BUG: the editor doesn't move
	if (this->width() <= LABEL_WIDTH)
	{
		rect.setX(0);
		rect.setWidth(this->width());
	}
	else
	{
		int x = rect.x() + (rect.width() / 2) - (LABEL_WIDTH / 2);

		if (x < 0)
			rect.setX(0);
		else if (x + LABEL_WIDTH > this->width())
			rect.setX(x - (x + LABEL_WIDTH - this->width()));
		else
			rect.setX(x);

		rect.setWidth(LABEL_WIDTH);
	}

    int y = (rect.height() - LABEL_HEIGHT) / 2;
    if (y < 0) y = 0;
	rect.setY(y);
	rect.setHeight(LABEL_HEIGHT);


	m_textEditor->setGeometry(rect);
	m_textEditor->setText(itemView->item()->text());
	m_textEditor->setFocus(Qt::NoFocusReason);
	m_textEditor->show();
}

void TierView::setTextEditor()
{
	if (m_textEditor)
		delete m_textEditor;

	m_textEditor = new ItemTextEdit(this);
	connect(m_textEditor, SIGNAL(editingFinished()), this, SLOT(modifyItemText()));
    connect(m_textEditor, SIGNAL(editingCancelled()), this, SLOT(abandonItemChanges()));
}

void TierView::hideTextEditor()
{
	if (m_textEditor->isVisible())
		m_textEditor->exit();
}


void TierView::updateWindow(double start, double end)
{
	m_window->setWindow(start, end);

	this->displayItems();
}

DMSelection * TierView::window() const
{
	return m_window;
}

void TierView::displayItems()
{
	Item *selectedItem = NULL;
	ItemView *selectedItemView = NULL;

	// if we are editing, backup text
	QString textBuffer;
	bool textNeedsRedraw = m_textEditor->isVisible();
	if (textNeedsRedraw)
	{
		m_textEditor->hide();
		textBuffer = m_textEditor->text();
	}

	// clean up tier
	foreach(ItemView *sp, m_itemViews)
	{
		if (sp->isSelected())
			selectedItem = sp->item();

		delete sp;
	}
	m_itemViews.clear();

	QList<Item*> items = m_tier->items();
	Item *item;
	int i;
	for (i = 0; i < items.size(); ++i)
	{
		item = items[i];

		if (fitsWindow(item))
		{
			SpanBoundary *sharedBoundary = NULL;

			if (isInstance(item, DSpan) && ! m_itemViews.isEmpty())
				sharedBoundary = m_itemViews.last()->sharedBoundaryWithNextItem(item->left());

			ItemView *itemView = new ItemView(this, item, sharedBoundary);
			m_itemViews.append(itemView);

			int left = this->time2pos(item->left());
			int right = this->time2pos(item->right());
			itemView->display(left, right);

			if (selectedItem == item)
			{
				itemView->setSelected(true);
				selectedItemView = itemView;
				/* Only send the signal if there is a selected item */
				emit updateSelectedItem(selectedItemView);
			}
		}
	}

	if (selectedItemView && textNeedsRedraw)
		redrawTextEdit(selectedItemView, textBuffer);
}

void TierView::redrawTextEdit(ItemView *itemView, const QString &text)
{
	this->editItemText(itemView);
	m_textEditor->setText(text);
}

bool TierView::fitsWindow(Item *item) const
{
	return (item->left() >= m_window->start && item->left() <= m_window->end) ||
			(item->right() >= m_window->start && item->right() <= m_window->end);
}

int TierView::time2pos(double time) const
{
	return (int)(((time - m_window->start) * this->width()) / m_window->duration());
}

double TierView::pos2time(int pos) const
{
	return (((double)pos * m_window->duration()) / this->width()) + m_window->start;
}

ItemView * TierView::itemAtPos(int pos) const
{
	ItemView *itemView;

	foreach(itemView, m_itemViews)
	{
		QPair<int,int> screenPos = itemView->screenPos();

		if (screenPos.first <= pos && pos <= screenPos.second)
			return itemView;
	}

	return NULL;
}

void TierView::modifyItemText()
{
	m_textEditor->hide();

	QString text = m_textEditor->text();
	m_textEditor->clear();

	emit modifySelectedItemText(text);
}

void TierView::abandonItemChanges()
{
    m_textEditor->hide();
    m_textEditor->clear();
}

ItemBoundary * TierView::boundaryClickedAt(int pos) const
{
	ItemBoundary *boundary = NULL;
	ItemView *item = NULL;

	foreach(item, m_itemViews)
	{
		boundary = item->boundaryAt(pos);
		if (boundary)
			return boundary;
	}

	return NULL;
}

TierView::~TierView()
{
	delete m_window;
	delete m_textEditor;

	foreach(ItemView *i, m_itemViews)
		delete i;
}
