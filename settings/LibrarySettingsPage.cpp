//-----------------------------------------------------------------------------
// File: LibrarySettingsPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 31.8.2011
//
// Description:
// Library settings page for the settings dialog.
//-----------------------------------------------------------------------------

#include "LibrarySettingsPage.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDir>
#include <QGroupBox>
#include <QFileDialog>
#include <QLabel>
#include <QCoreApplication>
#include <QMessageBox>
#include <QDialogButtonBox>

//-----------------------------------------------------------------------------
// Function: LibrarySettingsPage()
//-----------------------------------------------------------------------------
LibrarySettingsPage::LibrarySettingsPage(QSettings& settings) : settings_(settings),
                                                                libLocationsList_(0),
                                                                addLocationButton_(0),
                                                                removeLocationButton_(0),
                                                                autoRefreshCheckBox_(0),
                                                                askBeforeRefreshCheckBox_(0)
{
    // Create the library location group box.
    QGroupBox* locationGroup = new QGroupBox(tr("Library locations"), this);

    libLocationsList_ = new QListWidget(locationGroup);
    libLocationsList_->setFixedHeight(120);

    addLocationButton_ = new QPushButton(QIcon(":/icons/graphics/add.png"), QString(), this);
    removeLocationButton_ = new QPushButton(QIcon(":/icons/graphics/remove.png"), QString(), this);
    removeLocationButton_->setEnabled(false);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Vertical);
    buttonBox->addButton(addLocationButton_, QDialogButtonBox::ActionRole);
    buttonBox->addButton(removeLocationButton_, QDialogButtonBox::ActionRole);

    QHBoxLayout* locationLayout = new QHBoxLayout(locationGroup);
    locationLayout->addWidget(libLocationsList_);
    locationLayout->addWidget(buttonBox);

    // Create the check boxes.
    autoRefreshCheckBox_ = new QCheckBox(tr("Auto-refresh the library on start-up"), this);

    askBeforeRefreshCheckBox_ = new QCheckBox(tr("Ask before refresh"), this);
    askBeforeRefreshCheckBox_->setEnabled(false);

    QHBoxLayout* childLayout = new QHBoxLayout();
    childLayout->addSpacing(20);
    childLayout->addWidget(askBeforeRefreshCheckBox_);

    // Setup the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(locationGroup);
    layout->addSpacing(12);
    layout->addWidget(autoRefreshCheckBox_);
    layout->addLayout(childLayout);
    layout->addStretch(1);

    // Connect the signals.
    connect(addLocationButton_, SIGNAL(clicked()), this, SLOT(addLocation()));
    connect(removeLocationButton_, SIGNAL(clicked()), this, SLOT(removeLocation()));
    connect(libLocationsList_, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(onSelectLocation(QListWidgetItem*, QListWidgetItem*)));
    connect(autoRefreshCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onToggleAutoRefresh(bool)));

    loadSettings();
}

//-----------------------------------------------------------------------------
// Function: ~LibrarySettingsPage()
//-----------------------------------------------------------------------------
LibrarySettingsPage::~LibrarySettingsPage()
{
}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool LibrarySettingsPage::validate()
{
    Q_ASSERT(prevalidate());

    return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void LibrarySettingsPage::apply()
{
    // Create a string list containing all the location and save it to the settings.
    QStringList locations;

    for (int i = 0; i < libLocationsList_->count(); ++i)
    {
        locations.append(libLocationsList_->item(i)->text());
    }

    settings_.setValue("library/locations", locations);

    // Save the check box states.
    settings_.setValue("library/autoRefresh", autoRefreshCheckBox_->isChecked());
    settings_.setValue("library/askBeforeRefresh", askBeforeRefreshCheckBox_->isChecked());
}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool LibrarySettingsPage::onPageChange()
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

//-----------------------------------------------------------------------------
// Function: addLocation()
//-----------------------------------------------------------------------------
void LibrarySettingsPage::addLocation()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select library location:"));

    if (!dir.isEmpty())
    {
        QListWidgetItem* item = new QListWidgetItem(dir);
        libLocationsList_->addItem(item);
        libLocationsList_->setCurrentItem(item);
    }
}

//-----------------------------------------------------------------------------
// Function: removeLocation()
//-----------------------------------------------------------------------------
void LibrarySettingsPage::removeLocation()
{
    if (libLocationsList_->currentRow() >= 0)
    {
        QListWidgetItem* item = libLocationsList_->takeItem(libLocationsList_->currentRow());
        delete item;
    }
}

//-----------------------------------------------------------------------------
// Function: onToggleAutoRefresh()
//-----------------------------------------------------------------------------
void LibrarySettingsPage::onToggleAutoRefresh(bool checked)
{
    askBeforeRefreshCheckBox_->setEnabled(checked);
}

//-----------------------------------------------------------------------------
// Function: loadSettings()
//-----------------------------------------------------------------------------
void LibrarySettingsPage::loadSettings()
{
    // Load the library locations.
    QStringList locations = settings_.value("library/locations", QStringList()).toStringList();

    foreach (QString location, locations)
    {
        QListWidgetItem* item = new QListWidgetItem(location);
        libLocationsList_->addItem(item);
    }

    // Load the check box states.
    autoRefreshCheckBox_->setChecked(settings_.value("library/autoRefresh", true).toBool());
    askBeforeRefreshCheckBox_->setChecked(settings_.value("library/askBeforeRefresh", false).toBool());
}

//-----------------------------------------------------------------------------
// Function: onSelectLocation()
//-----------------------------------------------------------------------------
void LibrarySettingsPage::onSelectLocation(QListWidgetItem* cur, QListWidgetItem*)
{
    removeLocationButton_->setEnabled(cur != 0);
}
