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
    void earlierDateComesFirst();
    void realDateComesBeforeEmpty();
    void emptyDateNeverComesFirst();
    void summaryOfNothing();
    void summaryCountsEverythingAsTracked();
    void summaryCountsSubmittedAndInterviews();
    void summaryDueSoonWindow();
    void summaryDueSoonIgnoresSubmitted();
    void dueNoteCountsDown();
    void dueNoteOnTheDueDate();
    void dueNotePastDue();
    void dueNoteIsEmptyWithoutADate();
    void dueNoteIsEmptyOnceSubmitted();
    void filterByStatusOnly();
    void filterBySearchOnly();
    void filterByStatusAndSearch();
    void filterKeepsTheOrder();
    void countsByStatus();
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

void TestLogic::earlierDateComesFirst()
{
    QVERIFY(dateComesFirst(QDate(2026, 11, 1), QDate(2026, 11, 20)));
    QVERIFY(!dateComesFirst(QDate(2026, 11, 20), QDate(2026, 11, 1)));
    QVERIFY(!dateComesFirst(QDate(2026, 11, 5), QDate(2026, 11, 5)));  // equal dates
}

void TestLogic::realDateComesBeforeEmpty()
{
    QVERIFY(dateComesFirst(QDate(2026, 11, 20), QDate()));
}

void TestLogic::emptyDateNeverComesFirst()
{
    QVERIFY(!dateComesFirst(QDate(), QDate(2026, 11, 20)));
    QVERIFY(!dateComesFirst(QDate(), QDate()));
}

static Application withStatusAndDue(Status status, const QDate& due)
{
    Application app;
    app.status = status;
    app.dueDate = due;
    return app;
}

void TestLogic::summaryOfNothing()
{
    Summary s = summarize({}, QDate(2026, 11, 5));
    QCOMPARE(s.tracked, 0);
    QCOMPARE(s.submitted, 0);
    QCOMPARE(s.interviews, 0);
    QCOMPARE(s.dueSoon, 0);
}

void TestLogic::summaryCountsEverythingAsTracked()
{
    QList<Application> apps = {
        withStatusAndDue(Status::NotStarted, QDate()),
        withStatusAndDue(Status::Complete, QDate()),
        withStatusAndDue(Status::Rejected, QDate()),
    };
    QCOMPARE(summarize(apps, QDate(2026, 11, 5)).tracked, 3);
}

void TestLogic::summaryCountsSubmittedAndInterviews()
{
    QList<Application> apps = {
        withStatusAndDue(Status::NotStarted, QDate()),
        withStatusAndDue(Status::InProgress, QDate()),
        withStatusAndDue(Status::Complete, QDate()),
        withStatusAndDue(Status::Interview, QDate()),
        withStatusAndDue(Status::Interview, QDate()),
        withStatusAndDue(Status::Offer, QDate()),
        withStatusAndDue(Status::Rejected, QDate()),
    };
    Summary s = summarize(apps, QDate(2026, 11, 5));
    QCOMPARE(s.submitted, 5);   // everything past "In Progress"
    QCOMPARE(s.interviews, 2);
}

void TestLogic::summaryDueSoonWindow()
{
    QDate today(2026, 11, 5);
    QList<Application> apps = {
        withStatusAndDue(Status::NotStarted, QDate(2026, 11, 4)),   // yesterday: too late
        withStatusAndDue(Status::NotStarted, QDate(2026, 11, 5)),   // today: counts
        withStatusAndDue(Status::InProgress, QDate(2026, 11, 12)),  // 7 days: counts
        withStatusAndDue(Status::NotStarted, QDate(2026, 11, 13)),  // 8 days: too far
        withStatusAndDue(Status::NotStarted, QDate()),              // no date
    };
    QCOMPARE(summarize(apps, today).dueSoon, 2);
}

void TestLogic::summaryDueSoonIgnoresSubmitted()
{
    QDate today(2026, 11, 5);
    QList<Application> apps = {
        withStatusAndDue(Status::Complete, QDate(2026, 11, 6)),
        withStatusAndDue(Status::Interview, QDate(2026, 11, 6)),
    };
    QCOMPARE(summarize(apps, today).dueSoon, 0);
}

void TestLogic::dueNoteCountsDown()
{
    Application app = withStatusAndDue(Status::NotStarted, QDate(2026, 11, 10));
    QCOMPARE(dueNote(app, QDate(2026, 11, 5)), QString("in 5d"));
    QCOMPARE(dueNote(app, QDate(2026, 11, 9)), QString("in 1d"));
}

void TestLogic::dueNoteOnTheDueDate()
{
    Application app = withStatusAndDue(Status::InProgress, QDate(2026, 11, 10));
    QCOMPARE(dueNote(app, QDate(2026, 11, 10)), QString("Due today"));
}

void TestLogic::dueNotePastDue()
{
    Application app = withStatusAndDue(Status::NotStarted, QDate(2026, 11, 10));
    QCOMPARE(dueNote(app, QDate(2026, 11, 13)), QString("3d past due"));
}

void TestLogic::dueNoteIsEmptyWithoutADate()
{
    Application app = withStatusAndDue(Status::NotStarted, QDate());
    QVERIFY(dueNote(app, QDate(2026, 11, 5)).isEmpty());
}

void TestLogic::dueNoteIsEmptyOnceSubmitted()
{
    Application app = withStatusAndDue(Status::Complete, QDate(2026, 11, 10));
    QVERIFY(dueNote(app, QDate(2026, 11, 5)).isEmpty());
}

static QList<Application> filterSamples()
{
    Application a;
    a.title = "Backend Intern";
    a.company = "Acme";
    a.status = Status::Interview;

    Application b;
    b.title = "Data Intern";
    b.company = "Globex";
    b.status = Status::Complete;

    Application c;
    c.title = "Test Engineer";
    c.company = "Initech";
    c.status = Status::Interview;

    return {a, b, c};
}

void TestLogic::filterByStatusOnly()
{
    QList<Application> result = filterApplications(filterSamples(), "", Status::Interview);
    QCOMPARE(result.size(), 2);

    // No status means every status.
    QCOMPARE(filterApplications(filterSamples(), "", std::nullopt).size(), 3);
}

void TestLogic::filterBySearchOnly()
{
    QList<Application> result = filterApplications(filterSamples(), "intern", std::nullopt);
    QCOMPARE(result.size(), 2);  // Backend Intern and Data Intern
}

void TestLogic::filterByStatusAndSearch()
{
    QList<Application> result = filterApplications(filterSamples(), "intern", Status::Interview);
    QCOMPARE(result.size(), 1);
    QCOMPARE(result[0].company, QString("Acme"));
}

void TestLogic::filterKeepsTheOrder()
{
    QList<Application> result = filterApplications(filterSamples(), "", Status::Interview);
    QCOMPARE(result[0].company, QString("Acme"));
    QCOMPARE(result[1].company, QString("Initech"));
}

void TestLogic::countsByStatus()
{
    QCOMPARE(countWithStatus(filterSamples(), Status::Interview), 2);
    QCOMPARE(countWithStatus(filterSamples(), Status::Complete), 1);
    QCOMPARE(countWithStatus(filterSamples(), Status::Offer), 0);
}

QTEST_GUILESS_MAIN(TestLogic)
#include "tst_logic.moc"
