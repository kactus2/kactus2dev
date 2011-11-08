//-----------------------------------------------------------------------------
// File: GeneralSettingsPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.6.2011
//
// Description:
// General settings property page.
//-----------------------------------------------------------------------------

#include "GeneralSettingsPage.h"

#include <QVBoxLayout>
#include <QDir>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QCoreApplication>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: GeneralSettingsPage()
//-----------------------------------------------------------------------------
GeneralSettingsPage::GeneralSettingsPage(QSettings& settings) : settings_(settings),
                                                                directoryEdit_(0),
                                                                usernameEdit_(0)
{
    // Create the directory line edit and label.
    QLabel *directoryLabel = new QLabel(tr("Default directory:"), this);

    QString defaultDir = settings_.value("general/defaultDir", QCoreApplication::applicationDirPath()).toString();
    directoryEdit_ = new QLineEdit(defaultDir, this);
    connect(directoryEdit_, SIGNAL(textChanged(QString const&)), this, SIGNAL(contentChanged()));

    QPushButton* pathButton = new QPushButton("Browse...", this);
    connect(pathButton, SIGNAL(clicked()), this, SLOT(selectDirectory()));
    QHBoxLayout *pathLayout = new QHBoxLayout;
    pathLayout->addWidget(directoryEdit_, 1);
    pathLayout->addWidget(pathButton);

    // Create the username line edit and label.
    QLabel* usernameLabel = new QLabel(tr("User name:"), this);

    QString username = settings_.value("general/username", getenv("USERNAME")).toString();
    usernameEdit_ = new QLineEdit(username, this);

    // Setup the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(usernameLabel);
    layout->addWidget(usernameEdit_);
    layout->addSpacing(12);
    layout->addWidget(directoryLabel);
    layout->addLayout(pathLayout);
    layout->addStretch(1);
}

//-----------------------------------------------------------------------------
// Function: ~GeneralSettingsPage()
//-----------------------------------------------------------------------------
GeneralSettingsPage::~GeneralSettingsPage()
{
}

//-----------------------------------------------------------------------------
// Function: selectDirectory()
//-----------------------------------------------------------------------------
void GeneralSettingsPage::selectDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                    directoryEdit_->text());

    if (!dir.isEmpty())
    {
        directoryEdit_->setText(dir);
    }
}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool GeneralSettingsPage::validate()
{
    Q_ASSERT(prevalidate());

    // Check for a valid username.
    if (usernameEdit_->text().isEmpty())
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                           tr("The user name must be set."), QMessageBox::Ok, this);
        msgBox.exec();
        return false;
    }

    // Check if the directory does not exist.
    if (directoryEdit_->text().isEmpty() || !QDir(directoryEdit_->text()).exists())
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                           tr("The specified path for the default directory is invalid. "
                              "Please enter a valid path."), QMessageBox::Ok, this);
        msgBox.exec();
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void GeneralSettingsPage::apply()
{
    // Save the settings.
    settings_.setValue("general/defaultDir", directoryEdit_->text());
    settings_.setValue("general/username", usernameEdit_->text());
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool GeneralSettingsPage::onPageChange()
{
    // Do not change the page if the settings are invalid.
    if (!validate())
    {
        return false;
    }

    // If they are valid, save them and allow the page to be changed.
    apply();
    return true;
}
