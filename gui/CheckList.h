/*
 * CheckList.h
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
 * Purpose: implement a custom list of checkable items. The list is presented in a
 * group box, with an additional button to check all the items on or off.
 * Created: 25/09/2010
 */

#ifndef CHECKLIST_H
#define CHECKLIST_H

#include <QLabel>
#include <QListWidget>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QStringList>
#include <QDebug>


// Helper class for CheckListBox
class CheckList : public QListWidget
{
	Q_OBJECT
public:
	CheckList(const QStringList &labels, QWidget *parent = 0, QStringList tooltips = QStringList());
	QList<QCheckBox*> buttons();
	QList<QCheckBox*> checkedItems();
	void appendItem(QString label, QString tooltip = QString());
	void removeItem(QString text);
    void resetLabels(QStringList labels, QStringList toolTips = QStringList());

signals:
	void stateChanged(int index, int state);

private slots:
    void forwardState(int);

private:
	QButtonGroup *group;
    QStringList   m_labels, m_tooltips;
    bool          hasTips;

    int indexFromCheckbox(QCheckBox *box);
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

class CheckListBox : public QWidget
{
	Q_OBJECT

public:
    CheckListBox(const QString &title, const QStringList &labels, QWidget *parent = NULL);

	QString text(int index) const;
	int index(QString text) const;
	void addItem(QString item);
	void removeItem(QString item);
    QString title() const;

signals:
	void stateChanged(int index, int state);

public slots:
	void checkAll(int);
	QStringList checkedLabels();

private:
	QVBoxLayout *layout;
    QCheckBox   *switch_button;
    CheckList   *checkList;
    QString      m_title;
};




#endif // CHECKLIST_H
