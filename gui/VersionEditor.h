/*
 * VersionEditor.h
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
 * Purpose: create a new Version object
 * Created: 13/02/2011
 */

#ifndef VERSIONEDITOR_H
#define VERSIONEDITOR_H

#include <QButtonGroup>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QListWidget>
#include <QTextEdit>
#include <QDate>

#include "dolmen.h"
#include "gui/AuthorEditor.h"
#include "lib/metadata.h"
using namespace metadata;

class VersionEditor : public QDialog
{
	Q_OBJECT

public:
	VersionEditor(QWidget *parent, Version *version = NULL);

signals:
	void versionAvailable(Version *);
	void versionModified();

public slots:
	void accept();
	void reject();

private slots:
	void onAddAuthorClicked();
	void addAuthor(Author *author);
	void removeAuthor();
	void enableRemoveAuthorButton();

private:
    QDialogButtonBox *btn_box;
    QPushButton      *btn_remove_author;
    QVBoxLayout      *main_layout;
    QTextEdit        *desc_edit;
    QLineEdit        *date_line, *number_line;
    QListWidget      *author_list;

	// if true, we are creating a new version. If false, we're editing an existing one
    bool version_exists;

	Version *m_version;

	void setupUi();
	void displayAuthors();

};

#endif // VERSIONEDITOR_H
