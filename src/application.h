#pragma once

#include <QDate>
#include <QString>
#include <QStringList>

enum class Status {
    NotStarted,
    InProgress,
    Complete,
    Interview,
    Offer,
    Rejected,
};

struct Application {
    int id = -1;           // -1 until it is saved in the database
    QString title;
    QString company;
    QString url;
    QDate dueDate;         // an invalid QDate means no deadline
    QDate appliedDate;
    Status status = Status::NotStarted;
    QStringList files;     // paths to resumes, cover letters, etc.
    QString notes;
};
