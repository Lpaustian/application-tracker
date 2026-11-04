#include "applicationdelegate.h"

#include "applicationmodel.h"
#include "style.h"

#include <QBrush>
#include <QFontMetrics>
#include <QPainter>

namespace {

struct PillColors {
    QColor background;
    QColor text;
};

PillColors pillColors(Status status, bool overdue)
{
    if (overdue)
        return {QColor(style::badSoft), QColor(style::bad)};

    switch (status) {
    case Status::NotStarted: return {QColor(style::sunken), QColor(style::muted)};
    case Status::InProgress: return {QColor(style::warnSoft), QColor(style::warn)};
    case Status::Complete:   return {QColor(style::infoSoft), QColor(style::info)};
    case Status::Interview:  return {QColor(style::accentSoft), QColor(style::accent)};
    case Status::Offer:      return {QColor(style::accent), QColor(Qt::white)};
    case Status::Rejected:   return {QColor(style::badSoft), QColor(style::bad)};
    }
    return {QColor(style::sunken), QColor(style::muted)};
}

// A rounded label with a small dot, like the status chips in the web version.
void drawPill(QPainter* painter, const QRect& area, const QFont& baseFont, const QString& text,
              const PillColors& colors)
{
    QFont font = baseFont;
    font.setPointSizeF(baseFont.pointSizeF() * 0.9);
    font.setWeight(QFont::Medium);
    painter->setFont(font);

    const QFontMetrics metrics(font);
    const int dot = 7;
    const int width = 10 + dot + 6 + metrics.horizontalAdvance(text) + 10;
    const int height = 24;
    const QRect pill(area.left(), area.top() + (area.height() - height) / 2, width, height);

    painter->setPen(Qt::NoPen);
    painter->setBrush(colors.background);
    painter->drawRoundedRect(pill, height / 2.0, height / 2.0);

    painter->setBrush(colors.text);
    painter->drawEllipse(QRect(pill.left() + 10, pill.center().y() - dot / 2, dot, dot));

    painter->setPen(colors.text);
    painter->drawText(QRect(pill.left() + 10 + dot + 6, pill.top(), width, height),
                      Qt::AlignVCenter | Qt::AlignLeft, text);
}

}  // namespace

void ApplicationDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                const QModelIndex& index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    // Row background: the selection color, the model's color (red for
    // overdue), or plain white. Then a thin line along the bottom.
    QColor background(style::surface);
    const QVariant modelBackground = index.data(Qt::BackgroundRole);
    if (modelBackground.isValid())
        background = qvariant_cast<QBrush>(modelBackground).color();
    if (option.state & QStyle::State_Selected)
        background = QColor(style::accentSoft);

    const QRect cell = option.rect;
    painter->fillRect(cell, background);
    painter->setPen(QColor(style::line));
    painter->drawLine(cell.bottomLeft(), cell.bottomRight());

    const QRect content = cell.adjusted(12, 0, -12, -1);
    const QString text = index.data(Qt::DisplayRole).toString();
    const QFontMetrics metrics(option.font);

    switch (index.column()) {
    case ApplicationModel::StatusColumn: {
        const auto status = static_cast<Status>(index.data(ApplicationModel::StatusRole).toInt());
        const bool overdue = index.data(ApplicationModel::OverdueRole).toBool();
        drawPill(painter, content, option.font, text, pillColors(status, overdue));
        break;
    }
    case ApplicationModel::DueColumn: {
        const QString note = index.data(ApplicationModel::DueNoteRole).toString();
        const auto tone = static_cast<ApplicationModel::Tone>(index.data(ApplicationModel::DueToneRole).toInt());

        QFont small = option.font;
        small.setPointSizeF(option.font.pointSizeF() * 0.85);

        if (text.isEmpty()) {
            painter->setFont(small);
            painter->setPen(QColor(style::muted));
            painter->drawText(content, Qt::AlignVCenter | Qt::AlignLeft, "No deadline");
        } else if (note.isEmpty()) {
            painter->setPen(QColor(style::text));
            painter->drawText(content, Qt::AlignVCenter | Qt::AlignLeft, text);
        } else {
            // The date on the first line, the note underneath it.
            const QRect top = content.adjusted(0, 0, 0, -content.height() / 2);
            const QRect bottom = content.adjusted(0, content.height() / 2, 0, 0);

            painter->setPen(QColor(style::text));
            painter->drawText(top, Qt::AlignBottom | Qt::AlignLeft, text);

            QFont noteFont = small;
            QColor noteColor(style::muted);
            if (tone != ApplicationModel::PlainTone) {
                noteFont.setBold(true);
                noteColor = QColor(tone == ApplicationModel::LateTone ? style::bad : style::warn);
            }
            painter->setFont(noteFont);
            painter->setPen(noteColor);
            painter->drawText(bottom, Qt::AlignTop | Qt::AlignLeft, note);
        }
        break;
    }
    case ApplicationModel::AppliedColumn:
        if (text.isEmpty()) {
            QFont small = option.font;
            small.setPointSizeF(option.font.pointSizeF() * 0.85);
            painter->setFont(small);
            painter->setPen(QColor(style::muted));
            painter->drawText(content, Qt::AlignVCenter | Qt::AlignLeft, "Not yet");
        } else {
            painter->setPen(QColor(style::text));
            painter->drawText(content, Qt::AlignVCenter | Qt::AlignLeft, text);
        }
        break;
    default: {
        QFont font = option.font;
        if (index.column() == ApplicationModel::TitleColumn)
            font.setWeight(QFont::DemiBold);
        painter->setFont(font);
        painter->setPen(QColor(style::text));
        painter->drawText(content, Qt::AlignVCenter | Qt::AlignLeft,
                          QFontMetrics(font).elidedText(text, Qt::ElideRight, content.width()));
        break;
    }
    }

    painter->restore();
}

QSize ApplicationDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index);
    return QSize(option.rect.width(), 52);
}
