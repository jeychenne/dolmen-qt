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
 * Purpose: main search window
 * Created: 23/09/2010
 */

#include <QScrollArea>
#include "SearchWindow.h"

SearchWindow::SearchWindow(QueryParser *engine, QWidget *parent, Grammar *grammar) :
		QDialog(parent)
{
	this->engine = engine;
    m_grammar = grammar;

	setupUi();

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void SearchWindow::setupUi()
{
    QString title = tr("Search window");
    QWidget *tab;

    if (m_grammar)
    {
        m_grammar_tab = new SearchGrammarTab(m_grammar);
        tab = m_grammar_tab;
        title.append(": ");
        title.append(m_grammar->name());
    }
    else
    {
        tab = new QWidget;
        m_grammar_tab = nullptr;
        QVBoxLayout *main_tab_outer_layout = new QVBoxLayout;
        main_tab_outer_layout->setContentsMargins(0,0,0,0);
        main_tab_outer_layout->setSpacing(0);
        top_layout = new QHBoxLayout;

        setupFileBox();
        setupSearchBox();

        top_layout->addWidget(fileButton_box);
        top_layout->addSpacing(10);
        top_layout->addWidget(search_box);
        main_tab_outer_layout->addLayout(top_layout);
        main_tab_outer_layout->addStretch();
        tab->setLayout(main_tab_outer_layout);
    }

	setupPropertyBox();

	// description
	QHBoxLayout *desc_layout = new QHBoxLayout;
	QLabel *desc_label = new QLabel(tr("Description "));

    description_box = new QComboBox;
	description_box->addItem(tr("is exactly"));
	description_box->addItem(tr("is not"));
	description_box->addItem(tr("contains"));
	description_box->addItem(tr("doesn't contain"));
	description_box->setCurrentIndex(2);
	description_line = new QLineEdit;
	description_line->setToolTip(tr("Filter files based on whether their description is (or not)\nan exact text or contains (or not) a regular expression."));


	desc_layout->addWidget(desc_label);
	desc_layout->addWidget(description_box);
	desc_layout->addWidget(description_line);

    QScrollArea* scroll_area = new QScrollArea;
    scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll_area->setWidgetResizable(true);
    scroll_area->setWidget(tab);

    main_layout = new QVBoxLayout;
    main_layout->addWidget(scroll_area);
    setWindowTitle(title);

	main_layout->addSpacing(10);
    setupPropertyArrow();
    main_layout->addWidget(tag_box);
	main_layout->addLayout(desc_layout);

	buttonBox = new QDialogButtonBox(this);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

	main_layout->addStretch(2);
	main_layout->addWidget(buttonBox);

    this->setWindowFlags(Qt::Window);
    this->setLayout(main_layout);

    if (!m_grammar) {
        search_line_list.at(search_line_list.count()-1)->setFocus(Qt::OtherFocusReason);
    }
}

void SearchWindow::setupPropertyArrow()
{
    // Let the user show or hide properties by clicking on the arrow
    showHideProperties_layout = new QHBoxLayout;
    showHideProperties_arrow  = new QToolButton;
    showHideProperties_label  = new QLabel(tr("Hide properties"));
    showHideProperties_layout->addWidget(showHideProperties_arrow);
    showHideProperties_layout->addWidget(showHideProperties_label);
    showHideProperties_arrow->setArrowType(Qt::DownArrow);
    showHideProperties_arrow->setText("Advanced");
    main_layout->addLayout(showHideProperties_layout);

    connect(showHideProperties_arrow, SIGNAL(clicked()), this, SLOT(showHideProperties()));
}

bool SearchWindow::hasGrammar() const
{
    return m_grammar != nullptr;
}

QString SearchWindow::getSeparator() const
{
    QString sep = separator_line->text();
    return sep.replace(" ", "%SPACE%");
}

void SearchWindow::setupFileBox()
{
	fileButton_box = new QGroupBox(tr("Files"));

//	fileButton_box->setToolTip(tr("Select the type of files you want to search in"));

    QVBoxLayout *file_layout = new QVBoxLayout; // main box layout
    QVBoxLayout *button_layout = new QVBoxLayout; // top layout

    annotations_button = new QRadioButton(tr("Annotation files"));
    annotations_button->setChecked(true);
//    sounds_button = new QRadioButton(tr("Sound files"));
//    sounds_button->setEnabled(false);
    documents_button = new QRadioButton(tr("Documents"));

    // make buttons work together
    file_buttons = new QButtonGroup;
    file_buttons->addButton(documents_button);
    file_buttons->addButton(annotations_button);
//    file_buttons->addButton(sounds_button);

    button_layout->addWidget(documents_button);
    button_layout->addWidget(annotations_button);
//    buttons_layout->addWidget(sounds_button);
    button_layout->addStretch(1);


    selectedFiles_box = new CheckList(QStringList(), this);
    setDisplayedFiles();

    file_layout->addLayout(button_layout);
    file_layout->addWidget(selectedFiles_box);
    fileButton_box->setLayout(file_layout);

    connect(documents_button, SIGNAL(clicked()), this, SLOT(setDocumentsSelected()));
    connect(annotations_button, SIGNAL(clicked()), this, SLOT(setAnnotationsSelected()));
}

void SearchWindow::setDisplayedFiles()
{
    // file names are displayed in the file box ; the full path is stored in
    // the item's toolTip;
    QStringList fullPaths;

    if (annotations_button->isChecked())
    {

        foreach (DFile *f, Global::FileHash.values())
        {
            if (isInstance(f, Annotation))
                fullPaths << f->path();
        }

    }
    else if (documents_button->isChecked())
    {
        foreach (DFile *f, Global::FileHash.values())
        {
            if (isInstance(f, Document))
                fullPaths << f->path();
        }
    }
    else if (sounds_button->isChecked())
    {
        foreach (DFile *f, Global::FileHash.values())
        {
            if (isInstance(f, Sound))
                fullPaths << f->path();
        }
    }
    else
    {
        fullPaths = Global::FileHash.keys();
    }
    fullPaths.sort();

    QStringList shortPaths;
    foreach (QString file, fullPaths)
        shortPaths << QFileInfo(file).fileName();

    selectedFiles_box->resetLabels(shortPaths, fullPaths);
}

void SearchWindow::setupSearchBox()
{
	search_box = new QGroupBox(tr("Search"));
	search_box->setToolTip(tr("Enter the text or the regular expression you want to search for"));

	QVBoxLayout *searchBox_layout = new QVBoxLayout;

	addLine_btn = new QToolButton;
    addLine_btn->setIcon(QIcon(":/icons/add.png"));
	addLine_btn->setShortcut(QKeySequence("alt++"));
	addLine_btn->setToolTip(tr("Add search tier (alt +)"));
	rmLine_btn  = new QToolButton;
    rmLine_btn->setIcon(QIcon(":/icons/remove.png"));
	rmLine_btn->setShortcut(QKeySequence("alt+-"));
	rmLine_btn->setToolTip(tr("Remove search tier (alt -)"));
	rmLine_btn->setEnabled(false);

	QHBoxLayout *display_layout = new QHBoxLayout;
	displayTier_label = new QLabel(tr("Display text:"));
	display_layout->addWidget(displayTier_label);
	displayTier_spinbox = this->initTierSpinBox(1);
	displayTier_spinbox->hide();
	displayTier_label->hide();

    QHBoxLayout *sep_layout = new QHBoxLayout;
    separator_line = new QLineEdit(" ");
    sep_layout->addWidget(new QLabel(tr("Separator (space by default):")));
    sep_layout->addWidget(separator_line);
    sep_layout->addStretch();
    display_layout->addWidget(displayTier_spinbox);

	display_layout->addStretch();

	QHBoxLayout *btn_layout = new QHBoxLayout;
	//btn_layout->addWidget(new QLabel(tr("Add/Remove search tiers:")));
	btn_layout->addWidget(addLine_btn);
	btn_layout->addWidget(rmLine_btn);
	btn_layout->addStretch();

    QHBoxLayout *regex_layout = new QHBoxLayout;
	searchStyle_box = this->initSearchStyleBox();
	case_checkBox = new QCheckBox(tr("case sensitive"));
    regex_layout->addWidget(searchStyle_box);
    regex_layout->addWidget(case_checkBox);
    regex_layout->addStretch();

	//QHBoxLayout *option_layout = new QHBoxLayout;

	sl_layout = new QVBoxLayout;
	sl_layout->addLayout(this->addSearchLineLayout(0));

	searchBox_layout->addLayout(display_layout);
	searchBox_layout->addLayout(sl_layout);
    searchBox_layout->addLayout(btn_layout);
    searchBox_layout->addLayout(regex_layout);
	//searchBox_layout->addWidget(new QLabel(tr("Search options:")));
	//searchBox_layout->addLayout(option_layout);
    searchBox_layout->addLayout(sep_layout);
	searchBox_layout->addStretch();

	search_box->setLayout(searchBox_layout);

	connect(addLine_btn, SIGNAL(clicked()), this, SLOT(addSearchLine()));
	connect(rmLine_btn, SIGNAL(clicked()), this, SLOT(removeSearchLine()));
}


/* Properties are grouped by category. Each category is represented by a checklist, with each property being
a checkable item in the list. Properties are displayed in a grid layout, with 3 columns per row. */
void SearchWindow::setupPropertyBox()
{
	QMultiHash<QString, QString> hash = hashProperties(Property::properties);
	QStringList categories = hash.uniqueKeys();
	categories.sort();

    tag_box = new QWidget;
    QGridLayout *prop_layout = new QGridLayout;
    prop_layout->setVerticalSpacing(10);
    prop_layout->setHorizontalSpacing(10);
    prop_layout->setContentsMargins(0,0,0,0);

	int col = 0;
	int row = 0;


	for (int i = 0; i < categories.size(); i++)
	{
		QString label = (categories[i] == "")? DM_DEFAULT_CAT: categories[i];
		QStringList values = hash.values(categories[i]);
		values.sort();

		if (dm_isListNumeric(values))
		{
			QGroupBox *nbox = new QGroupBox(label);
			QVBoxLayout *vlayout = new QVBoxLayout;
			NumericValueEdit *edit = new NumericValueEdit(label);

			//vlayout->addWidget(new QLabel(tr("Enter numeric value:")));
			vlayout->addLayout(edit);
			vlayout->addStretch(1); // expand as needed
			nbox->setLayout(vlayout);
            prop_layout->addWidget(nbox, row, col);

			numCategoryList << edit;
		}
		else
		{
            CheckListBox *lbox = new CheckListBox(label, values);

            prop_layout->addWidget(lbox, row, col);
			categoryList << lbox;
		}

		if ((i+1) % 3 == 0)
		{
			row++;
			col = 0;
		}
		else
			col++;
	}

    tag_box->setLayout(prop_layout);

	if (categories.size() == 0)
		tag_box->hide();
}

void SearchWindow::accept()
{
    if (! hasGrammar())
    {
        for (int i = 0; i < search_line_list.count(); ++i)
        {
            if (search_line_list.at(i)->text().isEmpty())
                return;
        }

        if (search_line_list.size() != 1 && tier_spinbox_list.at(0)->value() == 0)
        {
            QMessageBox::critical(this, tr("Cross-tier search error"),
                                  tr("You need to select a specific tier as the first search tier"));
            return;
        }
    }

    // build query
    Query *query = engine->parse(this->buildQuery(), m_grammar);
    if (query) emit sendQuery(query);

    QDialog::accept();
}

QString SearchWindow::buildQuery()
{
    if (! hasGrammar())
    {
        if (search_line_list.size() == 1)
        {
            if (documents_button->isChecked())
                return buildDocumentQuery();

            else if (annotations_button->isChecked())
                return buildSingleTierQuery();
        }
        else
            return buildCrossTierQuery();
    }
    else
    {
        return buildSearchGrammarQuery(m_grammar_tab);
    }
}

QString SearchWindow::buildSearchGrammarQuery(SearchGrammarTab *tab)
{
    QString tierName = "";
    QString searchStm, data;
    bool compare = tab->compareAnnotators();

    // SEARCH statement
    QString flags = QString(DM_SEARCH_FLAG_PERL) + QString(" ");

    if (tab->caseSensitive())
        flags += QString(DM_SEARCH_FLAG_CASE) + QString(" ");

    if (compare)
        flags += buildAnnotatorFlag(tab->referenceAnnotator());

    if (tab->tier() == -1)
         searchStm = QString("SEARCH @tier[0] %1").arg(flags);
    else
        searchStm = QString("SEARCH @tier[%1] %2").arg(tab->tier()).arg(flags);

    // FROM statement
    QString fromStm = "FROM " + tab->filetype() + " ";

    // WHERE statement : if there is a tier selecting field, use the tier names of
    // the selected values
    QStringList names;
    tierName = tab->tierName();

    if (!tierName.isEmpty())
    {
        tierName = QString("{$%1}").arg(tierName);
    }

    data = QString("(@item%1.text ~ \"%2\")").arg(tierName).arg(tab->regex());
    QString whereStm = QString("WHERE %1 && %2 ").arg(data, buildmetaStm(compare));

    // RETURN statement
    QString returnStm = QString("RETURN @item.match;");


    qDebug() << searchStm + fromStm + whereStm + returnStm;
    return searchStm + fromStm + whereStm + returnStm;
}

QString SearchWindow::buildAnnotatorFlag(QString ref_annotator) const
{
    QStringList annotators;

    foreach (CheckListBox *box, categoryList)
    {
        if (box->title() == "Annotator")
        {
            annotators = box->checkedLabels();
            if (annotators.isEmpty())
            {
                box->checkAll(Qt::Checked);
                annotators = box->checkedLabels();
            }
            break;
        }
    }

    // annotators are separated by slashes. The reference annotator is repeated at the beginning of the list
    QString flag = QString("-annotators-ref/%1//%2/ ").arg(ref_annotator).arg(annotators.join("/"));

    return flag;
}


QString SearchWindow::buildDocumentQuery()
{
//SEARCH @file[0]  -perl FROM %document WHERE (@document.text ~ &quot;plus&quot;) &amp;&amp; () RETURN @document.match;
    QString searchStm, text, data;


    // SEARCH statement
    searchStm = QString("SEARCH @file[x] %1").arg(this->getSearchFlags());


    // FROM statement
    QString fromStm = "FROM ";
    QStringList files;
    foreach (QCheckBox *item, selectedFiles_box->checkedItems())
        files << item->toolTip();

    if (files.size() == 0)
        fromStm += "%document ";
    else
        fromStm += files.join(", ") + " ";


    // WHERE statement
    text = search_line_list.at(0)->text();
    if (searchStyle_box->currentText() == DM_SEARCH_STYLE_PERL)
    {
        int dummyTier = -1;
        this->applySpecialModes(text, dummyTier);
    }

    data = QString("(@document.text ~ \"%1\")").arg(text);
    QString whereStm = QString("WHERE %1 && %2 ").arg(data, buildmetaStm());

    // RETURN statement
    QString returnStm = QString("RETURN @document.match;");

    qDebug() << searchStm + fromStm + whereStm + returnStm;
    return searchStm + fromStm + whereStm + returnStm;
}

QString SearchWindow::buildSingleTierQuery()
{
    QString searchStm, text, data;
    QString tierName;
	int firstTier = -1;

	// SEARCH statement
	firstTier = tier_spinbox_list.at(0)->value();
    tierName = tier_name_list.at(0)->text();


    if (! tierName.isEmpty())
    {
        // if there is a tier name, we will search by name rather than by number.
        // In this case, we make sure that we search in all tiers
        firstTier = 0;
        // format the @item argument
        tierName = QString("{$%1}").arg(tierName);
    }

	searchStm = QString("SEARCH @tier[%1] %2").arg(firstTier).arg(this->getSearchFlags());


	// FROM statement
	QString fromStm = "FROM ";
	QStringList files;
	foreach (QCheckBox *item, selectedFiles_box->checkedItems())
		files << item->toolTip();

	if (files.size() == 0)
		fromStm += "%annotation ";
	else
		fromStm += files.join(", ") + " ";


	// WHERE statement
	text = search_line_list.at(0)->text();
    if (searchStyle_box->currentText() == DM_SEARCH_STYLE_PERL)
        this->applySpecialModes(text, firstTier);

    data = QString("(@item%1.text ~ \"%2\")").arg(tierName).arg(text);
	QString whereStm = QString("WHERE %1 && %2 ").arg(data, buildmetaStm());

	// RETURN statement
    QString returnStm = QString("RETURN <%1> @item.match;").arg(getSeparator());

	qDebug() << searchStm + fromStm + whereStm + returnStm;
	return searchStm + fromStm + whereStm + returnStm;
}


QString SearchWindow::buildCrossTierQuery()
{
    QString searchStm, text, data;
    QStringList texts;

	// SEARCH statement
    searchStm = QString("SEARCH @tier[x]%1").arg(this->getSearchFlags());

	// FROM statement
	QString fromStm = "FROM ";
	QStringList files;
    for(QCheckBox *item : selectedFiles_box->checkedItems())
		files << item->toolTip();

	if (files.size() == 0)
		fromStm += "%annotation ";
	else
		fromStm += files.join(", ") + " ";

	// WHERE statement
	for (int i = 0; i < search_line_list.count(); ++i)
	{
		text = search_line_list.at(i)->text();
		this->applySpecialModes(text, tier_spinbox_list.at(i)->value());
		texts << text;
	}

    QStringList stms;

	for (int i = 0; i < search_line_list.count(); ++i)
	{
        QString tierName = tier_name_list.at(i)->text();
        QString option;
        if (tierName.isEmpty())
            option = tier_spinbox_list.at(i)->cleanText();
        else
            option = QString("$") + tierName;

        stms << QString("(@item{%1}.text ~ \"%2\")").arg(option).arg(texts.at(i));

        QString relation;
        if (tier_relation_list.at(i)->isEnabled())
        {
            auto value = tier_relation_list.at(i)->currentText();

            if (value == "is aligned with") {
                relation = "AND:align";
            }
            else if (value == "precedes") {
                relation = "AND:prec";
            }
            else if (value == "dominates") {
                relation = "AND:dom";
            }

            stms << relation;
        }
	}

    data = QString("(%1)").arg(stms.join(" "));
	QString whereStm = QString("WHERE %1 && %2 ").arg(data, buildmetaStm());

	// RETURN statement
	int dispTier = displayTier_spinbox->value();
	int firstTier = tier_spinbox_list.at(0)->value();
	QString returnStm = "";
	if (dispTier == firstTier)
		returnStm = QString("RETURN @item.match;");
	else
        returnStm = QString("RETURN <%1> @graphnode{%2,%3}.crosstext;")
                    .arg(getSeparator())
                    .arg(firstTier) // root node tier
                    .arg(dispTier); // display tier

	qDebug() << searchStm + fromStm + whereStm + returnStm;
	return searchStm + fromStm + whereStm + returnStm;
}


QString SearchWindow::getSearchFlags()
{
    QStringList flags;
    QString caseFlag = case_checkBox->isChecked()? DM_SEARCH_FLAG_CASE: "";
	QString styleFlag = searchStyle_box->currentText();

	if (styleFlag == DM_SEARCH_STYLE_PERL)
        styleFlag = DM_SEARCH_FLAG_PERL;

	else if (styleFlag == DM_SEARCH_STYLE_TEXT)
        styleFlag = DM_SEARCH_FLAG_TEXT;

    flags << caseFlag << styleFlag;

    return " " + flags.join(" ") + " ";
}

QString SearchWindow::buildmetaStm(bool ignoreAnnotators)
{
	QStringList meta;

	// regular properties
	foreach (CheckListBox *box, categoryList)
	{
		QStringList catConditions;
		QString cat = (box->title() == DM_DEFAULT_CAT)? "": box->title();
		dm_escape(cat);

        if (! ignoreAnnotators || cat != "Annotator")
        {
            foreach (QString label, box->checkedLabels())
            {
                dm_escape(label);
                catConditions << QString("((@tag.category = \"%1\") AND (@tag.label = \"%2\"))").arg(cat, label);
            }

            if (catConditions.size() == 1)
                meta << catConditions[0];
            else if (catConditions.size() > 1)
                meta << QString("(%1)").arg(catConditions.join(" OR "));
        }
	}

	// numeric properties
	foreach (NumericValueEdit *num, numCategoryList)
	{
		if (num->value() == 0)
			continue;

		QString cat = num->description();
		dm_escape(cat);
		meta << QString("((@tag.category = \"%1\") AND (@tag.label %2 \"%3\"))")
				.arg(cat).arg(num->opcode()).arg(num->value());
	}

	// description
	QString desc = description_line->text();
	dm_escape(desc);

	if (desc != "")
	{
		switch (description_box->currentIndex())
		{
		case 0: // is exactly
			desc = "(@file.description = \"" + desc + "\")";
			break;
		case 1: // is not
			desc = "(NOT @file.description = \"" + desc + "\")";
			break;
		case 2: // contains
			desc = "(@file.description ~ \"" + desc + "\")";
			break;
		case 3: // does not contain
			desc = "(NOT @file.description ~ \"" + desc + "\")";
			break;
		default:
			break;
		}

		meta << desc;
	}


	// format result depending on # of items
	switch (meta.size())
	{
	case 0:
		return "()";
	case 1:
		return meta[0];
	default:
		return QString("(%1)").arg(meta.join(" AND "));
	}
}

void SearchWindow::showHideProperties()
{
	switch (showHideProperties_arrow->arrowType())
	{
	case Qt::RightArrow:
		showHideProperties_arrow->setArrowType(Qt::DownArrow);
		foreach (CheckListBox *box, categoryList)
			box->show();
		foreach (NumericValueEdit *num, numCategoryList)
			num->show();
		tag_box->show();

		showHideProperties_label->setText(tr("Hide properties"));
		break;
	case Qt::DownArrow:
		showHideProperties_arrow->setArrowType(Qt::RightArrow);
		foreach (CheckListBox *box, categoryList)
			box->hide();
		foreach (NumericValueEdit *num, numCategoryList)
			num->hide();
		tag_box->hide();

		showHideProperties_label->setText(tr("Show properties"));
		break;

	default:
		break;
	}
}


void SearchWindow::applySpecialModes(QString &text, int tier)
{
	// escape '#' before anything else
	text.replace("\\#", "__%__"); // escape '#'
	// PFC/PAC
	if (tier == 2 && Global::ApplicationMode == "PAC")
	{
		if (text == "***")
			text = "[0-3][1-2][1-2]"; //TODO: get full pattern "[0-3][1-2][1-2](h|rh)?" (parentheses are chopped off by the query parser)
		else
			text.replace('*', "[0-3]");
	}
	else if (tier == 2 && Global::ApplicationMode == "PFC")
	{
		text.replace('%', "[^e]");
		text.replace('*', "\\d");
	}
	else if (tier == 3 && Global::ApplicationMode == "PFC")
	{
		text.replace('*', "[0-4]");
		text.replace('C', "[bcdfgjklmnpqrstvwxzJRSZ]");
	}
	// "#*" and "*#" are not compatible with PAC and PFC modes
	else
	{
		// General case
		text.replace("#*", "\\b\\w*");
		text.replace("*#", "\\w*\\b");
	}
	// "#" is a convenience symbol for word boundaries
	text.replace('#', "\\b");
	// restore escaped "#"
	text.replace("__%__", "#");
	// convenience variables
	text.replace("%LINE", "^\\.+$");
	text.replace("%WORD", "\\b\\w+\\b"); // match a word in text

	dm_escape(text);
}


QSpinBox * SearchWindow::initTierSpinBox(int start)
{
	QSpinBox *sb = new QSpinBox;
	sb->setRange(start, 100);
	sb->setSingleStep(1);
	sb->setPrefix(tr("In tier "));
	if (start == 0)
		sb->setSpecialValueText(tr("In any tier"));
	sb->setValue(start);

	return sb;
}

QComboBox * SearchWindow::initSearchStyleBox()
{
	QComboBox *cb = new QComboBox;
	cb->addItem(DM_SEARCH_STYLE_TEXT);
	cb->addItem(DM_SEARCH_STYLE_PERL);

	for (int i = 0; i < cb->count(); i++)
	{
		if (cb->itemText(i) == Global::DefaultSearchStyle)
		{
			cb->setCurrentIndex(i); break;
		}
	}

	return cb;
}

QHBoxLayout * SearchWindow::addSearchLineLayout(int start)
{
    QHBoxLayout *layout     = new QHBoxLayout;
	DMLineEdit *search_line = new DMLineEdit(tr("Search..."));
	QSpinBox *tier_spinbox  = this->initTierSpinBox(start);
    DMLineEdit *tier_name   = new DMLineEdit(tr("tier name pattern..."));
    QComboBox *relation     = new QComboBox;

    relation->addItem(tr("is aligned with"));
    relation->addItem(tr("precedes"));
    relation->addItem(tr("dominates"));

	search_line_list << search_line;
	tier_spinbox_list << tier_spinbox;
    tier_name_list << tier_name;
    tier_relation_list << relation;

	layout->addWidget(search_line);
	layout->addWidget(tier_spinbox);
    layout->addWidget(tier_name);
    layout->addWidget(relation);

    for (int i = 1; i <= tier_relation_list.size(); i++)
    {
        bool value = (i != tier_relation_list.size());
        tier_relation_list.at(i-1)->setEnabled(value);
    }

	search_line_list.at(search_line_list.count()-1)->setFocus(Qt::OtherFocusReason);

	return layout;
}

void SearchWindow::addSearchLine()
{
	sl_layout->addLayout(this->addSearchLineLayout(1));

	if (! rmLine_btn->isEnabled())
	{
		rmLine_btn->setEnabled(true);
		displayTier_spinbox->show();
		displayTier_label->show();
	}

	if (tier_spinbox_list.at(0)->value() == 0)
	{
		tier_spinbox_list.at(0)->setValue(1);
        // adjust since we're adding the first line
		if (tier_spinbox_list.count() >= 2 && tier_spinbox_list.at(1)->value() == 1)
			tier_spinbox_list.at(1)->setValue(2);
	}

    bool enabled = tier_relation_list.size() > 1;
    tier_relation_list.first()->setEnabled(enabled);

    search_line_list.at(search_line_list.count()-1)->setFocus(Qt::OtherFocusReason);
}

void SearchWindow::removeSearchLine()
{
	QHBoxLayout *layout = static_cast<QHBoxLayout*>(sl_layout->takeAt(sl_layout->count()-1));

	if (layout)
	{
		QLayoutItem *item = NULL;
		while ((item = layout->takeAt(0)) != 0)
		{
			QWidget *widget = item->widget();
			if (widget)
			{
				widget->hide();
				delete widget;
			}
		}
		delete layout;

		search_line_list.removeLast();
		tier_spinbox_list.removeLast();
        tier_name_list.removeLast();
        tier_relation_list.removeLast();
	}

	if (search_line_list.size() == 1)
	{
		rmLine_btn->setEnabled(false);
		displayTier_spinbox->hide();
		displayTier_label->hide();
	}

    for (int i = 1; i <= tier_relation_list.size(); i++)
    {
        bool value = (i != tier_relation_list.size());
        tier_relation_list.at(i-1)->setEnabled(value);
    }

	search_line_list.at(search_line_list.count()-1)->setFocus(Qt::OtherFocusReason);
}

void SearchWindow::setAnnotationsSelected()
{
    toggleVisibleSearchWidgets(true);
    setDisplayedFiles();
}

void SearchWindow::setDocumentsSelected()
{
    // Get rid of cross-tier search fields
    while (rmLine_btn->isEnabled())
        removeSearchLine();

    toggleVisibleSearchWidgets(false);
    setDisplayedFiles();
}

void SearchWindow::toggleVisibleSearchWidgets(bool visible)
{
    addLine_btn->setVisible(visible);
    rmLine_btn->setVisible(visible);
    tier_spinbox_list.at(0)->setVisible(visible);

    foreach (DMLineEdit *edt, tier_name_list)
        edt->setVisible(visible);
}

/*
 *
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////::
			QString::fromUtf8("SEARCH @tier[2] "
					"FROM /home/julien/tmp/dp1/11adp1gg.TextGrid, /home/julien/tmp/dp1/11adp1lg.TextGrid WHERE (@item.text ~ \"1412\") && "
					"() "
					"RETURN @item.match;");
SEARCH @tier[2] FROM %annotation WHERE (@item.text ~ "1412") &&
(((@tag.category = "task") AND (@tag.label = "libre")) OR ((@tag.category = "task") AND (@tag.label = "guidé"))) RETURN @item.match;

 \\d\\d\\d\\d\
"SEARCH @tier[1] "
					"FROM %annotation WHERE (@item.text ~ \"parlez\") && "
					"(NOT (@tag.category = \"task\") AND (@tag.label = \"liste__de__mots\")) "
					//"(((@tag.category = \"task\") AND (NOT @tag.label = \"liste__de__mots\")) AND ((@tag.category = \"task\") AND (NOT @tag.label = \"libre\"))) "
					"RETURN @item.match;"
*/
