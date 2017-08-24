#include "ChangelogBlock.h"

ChangelogBlock::ChangelogBlock(Changelog *changelog, QWidget *parent) :
        QVBoxLayout(parent)
{
	m_changelog = changelog;
	this->setupUi();
}

void ChangelogBlock::setupUi()
{
	this->setupEditVersions();
}


void ChangelogBlock::setupEditVersions()
{
	const int SPACE = 10;

	this->addSpacing(SPACE);

	// Label
	QLabel *version_label = new QLabel(tr("Manage versions:"));
	QHBoxLayout *versionLabel_layout = new QHBoxLayout;
	versionLabel_layout->addSpacing(SPACE);
	versionLabel_layout->addWidget(version_label);

	this->addLayout(versionLabel_layout);
	// Pack version list
	version_list = new QListWidget;
	this->displayVersions();

	QHBoxLayout *dummyVersion_layout = new QHBoxLayout;
	dummyVersion_layout->addSpacing(SPACE);
	dummyVersion_layout->addWidget(version_list);
	dummyVersion_layout->addStretch(0);

	this->addLayout(dummyVersion_layout);

	// Pack version buttons
	QPushButton *addVersion_btn = new QPushButton(tr("Add"));
	editVersion_btn = new QPushButton(tr("Edit"));
	removeVersion_btn = new QPushButton(tr("Remove"));
	QHBoxLayout *addVersion_layout = new QHBoxLayout;
	addVersion_layout->addSpacing(SPACE);
	addVersion_layout->addWidget(addVersion_btn);
	addVersion_layout->addWidget(editVersion_btn);
	addVersion_layout->addWidget(removeVersion_btn);
	addVersion_layout->addStretch(0);
	this->addLayout(addVersion_layout);

	this->toggleVersionButtons(false);

	connect(addVersion_btn, SIGNAL(clicked()), this, SLOT(onAddVersionClicked()));
	connect(editVersion_btn, SIGNAL(clicked()), this, SLOT(onEditVersionClicked()));
	connect(removeVersion_btn, SIGNAL(clicked()), this, SLOT(removeVersion()));
	connect(version_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
				this, SLOT(getVersion()));
	connect(version_list, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
			this, SLOT(toggleVersionButtons()));

}

void ChangelogBlock::getVersion()
{
	int index = version_list->currentIndex().row();

	if (index > -1)
	{
		Version *v = m_changelog->version(index);
		emit showVersion(v->toHtml());
	}
}

void ChangelogBlock::displayVersions()
{
	version_list->clear();

	foreach(Version *version, m_changelog->versions())
	{
		QListWidgetItem *item = new QListWidgetItem(version->toString(), version_list);
		Q_UNUSED(item);
	}
}

void ChangelogBlock::onAddVersionClicked()
{
	VersionEditor *editor = new VersionEditor(this->parentWidget());
	connect(editor, SIGNAL(versionAvailable(Version*)), this, SLOT(addVersion(Version*)));
	editor->show();
}

void ChangelogBlock::addVersion(Version *version)
{
	m_changelog->addVersion(version);
	this->displayVersions();
	this->toggleVersionButtons(false);
}

void ChangelogBlock::onEditVersionClicked()
{
	Version *v = m_changelog->version(version_list->currentIndex().row());
	VersionEditor *editor = new VersionEditor(this->parentWidget(), v);
	connect(editor, SIGNAL(versionModified()), m_changelog, SIGNAL(changelog_changed()));

	editor->show();
}

void ChangelogBlock::removeVersion()
{
	m_changelog->removeVersion(version_list->currentIndex().row());
	this->displayVersions();
	this->toggleVersionButtons(false);
	// clear metadata
	emit showVersion("");
}

void ChangelogBlock::toggleVersionButtons(bool value)
{
	removeVersion_btn->setEnabled(value);
	editVersion_btn->setEnabled(value);
}


