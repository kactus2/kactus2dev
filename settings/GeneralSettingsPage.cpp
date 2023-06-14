//-----------------------------------------------------------------------------
// File: GeneralSettingsPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 13.6.2011
//
// Description:
// General settings property page.
//-----------------------------------------------------------------------------

#include "GeneralSettingsPage.h"

#include <KactusAPI/include/utils.h>

#include <IPXACTmodels/common/Document.h>

#include <QCoreApplication>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSettings>

//-----------------------------------------------------------------------------
// Function: GeneralSettingsPage()
//-----------------------------------------------------------------------------
GeneralSettingsPage::GeneralSettingsPage(QSettings& settings):
SettingsPage(settings),
usernameEdit_(new QLineEdit(this)),
revisionEdit_(new QComboBox(this))
{
    QString username = settings.value("General/Username", Utils::getCurrentUser()).toString();
    usernameEdit_->setText(username);

    revisionEdit_->addItem(Document::toString(Document::Revision::Std22));
    revisionEdit_->addItem(Document::toString(Document::Revision::Std14));

    auto defaultRevision = settings.value("General/Revision", Document::toString(Document::Revision::Std22)).toString();
    revisionEdit_->setCurrentText(defaultRevision);

    // Setup the layout.
    auto layout = new QFormLayout(this);
    layout->addRow(tr("Settings file:"), new QLabel(settings.fileName(), this));
    layout->addRow(tr("User name:"), usernameEdit_);
    layout->addRow(tr("Default IP-XACT version:"), revisionEdit_);
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

    return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void GeneralSettingsPage::apply()
{
    // Save the settings.
    settings().setValue("General/Username", usernameEdit_->text());
    settings().setValue("General/Revision", revisionEdit_->currentText());
}
