#pragma once

#include <QDate>
#include <QWidget>

class QCalendarWidget;
class QLineEdit;
class QMenu;
class QToolButton;

// A date box with a calendar. The box shows the date and can't be typed in;
// clicking it or the arrow button opens a calendar to pick from.
class DatePicker : public QWidget {
    Q_OBJECT

public:
    explicit DatePicker(QWidget* parent = nullptr);

    QDate date() const;
    void setDate(const QDate& date);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void showCalendar();

    QDate m_date;
    QLineEdit* m_text;
    QToolButton* m_button;
    QMenu* m_menu;
    QCalendarWidget* m_calendar;
};
