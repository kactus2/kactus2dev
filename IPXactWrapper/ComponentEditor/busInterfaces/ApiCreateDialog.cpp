//-----------------------------------------------------------------------------
// File: ApiCreateDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 30.8.2011
//
// Description:
// API creation dialog.
//-----------------------------------------------------------------------------

#include "ApiCreateDialog.h"

#include <LibraryManager/vlnv.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QDir>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QCoreApplication>
#include <QDialogButtonBox>

//-----------------------------------------------------------------------------
// Function: ApiCreateDialog()
//-----------------------------------------------------------------------------
ApiCreateDialog::ApiCreateDialog(VLNV const& vlnv, QWidget* parent) : QDialog(parent)
{
    setWindowTitle(QCoreApplication::applicationName());

    QString vlnvStr = vlnv.getVendor() + ":" + vlnv.getLibrary() + ":" +
                      vlnv.getName() + ":" + vlnv.getVersion();

    // Create the widgets.
    QLabel* msgLabel = new QLabel(tr("API definition %1 does not exist in the library.\nDo you want to "
                                     "create a new API definition with the given VLNV?").arg(vlnvStr), this);

    QLabel *directoryLabel = new QLabel(tr("Directory for the API definition:"), this);

    directoryEdit_ = new QLineEdit(createPath(vlnv), this);

    QPushButton* pathButton = new QPushButton(tr("Browse..."), this);
    connect(pathButton, SIGNAL(clicked()), this, SLOT(selectDirectory()));

    QHBoxLayout *pathLayout = new QHBoxLayout;
    pathLayout->addWidget(directoryEdit_, 1);
    pathLayout->addWidget(pathButton);

    QPushButton* btnYes = new QPushButton(tr("&Yes"), this);
    connect(btnYes, SIGNAL(clicked(bool)), this, SLOT(accept()));

    QPushButton* btnNo = new QPushButton(tr("&No"), this);
    connect(btnNo, SIGNAL(clicked(bool)), this, SLOT(reject()));

    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(btnYes, QDialogButtonBox::ActionRole);
    buttonBox->addButton(btnNo, QDialogButtonBox::ActionRole);

    // Create the layout for the dialog.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(msgLabel);
    layout->addSpacing(12);
    layout->addWidget(directoryLabel);
    layout->addLayout(pathLayout);
    layout->addWidget(buttonBox);
}

//-----------------------------------------------------------------------------
// Function: ~ApiCreateDialog()
//-----------------------------------------------------------------------------
ApiCreateDialog::~ApiCreateDialog()
{
}

//-----------------------------------------------------------------------------
// Function: selectDirectory()
//-----------------------------------------------------------------------------
void ApiCreateDialog::selectDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                    directoryEdit_->text());

    if (!dir.isEmpty())
    {
        directoryEdit_->setText(dir);
    }
}

//-----------------------------------------------------------------------------
// Function: getDirectory()
//-----------------------------------------------------------------------------
QString ApiCreateDialog::getDirectory() const
{
    return directoryEdit_->text();
}

//-----------------------------------------------------------------------------
// Function: createPath()
//-----------------------------------------------------------------------------
QString ApiCreateDialog::createPath(VLNV const& vlnv)
{
    QSettings settings;
    QString dir = settings.value("library/defaultLocation", QCoreApplication::applicationDirPath()).toString();

    if (!vlnv.getVendor().isEmpty())
    {
        dir += "/" + vlnv.getVendor();

        if (!vlnv.getLibrary().isEmpty())
        {
            dir += "/" + vlnv.getLibrary();

            if (!vlnv.getName().isEmpty())
            {
                dir += "/" + vlnv.getName();

                if (!vlnv.getVersion().isEmpty())
                {
                    dir += "/" + vlnv.getVersion();
                }
            }
        }
    }

    return dir;
}
