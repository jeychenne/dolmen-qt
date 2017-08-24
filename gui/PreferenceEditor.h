/*
 * PreferenceEditor.h
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
 * Purpose: Edit preferences
 * Created: 13/01/2011
 */

#ifndef PREFERENCEEDITOR_H
#define PREFERENCEEDITOR_H

#include <QAction>
#include <QButtonGroup>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QFileDialog>
#include <QCheckBox>

#include "dolmen.h"
#include "utils/Global.h"
#include "utils/routines.h"

class PreferenceEditor: public QDialog
{
	Q_OBJECT

public:
	PreferenceEditor(QWidget *parent);

public slots:
	void accept();

private slots:
	void setAppDataDirPath();
	void setPraatPath();

private:
    QDialogButtonBox *box_buttons;
    QVBoxLayout      *main_layout, *gen_layout, *praat_layout, *advanced_layout;
    QTabWidget       *tabs;
    QLineEdit        *edit_appdatadir, *edit_praat;
    QWidget          *tab_general, *tab_praat, *tab_advanced;
    QSpinBox         *spinbox_match_context_window;
    QComboBox        *box_search_style, *box_mode;
    QCheckBox        *checkbox_antialiasing, *checkbox_status;
	void setupUi();
	void setupGeneralTab();
	void setupPraatTab();
	void setupAdvancedTab();

	void setLineEditFolder(QLineEdit *);
	void setLineEditFile(QLineEdit *);
};

#endif // PREFERENCEEDITOR_H
