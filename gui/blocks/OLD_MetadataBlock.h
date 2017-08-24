/*
 * MetadataBlock.h
 *
 * Copyright (C) 2010-2011 Julien Eychenne
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
 * Purpose: "Blocks" are GUI components that are used across views
 * Created: 01/03/2011
 */


#ifndef METADATABLOCK_H
#define METADATABLOCK_H

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QLabel>
#include <QTextEdit>
#include <QToolButton>
#include <QLabel>

#include "lib/DFile.h"
#include "gui/blocks/HideableBlock.h"

class MetadataBlock: public HideableBlock
{
	Q_OBJECT

public:
	MetadataBlock(DFile *file);

private slots:
	void updateDescription();

private:
	DFile *m_file;
	QTextEdit *desc_edit, *meta_edit;
	QLabel *desc_label, *desc_html;
	void setupUI();
	void hideWidgets();
	void showWidgets();


};

#endif // METADATABLOCK_H
