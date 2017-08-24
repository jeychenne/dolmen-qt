#include "AnnotationCreator.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>

AnnotationCreator::AnnotationCreator(const QString &suggested_name, const QString &sound_ref, QWidget *parent) :
    QDialog(parent)
{
    this->sound_ref = sound_ref;
    this->setupUi(suggested_name);
}

void AnnotationCreator::setupUi(const QString &suggested_name)
{
    setWindowTitle(tr("Create annotation..."));
    if (objectName().isEmpty())
        setObjectName(QString::fromUtf8("PropertyEditor"));

    resize(400, 420);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setContentsMargins(10, 10, 10, 10);

    // layouts for file name and tier name and type
    QGridLayout *gl = new QGridLayout;

    line_file_name = new QLineEdit;
    line_tier_name = new QLineEdit;
    box_tier_type  = new QComboBox;

    box_tier_type->insertItem(0, tr("Spans"));
    box_tier_type->insertItem(1, tr("Points"));
    box_tier_type->setCurrentIndex(0);

    line_file_name->setText(suggested_name);
    line_file_name->selectAll();

    gl->addWidget(new QLabel(tr("File name:")), 0, 0);
    gl->addWidget(line_file_name, 0, 1);
    gl->addWidget(new QLabel(""));
    gl->addWidget(new QLabel(tr("Tier name:")), 2, 0);
    gl->addWidget(line_tier_name, 2, 1);
    gl->addWidget(new QLabel(tr("Tier items:")), 3, 0);
    gl->addWidget(box_tier_type, 3, 1);

    // add/remove tiers
    QHBoxLayout *tier_btn_layout = new QHBoxLayout;
    QPushButton *btn_add_tier = new QPushButton(tr("Add tier"));
    QPushButton *btn_rm_tier = new QPushButton(tr("Remove tier"));
    tier_btn_layout->addWidget(btn_add_tier);
    tier_btn_layout->addWidget(btn_rm_tier);
    tier_btn_layout->addStretch();

    table_tiers = new QTableWidget(0, 2);
    table_tiers->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_tiers->verticalHeader()->hide();
    table_tiers->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Name")));
    table_tiers->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Items")));
    table_tiers->horizontalHeader()->setStretchLastSection(true);
    table_tiers->setColumnWidth(0, 250);


    vlayout->addLayout(gl);
    vlayout->addWidget(table_tiers);
    vlayout->addLayout(tier_btn_layout);

    QHBoxLayout *hl = new QHBoxLayout;
    hl->addStretch();
    hl->addWidget(buttonBox);
    vlayout->addStretch();
    vlayout->addLayout(hl);
    this->setLayout(vlayout);

    connect(btn_add_tier, SIGNAL(clicked()), this, SLOT(addTier()));
    connect(btn_rm_tier, SIGNAL(clicked()), this, SLOT(removeTier()));
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}


void AnnotationCreator::addTier()
{
    int row = table_tiers->rowCount();

    table_tiers->insertRow(row);
    table_tiers->setItem(row, 0, new QTableWidgetItem(line_tier_name->text()));
    table_tiers->setItem(row, 1, new QTableWidgetItem(box_tier_type->currentText()));

    line_tier_name->setText("");
}

void AnnotationCreator::removeTier()
{
    int row = table_tiers->currentIndex().row();
    table_tiers->removeRow(row);
}


void AnnotationCreator::accept()
{
    QList<tier_type_t> types;
    QStringList tier_names;

    if (table_tiers->rowCount() == 0)
        return;

    for (int i = 0; i < table_tiers->rowCount(); ++i)
    {
        if (table_tiers->item(i, 1)->text() == tr("Points"))
        {
            types << DM_TIER_TYPE_POINT;
        }
        else
        {
            types << DM_TIER_TYPE_SPAN;
        }

        tier_names << table_tiers->item(i, 0)->text();
    }

    emit createAnnotation(line_file_name->text(), sound_ref, tier_names, types);

    QDialog::accept();
}
