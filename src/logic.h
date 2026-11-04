#pragma once

#include "application.h"

#include <QList>

#include <optional>

// "Not Started", "In Progress", "Complete", "Interview", "Offer", "Rejected"
QString statusToString(Status status);

/*
 * Same names as above, ignoring upper/lower case. Returns nothing for text
 * that isn't a status.
 */
std::optional<Status> statusFromString(const QString& text);

/*
 * An application is overdue when it has a due date that has already passed and
 * it still isn't submitted (the status is Not Started or In Progress).
 */
bool isOverdue(const Application& app, const QDate& today);

/*
 * Days from today until the due date: 0 means due today, negative means it
 * already passed. Returns nothing if there is no due date.
 */
std::optional<int> daysUntilDue(const Application& app, const QDate& today);

/*
 * True if the search text appears in the title, company, or notes, ignoring
 * case. Empty search text matches everything.
 */
bool matchesSearch(const Application& app, const QString& text);

// The four numbers shown at the top of the window.
struct Summary {
    int tracked = 0;     // every application
    int submitted = 0;   // Complete, Interview, Offer, or Rejected
    int interviews = 0;  // Interview
    int dueSoon = 0;     // due within the next 7 days (today counts) and not submitted yet
};

/*
 * Counts the applications for the summary. "Not submitted yet" means the
 * status is Not Started or In Progress. An application due today counts as
 * due soon, and one due 7 days from now does too, but one that is already
 * past due, or has no due date, does not.
 */
Summary summarize(const QList<Application>& applications, const QDate& today);

/*
 * For sorting dates: true if `a` should come before `b`, earliest first. An
 * empty date always comes after a real one.
 */
bool dateComesFirst(const QDate& a, const QDate& b);

/*
 * A copy of the application with its status changed. When the new status is
 * Complete and there is no applied date yet, the applied date becomes today.
 */
Application withStatus(const Application& app, Status status, const QDate& today);
