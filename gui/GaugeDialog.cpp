#include "GaugeDialog.h"

GaugeDialog::GaugeDialog(QWidget *parent) :
	QDialog(parent)
{
	setWindowTitle(tr("Compare speakers..."));
	if (objectName().isEmpty())
		setObjectName(QString::fromUtf8("GaugeDialog"));
	resize(250, 200);

	verticalLayoutWidget = new QWidget(this);
	vlayout = new QVBoxLayout(verticalLayoutWidget);
	vlayout->addWidget(new QLabel(tr("Use the following task as a gauge:")));
	vlayout->addSpacing(20);
	task_box = new QComboBox;
	vlayout->addWidget(task_box);
	vlayout->addSpacing(20);
	QHBoxLayout *hl = new QHBoxLayout;

	tier_spin = new QSpinBox;
	tier_spin->setRange(1, 100);
	tier_spin->setSingleStep(1);
	tier_spin->setPrefix(tr("In span tier "));
	tier_spin->setValue(1);
	hl->addWidget(tier_spin);
	hl->addStretch();
	vlayout->addLayout(hl);
	vlayout->addStretch();

	buttonBox = new QDialogButtonBox(this);
	buttonBox->setOrientation(Qt::Horizontal);
	buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
	QHBoxLayout *btn_layout = new QHBoxLayout;
	btn_layout->addStretch();
	btn_layout->addWidget(buttonBox);
	vlayout->addLayout(btn_layout);

	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	// setup task
	QSet<QString> taskSet;

	foreach (Property *tag, Property::properties.values())
	{
		categories.insert(tag->category());
		if (tag->category() == tr("Task"))
			taskSet.insert(tag->value());
	}
	categories.remove("");
	QDialog::show();
	this->checkProperties();

	QStringList tasks = QStringList::fromSet(taskSet);
	tasks.sort();

	foreach (QString val, tasks)
	{
		task_box->addItem(val);
		// Built-in biais
		if (val == tr("Word list"))
			task_box->setCurrentIndex(task_box->count() - 1);
	}
	task_box->setFocus(Qt::OtherFocusReason);
}


// for the gauge to be usable, the project must include the following:
// - a Speaker or Subject category
// - a Task category
// - a Survey or Group category (optional)
void GaugeDialog::checkProperties()
{
	QString title = tr("Missing category");

	if (! (categories.contains(tr("Speaker")) || categories.contains(tr("Subject"))))
	{
		QMessageBox::critical(this, title,
							  tr("Your project must have a \"Speaker\" or \"Subject\" category"),
							  QMessageBox::Ok, QMessageBox::NoButton);
		this->close();//->reject();
	}

	else if (!categories.contains(tr("Task")))
	{
		QMessageBox::critical(this, title,
							  tr("Your project must have a \"Task\" category"),
							  QMessageBox::Ok, QMessageBox::NoButton);
		this->reject();
	}
}

void GaugeDialog::accept()
{
	emit compareSpeakers(task_box->currentText(), tier_spin->value() - 1);
	QDialog::accept();
}
