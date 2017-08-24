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

#include "PropertyEditor.h"


/**************************************************************************************************
 ***                            CLASS ADD_PROPERTY_EDITOR                                      ****
 **************************************************************************************************/

AddPropertyEditor::AddPropertyEditor(QWidget *parent, DFile *df) :
	QDialog(parent)
{
	m_file = df;

	/* Only display properties that the file doesn't already have */
	if (df)
	{
		foreach(Property *tag, Property::properties.values())
		{
			if (! df->hasProperty(tag))
				m_properties.append(tag);
		}
	}
	/* Display global properties */
	else
		m_properties = Property::properties.values();

	setupUi();
}

void AddPropertyEditor::setupUi()
{
	setWindowTitle(tr("Edit tag"));
	if (objectName().isEmpty())
		setObjectName(QString::fromUtf8("PropertyEditor"));

	resize(400, 280);

	buttonBox = new QDialogButtonBox;
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
	vlayout = new QVBoxLayout;
	vlayout->setContentsMargins(10, 10, 10, 10);
    label_category = new QLabel(tr("Category:"));

    vlayout->addWidget(label_category);

	setupCategories();
	setupValues();

	QHBoxLayout *hl = new QHBoxLayout;
	hl->addStretch();
	hl->addWidget(buttonBox);
	vlayout->addStretch();
	vlayout->addLayout(hl);
	this->setLayout(vlayout);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(box_cat, SIGNAL(currentIndexChanged(int)), this, SLOT(changeCurrentCategory(int)));
	if (m_file)
	{
        connect(box_cat, SIGNAL(currentIndexChanged(int)), this, SLOT(resetValues()));
        connect(box_value, SIGNAL(currentIndexChanged(int)), this, SLOT(changeCurrentValue(int)));
	}
}

void AddPropertyEditor::setupCategories()
{
	QSet<QString> categories = QSet<QString>();

    box_cat = new QComboBox;
    box_cat->addItem(DM_DEFAULT_CAT);

	foreach (Property *tag, m_properties)
		categories.insert(tag->category());
	categories.remove("");

	foreach (QString cat, categories)
        box_cat->addItem(cat);
    box_cat->addItem(tr("New category"));

    vlayout->addWidget(box_cat);

	// new category
    label_new_cat = new QLabel;
    vlayout->addWidget(label_new_cat);

    edit_new_cat = new QLineEdit;
    edit_new_cat->setReadOnly(true);
    edit_new_cat->hide();
    vlayout->addWidget(edit_new_cat);
}

void AddPropertyEditor::setupValues()
{
	if (m_file)
	{
        label_value = new QLabel(tr("Value:"));
        box_value = new QComboBox;
        vlayout->addWidget(label_value);
		this->resetValues();
        vlayout->addWidget(box_value);
	}

    label_new_value = new QLabel(tr("New value:"));
    vlayout->addWidget(label_new_value);

    edit_new_value = new QLineEdit;
    vlayout->addWidget(edit_new_value);

	if (m_file)
        box_value->setFocus(Qt::OtherFocusReason);
	else
        edit_new_value->setFocus(Qt::OtherFocusReason);
}

void AddPropertyEditor::resetValues()
{
    box_value->clear();

    QString currentCategory = box_cat->currentText();
	if (currentCategory == DM_DEFAULT_CAT)
		currentCategory = "";

	QStringList labels = QStringList();

	foreach (Property *tag, m_properties)
	{
		if (tag->category() == currentCategory)
		{
			if (! labels.contains(tag->value()))
				labels.append(tag->value());
		}
	}
	labels.sort();
	labels.append(tr("New value"));

    box_value->addItems(labels);
    box_value->setCurrentIndex(0);
    box_value->setEnabled(!labels.isEmpty());
}

void AddPropertyEditor::changeCurrentValue(int index)
{
    bool newValueVisible = index == box_value->count() - 1;

	if (newValueVisible)
	{
        label_new_value->setText(tr("Enter new value:"));
        edit_new_value->setFocus(Qt::OtherFocusReason);
	}
	else
	{
        edit_new_value->setText("");
        box_value->setFocus(Qt::OtherFocusReason);
	}

    label_new_value->setVisible(newValueVisible);
    edit_new_value->setVisible(newValueVisible);
}

