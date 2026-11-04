#include "style.h"

namespace style {

QString styleSheet()
{
    // Qt stylesheets work like CSS. The @name@ markers are swapped for the
    // colors above.
    QString css = QStringLiteral(R"(
        QMainWindow, QDialog, QWidget#central { background: @background@; }
        QLabel { color: @text@; }
        QLabel#title { font-size: 26px; font-weight: 700; }
        QLabel#subtitle { color: @muted@; font-size: 13px; }

        QPushButton {
            background: @surface@; color: @text@; border: 1px solid @line@;
            border-radius: 8px; padding: 7px 14px;
        }
        QPushButton:hover { border-color: @muted@; }
        QPushButton#primary {
            background: @accent@; color: white; border: 1px solid @accent@; font-weight: 600;
        }
        QPushButton#primary:hover { background: @accentHover@; }

        QLineEdit, QComboBox, QDateEdit, QPlainTextEdit, QListWidget {
            background: @surface@; color: @text@; border: 1px solid @line@;
            border-radius: 8px; padding: 7px 10px;
            selection-background-color: @accentSoft@; selection-color: @text@;
        }
        QLineEdit:focus, QComboBox:focus, QDateEdit:focus, QPlainTextEdit:focus {
            border: 1px solid @accent@;
        }
        QLineEdit:disabled, QDateEdit:disabled { color: @muted@; background: @sunken@; }
        QCheckBox { color: @text@; spacing: 6px; }

        QFrame#stat { background: @surface@; border: 1px solid @line@; border-radius: 10px; }
        QLabel#statValue { font-size: 26px; font-weight: 700; }
        QLabel#statValueAlert { font-size: 26px; font-weight: 700; color: @warn@; }
        QLabel#statCaption { color: @muted@; font-size: 11px; }

        QTableView {
            background: @surface@; border: 1px solid @line@; border-radius: 10px;
            selection-background-color: @accentSoft@; selection-color: @text@; outline: 0;
        }
        QTableView::item { padding: 4px 12px; border-bottom: 1px solid @line@; }
        QTableView::item:hover { background: @sunken@; }
        QTableView::item:selected { background: @accentSoft@; color: @text@; }
        QHeaderView::section {
            background: @sunken@; color: @muted@; border: none; padding: 8px 12px;
            font-size: 11px; font-weight: 600;
        }

        QMenu {
            background: @surface@; color: @text@; border: 1px solid @line@;
            border-radius: 8px; padding: 5px;
        }
        QMenu::item { padding: 7px 24px 7px 12px; border-radius: 6px; }
        QMenu::item:selected { background: @sunken@; color: @text@; }
        QMenu::item:disabled { color: #9aa7a1; }
        QMenu::separator { height: 1px; background: @line@; margin: 4px 2px; }
    )");

    const struct { const char* name; const char* value; } colors[] = {
        {"background", background}, {"surface", surface}, {"sunken", sunken}, {"text", text},
        {"muted", muted}, {"line", line}, {"accentHover", accentHover}, {"accentSoft", accentSoft},
        {"accent", accent}, {"warn", warn}, {"warnSoft", warnSoft}, {"badSoft", badSoft},
        {"bad", bad}, {"infoSoft", infoSoft}, {"info", info},
    };
    for (const auto& color : colors)
        css.replace(QStringLiteral("@%1@").arg(QLatin1String(color.name)), QLatin1String(color.value));
    return css;
}

}  // namespace style
