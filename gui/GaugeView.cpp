#include "GaugeView.h"


GaugeView::GaugeView(QTabWidget *parent, Project *p, const QString &task, int tier_no) :
	View(parent, tr("Compare speakers"))
{
	connect(this, SIGNAL(error(QString)), this, SLOT(showError(QString)));
	this->setSpeakers();
	project = p;
	task_cat = task;
	current_sound = NULL;
	this->tier_no = tier_no;

	this->setSpeakers();
	if (this->setGauge())
		this->setupUi();
	else
		this->close();
}


void GaugeView::setupUi()
{
/*
------------------------------------------
|             |          |               |
| task        | groups   |               |
|             |          |               |
|             |----------|               |
|             |          |               |
|             | speakers |               |
|             |          |               |
------------------------------------------
*/
	QHBoxLayout *hlayout = new QHBoxLayout;
	QVBoxLayout *speaker_layout = new QVBoxLayout;

    task_box = new CheckListBox(task_cat, gauge);
	hlayout->addSpacing(10);
	hlayout->addWidget(task_box);
	hlayout->addSpacing(10);

	if (has_groups)
	{
		QStringList keys = groups.keys();
		keys.sort();
        group_box = new CheckListBox(group_cat, keys);
		speaker_layout->addWidget(group_box);

		QStringList lst;
        speaker_box = new CheckListBox(speaker_cat, lst);
	}
	else
	{
		group_box = NULL;
		QStringList lst = taskFiles.keys();
		lst.sort();
        speaker_box = new CheckListBox(speaker_cat, lst);
	}
	speaker_layout->addWidget(speaker_box);
	hlayout->addLayout(speaker_layout);
	hlayout->addStretch();
	layout->addSpacing(5);
	this->setButtonLayout();
	layout->addLayout(hlayout);

	connect(group_box, SIGNAL(stateChanged(int,int)), this, SLOT(onGroupChanged(int,int)));
}

void GaugeView::setButtonLayout()
{
	play_btn = new QToolButton;
	play_btn->setIcon(QIcon(":/icons/play.png"));
	play_btn->setToolTip(tr("Play"));

	stop_btn = new QToolButton;
	stop_btn->setIcon(QIcon(":/icons/stop.png"));
	stop_btn->setToolTip(tr("Stop"));

	QHBoxLayout *btn_layout = new QHBoxLayout;
	btn_layout->addSpacing(10);
	btn_layout->addWidget(play_btn);
	btn_layout->addWidget(stop_btn);
	btn_layout->addStretch();
	layout->addLayout(btn_layout);

	connect(play_btn, SIGNAL(clicked()), this, SLOT(play()));
	connect(stop_btn, SIGNAL(clicked()), this, SLOT(interrupt()));
}

void GaugeView::setSpeakers()
{
	QString gp_str = tr("Group");
	QString sv_str = tr("Survey");
	QString sp_str = tr("Speaker");
	QString su_str = tr("Subject");
	QString tk_str = tr("Task");
	QSet<QString> categories;

	/* find labels to use */
	foreach (Property *tag, Property::properties.values())
	{
		categories.insert(tag->category());
	}

	if (categories.contains(gp_str))
	{
		group_cat = gp_str;
		has_groups = true;
	}
	else if (categories.contains(sv_str))
	{
		group_cat = sv_str;
		has_groups = true;
	}
	else
		has_groups = false;

	// the gauge dialog tested that at least one of these is valid
	if (categories.contains(sp_str))
		speaker_cat = sp_str;
	else
		speaker_cat = su_str;

	/* Find all speakers. Assumes that each annotation is tagged
	  with a tag, a speaker and (optionnally) a group, and that
	  each speaker is unique */
	foreach (DFile *f, project->files())
	{
		if (isInstance(f, Annotation))
		{
			Annotation *annot = qobject_cast<Annotation*>(f);
			QString group, speaker, c, v;
			bool isTask = false;

			foreach (Property *tag, annot->properties())
			{
				c = tag->category();
				v = tag->value();

				if (c == tk_str && v == task_cat)
					isTask = true;
				else if (c == group_cat)
					group = v;
				else if (c == speaker_cat)
					speaker = v;
			}

			if (isTask)
			{
				if (!annot->soundFile())
					emit error(tr("Unbound annotation ignored: %1").arg(annot->path()));
				else if (speaker.isEmpty())
					emit error(tr("File ignored: %1").arg((annot->path())));
				else
				{
					taskFiles.insert(speaker, annot);
					if (! group.isEmpty())
					{
						if (groups.contains(group))
							groups.value(group)->append(speaker);
						else
							groups.insert(group, new QStringList);
					}
				}
			}
		}
	}

	foreach (QString g, groups.keys())
		groups.value(g)->sort();
}

