#include "applicationstore.h"
#include "mainwindow.h"
#include "style.h"

#include <QApplication>
#include <QDir>
#include <QFont>
#include <QIcon>
#include <QMessageBox>
#include <QStandardPaths>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Internship Tracker");
    QApplication::setWindowIcon(QIcon(":/resources/app-icon.png"));
    QApplication::setFont(QFont("Segoe UI", 10));
    app.setStyleSheet(style::styleSheet());

    // The database goes in the user's app data folder, unless a path is given
    // on the command line.
    QString path;
    if (argc > 1) {
        path = argv[1];
    } else {
        QString folder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(folder);
        path = folder + "/tracker.db";
    }

    ApplicationStore store(path);
    if (!store.open()) {
        QMessageBox::critical(nullptr, "Can't open the database", store.lastError());
        return 1;
    }

    MainWindow window(store);
    window.resize(1040, 680);
    window.show();

    return app.exec();
}
