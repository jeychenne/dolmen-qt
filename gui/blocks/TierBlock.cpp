/*
 * TierBlock.cpp
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
 * Purpose: A class a block of tiers. A tier block is bound to a signal block.
 * Created: 08/08/2011
 */

#include "TierBlock.h"


TierBlock::TierBlock(Annotation *annot):
	QVBoxLayout()
{
	m_annot				= annot;
	m_window			= new DMSelection(0.0, DEFAULT_WINDOW_LENGTH);
	selected_item		= NULL;
	hidden_selected_item	= NULL;
	tier_no				= INVALID_INDEX;

	this->setSpacing(0);

	int idx = 0;
	foreach(Tier *tier, annot->tiers())
		m_tiers << new TierView(tier, idx++);

	foreach (TierView *tier, m_tiers)
	{
		connect(tier, SIGNAL(selectItem(ItemView*)), this, SLOT(selectItem(ItemView*)));
		connect(tier, SIGNAL(soundSelection(int,int)), this, SIGNAL(soundSelection(int,int)));
		connect(tier, SIGNAL(updateSelectedItem(ItemView*)), this, SLOT(updateSelectedItem(ItemView*)));
		connect(tier, SIGNAL(modifySelectedItemText(QString)), this, SLOT(updateSelectedItemText(QString)));		
		/* make sure the hideTextEditor signal is received by all tiers */
		connect(tier, SIGNAL(hideAllTextEditors()), this, SIGNAL(hideTextEditor()));
		connect(this, SIGNAL(hideTextEditor()), tier, SLOT(hideTextEditor()));

		this->addWidget(tier);
	}
	this->addStretch();

	//this->updateDisplay(m_window->start, m_window->end);
}

void TierBlock::updateCurrentWindow(DMSelection sel)
{
	m_window->setWindow(sel.start, sel.end);
	this->updateDisplay(sel.start, sel.end);
}

void TierBlock::selectItem(ItemView *itemView)
{
	if (selected_item)
		selected_item->setSelected(false);

	if (itemView) // if no item is selected, NULL is received
	{
		itemView->setSelected(true);
		tier_no = itemView->tierView()->index();

		emit displayText(itemView->toHtml());
	}

	selected_item = itemView;
	hidden_selected_item = NULL;

	//m_tiers[tier_no]->signalSelectedItem(itemView);
}

void TierBlock::updateSelectedItem(ItemView *i)
{
	selected_item = i;
	//TODO: check that there's no problem with hidden item
}

void TierBlock::updateDisplay(double start, double end)
{
	Item *item = NULL;

	/* Keep the underlying item that must be selected in the UI */
	if (selected_item)
	{
		item = selected_item->item();
		selected_item = NULL;
	}

	int i;
	for (i = 0; i < m_tiers.size(); ++i)
	{
		TierView *tierView = m_tiers.at(i);
		tierView->updateWindow(start, end);

		// update selected item
		if (i == tier_no)
		{
			/* if there is a hidden item, use it, otherwise try to use the selected item, if any */
			if (hidden_selected_item)
			{
				item = hidden_selected_item;
				hidden_selected_item = NULL;
			}

			if (item)
			{
				ItemView *itemView = tierView->findItemView(item);
				// if item fits the window, display it. Otherwise,
				// keep it for later
				if (itemView)
				{
					this->selectItem(itemView);
					selected_item = itemView;
				}
				else
				{
					hidden_selected_item = item;
				}
			}
		}
	}
}

void TierBlock::updateSelectedItemText(const QString &text)
{
	if (selected_item)
	{
		if (selected_item->setText(text))
		{
			emit displayText(selected_item->toHtml());
			emit tiersModified();
		}
	}
}

TierBlock::~TierBlock()
{
	delete m_window;

	foreach(TierView *t, m_tiers)
		delete t;
}
