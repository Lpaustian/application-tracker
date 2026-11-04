#pragma once

#include <QStyledItemDelegate>

// Draws each cell of the table: the row background, the colored status
// pills, and the two-line due date.
class ApplicationDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
