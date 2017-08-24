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

#include "VersionEditor.h"

VersionEditor::VersionEditor(QWidget *parent, Version *version) :
		QDialog(parent)
{
	if (version)
	{
		m_version = version;
		version_exists = true;
	}
	else
	{
		m_version = new Version("");
		version_exists = false;
	}

	this->setupUi();
}

void VersionEditor::setupUi()
{
	setWindowTitle(tr("Edit version..."));
	resize(500, 500);
	main_layout = new QVBoxLayout(this);

	desc_edit = new QTextEdit(m_version->description());
	date_line = new QLineEdit(m_version->date().toString(DM_DATE_FORMAT));
	number_line = new QLineEdit(m_version->number());
	number_line->setToolTip(tr("Example: \"1.2\" (without quotes)"));
	author_list = new QListWidget;
	this->displayAuthors();

	QHBoxLayout *author_layout = new QHBoxLayout;
	QPushButton *addAuthor_btn = new QPushButton(tr("Add author"));
	btn_remove_author = new QPushButton(tr("Remove author"));
	author_layout->addWidget(addAuthor_btn);
	author_layout->addWidget(btn_remove_author);
	author_layout->addStretch(0);

	main_layout->addWidget(new QLabel(tr("Description:")));
	main_layout->addWidget(desc_edit);
	main_layout->addWidget(new QLabel(tr("Date:")));
	main_layout->addWidget(date_line);
	main_layout->addWidget(new QLabel(tr("Version number:")));
	main_layout->addWidget(number_line);
	main_layout->addWidget(new QLabel(tr("Authors:")));
	main_layout->addWidget(author_list);
	main_layout->addLayout(author_layout);

	desc_edit->setToolTip(tr("The description summarizes the changes in a version.\nIt is the only compulsory field."));
	desc_edit->setFocus(Qt::OtherFocusReason);

	btn_box = new QDialogButtonBox(this);
	btn_box->setOrientation(Qt::Horizontal);
	btn_box->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

	main_layout->addWidget(btn_box);

	btn_remove_author->setEnabled(false);

	connect(btn_box, SIGNAL(accepted()), this, SLOT(accept()));
	connect(btn_box, SIGNAL(rejected()), this, SLOT(reject()));
	connect(addAuthor_btn, SIGNAL(clicked()), this, SLOT(onAddAuthorClicked()));
	connect(btn_remove_author, SIGNAL(clicked()), this, SLOT(removeAuthor()));
	connect(author_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
			this, SLOT(enableRemoveAuthorButton()));
}

void VersionEditor::accept()
{
	// only allow versions that have a description
	if (desc_edit->toPlainText() == "") return;

	m_version->setDescription(desc_edit->toPlainText());
	m_version->setDate(QDate::fromString(date_line->text(), DM_DATE_FORMAT));
	m_version->setNumber(number_line->text());

	if (version_exists)
		emit versionModified();
	else
		emit versionAvailable(m_version);

	QDialog::accept();
}

void VersionEditor::reject()
{
	delete m_version;
	QDialog::reject();
}

void VersionEditor::onAddAuthorClicked()
{
	AuthorEditor *editor = new AuthorEditor(this);
	connect(editor, SIGNAL(authorAvailable(Author*)), this, SLOT(addAuthor(Author *)));
	editor->show();
}

void VersionEditor::addAuthor(Author *author)
{
	m_version->addAuthor(author);
	this->displayAuthors();
	btn_remove_author->setEnabled(false);
}

void VersionEditor::removeAuthor()
{
	m_version->removeAuthor(author_list->currentIndex().row());
	this->displayAuthors();
	btn_remove_author->setEnabled(false);
}

void VersionEditor::displayAuthors()
{
	if (m_version)
	{
		author_list->clear();

		foreach (Author *author, m_version->authors())
		{
			QListWidgetItem *item = new QListWidgetItem(author->name(), author_list);
			Q_UNUSED(item)
		}
	}
}

void VersionEditor::enableRemoveAuthorButton()
{
	btn_remove_author->setEnabled(true);
}
