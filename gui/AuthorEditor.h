/*
 * AuthorEditor.h
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
 * Purpose:
 * Created: 01/03/2011
 */


#ifndef AUTHOREDITOR_H
#define AUTHOREDITOR_H

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

#include "lib/metadata.h"
using namespace metadata;


class AuthorEditor : public QDialog
{
	Q_OBJECT

signals:
	void authorAvailable(Author *);

public slots:
	void accept();

public:
	AuthorEditor(QWidget *parent, Author *author = NULL);

private:
    QDialogButtonBox  *buttonBox;
    QVBoxLayout       *main_layout;
    QLineEdit         *name_line, *email_line;

    Author            *m_author;
};

#endif // AUTHOREDITOR_H
