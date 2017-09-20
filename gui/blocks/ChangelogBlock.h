/*
 * ChangelogBlock.h
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

#ifndef CHANGELOGBLOCK_H
#define CHANGELOGBLOCK_H

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>

#include "gui/VersionEditor.h"
#include "lib/metadata.h"

using namespace metadata;

class ChangelogBlock : public QVBoxLayout
{
	Q_OBJECT

public:
	ChangelogBlock(Changelog *changelog, QWidget *parent = NULL);

signals:
	void showVersion(const QString &);

private slots:
	void onAddVersionClicked();
	void onEditVersionClicked();
	void addVersion(Version *);
	void removeVersion();
	void toggleVersionButtons(bool value = true);
	void getVersion();

private:
	QListWidget *version_list;
	QPushButton *removeVersion_btn, *editVersion_btn;
	Changelog *m_changelog;

	void setupUi();
	void setupEditVersions();
	void displayVersions();
};

#endif // CHANGELOGBLOCK_H
