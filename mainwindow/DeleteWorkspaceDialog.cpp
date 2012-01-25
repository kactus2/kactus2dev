//-----------------------------------------------------------------------------
// File: DeleteWorkspaceDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.1.2012
//
// Description:
// Dialog for deleting workspaces.
//-----------------------------------------------------------------------------

#include "DeleteWorkspaceDialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: DeleteWorkspaceDialog()
//-----------------------------------------------------------------------------
DeleteWorkspaceDialog::DeleteWorkspaceDialog(QWidget* parent) : QDialog(parent), workspaceCombo_(0), btnOk_(0)
{
    // Create the name label and field.
    QLabel* nameLabel = new QLabel(tr("Workspace:"), this);
    workspaceCombo_ = new QComboBox(this);

    QHBoxLayout* nameLayout = new QHBoxLayout();
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(workspaceCombo_, 1);

    // Create the info label.
    QLabel* infoLabel = new QLabel(tr("Notice: The default workspace and the currently active workspace cannot be deleted."), this);
    infoLabel->setWordWrap(true);

    // Create the dialog buttons.
    btnOk_ = new QPushButton(tr("OK") , this);
    btnOk_->setEnabled(false);
    QPushButton* btnCancel = new QPushButton(tr("Cancel"), this);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(btnOk_);
    buttonLayout->addWidget(btnCancel);

    // Create the main mainLayout.
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(nameLayout);
    mainLayout->addWidget(infoLabel);
    mainLayout->addLayout(buttonLayout);

    connect(btnOk_, SIGNAL(clicked()), this, SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

    setWindowTitle(tr("Delete Workspace"));
    setFixedWidth(300);
    setFixedHeight(sizeHint().height());
}

//-----------------------------------------------------------------------------
// Function: ~DeleteWorkspaceDialog()
//-----------------------------------------------------------------------------
DeleteWorkspaceDialog::~DeleteWorkspaceDialog()
{
}

//-----------------------------------------------------------------------------
// Function: getName()
//----------------------------------------------------------------------------- 
QString DeleteWorkspaceDialog::getName() const
{
    return workspaceCombo_->currentText();
}

//-----------------------------------------------------------------------------
// Function: addWorkspaceName()
//-----------------------------------------------------------------------------
void DeleteWorkspaceDialog::addWorkspaceName(QString const& name)
{
    workspaceCombo_->addItem(name);
    workspaceCombo_->setCurrentIndex(0);
    btnOk_->setEnabled(true);
}