QStringList GaugeView::getSpeakers(QStringList groupList) const
{
	QStringList speakerList;
	QString g, s;

	if (groupList.isEmpty())
		groupList = groups.keys();
	groupList.sort();

	foreach (g, groupList)
	{
		foreach (s, *(groups.value(g)))
			speakerList << s;
	}

	return speakerList;
}

bool GaugeView::setGauge()
{
	if (taskFiles.isEmpty())
	{
		emit error(tr("No file for the given task"));
		return false;
	}

	QList<Annotation*> files = taskFiles.values();

	// Randomly pick a file to get the gauge tier
	Annotation *annot = files.at(0);
	if (annot->countTiers() < tier_no + 1)
	{
		emit error(tr("Could not create gauge: not enough tiers in %1").arg(annot->path()));
		return false;
	}

	QString text;
	foreach (Item *item, annot->tier(tier_no)->items())
	{
		text = item->text();
		if (! text.isEmpty() && isInstance(item, DSpan))
			gauge << text;
	}

	int count = gauge.count();
	foreach (Annotation *a, files)
		if (this->countNonEmptyLabels(a->tier(tier_no)) != count)
		{
			emit error(tr("%1 should have %2 non-empty labels. %3 found.")
					   .arg(a->path()).arg(count).arg(countNonEmptyLabels(a->tier(tier_no))));
			return false;
		}

	return true;
}

int GaugeView::countNonEmptyLabels(Tier *tier)
{
	int count = 0;
	Item *item = NULL;
	QString text;

	foreach (item, tier->items())
	{
		text = item->text();

		if (! text.isEmpty() && isInstance(item, DSpan))
			++count;
	}
	return count;
}

void GaugeView::onGroupChanged(int index, int state)
{
	QString group = group_box->text(index);

	if (state)
		this->addSpeakers(group);
	else
		this->removeSpeakers(group);
}

void GaugeView::addSpeakers(QString group)
{
	QString speaker;
	QStringList *lst = groups[group];

	if (!lst)
	{
		emit error(tr("DEBUG: empty pointer in GaugeView::addSpeakers()"));
		return;
	}

	foreach (speaker, *lst)
		speaker_box->addItem(speaker);
}

void GaugeView::removeSpeakers(QString group)
{
	QString speaker;
	QStringList *lst = groups[group];

	foreach (speaker, *lst)
		speaker_box->removeItem(speaker);
}

void GaugeView::showError(const QString &s)
{
	QMessageBox::critical(this, tr("Problem setting the gauge"), s);
}


void GaugeView::play()
{
	// if several items are checked, we want to compare speaker on each item
	// rather than listen to all items for each speaker.
	if (current_sound && current_sound->isPlaying()) return;

	foreach (QString label, task_box->checkedLabels())
	{
		foreach (QString speaker, speaker_box->checkedLabels())
		{
			Interval time = this->getInterval(speaker, label);

			if (time != Interval())
				queue.enqueue(QPair<Annotation*, Interval>(taskFiles[speaker], time));
		}
	}

	this->playQueue();
}

void GaugeView::playQueue()
{
	if (current_sound)
	{
		if (current_sound->isPlaying())
			current_sound->stop();
		disconnect(current_sound, SIGNAL(finished()), this, SLOT(playQueue()));
	}

	if (queue.isEmpty())
		return;

	// Get next sound file
	QPair<Annotation*, Interval> pair = queue.dequeue();
	current_sound = pair.first->soundFile();
	connect(current_sound, SIGNAL(finished()), this, SLOT(playQueue()));
	current_sound->play(pair.second.first, pair.second.second);
}

void GaugeView::interrupt()
{
	if (current_sound && current_sound->isPlaying())
		current_sound->stop();

	current_sound = NULL;
	queue.clear();
}


Interval GaugeView::getInterval(QString speaker, QString taskItem)
{
	Interval result = Interval();
	Annotation *a = taskFiles.value(speaker);
	int taskIndex = task_box->index(taskItem);

	if (taskIndex != -1)
	{
		Tier *tier = a->tier(tier_no);
		int i, index = -1;
		for (i = 0; i < tier->countItems(); ++i)
		{
			Item *item = tier->item(i);
			if (! item->text().isEmpty())
			{
				index++;
				if (index == taskIndex)
				{
					result.first = item->left();
					result.second = item->right();
				}
			}
		}
	}

	return result;
}


QString GaugeView::id() const
{
	return QString("Compare speakers");
}

void GaugeView::leave()
{

}


