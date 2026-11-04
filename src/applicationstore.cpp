#include "applicationstore.h"

#include "logic.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>
#include <QVariant>

namespace {

const QString kColumns = QStringLiteral(
    "id, title, company, url, due_date, applied_date, status, files, notes");

// A date with no value is saved as NULL, otherwise as text like "2026-11-10".
QVariant dateToDb(const QDate& date)
{
    if (!date.isValid())
        return QVariant(QMetaType::fromType<QString>());
    return date.toString(Qt::ISODate);
}

QDate dateFromDb(const QVariant& value)
{
    if (value.isNull())
        return QDate();
    return QDate::fromString(value.toString(), Qt::ISODate);
}

/*
 * A QString that was never set is "null", which Qt saves as NULL. These text
 * columns can't be NULL, so make it an empty string instead.
 */
QString textToDb(const QString& text)
{
    return text.isNull() ? QStringLiteral("") : text;
}

// The files are saved in one text column, one path per line.
QString filesToDb(const QStringList& files)
{
    return textToDb(files.join('\n'));
}

QStringList filesFromDb(const QString& text)
{
    if (text.isEmpty())
        return {};
    return text.split('\n');
}

Application applicationFromRow(const QSqlQuery& query)
{
    Application app;
    app.id = query.value(0).toInt();
    app.title = query.value(1).toString();
    app.company = query.value(2).toString();
    app.url = query.value(3).toString();
    app.dueDate = dateFromDb(query.value(4));
    app.appliedDate = dateFromDb(query.value(5));
    app.status = statusFromString(query.value(6).toString()).value_or(Status::NotStarted);
    app.files = filesFromDb(query.value(7).toString());
    app.notes = query.value(8).toString();
    return app;
}

}  // namespace

ApplicationStore::ApplicationStore(const QString& path)
    : m_path(path)
    , m_connectionName(QUuid::createUuid().toString(QUuid::WithoutBraces))
{
}

ApplicationStore::~ApplicationStore()
{
    {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName, false);
        if (db.isOpen())
            db.close();
    }
    QSqlDatabase::removeDatabase(m_connectionName);
}

QSqlDatabase ApplicationStore::database() const
{
    return QSqlDatabase::database(m_connectionName, false);
}

QString ApplicationStore::lastError() const
{
    return m_lastError;
}

bool ApplicationStore::open()
{
    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), m_connectionName);
    db.setDatabaseName(m_path);
    if (!db.open()) {
        m_lastError = db.lastError().text();
        return false;
    }

    QSqlQuery query(db);
    const bool ok = query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS applications ("
        " id INTEGER PRIMARY KEY AUTOINCREMENT,"
        " title TEXT NOT NULL DEFAULT '',"
        " company TEXT NOT NULL DEFAULT '',"
        " url TEXT NOT NULL DEFAULT '',"
        " due_date TEXT,"
        " applied_date TEXT,"
        " status TEXT NOT NULL DEFAULT 'Not Started',"
        " files TEXT NOT NULL DEFAULT '',"
        " notes TEXT NOT NULL DEFAULT ''"
        ")"));
    if (!ok) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

std::optional<int> ApplicationStore::add(const Application& app)
{
    QSqlQuery query(database());
    /*
     * The ? marks are filled in by addBindValue, which keeps whatever the user
     * typed from being treated as SQL.
     */
    query.prepare(QStringLiteral(
        "INSERT INTO applications (title, company, url, due_date, applied_date, status, files, notes) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?)"));
    query.addBindValue(textToDb(app.title));
    query.addBindValue(textToDb(app.company));
    query.addBindValue(textToDb(app.url));
    query.addBindValue(dateToDb(app.dueDate));
    query.addBindValue(dateToDb(app.appliedDate));
    query.addBindValue(statusToString(app.status));
    query.addBindValue(filesToDb(app.files));
    query.addBindValue(textToDb(app.notes));

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return std::nullopt;
    }
    return query.lastInsertId().toInt();
}

std::optional<Application> ApplicationStore::get(int id) const
{
    QSqlQuery query(database());
    query.prepare(QStringLiteral("SELECT ") + kColumns + QStringLiteral(" FROM applications WHERE id = ?"));
    query.addBindValue(id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return std::nullopt;
    }
    if (!query.next()) {
        m_lastError = QStringLiteral("No application with id %1").arg(id);
        return std::nullopt;
    }
    return applicationFromRow(query);
}

QList<Application> ApplicationStore::all() const
{
    // "due_date IS NULL" is 1 for applications with no due date, so they sort last.
    QSqlQuery query(database());
    if (!query.exec(QStringLiteral("SELECT ") + kColumns +
                    QStringLiteral(" FROM applications ORDER BY due_date IS NULL, due_date, id"))) {
        m_lastError = query.lastError().text();
        return {};
    }

    QList<Application> result;
    while (query.next())
        result.append(applicationFromRow(query));
    return result;
}

bool ApplicationStore::update(const Application& app)
{
    QSqlQuery query(database());
    query.prepare(QStringLiteral(
        "UPDATE applications SET title = ?, company = ?, url = ?, due_date = ?, "
        "applied_date = ?, status = ?, files = ?, notes = ? WHERE id = ?"));
    query.addBindValue(textToDb(app.title));
    query.addBindValue(textToDb(app.company));
    query.addBindValue(textToDb(app.url));
    query.addBindValue(dateToDb(app.dueDate));
    query.addBindValue(dateToDb(app.appliedDate));
    query.addBindValue(statusToString(app.status));
    query.addBindValue(filesToDb(app.files));
    query.addBindValue(textToDb(app.notes));
    query.addBindValue(app.id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    if (query.numRowsAffected() == 0) {
        m_lastError = QStringLiteral("No application with id %1").arg(app.id);
        return false;
    }
    return true;
}

bool ApplicationStore::remove(int id)
{
    QSqlQuery query(database());
    query.prepare(QStringLiteral("DELETE FROM applications WHERE id = ?"));
    query.addBindValue(id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    if (query.numRowsAffected() == 0) {
        m_lastError = QStringLiteral("No application with id %1").arg(id);
        return false;
    }
    return true;
}
