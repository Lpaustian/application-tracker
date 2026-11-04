#pragma once

#include "application.h"

#include <QDialog>

class QCheckBox;
class QComboBox;
class QDateEdit;
class QLineEdit;
class QListWidget;
class QPlainTextEdit;

// The form for adding a new application or editing an existing one.
class EditDialog : public QDialog {
    Q_OBJECT

public:
    // Pass an existing application to edit it, or leave it empty to add a new one.
    explicit EditDialog(const Application& app = Application(), QWidget* parent = nullptr);

    // What is currently typed into the form.
    Application application() const;

    // Closes the dialog with OK, unless the title is empty.
    void accept() override;

private slots:
    void addFile();
    void removeFile();

private:
    int m_id;  // -1 when adding a new application
    QLineEdit* m_title;
    QLineEdit* m_company;
    QLineEdit* m_url;
    QCheckBox* m_hasDue;
    QDateEdit* m_due;
    QCheckBox* m_hasApplied;
    QDateEdit* m_applied;
    QComboBox* m_status;
    QListWidget* m_files;
    QPlainTextEdit* m_notes;
};
