#include "applicationmodel.h"

#include "logic.h"

#include <QBrush>
#include <QColor>

ApplicationModel::ApplicationModel(QObject* parent)
    : QAbstractTableModel(parent)
    , m_today(QDate::currentDate())
{
}

void ApplicationModel::setApplications(const QList<Application>& applications)
{
    beginResetModel();
    m_applications = applications;
    endResetModel();
}

const Application& ApplicationModel::applicationAt(int row) const
{
    return m_applications.at(row);
}

void ApplicationModel::setToday(const QDate& today)
{
    beginResetModel();
    m_today = today;
    endResetModel();
}

int ApplicationModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_applications.size();
}

int ApplicationModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant ApplicationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
    case TitleColumn:   return "Title";
    case CompanyColumn: return "Company";
    case StatusColumn:  return "Status";
    case DueColumn:     return "Due";
    case AppliedColumn: return "Applied";
    }
    return QVariant();
}


QVariant ApplicationModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const Application& app = m_applications.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case TitleColumn:   return app.title;
        case CompanyColumn: return app.company;
        case StatusColumn:
            return isOverdue(app, m_today) ? QStringLiteral("Overdue")
                                           : statusToString(app.status);
        case DueColumn:     return app.dueDate.toString("MMM d, yyyy");
        case AppliedColumn: return app.appliedDate.toString("MMM d, yyyy");
        }
        break;
    case Qt::BackgroundRole:
        if (isOverdue(app, m_today))
            return QBrush(QColor(255, 220, 220));
        break;
    }

    return QVariant();
}
