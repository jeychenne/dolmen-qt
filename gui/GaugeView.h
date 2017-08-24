/*
 * GaugeView.h
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
 * Created: 01/04/2011
 */

/*
The gauge is a tool that enables one to compare subjects' productions on a standardized task.
It assumes that there is either a "Subject" or a "Speaker" category and a "Task" category.
Furthermore, if either "Group" or "Survey" exists, they will be displayed in the gauge to allow
for quicker speaker selection. "Subject" and "Group" have precedence over "Speaker" and "Survey"
respectively, in case both exist in either case.
The user selects the task they want to compare subjects on (word lists only for PFC and PAC).
The gauge assumes uses the first tier of every task annotation; it assumes that all tiers contain
the same number of non-empty items (in the same order). It then displays the items in a checklist
and users can select which items they want to compare for which speakers.
*/

#ifndef GAUGEVIEW_H
#define GAUGEVIEW_H

#include <QWidget>
#include <QComboBox>
#include <QHash>
#include <QStringList>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QMessageBox>
#include <QPair>
#include <QQueue>

#include "lib/Project.h"
#include "gui/View.h"
#include "gui/CheckList.h"
#include "utils/Global.h"
#include "lib/Sound.h"
#include "lib/metadata.h"
using namespace metadata;

typedef QPair<double, double> Interval;

class GaugeView : public View
{
	Q_OBJECT

public:
	GaugeView(QTabWidget *parent, Project *p, const QString &task, int tier_no);
	void leave();
	QString id() const;

signals:
	void error(const QString &path);

private slots:
	void showError(const QString &s);
	void play();
	void interrupt();
	void playQueue();

public slots:
	void onGroupChanged(int index, int state);

private:
	void setupUi();
	CheckListBox *speaker_box, *group_box, *task_box;
    QComboBox    *task_list;
    QToolButton  *play_btn, *stop_btn;
    Project      *project;
    bool          has_groups;
    QString       task_cat, group_cat, speaker_cat;
    QStringList   gauge;
    int           tier_no;
    Sound        *current_sound;
    QHash<QString,QStringList*>  groups; // ex: "11a" -> ["11aal1", "11agm2", ...]
    QHash<QString,Annotation*>   taskFiles;
    QQueue< QPair<Annotation*,Interval> > queue; // store intervals to play

	void setSpeakers();
	bool setGauge();
	void setButtonLayout();
	int countNonEmptyLabels(Tier *tier);
	QStringList getSpeakers(QStringList groupList = QStringList()) const;
	void addSpeakers(QString group);
	void removeSpeakers(QString group);
	Interval getInterval(QString speaker, QString taskItem);

};

#endif // GAUGEVIEW_H
