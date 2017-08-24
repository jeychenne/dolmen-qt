/*
 * dolmen.h
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
 * Created: //
 */

#include "AuthorEditor.h"


AuthorEditor::AuthorEditor(QWidget *parent, Author *author) :
		QDialog(parent)
{
	m_author = author;

	setWindowTitle(tr("Edit author..."));
	resize(300, 100);
	main_layout = new QVBoxLayout(this);

	name_line = new QLineEdit;
	email_line = new QLineEdit;

	buttonBox = new QDialogButtonBox(this);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

	main_layout->addWidget(new QLabel("Name:"));
	main_layout->addWidget(name_line);
	main_layout->addWidget(new QLabel(tr("Email:")));
	main_layout->addWidget(email_line);
	main_layout->addWidget(buttonBox);

	if (m_author)
	{
		name_line->setText(m_author->name());
		email_line->setText(m_author->email());
	}

	name_line->setFocus(Qt::OtherFocusReason);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

}

void AuthorEditor::accept()
{
	if (name_line->text() == "") return;

	if (m_author)
	{
		m_author->setName(name_line->text());
		m_author->setEmail(email_line->text());
	}
	else
		m_author = new Author(name_line->text(), email_line->text());

	emit authorAvailable(m_author);
	QDialog::accept();
}

