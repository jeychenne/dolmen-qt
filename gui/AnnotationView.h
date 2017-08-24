/*
 * AnnotationView.h
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

#ifndef ANNOTATIONVIEW_H
#define ANNOTATIONVIEW_H

#include <QGroupBox>
#include <QTextEdit>
#include <QLabel>

#include "blocks/SignalBlock.h"
#include "blocks/TierBlock.h"
#include "View.h"
#include "lib/Annotation.h"

class AnnotationView: public View
{
	Q_OBJECT

public:
	AnnotationView(QTabWidget *parent, Annotation *annot, double start = 0., double end = 0);
	~AnnotationView();
	void leave();
	QString id() const;

private slots:

private:
	SignalBlock		*wave_block;
	TierBlock		*tier_block;
	Annotation		*m_annot;

	void setSignalAndTiers(double start, double end);
	void setTiersOnly(double start, double end);

};

#endif // ANNOTATIONVIEW_H
