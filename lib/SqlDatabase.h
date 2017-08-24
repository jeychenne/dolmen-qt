#ifndef SQLDATABASE_H
#define SQLDATABASE_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QtPlugin>
#include "lib/DFile.h"
#include "lib/Annotation.h"


#ifdef Q_OS_MAC
    //Q_IMPORT_PLUGIN(qsqlite)
#endif

class SqlDatabase : public QObject
{
    Q_OBJECT
public:
    SqlDatabase(QObject *parent, QString path);
    void close();
    bool status() const;
    QString error() const;
    
signals:
    
public slots:
    void commit();
    
protected:
    QSqlDatabase m_db;
    bool m_status;
};


class MainDatabase : public SqlDatabase
{
    Q_OBJECT
public:
    MainDatabase(QObject *parent, QString path);
    ~MainDatabase();
    void createMainTable();
    bool hasCategory(QString cat) const;
    bool hasFile(QString path) const;
    void addCategory(QString cat);

public slots:
    void addFile(DFile *file);
    void saveFileMetadata(DFile *file);
    void setFileMetadata(DFile *file);

private:
    QString getValue(QString path, QString cat) const;
};

#endif // SQLDATABASE_H
