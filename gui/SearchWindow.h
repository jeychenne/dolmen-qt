/*
 * SearchWindow.h
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
 * Purpose: implement main search window. Since 0.9.6, the window is tabbed:
 * by default, there is one search tab, called "Main". Other search tabs can be
 * defined via search grammars.
 * Created: 23/09/2010
 */

#ifndef SEARCHWINDOW_H
#define SEARCHWINDOW_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QRadioButton>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QToolButton>
#include <QMessageBox>
#include <QTabWidget>

#include "dolmen.h"
#include "lib/metadata.h"
#include "search/QueryParser.h"
#include "search/Query.h"
#include "gui/CheckList.h"
#include "gui/FieldBox.h"
#include "gui/NumericValueEdit.h"
#include "gui/SearchGrammarTab.h"
#include "utils/Global.h"


using namespace metadata;



class SearchWindow : public QDialog
{
	Q_OBJECT

public:
    SearchWindow(QueryParser *engine, QWidget *parent, Grammar *grammar);

public slots:
	void accept();

private slots:
	void showHideProperties();
	void addSearchLine();
	void removeSearchLine();
    void setAnnotationsSelected();
    void setDocumentsSelected();

signals:
	void sendQuery(Query*);

private:
	QDialogButtonBox *buttonBox;
    QVBoxLayout      *main_layout;
    QHBoxLayout      *top_layout;
    QButtonGroup     *file_buttons;
    QRadioButton     *annotations_button, *sounds_button, *documents_button;
    QGroupBox        *fileButton_box, *search_box;
    QWidget          *tag_box;
    QLineEdit        *description_line;
    QCheckBox        *case_checkBox;
    QComboBox        *searchStyle_box, *description_box;
    QToolButton      *addLine_btn, *rmLine_btn;
    Grammar          *m_grammar;
    SearchGrammarTab *m_grammar_tab;

	// Keep track of search layout to add/remove search lines
	QVBoxLayout *sl_layout;

	// For each search line, we use a QHBoxLayout that contains:
	// - a DmLineEdit (text)
	// - a QSpinBox (search tier)
    QList<DMLineEdit*> search_line_list, tier_name_list;
    QList<QSpinBox*>   tier_spinbox_list;
	// display tier in cross-tier search
    QSpinBox          *displayTier_spinbox;
    QLabel            *displayTier_label; // hide in single-tier search

    QHBoxLayout       *showHideProperties_layout;
    QToolButton       *showHideProperties_arrow;
    QLabel            *showHideProperties_label;


    QList<CheckListBox*>     categoryList; // store regular categories
	QList<NumericValueEdit*> numCategoryList; // store numeric categories
    CheckList               *selectedFiles_box;
    QueryParser             *engine;

	void setupUi();
	void setupFileBox();
	void setupSearchBox();
	void setupPropertyBox();
    void setDisplayedFiles(); // filter files depending on which file type is selected
	QSpinBox * initTierSpinBox(int start = 0);
	QComboBox * initSearchStyleBox();
	QHBoxLayout * addSearchLineLayout(int start);

	QString getSearchFlags();

	QString buildQuery();
	QString buildSingleTierQuery();
    QString buildSearchGrammarQuery(SearchGrammarTab *tab);
    QString buildAnnotatorFlag(QString ref_annotator) const;
    QString buildDocumentQuery();
	QString buildCrossTierQuery();
    QString buildmetaStm(bool ignoreAnnotators = false);

    void toggleVisibleSearchWidgets(bool visible);
	void applySpecialModes(QString &text, int tier);
    void setupPropertyArrow();
    bool hasGrammar() const;
};

#endif // SEARCHWINDOW_H
