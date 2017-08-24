/*
 * AnnotationView.cpp
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
 * Purpose: GUI view for an annotation
 * Created: 01/03/2011
 */

#include "AnnotationView.h"


AnnotationView::AnnotationView(QTabWidget *parent, Annotation *annot, double start, double end):
		View(parent)
{
	m_annot = annot;
	QFileInfo info(annot->path());
	int index = parent->indexOf(this);
	parent->setTabText(index, info.fileName());

	layout->addSpacing(5);

	/* if the item is a point, display 2 seconds around it */
	if (end && start == end)
	{
		start -= 1.0;
		end   += 1.0;
		if (start < 0)
			start = 0;
	}

	if (annot->soundFile() != NULL)
	{
		if (end > annot->soundFile()->length())
			end = annot->soundFile()->length();

		this->setSignalAndTiers(start, end);
	}
	else
		this->setTiersOnly(start, end);

	layout->addStretch();
	parent->setCurrentWidget(this);

	connect(tier_block, SIGNAL(displayText(QString)), this, SIGNAL(displayText(QString)));
	connect(tier_block, SIGNAL(tiersModified()), m_annot, SLOT(modifiedExternally()));
}

void AnnotationView::setSignalAndTiers(double start, double end)
{
	wave_block = new SignalBlock(this, m_annot->soundFile(), start, end);
	tier_block = new TierBlock(m_annot);

	connect(wave_block, SIGNAL(currentWindow(DMSelection)), tier_block, SLOT(updateCurrentWindow(DMSelection)));

	wave_block->refresh(); // re-emit missed signal

	layout->addLayout(wave_block);
	wave_block->insertTierLayout(tier_block);

	connect(tier_block, SIGNAL(soundSelection(int,int)), wave_block, SIGNAL(updateSelection(int,int)));
}

void AnnotationView::setTiersOnly(double start, double end)
{
    Q_UNUSED(start); Q_UNUSED(end);

    wave_block = NULL;
    tier_block = new TierBlock(m_annot);
    layout->addLayout(tier_block); //FIXME: this should perhaps be changed to avoid unnecessary layouts
}

QString AnnotationView::id() const
{
	return m_annot->path();
}

void AnnotationView::leave()
{
	/* clear output */
	emit displayText("");
}

AnnotationView::~AnnotationView()
{
	//BUG: crashes on deleting the sound block
	//delete wave_block;
	if (tier_block)
		delete tier_block;
}

