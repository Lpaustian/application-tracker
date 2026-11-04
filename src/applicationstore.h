#pragma once

#include "application.h"

#include <QList>
#include <QSqlDatabase>
#include <QString>

#include <optional>

// Saves and loads applications in a SQLite file.
class ApplicationStore {
public:
    // Use ":memory:" as the path for a throwaway database in tests.
    explicit ApplicationStore(const QString& path);
    ~ApplicationStore();

    ApplicationStore(const ApplicationStore&) = delete;
    ApplicationStore& operator=(const ApplicationStore&) = delete;

    // Opens the file and creates the table if it isn't there yet.
    bool open();

    // Saves a new application and returns its id. The id in `app` is ignored.
    std::optional<int> add(const Application& app);

    std::optional<Application> get(int id) const;

    // Everything, soonest due date first. Applications with no due date come last.
    QList<Application> all() const;

    // Saves changes to an existing application. False if the id isn't found.
    bool update(const Application& app);

    // False if the id isn't found.
    bool remove(int id);

    QString lastError() const;

private:
    QSqlDatabase database() const;

    QString m_path;
    QString m_connectionName;
    mutable QString m_lastError;
};
