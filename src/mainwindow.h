#pragma once

#include "applicationmodel.h"
#include "applicationstore.h"

#include <QList>
#include <QMainWindow>

#include <optional>

class QHBoxLayout;
class QLabel;
class QLineEdit;
class QTableView;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(ApplicationStore& store, QWidget* parent = nullptr);

private slots:
    // Runs every time the text in the search box changes.
    void showMatching();

    // Runs when the user right-clicks the table.
    void showMenu(const QPoint& position);

    // Runs when the user double-clicks a row.
    void editRow(const QModelIndex& index);

private:
    // Loads everything from the database, then applies the search.
    void reload();

    // Refreshes the four numbers at the top.
    void updateSummary();

    // Rebuilds the row of status buttons under the search box.
    void rebuildChips();

    void addApplication();
    void editApplication(const Application& app);
    void openLink(const Application& app);
    void openFiles(const Application& app);
    void changeStatus(const Application& app, Status status);
    void deleteApplication(const Application& app);

    ApplicationStore& m_store;
    ApplicationModel m_model;
    QList<Application> m_all;  // everything in the database
    std::optional<Status> m_statusFilter;  // empty means show every status
    QHBoxLayout* m_chipRow;
    QLineEdit* m_search;
    QTableView* m_table;
    QLabel* m_trackedCount;
    QLabel* m_submittedCount;
    QLabel* m_interviewCount;
    QLabel* m_dueSoonCount;
};
