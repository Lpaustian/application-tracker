#pragma once

#include "application.h"

#include <QAbstractTableModel>
#include <QDate>
#include <QList>

// Gives the table view its data: how many rows and columns there are, and what
// goes in each cell.
class ApplicationModel : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column {
        TitleColumn,
        CompanyColumn,
        StatusColumn,
        DueColumn,
        AppliedColumn,
        ColumnCount
    };

    explicit ApplicationModel(QObject* parent = nullptr);

    // Replaces everything in the table.
    void setApplications(const QList<Application>& applications);
    const Application& applicationAt(int row) const;

    // The date used to decide what is overdue. Tests set this so they don't
    // depend on the real date.
    void setToday(const QDate& today);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Called by the table when you click a column header.
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

private:
    void sortApplications();

    QList<Application> m_applications;
    QDate m_today;
    int m_sortColumn = -1;  // -1 means keep the order they were given in
    Qt::SortOrder m_sortOrder = Qt::AscendingOrder;
};
