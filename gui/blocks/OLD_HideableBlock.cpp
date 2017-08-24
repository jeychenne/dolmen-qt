#include "HideableBlock.h"


HideableBlock::HideableBlock() :
		QVBoxLayout::QVBoxLayout()
{

	header_label = new QLabel;
	showHide_arrow = new QToolButton;
	showHide_arrow->setArrowType(Qt::RightArrow);

	QHBoxLayout *header_layout = new QHBoxLayout;
	header_layout->addWidget(showHide_arrow);
	header_layout->addWidget(header_label);

	this->addLayout(header_layout);
	connect(showHide_arrow, SIGNAL(clicked()), this, SLOT(onArrowClicked()));
}


void HideableBlock::onArrowClicked()
{
	switch (showHide_arrow->arrowType())
	{
	case Qt::RightArrow:
		this->showWidgets();
		break;
	case Qt::DownArrow:
		this->hideWidgets();
		break;
	default:
		break;
	}
}
