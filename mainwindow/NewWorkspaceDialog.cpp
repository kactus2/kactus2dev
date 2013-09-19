//-----------------------------------------------------------------------------
// File: NewWorkspaceDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.1.2012
//
// Description:
// Dialog for creating new workspaces.
//-----------------------------------------------------------------------------

#include "NewWorkspaceDialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QCoreApplication>
#include <QSettings>

//-----------------------------------------------------------------------------
// Function: NewWorkspaceDialog()
//-----------------------------------------------------------------------------
NewWorkspaceDialog::NewWorkspaceDialog(QWidget* parent) : QDialog(parent), nameEdit_(0), btnOk_(0)
{
    // Create the name label and field.
    QLabel* nameLabel = new QLabel(tr("Name:"), this);
    nameEdit_ = new QLineEdit(this);

    // Create the dialog buttons.
    btnOk_ = new QPushButton(tr("OK") , this);
    QPushButton* btnCancel = new QPushButton(tr("Cancel"), this);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(btnOk_);
    buttonLayout->addWidget(btnCancel);

    // Create the main layout.
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(nameLabel, 0, 0, 1, 1);
    layout->addWidget(nameEdit_, 0, 1, 1, 1);
    layout->addLayout(buttonLayout, 1, 0, 1, 2);

    connect(btnOk_, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(nameEdit_, SIGNAL(textChanged(QString const&)), this, SLOT(onContentChanged()));

    setWindowTitle(tr("New Workspace"));
    setFixedSize(sizeHint());
    onContentChanged();
}

//-----------------------------------------------------------------------------
// Function: ~NewWorkspaceDialog()
//-----------------------------------------------------------------------------
NewWorkspaceDialog::~NewWorkspaceDialog()
{
}

//-----------------------------------------------------------------------------
// Function: getName()
//----------------------------------------------------------------------------- 
QString NewWorkspaceDialog::getName() const
{
    return nameEdit_->text();
}

//-----------------------------------------------------------------------------
// Function: accept()
//-----------------------------------------------------------------------------
void NewWorkspaceDialog::accept()
{
    // Validate the name.
    QSettings settings;
    if (settings.contains("Workspaces/" + nameEdit_->text() + "/Geometry"))
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                           tr("Workspace '%1' already exists.").arg(nameEdit_->text()),
                           QMessageBox::Ok, this);
        msgBox.exec();
        return;
    }

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: onContentChanged()
//-----------------------------------------------------------------------------
void NewWorkspaceDialog::onContentChanged()
{
    btnOk_->setEnabled(!nameEdit_->text().isEmpty());
}
