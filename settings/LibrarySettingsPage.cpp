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
LibrarySettingsPage::LibrarySettingsPage(QSettings& settings):
settings_(settings),
libLocationsList_(0),
addLocationButton_(0),
removeLocationButton_(0),
changed_(false)
{
    // Create the library location group box.
    QGroupBox* locationGroup = new QGroupBox(tr("Library locations (check the default directory)"), this);

    libLocationsList_ = new QListWidget(locationGroup);
    libLocationsList_->setFixedHeight(120);
	connect(libLocationsList_, SIGNAL(itemClicked(QListWidgetItem*)),
		this, SLOT(onItemClicked(QListWidgetItem*)), Qt::UniqueConnection);

    addLocationButton_ = new QPushButton(QIcon(":/icons/graphics/add.png"), QString(), this);
    removeLocationButton_ = new QPushButton(QIcon(":/icons/graphics/remove.png"), QString(), this);
    removeLocationButton_->setEnabled(false);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(Qt::Vertical);
    buttonBox->addButton(addLocationButton_, QDialogButtonBox::ActionRole);
    buttonBox->addButton(removeLocationButton_, QDialogButtonBox::ActionRole);

    QHBoxLayout* locationLayout = new QHBoxLayout(locationGroup);
    locationLayout->addWidget(libLocationsList_);
    locationLayout->addWidget(buttonBox);

    // Setup the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(locationGroup);
    layout->addStretch(1);

    // Connect the signals.
    connect(addLocationButton_, SIGNAL(clicked()), this, SLOT(addLocation()));
    connect(removeLocationButton_, SIGNAL(clicked()), this, SLOT(removeLocation()));
    connect(libLocationsList_, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(onSelectLocation(QListWidgetItem*, QListWidgetItem*)));
    
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

	// the checked item in the list is the default location
	QString defaultLocation;

    for (int i = 0; i < libLocationsList_->count(); ++i)
    {
		QListWidgetItem* item = libLocationsList_->item(i);
		
		if (item->checkState() == Qt::Checked) {
			defaultLocation = item->text();
		}

        locations.append(item->text());
    }

    settings_.setValue("library/locations", locations);

	// save the default location is one was set
	settings_.setValue("library/defaultLocation", defaultLocation);

	if (changed_) {
		emit scanLibrary();
	}
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
		
		// if this is the only item on the list
		if (libLocationsList_->count() == 0) {
			item->setCheckState(Qt::Checked);
		}	
		// if there are others
		else {
			item->setCheckState(Qt::Unchecked);
		}

        libLocationsList_->addItem(item);
        libLocationsList_->setCurrentItem(item);
    }

	changed_ = true;
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

	changed_ = true;
}

//-----------------------------------------------------------------------------
// Function: loadSettings()
//-----------------------------------------------------------------------------
void LibrarySettingsPage::loadSettings()
{
	QString defaultLocation = settings_.value("library/defaultLocation", QString()).toString();

    // Load the library locations.
    QStringList locations = settings_.value("library/locations", QStringList()).toStringList();

    foreach (QString location, locations)
    {
        QListWidgetItem* item = new QListWidgetItem(location);

		if (location == defaultLocation) {
			item->setCheckState(Qt::Checked);
		}
		else {
			item->setCheckState(Qt::Unchecked);
		}

        libLocationsList_->addItem(item);
    }
}

//-----------------------------------------------------------------------------
// Function: onSelectLocation()
//-----------------------------------------------------------------------------
void LibrarySettingsPage::onSelectLocation(QListWidgetItem* cur, QListWidgetItem*)
{
    removeLocationButton_->setEnabled(cur != 0);
}

void LibrarySettingsPage::onItemClicked( QListWidgetItem* item ) {
	
	// if the item was checked then remove the checks of other items
	if (item->checkState() == Qt::Checked) {

		for (int i = 0; i < libLocationsList_->count(); ++i) {
			QListWidgetItem* tempItem = libLocationsList_->item(i);

			// if not the clicked item
			if (tempItem != item) {

				// uncheck other items
				tempItem->setCheckState(Qt::Unchecked);
			}
		}
	}
}

