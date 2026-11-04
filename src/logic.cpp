#include "logic.h"

QString statusToString(Status status)
{
    switch (status) {
	case Status::NotStarted:
		return "Not Started";
	case Status::InProgress:
		return "In Progress";
	case Status::Complete:
		return "Complete";
	case Status::Interview:
		return "Interview";
	case Status::Offer:
		return "Offer";
	case Status::Rejected:
		return "Rejected";
	}
    return QString();
}

std::optional<Status> statusFromString(const QString& text)
{
	const Status statuses[] = {Status::NotStarted, Status::InProgress, Status::Complete, Status::Interview, Status::Offer, Status::Rejected};
	for (Status s : statuses) {
		if (statusToString(s).compare(text,Qt::CaseInsensitive) == 0) {
			return s;
		}
	}
    return std::nullopt;
}

bool isOverdue(const Application& app, const QDate& today)
{
	return(daysUntilDue(app, today).value_or(0) < 0 && (app.status == Status::InProgress || app.status == Status::NotStarted));
}

std::optional<int> daysUntilDue(const Application& app, const QDate& today)
{
	if (app.dueDate.isValid()) {
		return today.daysTo(app.dueDate);
	}
    return std::nullopt;
}

bool matchesSearch(const Application& app, const QString& text)
{
	if (text.isEmpty()|| app.title.contains(text, Qt::CaseInsensitive)|| app.company.contains(text, Qt::CaseInsensitive)||app.notes.contains(text, Qt::CaseInsensitive)) {
		return true;
	}
    return false;
}
