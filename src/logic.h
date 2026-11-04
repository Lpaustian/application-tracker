#pragma once

#include "application.h"

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
