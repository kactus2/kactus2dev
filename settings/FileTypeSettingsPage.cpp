//-----------------------------------------------------------------------------
// File: FileTypeSettingsPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.6.2011
//
// Description:
// General settings property page.
//-----------------------------------------------------------------------------

#include "FileTypeSettingsPage.h"

#include "FileTypesDelegate.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QSizePolicy>

//-----------------------------------------------------------------------------
// Function: FileTypeSettingsPage()
//-----------------------------------------------------------------------------
FileTypeSettingsPage::FileTypeSettingsPage(QSettings& settings)
    : SettingsPage(settings),
      model_(settings, this),
      proxyModel_(this),
      view_(this),
      ignoredExtensionsEdit_(this)
{
    proxyModel_.setSourceModel(&model_);

    view_.setItemDelegate(new FileTypesDelegate(this));
    view_.setModel(&proxyModel_);
    view_.setSortingEnabled(true);
    view_.setColumnWidth(FILE_TYPES_COL_NAME, 150);
    view_.sortByColumn(0, Qt::AscendingOrder);

    ignoredExtensionsEdit_.setText(settings.value("FileTypes/IgnoredExtensions").toString());

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("File types:"), this), 0);
    layout->addWidget(&view_, 1);
    layout->addWidget(new QLabel(tr("Ignored extensions:"), this));
    layout->addWidget(&ignoredExtensionsEdit_);

    connect(&view_, SIGNAL(addItem(const QModelIndex&)),
            &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
    connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
            &model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ~FileTypeSettingsPage()
//-----------------------------------------------------------------------------
FileTypeSettingsPage::~FileTypeSettingsPage()
{
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void FileTypeSettingsPage::apply()
{
    model_.apply(settings());
    settings().setValue("FileTypes/IgnoredExtensions", ignoredExtensionsEdit_.text());
}
