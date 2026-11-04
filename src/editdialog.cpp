#include "editdialog.h"

#include "datepicker.h"
#include "logic.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

EditDialog::EditDialog(const Application& app, QWidget* parent)
    : QDialog(parent)
    , m_id(app.id)
    , m_title(new QLineEdit(app.title))
    , m_company(new QLineEdit(app.company))
    , m_url(new QLineEdit(app.url))
    , m_hasDue(new QCheckBox("Has a due date"))
    , m_due(new DatePicker)
    , m_hasApplied(new QCheckBox("Already applied"))
    , m_applied(new DatePicker)
    , m_status(new QComboBox)
    , m_files(new QListWidget)
    , m_notes(new QPlainTextEdit(app.notes))
{
    setWindowTitle(app.id < 0 ? "Add application" : "Edit application");

    // Each date has a checkbox, because a date can be missing. The date box
    // is greyed out until the checkbox is ticked.
    m_hasDue->setChecked(app.dueDate.isValid());
    m_due->setDate(app.dueDate.isValid() ? app.dueDate : QDate::currentDate());
    m_due->setEnabled(m_hasDue->isChecked());
    connect(m_hasDue, &QCheckBox::toggled, m_due, &QWidget::setEnabled);

    m_hasApplied->setChecked(app.appliedDate.isValid());
    m_applied->setDate(app.appliedDate.isValid() ? app.appliedDate : QDate::currentDate());
    m_applied->setEnabled(m_hasApplied->isChecked());
    connect(m_hasApplied, &QCheckBox::toggled, m_applied, &QWidget::setEnabled);

    // Each choice shows the status name and remembers the status itself.
    const Status statuses[] = {Status::NotStarted, Status::InProgress, Status::Complete,
                               Status::Interview, Status::Offer, Status::Rejected};
    for (Status status : statuses)
        m_status->addItem(statusToString(status), static_cast<int>(status));
    m_status->setCurrentIndex(m_status->findData(static_cast<int>(app.status)));

    m_files->addItems(app.files);
    auto* addFileButton = new QPushButton("Add file...");
    auto* removeFileButton = new QPushButton("Remove selected");
    connect(addFileButton, &QPushButton::clicked, this, &EditDialog::addFile);
    connect(removeFileButton, &QPushButton::clicked, this, &EditDialog::removeFile);

    auto* dueRow = new QHBoxLayout;
    dueRow->setSpacing(14);
    dueRow->addWidget(m_hasDue);
    dueRow->addWidget(m_due);

    auto* appliedRow = new QHBoxLayout;
    appliedRow->setSpacing(14);
    appliedRow->addWidget(m_hasApplied);
    appliedRow->addWidget(m_applied);

    auto* fileButtons = new QHBoxLayout;
    fileButtons->addWidget(addFileButton);
    fileButtons->addWidget(removeFileButton);
    fileButtons->addStretch();

    auto* filesColumn = new QVBoxLayout;
    filesColumn->addWidget(m_files);
    filesColumn->addLayout(fileButtons);

    // A form layout puts a label on the left and a field on the right, row by row.
    auto* form = new QFormLayout;
    form->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    form->setHorizontalSpacing(18);
    form->setVerticalSpacing(12);
    form->addRow("Title", m_title);
    form->addRow("Company", m_company);
    form->addRow("Link", m_url);
    form->addRow("Due", dueRow);
    form->addRow("Applied", appliedRow);
    form->addRow("Status", m_status);
    form->addRow("Files", filesColumn);
    form->addRow("Notes", m_notes);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 20);
    layout->setSpacing(16);
    layout->addLayout(form);
    layout->addWidget(buttons);

    m_files->setMaximumHeight(110);
    m_notes->setMinimumHeight(90);
    setMinimumWidth(560);
    resize(600, 640);
}

Application EditDialog::application() const
{
    Application app;
    app.id = m_id;
    app.title = m_title->text().trimmed();
    app.company = m_company->text().trimmed();
    app.url = m_url->text().trimmed();
    app.dueDate = m_hasDue->isChecked() ? m_due->date() : QDate();
    app.appliedDate = m_hasApplied->isChecked() ? m_applied->date() : QDate();
    app.status = static_cast<Status>(m_status->currentData().toInt());
    app.notes = m_notes->toPlainText();

    for (int i = 0; i < m_files->count(); ++i) {
        app.files.append(m_files->item(i)->text());
    }

    return app;
}

void EditDialog::accept()
{
    if (m_title->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Missing title", "Please enter a title.");
        return;
    }
    QDialog::accept();
}

void EditDialog::addFile()
{
    m_files->addItems(QFileDialog::getOpenFileNames(this, "Attach files"));
}

void EditDialog::removeFile()
{
    qDeleteAll(m_files->selectedItems());
}
