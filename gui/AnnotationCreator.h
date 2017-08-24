#ifndef ANNOTATIONCREATOR_H
#define ANNOTATIONCREATOR_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QList>
#include <QTableWidget>
#include <QString>

#include "lib/Tier.h"

class AnnotationCreator : public QDialog
{
    Q_OBJECT

public:
    explicit AnnotationCreator(const QString &suggested_name, const QString &sound_ref, QWidget *parent = 0);
    
signals:
    void createAnnotation(const QString &filename, const QString &sound_ref, QStringList tier_names, QList<tier_type_t> tier_types);

public slots:
    void accept();


private slots:
    void addTier();
    void removeTier();

private:
    QLineEdit    *line_file_name, *line_tier_name;
    QComboBox    *box_tier_type;
    QTableWidget *table_tiers;
    QString       sound_ref;

    void setupUi(const QString &suggested_name);
};

#endif // ANNOTATIONCREATOR_H
