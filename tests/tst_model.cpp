#include "applicationmodel.h"

#include <QtTest>

class TestModel : public QObject {
    Q_OBJECT

private slots:
    void init();

    void countsRowsAndColumns();
    void headers();
    void showsTitleCompanyAndDates();
    void blankDatesShowNothing();
    void statusColumnShowsTheStatus();
    void statusColumnSaysOverdueWhenOverdue();
    void overdueRowsAreHighlighted();
    void otherRowsAreNotHighlighted();

private:
    QModelIndex cell(int row, int column) const { return m_model.index(row, column); }

    ApplicationModel m_model;
};

void TestModel::init()
{
    Application late;
    late.title = "Backend Intern";
    late.company = "Acme Robotics";
    late.dueDate = QDate(2026, 11, 10);
    late.appliedDate = QDate();
    late.status = Status::NotStarted;

    Application done;
    done.title = "Data Intern";
    done.company = "Globex";
    done.dueDate = QDate(2026, 11, 10);
    done.appliedDate = QDate(2026, 11, 2);
    done.status = Status::Complete;

    Application noDate;
    noDate.title = "Test Intern";
    noDate.company = "Initech";
    noDate.status = Status::InProgress;

    m_model.setToday(QDate(2026, 11, 12));
    m_model.setApplications({late, done, noDate});
}

void TestModel::countsRowsAndColumns()
{
    QCOMPARE(m_model.rowCount(), 3);
    QCOMPARE(m_model.columnCount(), 5);
}

void TestModel::headers()
{
    QCOMPARE(m_model.headerData(0, Qt::Horizontal).toString(), QString("Title"));
    QCOMPARE(m_model.headerData(2, Qt::Horizontal).toString(), QString("Status"));
    QCOMPARE(m_model.headerData(4, Qt::Horizontal).toString(), QString("Applied"));
}

void TestModel::showsTitleCompanyAndDates()
{
    QCOMPARE(m_model.data(cell(1, ApplicationModel::TitleColumn)).toString(), QString("Data Intern"));
    QCOMPARE(m_model.data(cell(1, ApplicationModel::CompanyColumn)).toString(), QString("Globex"));
    QCOMPARE(m_model.data(cell(1, ApplicationModel::DueColumn)).toString(), QString("Nov 10, 2026"));
    QCOMPARE(m_model.data(cell(1, ApplicationModel::AppliedColumn)).toString(), QString("Nov 2, 2026"));
}

void TestModel::blankDatesShowNothing()
{
    QCOMPARE(m_model.data(cell(2, ApplicationModel::DueColumn)).toString(), QString(""));
    QCOMPARE(m_model.data(cell(0, ApplicationModel::AppliedColumn)).toString(), QString(""));
}

void TestModel::statusColumnShowsTheStatus()
{
    QCOMPARE(m_model.data(cell(1, ApplicationModel::StatusColumn)).toString(), QString("Complete"));
    QCOMPARE(m_model.data(cell(2, ApplicationModel::StatusColumn)).toString(), QString("In Progress"));
}

void TestModel::statusColumnSaysOverdueWhenOverdue()
{
    // Row 0 is due Nov 10, not started, and "today" is Nov 12.
    QCOMPARE(m_model.data(cell(0, ApplicationModel::StatusColumn)).toString(), QString("Overdue"));
}

void TestModel::overdueRowsAreHighlighted()
{
    for (int column = 0; column < ApplicationModel::ColumnCount; ++column)
        QVERIFY(m_model.data(cell(0, column), Qt::BackgroundRole).isValid());
}

void TestModel::otherRowsAreNotHighlighted()
{
    QVERIFY(!m_model.data(cell(1, ApplicationModel::TitleColumn), Qt::BackgroundRole).isValid());
    QVERIFY(!m_model.data(cell(2, ApplicationModel::TitleColumn), Qt::BackgroundRole).isValid());
}

QTEST_GUILESS_MAIN(TestModel)
#include "tst_model.moc"
