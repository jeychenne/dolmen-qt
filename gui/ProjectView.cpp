#include "ProjectView.h"

ProjectView::ProjectView(QTabWidget *parent, Project *project) :
		View(parent, project->screenName())
{
	m_project = project;
	this->setupUi();
}

void ProjectView::setupUi()
{
	changelog_block = new ChangelogBlock(m_project->changelog());
	layout->addLayout(changelog_block);
	layout->addStretch(0);

	connect(changelog_block, SIGNAL(showVersion(QString)), m_project, SIGNAL(meta(QString)));
}

QString ProjectView::id() const
{
	return m_project->screenName();
}



void ProjectView::leave()
{

}
