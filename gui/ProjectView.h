/*
 * ProjectView
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
 * Purpose:
 * Created: 01/03/2011
 */

#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

/* View to edit project properties */

#include "lib/Project.h"
#include "gui/View.h"
#include "gui/blocks/ChangelogBlock.h"


using namespace metadata;

class ProjectView : public View
{
public:
	ProjectView(QTabWidget *parent, Project *project);

	QString id() const;
	void leave();

private:
    Project        *m_project;
	ChangelogBlock *changelog_block;

	void setupUi();
};

#endif // PROJECTVIEW_H
