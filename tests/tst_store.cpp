#include "applicationstore.h"

#include <QtTest>

#include <memory>

class TestStore : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void addReturnsDifferentIds();
    void getReturnsEverythingThatWasSaved();
    void getMissingIdReturnsNothing();
    void missingDatesComeBackEmpty();
    void allSortsByDueDateWithBlanksLast();
    void updateChangesFields();
    void updateMissingIdFails();
    void removeDeletesTheApplication();
    void removeMissingIdFails();
    void oddTextIsSavedSafely();

private:
    static Application sample();

    std::unique_ptr<ApplicationStore> m_store;
};

Application TestStore::sample()
{
    Application app;
    app.title = "Software Engineering Intern";
    app.company = "Acme Robotics";
    app.url = "https://example.com/jobs/42";
    app.dueDate = QDate(2026, 11, 10);
    app.notes = "Met a recruiter at the career fair";
    return app;
}

void TestStore::init()
{
    m_store = std::make_unique<ApplicationStore>(":memory:");
    QVERIFY2(m_store->open(), qPrintable(m_store->lastError()));
}

void TestStore::cleanup()
{
    m_store.reset();
}

void TestStore::addReturnsDifferentIds()
{
    auto first = m_store->add(sample());
    auto second = m_store->add(sample());

    QVERIFY(first.has_value());
    QVERIFY(second.has_value());
    QVERIFY(*first != *second);
}

void TestStore::getReturnsEverythingThatWasSaved()
{
    Application app = sample();
    app.appliedDate = QDate(2026, 11, 2);
    app.status = Status::Complete;
    app.files = {"C:/resumes/acme.docx", "C:/letters/acme.docx"};

    auto id = m_store->add(app);
    QVERIFY(id.has_value());

    auto loaded = m_store->get(*id);
    QVERIFY(loaded.has_value());
    QCOMPARE(loaded->id, *id);
    QCOMPARE(loaded->title, app.title);
    QCOMPARE(loaded->company, app.company);
    QCOMPARE(loaded->url, app.url);
    QCOMPARE(loaded->dueDate, app.dueDate);
    QCOMPARE(loaded->appliedDate, app.appliedDate);
    QCOMPARE(loaded->status, app.status);
    QCOMPARE(loaded->files, app.files);
    QCOMPARE(loaded->notes, app.notes);
}

void TestStore::getMissingIdReturnsNothing()
{
    QVERIFY(!m_store->get(999).has_value());
}

void TestStore::missingDatesComeBackEmpty()
{
    Application app = sample();
    app.dueDate = QDate();
    app.files.clear();

    auto id = m_store->add(app);
    auto loaded = m_store->get(*id);

    QVERIFY(loaded.has_value());
    QVERIFY(!loaded->dueDate.isValid());
    QVERIFY(!loaded->appliedDate.isValid());
    QVERIFY(loaded->files.isEmpty());
}

void TestStore::allSortsByDueDateWithBlanksLast()
{
    Application nov20 = sample();
    nov20.title = "Nov 20";
    nov20.dueDate = QDate(2026, 11, 20);

    Application none = sample();
    none.title = "No deadline";
    none.dueDate = QDate();

    Application nov1 = sample();
    nov1.title = "Nov 1";
    nov1.dueDate = QDate(2026, 11, 1);

    m_store->add(nov20);
    m_store->add(none);
    m_store->add(nov1);

    QList<Application> sorted = m_store->all();
    QCOMPARE(sorted.size(), 3);
    QCOMPARE(sorted[0].title, QString("Nov 1"));
    QCOMPARE(sorted[1].title, QString("Nov 20"));
    QCOMPARE(sorted[2].title, QString("No deadline"));
}

void TestStore::updateChangesFields()
{
    auto id = m_store->add(sample());
    auto app = m_store->get(*id);
    QVERIFY(app.has_value());

    app->status = Status::Interview;
    app->notes = "Phone screen on Friday";
    app->files = {"C:/resumes/acme.docx"};
    QVERIFY2(m_store->update(*app), qPrintable(m_store->lastError()));

    auto reloaded = m_store->get(*id);
    QVERIFY(reloaded.has_value());
    QCOMPARE(reloaded->status, Status::Interview);
    QCOMPARE(reloaded->notes, QString("Phone screen on Friday"));
    QCOMPARE(reloaded->files, app->files);
}

void TestStore::updateMissingIdFails()
{
    Application ghost = sample();
    ghost.id = 999;
    QVERIFY(!m_store->update(ghost));
}

void TestStore::removeDeletesTheApplication()
{
    auto id = m_store->add(sample());
    QVERIFY(m_store->remove(*id));
    QVERIFY(!m_store->get(*id).has_value());
    QVERIFY(m_store->all().isEmpty());
}

void TestStore::removeMissingIdFails()
{
    QVERIFY(!m_store->remove(999));
}

void TestStore::oddTextIsSavedSafely()
{
    Application app = sample();
    app.company = "O'Reilly'); DROP TABLE applications;--";
    app.notes = "Said \"call me\" \u2014 caf\u00e9";

    auto id = m_store->add(app);
    QVERIFY2(id.has_value(), qPrintable(m_store->lastError()));

    auto loaded = m_store->get(*id);
    QVERIFY(loaded.has_value());
    QCOMPARE(loaded->company, app.company);
    QCOMPARE(loaded->notes, app.notes);
    QCOMPARE(m_store->all().size(), 1);  // the table is still there
}

QTEST_GUILESS_MAIN(TestStore)
#include "tst_store.moc"
