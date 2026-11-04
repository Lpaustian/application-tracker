#include "datepicker.h"

#include <QCalendarWidget>
#include <QEvent>
#include <QHBoxLayout>
#include <QIcon>
#include <QLineEdit>
#include <QMenu>
#include <QToolButton>
#include <QWidgetAction>

DatePicker::DatePicker(QWidget* parent)
    : QWidget(parent)
    , m_text(new QLineEdit)
    , m_button(new QToolButton)
    , m_menu(new QMenu(this))
    , m_calendar(new QCalendarWidget)
{
    m_text->setReadOnly(true);
    m_text->setCursor(Qt::PointingHandCursor);
    m_text->installEventFilter(this);

    m_button->setObjectName("datePickerButton");
    m_button->setIcon(QIcon(":/resources/arrow-down.png"));
    m_button->setCursor(Qt::PointingHandCursor);

    m_calendar->setGridVisible(false);
    m_calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    // The calendar sits inside a menu, so it pops up under the box and closes
    // when you click away.
    auto* action = new QWidgetAction(m_menu);
    action->setDefaultWidget(m_calendar);
    m_menu->addAction(action);

    connect(m_button, &QToolButton::clicked, this, &DatePicker::showCalendar);
    connect(m_calendar, &QCalendarWidget::clicked, this, [this](const QDate& picked) {
        setDate(picked);
        m_menu->close();
    });

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    layout->addWidget(m_text, 1);
    layout->addWidget(m_button);

    setDate(QDate::currentDate());
}

QDate DatePicker::date() const
{
    return m_date;
}

void DatePicker::setDate(const QDate& date)
{
    m_date = date;
    m_text->setText(date.toString("MMM d, yyyy"));
}

bool DatePicker::eventFilter(QObject* watched, QEvent* event)
{
    // Clicking the text box opens the calendar too, but only while enabled.
    if (watched == m_text && event->type() == QEvent::MouseButtonRelease && isEnabled()) {
        showCalendar();
        return true;
    }
    return QWidget::eventFilter(watched, event);
}

void DatePicker::showCalendar()
{
    m_calendar->setSelectedDate(m_date);
    m_menu->exec(mapToGlobal(rect().bottomLeft()));
}