void AddPropertyEditor::changeCurrentCategory(int index)
{
    bool newCatVisible = index == box_cat->count() - 1;

	if (newCatVisible)
	{
        label_new_cat->setText(tr("Enter new category:"));
        edit_new_cat->setFocus(Qt::OtherFocusReason);
	}
	else
	{
		//newCat_lbl->setText("");
        edit_new_cat->setText("");
        edit_new_value->setFocus(Qt::OtherFocusReason);
	}

    edit_new_cat->setReadOnly(!newCatVisible);
    edit_new_cat->setVisible(newCatVisible);
}

void AddPropertyEditor::accept()
{
	QString value, category;
    const int index = box_cat->currentIndex();

	/* Value */
	if (m_file)
	{
        if (box_value->count() == 1 || box_value->currentIndex() == box_value->count()-1)
            value = edit_new_value->text().trimmed();
		else
            value = box_value->currentText();
	}
	else
        value = edit_new_value->text().trimmed();

	if (value.isEmpty())
	{
		QDialog::accept();
		return;
	}

	/* Category */
	if (index <= 0)
		category = "";
    else if (index == box_cat->count() - 1)
        category = edit_new_cat->text().trimmed();
	else
	{
        category = box_cat->currentText();
		value = value.trimmed();
	}

	Property *tag = new Property(value, category);

	if (m_file)
		m_file->addProperty(tag);
	else
		emit this->menu_newProperty(tag);

	QDialog::accept();
    emit tagAdded();
}


/**************************************************************************************************
 ***                            CLASS DEL_TAG_EDITOR                                           ****
 **************************************************************************************************/

DelPropertyEditor::DelPropertyEditor(QWidget *parent, DFile *df) :
	QDialog(parent)
{
	m_file = df;

	if (df)
		m_properties = QList<Property*>::fromSet(df->properties());
	else
		m_properties = Property::properties.values();
	setupUi();
	resetValues();
}

void DelPropertyEditor::setupUi()
{
	setWindowTitle(tr("Remove tag"));
	if (objectName().isEmpty())
		setObjectName(QString::fromUtf8("PropertyEditor"));
	resize(400, 250);
	buttonBox = new QDialogButtonBox(this);
	buttonBox->setGeometry(QRect(30, 200, 341, 32));
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
	verticalLayoutWidget = new QWidget(this);
	verticalLayoutWidget->setGeometry(QRect(10, 20, 381, 161));
	vlayout = new QVBoxLayout(verticalLayoutWidget);
	vlayout->setContentsMargins(0, 0, 0, 0);

	setupBoxes();


	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(cat_box, SIGNAL(currentIndexChanged(int)), this, SLOT(resetValues()));
} // setupUi

void DelPropertyEditor::setupBoxes()
{
	// CATEGORY

	cat_lbl = new QLabel(tr("Category:"), verticalLayoutWidget);
	vlayout->addWidget(cat_lbl);

	cat_box = new QComboBox(verticalLayoutWidget);
	cat_box->addItem(DM_DEFAULT_CAT);

	QSet<QString> categories = QSet<QString>();

	foreach (Property *tag, m_properties)
		categories.insert(tag->category());
	categories.remove("");

	foreach (QString cat, categories)
		cat_box->addItem(cat);


	vlayout->addWidget(cat_box);

	// VALUE

	value_lbl = new QLabel(tr("Value:"), verticalLayoutWidget);
	vlayout->addWidget(value_lbl);

	value_box = new QComboBox(verticalLayoutWidget);
	vlayout->addWidget(value_box);
}

void DelPropertyEditor::resetValues()
{
	value_box->clear();

	QString currentCategory = cat_box->currentText();
	if (currentCategory == DM_DEFAULT_CAT)
		currentCategory = "";

	QStringList labels = QStringList();

	foreach (Property *tag, m_properties)
	{
		if (tag->category() == currentCategory)
		{
			if (! labels.contains(tag->value()))
				labels.append(tag->value());
		}
	}

	value_box->addItems(labels);
	value_box->setEnabled(!labels.isEmpty());
}

void DelPropertyEditor::accept()
{
	QString category = cat_box->currentText();
	QString label = value_box->currentText();


	if (category == DM_DEFAULT_CAT)
		category = "";

	Property *tag = new Property(label, category);

	/* modify file */
	if (m_file)
		m_file->removeProperty(tag);
	/* remove global tag */
	else
		emit this->menu_rmProperty(tag);


	QDialog::accept();
    emit tagRemoved();
}
