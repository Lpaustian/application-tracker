#include "applicationmodel.h"

#include "logic.h"
#include "style.h"

#include <QBrush>
#include <QColor>

#include <algorithm>

ApplicationModel::ApplicationModel(QObject* parent)
    : QAbstractTableModel(parent)
    , m_today(QDate::currentDate())
{
}

void ApplicationModel::setApplications(const QList<Application>& applications)
{
    beginResetModel();
    m_applications = applications;
    sortApplications();
    endResetModel();
}

void ApplicationModel::sort(int column, Qt::SortOrder order)
{
    m_sortColumn = column;
    m_sortOrder = order;

    beginResetModel();
    sortApplications();
    endResetModel();
}

void ApplicationModel::sortApplications()
{
    if (m_sortColumn < 0 || m_sortColumn >= ColumnCount)
        return;

    const bool ascending = m_sortOrder == Qt::AscendingOrder;

    // Returns true if `a` should be above `b`. Equal rows keep their order,
    // because stable_sort never swaps rows that compare as equal.
    auto before = [&](const Application& a, const Application& b) {
        auto byNumber = [&](int x, int y) { return ascending ? x < y : x > y; };

        switch (m_sortColumn) {
        case TitleColumn:
            return byNumber(a.title.compare(b.title, Qt::CaseInsensitive), 0);
        case CompanyColumn:
            return byNumber(a.company.compare(b.company, Qt::CaseInsensitive), 0);
        case StatusColumn:
            return byNumber(static_cast<int>(a.status), static_cast<int>(b.status));
        case DueColumn:
        case AppliedColumn: {
            const QDate& x = m_sortColumn == DueColumn ? a.dueDate : a.appliedDate;
            const QDate& y = m_sortColumn == DueColumn ? b.dueDate : b.appliedDate;
            if (x.isValid() && y.isValid())
                return ascending ? x < y : y < x;
            return dateComesFirst(x, y);  // empty dates stay at the bottom either way
        }
        }
        return false;
    };

    std::stable_sort(m_applications.begin(), m_applications.end(), before);
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
            return QBrush(QColor(style::badSoft));
        break;
    }

    return QVariant();
}
