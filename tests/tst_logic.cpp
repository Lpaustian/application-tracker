#include "logic.h"

#include <QtTest>

class TestLogic : public QObject {
    Q_OBJECT

private slots:
    void statusNames();
    void statusFromStringIgnoresCase();
    void statusFromStringRejectsJunk();
    void overdueWhenPastDueAndNotSubmitted();
    void notOverdueOnceSubmitted();
    void notOverdueWithoutDueDate();
    void notOverdueOnTheDueDate();
    void daysUntilDueCounts();
    void daysUntilDueWithoutDate();
    void searchFindsTitleCompanyAndNotes();
    void searchIgnoresCase();
    void emptySearchMatchesEverything();
    void withStatusChangesTheStatus();
    void completeFillsInTheAppliedDate();
    void completeKeepsAnExistingAppliedDate();
    void otherStatusesLeaveTheAppliedDateAlone();
};

static Application sample()
{
    Application app;
    app.title = "Software Engineering Intern";
    app.company = "Acme Robotics";
    app.notes = "Met a recruiter at the career fair";
    app.dueDate = QDate(2026, 11, 10);
    return app;
}

void TestLogic::statusNames()
{
    QCOMPARE(statusToString(Status::NotStarted), QString("Not Started"));
    QCOMPARE(statusToString(Status::InProgress), QString("In Progress"));
    QCOMPARE(statusToString(Status::Complete), QString("Complete"));
    QCOMPARE(statusToString(Status::Interview), QString("Interview"));
    QCOMPARE(statusToString(Status::Offer), QString("Offer"));
    QCOMPARE(statusToString(Status::Rejected), QString("Rejected"));
}

void TestLogic::statusFromStringIgnoresCase()
{
    QVERIFY(statusFromString("interview") == Status::Interview);
    QVERIFY(statusFromString("INTERVIEW") == Status::Interview);
    QVERIFY(statusFromString("Offer") == Status::Offer);
    QVERIFY(statusFromString("not started") == Status::NotStarted);
    QVERIFY(statusFromString("IN PROGRESS") == Status::InProgress);
}

void TestLogic::statusFromStringRejectsJunk()
{
    QVERIFY(!statusFromString("").has_value());
    QVERIFY(!statusFromString("pending").has_value());
}

void TestLogic::overdueWhenPastDueAndNotSubmitted()
{
    Application app = sample();  // due Nov 10, status Not Started
    QVERIFY(isOverdue(app, QDate(2026, 11, 11)));

    app.status = Status::InProgress;
    QVERIFY(isOverdue(app, QDate(2026, 11, 11)));
}

void TestLogic::notOverdueOnceSubmitted()
{
    Application app = sample();
    app.status = Status::Complete;
    QVERIFY(!isOverdue(app, QDate(2026, 11, 11)));

    app.status = Status::Interview;
    QVERIFY(!isOverdue(app, QDate(2026, 11, 11)));
}

void TestLogic::notOverdueWithoutDueDate()
{
    Application app = sample();
    app.dueDate = QDate();
    QVERIFY(!isOverdue(app, QDate(2026, 11, 11)));
}

void TestLogic::notOverdueOnTheDueDate()
{
    Application app = sample();
    QVERIFY(!isOverdue(app, QDate(2026, 11, 10)));
}

void TestLogic::daysUntilDueCounts()
{
    Application app = sample();  // due Nov 10
    QCOMPARE(daysUntilDue(app, QDate(2026, 11, 7)).value(), 3);
    QCOMPARE(daysUntilDue(app, QDate(2026, 11, 10)).value(), 0);
    QCOMPARE(daysUntilDue(app, QDate(2026, 11, 12)).value(), -2);
}

void TestLogic::daysUntilDueWithoutDate()
{
    Application app = sample();
    app.dueDate = QDate();
    QVERIFY(!daysUntilDue(app, QDate(2026, 11, 7)).has_value());
}

void TestLogic::searchFindsTitleCompanyAndNotes()
{
    Application app = sample();
    QVERIFY(matchesSearch(app, "Engineering"));
    QVERIFY(matchesSearch(app, "Robotics"));
    QVERIFY(matchesSearch(app, "career fair"));
    QVERIFY(!matchesSearch(app, "Boeing"));
}

void TestLogic::searchIgnoresCase()
{
    Application app = sample();
    QVERIFY(matchesSearch(app, "acme"));
    QVERIFY(matchesSearch(app, "SOFTWARE"));
}

void TestLogic::emptySearchMatchesEverything()
{
    QVERIFY(matchesSearch(sample(), ""));
}

void TestLogic::withStatusChangesTheStatus()
{
    Application app = sample();  // Not Started
    Application changed = withStatus(app, Status::InProgress, QDate(2026, 11, 5));
    QCOMPARE(changed.status, Status::InProgress);
    QCOMPARE(changed.title, app.title);  // everything else is the same
}

void TestLogic::completeFillsInTheAppliedDate()
{
    Application app = sample();
    QVERIFY(!app.appliedDate.isValid());

    Application changed = withStatus(app, Status::Complete, QDate(2026, 11, 5));
    QCOMPARE(changed.status, Status::Complete);
    QCOMPARE(changed.appliedDate, QDate(2026, 11, 5));
}

void TestLogic::completeKeepsAnExistingAppliedDate()
{
    Application app = sample();
    app.appliedDate = QDate(2026, 11, 2);

    Application changed = withStatus(app, Status::Complete, QDate(2026, 11, 5));
    QCOMPARE(changed.appliedDate, QDate(2026, 11, 2));
}

void TestLogic::otherStatusesLeaveTheAppliedDateAlone()
{
    Application app = sample();

    Application changed = withStatus(app, Status::InProgress, QDate(2026, 11, 5));
    QVERIFY(!changed.appliedDate.isValid());
}

QTEST_GUILESS_MAIN(TestLogic)
#include "tst_logic.moc"
