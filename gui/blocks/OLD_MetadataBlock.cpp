#include "MetadataBlock.h"

MetadataBlock::MetadataBlock(DFile *file):
		HideableBlock::HideableBlock()
{
	m_file = file;
	setupUI();
}

void MetadataBlock::setupUI()
{
	desc_label = new QLabel(tr("<b>Description:</b>"));
	desc_edit = new QTextEdit(m_file->description());
	desc_html = new QLabel(m_file->metadataAsHtml(false));

	this->addWidget(desc_html);
	this->addWidget(desc_label);
	this->addWidget(desc_edit);


	// metadata is hidden by default when a new view is opened
	this->hideWidgets();

	connect(desc_edit, SIGNAL(textChanged()), this, SLOT(updateDescription()));
}

void MetadataBlock::hideWidgets()
{
	showHide_arrow->setArrowType(Qt::RightArrow);
	desc_edit->hide();
	desc_label->hide();
	desc_html->hide();

	header_label->setText(tr("Show metadata"));
}

void MetadataBlock::showWidgets()
{
	showHide_arrow->setArrowType(Qt::DownArrow);
	desc_edit->show();
	desc_label->show();
	desc_html->show();

	header_label->setText(tr("Metadata"));
}

void MetadataBlock::updateDescription()
{
	m_file->setDescription(desc_edit->toPlainText());
}
