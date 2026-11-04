#include "mainwindow.h"

#include "applicationdelegate.h"
#include "editdialog.h"
#include "logic.h"

#include <QAbstractItemView>
#include <QAction>
#include <QDate>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QFrame>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QTableView>
#include <QUrl>
#include <QVBoxLayout>

// One box in the summary strip: a big number with a small caption under it.
static QFrame* makeStat(const QString& caption, QLabel*& valueLabel, bool alert)
{
    auto* box = new QFrame;
    box->setObjectName("stat");

    valueLabel = new QLabel("0");
    valueLabel->setObjectName(alert ? "statValueAlert" : "statValue");

    auto* captionLabel = new QLabel(caption);
    captionLabel->setObjectName("statCaption");

    auto* layout = new QVBoxLayout(box);
    layout->setContentsMargins(16, 12, 16, 12);
    layout->setSpacing(2);
    layout->addWidget(valueLabel);
    layout->addWidget(captionLabel);
    return box;
}

MainWindow::MainWindow(ApplicationStore& store, QWidget* parent)
    : QMainWindow(parent)
    , m_store(store)
    , m_chipRow(new QHBoxLayout)
    , m_search(new QLineEdit)
    , m_table(new QTableView)
{
    setWindowTitle("Internship Application Log");

    m_search->setPlaceholderText("Search title, company, or notes");
    m_search->setClearButtonEnabled(true);

    m_table->setModel(&m_model);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->verticalHeader()->hide();
    m_table->verticalHeader()->setDefaultSectionSize(52);
    m_table->setItemDelegate(new ApplicationDelegate(m_table));
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->horizontalHeader()->setHighlightSections(false);
    m_table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_table->setShowGrid(false);
    m_table->setContextMenuPolicy(Qt::CustomContextMenu);

    // Clicking a column header sorts by that column. Start with the soonest
    // deadline at the top.
    m_table->setSortingEnabled(true);
    m_table->sortByColumn(ApplicationModel::DueColumn, Qt::AscendingOrder);

    auto* title = new QLabel("Internship Application Log");
    title->setObjectName("title");
    auto* subtitle = new QLabel("Summer 2027 search. Right-click an application for quick actions, "
                                "or double-click it to edit.");
    subtitle->setObjectName("subtitle");

    auto* addButton = new QPushButton("+ New application");
    addButton->setObjectName("primary");
    addButton->setCursor(Qt::PointingHandCursor);
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addApplication);

    // Title and subtitle stacked on the left, the button on the right.
    auto* titleColumn = new QVBoxLayout;
    titleColumn->setSpacing(2);
    titleColumn->addWidget(title);
    titleColumn->addWidget(subtitle);

    auto* headerRow = new QHBoxLayout;
    headerRow->addLayout(titleColumn);
    headerRow->addStretch();
    headerRow->addWidget(addButton, 0, Qt::AlignBottom);

    // The four numbers across the top.
    auto* stats = new QHBoxLayout;
    stats->setSpacing(12);
    stats->addWidget(makeStat("TRACKED", m_trackedCount, false));
    stats->addWidget(makeStat("SUBMITTED", m_submittedCount, false));
    stats->addWidget(makeStat("INTERVIEWS", m_interviewCount, false));
    stats->addWidget(makeStat("DUE IN 7 DAYS, NOT SENT", m_dueSoonCount, true));

    auto* layout = new QVBoxLayout;
    layout->setContentsMargins(24, 20, 24, 24);
    layout->setSpacing(16);
    layout->addLayout(headerRow);
    layout->addLayout(stats);
    layout->addWidget(m_search);
    m_chipRow->setSpacing(6);
    layout->addLayout(m_chipRow);
    layout->addWidget(m_table);

    auto* central = new QWidget;
    central->setObjectName("central");
    central->setLayout(layout);
    setCentralWidget(central);

    connect(m_search, &QLineEdit::textChanged, this, &MainWindow::showMatching);
    connect(m_table, &QWidget::customContextMenuRequested, this, &MainWindow::showMenu);
    connect(m_table, &QTableView::doubleClicked, this, &MainWindow::editRow);

    reload();
}

void MainWindow::reload()
{
    m_all = m_store.all();
    updateSummary();
    rebuildChips();
    showMatching();
}

