//-----------------------------------------------------------------------------
// File: FileTypeSettingsPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 13.6.2011
//
// Description:
// General settings property page.
//-----------------------------------------------------------------------------

#include "FileTypeSettingsPage.h"

#include "FileTypesDelegate.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: FileTypeSettingsPage::FileTypeSettingsPage()
//-----------------------------------------------------------------------------
FileTypeSettingsPage::FileTypeSettingsPage(QSettings& settings):
SettingsPage(settings),
    model_(settings, this),
    proxyModel_(this),
    view_(this),
    ignoredExtensionsEdit_(this)
{
    proxyModel_.setSourceModel(&model_);

    view_.setItemDelegate(new FileTypesDelegate(this));
    view_.setModel(&proxyModel_);
    view_.setSortingEnabled(true);
    view_.sortByColumn(0, Qt::AscendingOrder);

    ignoredExtensionsEdit_.setText(settings.value("FileTypes/IgnoredExtensions").toString());
    ignoredExtensionsEdit_.setToolTip(
        tr("Specify all file extensions that should be ignored in file dependency analysis. "
        "Multiple values can be given separated by a semi-colon e.g. \"jpg;png\"."));

    QGroupBox* typesBox = new QGroupBox(tr("File types"), this);
    QVBoxLayout* typeLayout = new QVBoxLayout(typesBox);
    typeLayout->addWidget(&view_, 1);

    QFormLayout* ignoreTypeLayout = new QFormLayout();
    ignoreTypeLayout->addRow(tr("Ignored extensions:"), &ignoredExtensionsEdit_);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(typesBox);
    topLayout->addLayout(ignoreTypeLayout);

    connect(&view_, SIGNAL(addItem(const QModelIndex&)),
            &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
            &model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileTypeSettingsPage::apply()
//-----------------------------------------------------------------------------
void FileTypeSettingsPage::apply()
{
    model_.apply(settings());
    settings().setValue("FileTypes/IgnoredExtensions", ignoredExtensionsEdit_.text());
}
