#include "datepicker.h"

#include <QtTest>

class TestDatePicker : public QObject {
    Q_OBJECT

private slots:
    void keepsTheDateItWasGiven();
    void startsOnToday();
};

void TestDatePicker::keepsTheDateItWasGiven()
{
    DatePicker picker;
    picker.setDate(QDate(2026, 11, 10));
    QCOMPARE(picker.date(), QDate(2026, 11, 10));
}

void TestDatePicker::startsOnToday()
{
    DatePicker picker;
    QCOMPARE(picker.date(), QDate::currentDate());
}

QTEST_MAIN(TestDatePicker)
#include "tst_datepicker.moc"