void MainWindow::rebuildChips()
{
    // Remove the old buttons. deleteLater is used because this can run from
    // inside one of those buttons' own click handlers.
    while (QLayoutItem* item = m_chipRow->takeAt(0)) {
        if (QWidget* widget = item->widget())
            widget->deleteLater();
        delete item;
    }

    auto addChip = [this](const QString& label, std::optional<Status> status, int count) {
        auto* chip = new QPushButton(QString("%1  %2").arg(label).arg(count));
        chip->setObjectName("chip");
        chip->setCheckable(true);
        chip->setChecked(m_statusFilter == status);
        chip->setCursor(Qt::PointingHandCursor);
        connect(chip, &QPushButton::clicked, this, [this, status] {
            m_statusFilter = status;
            rebuildChips();
            showMatching();
        });
        m_chipRow->addWidget(chip);
    };

    addChip("All", std::nullopt, m_all.size());

    // Only statuses that have applications get a button, plus the one that is
    // currently selected so it can be clicked off again.
    const Status statuses[] = {Status::NotStarted, Status::InProgress, Status::Complete,
                               Status::Interview, Status::Offer, Status::Rejected};
    for (Status status : statuses) {
        const int count = countWithStatus(m_all, status);
        if (count > 0 || m_statusFilter == status)
            addChip(statusToString(status), status, count);
    }
    m_chipRow->addStretch();
}

void MainWindow::updateSummary()
{
    Summary summary = summarize(m_all, QDate::currentDate());
    m_trackedCount->setText(QString::number(summary.tracked));
    m_submittedCount->setText(QString::number(summary.submitted));
    m_interviewCount->setText(QString::number(summary.interviews));
    m_dueSoonCount->setText(QString::number(summary.dueSoon));
}

void MainWindow::showMatching()
{
    m_model.setApplications(filterApplications(m_all, m_search->text(), m_statusFilter));
}

void MainWindow::showMenu(const QPoint& position)
{
    // The position is where the user clicked, inside the table's visible area.
    QModelIndex index = m_table->indexAt(position);
    if (!index.isValid())
        return;  // they clicked on empty space below the last row

    m_table->selectRow(index.row());
    const Application app = m_model.applicationAt(index.row());

    QMenu menu(this);

    QAction* edit = menu.addAction("Edit...");
    connect(edit, &QAction::triggered, this, [this, app] { editApplication(app); });

    menu.addSeparator();

    QAction* link = menu.addAction("Open posting link");
    link->setEnabled(!app.url.isEmpty());
    connect(link, &QAction::triggered, this, [this, app] { openLink(app); });

    QAction* files = menu.addAction("Open attached files");
    files->setEnabled(!app.files.isEmpty());
    connect(files, &QAction::triggered, this, [this, app] { openFiles(app); });

    menu.addSeparator();

    QMenu* markAs = menu.addMenu("Mark as");
    const Status statuses[] = {Status::NotStarted, Status::InProgress, Status::Complete,
                               Status::Interview, Status::Offer, Status::Rejected};
    for (Status status : statuses) {
        QAction* action = markAs->addAction(statusToString(status));
        connect(action, &QAction::triggered, this, [this, app, status] { changeStatus(app, status); });
    }

    menu.addSeparator();

    QAction* remove = menu.addAction("Delete...");
    connect(remove, &QAction::triggered, this, [this, app] { deleteApplication(app); });

    // Shows the menu and waits until the user picks something or clicks away.
    menu.exec(m_table->viewport()->mapToGlobal(position));
}

void MainWindow::editRow(const QModelIndex& index)
{
    editApplication(m_model.applicationAt(index.row()));
}

void MainWindow::addApplication()
{
    EditDialog dialog(Application(), this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    if (!m_store.add(dialog.application())) {
        QMessageBox::warning(this, "Couldn't save", m_store.lastError());
        return;
    }
    reload();
}

void MainWindow::editApplication(const Application& app)
{
    EditDialog dialog(app, this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    if (!m_store.update(dialog.application())) {
        QMessageBox::warning(this, "Couldn't save", m_store.lastError());
        return;
    }
    reload();
}

void MainWindow::openLink(const Application& app)
{
    QDesktopServices::openUrl(QUrl::fromUserInput(app.url));
}

void MainWindow::openFiles(const Application& app)
{
    for (const QString& path : app.files)
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MainWindow::changeStatus(const Application& app, Status status)
{
    Application changed = withStatus(app, status, QDate::currentDate());
    if (!m_store.update(changed)) {
        QMessageBox::warning(this, "Couldn't save", m_store.lastError());
        return;
    }
    reload();
}

void MainWindow::deleteApplication(const Application& app)
{
    QString question = QString("Delete \"%1\" at %2?").arg(app.title, app.company);
    if (QMessageBox::question(this, "Delete application", question) != QMessageBox::Yes)
        return;

    if (!m_store.remove(app.id)) {
        QMessageBox::warning(this, "Couldn't delete", m_store.lastError());
        return;
    }
    reload();
}
