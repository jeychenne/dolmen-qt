/*
 * PropertyEditor.h
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
 * Created: 10/10/2010
 */

#ifndef TAGEDITOR_H
#define TAGEDITOR_H

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
#include <QWidget>

#include <QList>

#include "lib/metadata.h"
#include "lib/DFile.h"
#include "utils/routines.h"

using namespace metadata;


/**************************************************************************************************
 ***                            CLASS ADD_TAG_EDITOR                                           ****
 **************************************************************************************************/

class AddPropertyEditor : public QDialog
{
    Q_OBJECT

public:
	AddPropertyEditor(QWidget *parent, DFile *df = NULL);

	void setupUi();

public slots:
	void accept();

private slots:
	void resetValues();
	void changeCurrentCategory(int);
	void changeCurrentValue(int);

signals:
	void menu_newProperty(Property *);
    void tagAdded();

private:
	QDialogButtonBox	*buttonBox;
	QVBoxLayout			*vlayout;
    QLabel				*label_category, *label_new_cat, *label_value, *label_new_value;
    QComboBox			*box_cat, *box_value;
	QSpacerItem			*hspacer;
    QLineEdit			*edit_new_value, *edit_new_cat;

    QList<Property*>     m_properties;
	DFile				*m_file;

	void setupCategories();
	void setupValues();

};




/**************************************************************************************************
 ***                            CLASS DEL_TAG_EDITOR                                           ****
 **************************************************************************************************/

class DelPropertyEditor : public QDialog
{
	Q_OBJECT

public:
	DelPropertyEditor(QWidget *parent, DFile *df = NULL);

	void setupUi();

public slots:
	void accept();

private slots:
	void resetValues();

signals:
	void menu_rmProperty(Property *);
    void tagRemoved();

private:
	QDialogButtonBox	*buttonBox;
	QWidget				*verticalLayoutWidget;
	QVBoxLayout			*vlayout;
	QLabel				*cat_lbl, *value_lbl;
	QComboBox			*cat_box, *value_box;
	QSpacerItem			*hspacer;
    QList<Property*>	 m_properties;
	DFile				*m_file;

	void setupBoxes();
};



#endif // TAGEDITOR_H
