#include "editdialog.h"

#include <QtTest>

class TestEditDialog : public QObject {
    Q_OBJECT

private slots:
    void readsBackWhatItWasGiven();
    void keepsTheId();
    void newApplicationHasNoId();
    void missingDatesStayMissing();
    void trimsTheTextFields();
};

static Application sample()
{
    Application app;
    app.id = 7;
    app.title = "Software Engineering Intern";
    app.company = "Acme Robotics";
    app.url = "https://example.com/jobs/42";
    app.dueDate = QDate(2026, 11, 10);
    app.appliedDate = QDate(2026, 11, 2);
    app.status = Status::Interview;
    app.files = {"C:/resumes/acme.docx", "C:/letters/acme.docx"};
    app.notes = "Met a recruiter at the career fair";
    return app;
}

void TestEditDialog::readsBackWhatItWasGiven()
{
    Application app = sample();
    EditDialog dialog(app);
    Application result = dialog.application();

    QCOMPARE(result.title, app.title);
    QCOMPARE(result.company, app.company);
    QCOMPARE(result.url, app.url);
    QCOMPARE(result.dueDate, app.dueDate);
    QCOMPARE(result.appliedDate, app.appliedDate);
    QCOMPARE(result.status, app.status);
    QCOMPARE(result.files, app.files);
    QCOMPARE(result.notes, app.notes);
}

void TestEditDialog::keepsTheId()
{
    EditDialog dialog(sample());
    QCOMPARE(dialog.application().id, 7);
}

void TestEditDialog::newApplicationHasNoId()
{
    EditDialog dialog;
    QCOMPARE(dialog.application().id, -1);
}

void TestEditDialog::missingDatesStayMissing()
{
    Application app = sample();
    app.dueDate = QDate();
    app.appliedDate = QDate();

    EditDialog dialog(app);
    Application result = dialog.application();

    QVERIFY(!result.dueDate.isValid());
    QVERIFY(!result.appliedDate.isValid());
}

void TestEditDialog::trimsTheTextFields()
{
    Application app = sample();
    app.title = "  Backend Intern  ";
    app.company = "\tGlobex ";

    EditDialog dialog(app);
    Application result = dialog.application();

    QCOMPARE(result.title, QString("Backend Intern"));
    QCOMPARE(result.company, QString("Globex"));
}

QTEST_MAIN(TestEditDialog)
#include "tst_editdialog.moc"
