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
 * Created: 13/01/2011
 */

#include "PreferenceEditor.h"

PreferenceEditor::PreferenceEditor(QWidget *parent):
		QDialog(parent)
{
	this->setupUi();
}

void PreferenceEditor::setupUi()
{
	setWindowTitle(tr("Edit preferences..."));
	resize(400, 300);
    main_layout = new QVBoxLayout(this);
	tabs = new QTabWidget;

    tab_general = new QWidget;
    gen_layout = new QVBoxLayout(tab_general);
    tabs->addTab(tab_general, tr("General"));
	this->setupGeneralTab();

    tab_advanced = new QWidget;
    advanced_layout = new QVBoxLayout(tab_advanced);
    tabs->addTab(tab_advanced, tr("Advanced"));
	this->setupAdvancedTab();


    box_buttons = new QDialogButtonBox(this);
//	buttonBox->setGeometry(QRect(30, 200, 341, 32));
    box_buttons->setOrientation(Qt::Horizontal);
    box_buttons->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    main_layout->addWidget(tabs);
    main_layout->addWidget(box_buttons);

//	labEdit->setFocus(Qt::OtherFocusReason);

    connect(box_buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(box_buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void PreferenceEditor::setupGeneralTab()
{
	QString choose = tr("choose...");

	// App data dir
	QHBoxLayout *hbox1 = new QHBoxLayout;
	QPushButton *appdir_btn = new QPushButton(choose);
    edit_appdatadir = new QLineEdit;
    edit_appdatadir->setText(Global::AppDataDir);

	connect(appdir_btn, SIGNAL(clicked()), this, SLOT(setAppDataDirPath()));

    hbox1->addWidget(edit_appdatadir);
	hbox1->addWidget(appdir_btn);

	// Context window
	QHBoxLayout *hbox2 = new QHBoxLayout;
	QLabel *contextLabel = new QLabel(tr("Match context window:"));
    spinbox_match_context_window = new QSpinBox;
    spinbox_match_context_window->setRange(10, 100);
    spinbox_match_context_window->setSingleStep(1);
    spinbox_match_context_window->setValue(Global::MatchCotextLength);

	hbox2->addWidget(contextLabel);
    hbox2->addWidget(spinbox_match_context_window);

	// Search style
	QHBoxLayout *hbox3 = new QHBoxLayout;
	QLabel *ssLabel = new QLabel(tr("Search style:"));
    box_search_style = new QComboBox;
    box_search_style->addItem(DM_SEARCH_STYLE_TEXT);
    box_search_style->addItem(DM_SEARCH_STYLE_PERL);

    for (int i = 0; i < box_search_style->count(); ++i)
	{
        if (box_search_style->itemText(i) == Global::DefaultSearchStyle)
		{
            box_search_style->setCurrentIndex(i); break;
		}
	}

	hbox3->addWidget(ssLabel);
    hbox3->addWidget(box_search_style);

    gen_layout->addWidget(new QLabel(tr("Application data folder:")));
    gen_layout->addLayout(hbox1);
    gen_layout->addSpacing(20);
    gen_layout->addLayout(hbox2);
    gen_layout->addLayout(hbox3);
    gen_layout->addStretch();
}

void PreferenceEditor::setupAdvancedTab()
{
	// Praat Path
	QHBoxLayout *hboxPraat = new QHBoxLayout;
	QPushButton *praatPath_btn = new QPushButton(tr("Choose..."));
    edit_praat = new QLineEdit;
    edit_praat->setText(Global::PraatPath);

	connect(praatPath_btn, SIGNAL(clicked()), this, SLOT(setPraatPath()));

    hboxPraat->addWidget(edit_praat);
	hboxPraat->addWidget(praatPath_btn);

    // Antialiasing
    checkbox_antialiasing = new QCheckBox(tr("Use antialiasing in plots"));
    checkbox_antialiasing->setChecked(Global::UseAntialiasingInPlots);

    // Show/hide status bar
    checkbox_status = new QCheckBox(tr("Enable status bar"));
    checkbox_status->setChecked(Global::EnableStatusBar);

	// Application mode
	QHBoxLayout *hbox1 = new QHBoxLayout;
    box_mode = new QComboBox;
    box_mode->insertItem(0, "Default");
    box_mode->insertItem(1, "PFC");
    box_mode->insertItem(2, "PAC");

	if (Global::ApplicationMode == "PFC")
        box_mode->setCurrentIndex(1);
	else if (Global::ApplicationMode == "PAC")
        box_mode->setCurrentIndex(2);
	else
        box_mode->setCurrentIndex(0);

	hbox1->addWidget(new QLabel(tr("Application mode (most users should select \"Default\"):")));
    hbox1->addWidget(box_mode);

    advanced_layout->addWidget(new QLabel(tr("Praat path:")));
    advanced_layout->addLayout(hboxPraat);
    advanced_layout->addSpacing(10);
    advanced_layout->addLayout(hbox1);
    advanced_layout->addSpacing(10);
    advanced_layout->addWidget(checkbox_antialiasing);
    advanced_layout->addWidget(checkbox_status);
    advanced_layout->addStretch(0);
}

void PreferenceEditor::accept()
{
	// General tab
    Global::AppDataDir = edit_appdatadir->text();
    Global::MatchCotextLength = spinbox_match_context_window->value();
    Global::DefaultSearchStyle = box_search_style->currentText();

	// Advanced tab
    Global::PraatPath = edit_praat->text();
    Global::ApplicationMode = box_mode->currentText();
    Global::UseAntialiasingInPlots = checkbox_antialiasing->isChecked();
    Global::EnableStatusBar = checkbox_status->isChecked();

	QDialog::accept();
}

void PreferenceEditor::setAppDataDirPath()
{
    this->setLineEditFolder(edit_appdatadir);
}

void PreferenceEditor::setPraatPath()
{
    this->setLineEditFile(edit_praat);
}

void PreferenceEditor::setLineEditFolder(QLineEdit *line)
{
	QString path = nat(QFileDialog::getExistingDirectory(this->parentWidget(), tr("Choose folder..."),
														 dm_home()));

	if (path != "")
		line->setText(path);
}

void PreferenceEditor::setLineEditFile(QLineEdit *line)
{
	QString path = nat(QFileDialog::getOpenFileName(this->parentWidget(), tr("Choose folder..."),
													dm_home()));

	if (path != "")
		line->setText(path);
}

