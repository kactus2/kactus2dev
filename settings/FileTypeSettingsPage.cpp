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

//-----------------------------------------------------------------------------
// Function: FileTypeSettingsPage()
//-----------------------------------------------------------------------------
FileTypeSettingsPage::FileTypeSettingsPage(QSettings& settings)
    : settings_(settings),
      model_(settings, this),
      proxyModel_(this),
      view_(this)
{
    proxyModel_.setSourceModel(&model_);

    view_.setItemDelegate(new FileTypesDelegate(this));
    view_.setModel(&proxyModel_);
    view_.setSortingEnabled(true);
    view_.setColumnWidth(FILE_TYPES_COL_NAME, 150);
    view_.sortByColumn(0, Qt::AscendingOrder);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(tr("File types:"), this));
    layout->addWidget(&view_, 1);

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
// Function: validate()
//-----------------------------------------------------------------------------
bool FileTypeSettingsPage::validate()
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void FileTypeSettingsPage::apply()
{
    model_.apply(settings_);
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool FileTypeSettingsPage::onPageChange()
{
    // Do not change the page if the settings are invalid.
    if (!validate())
    {
        return false;
    }

    return true;
}
