//-----------------------------------------------------------------------------
// File: LibrarySettingsDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.08.2012
//
// Description:
// Dialog for configuring library locations.
//-----------------------------------------------------------------------------

#include "LibrarySettingsDialog.h"

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
// Function: LibrarySettingsDialog()
//-----------------------------------------------------------------------------
LibrarySettingsDialog::LibrarySettingsDialog(QSettings& settings, QWidget* parent)
    : QDialog(parent),
      settings_(settings),
      libLocationsList_(0),
      addLocationButton_(0),
      removeLocationButton_(0),
      changed_(false)
{
    setWindowTitle(tr("Configure Library"));

    // Create the library location group box.
    QGroupBox* locationGroup = new QGroupBox(tr("Library locations (check the default directory)"), this);

    libLocationsList_ = new QListWidget(locationGroup);
    libLocationsList_->setFixedHeight(120);
    connect(libLocationsList_, SIGNAL(itemClicked(QListWidgetItem*)),
        this, SLOT(onItemClicked(QListWidgetItem*)), Qt::UniqueConnection);

    addLocationButton_ = new QPushButton(QIcon(":/icons/graphics/add.png"), QString(), this);
    removeLocationButton_ = new QPushButton(QIcon(":/icons/graphics/remove.png"), QString(), this);
    removeLocationButton_->setEnabled(false);

    QDialogButtonBox* listButtonBox = new QDialogButtonBox(Qt::Vertical);
    listButtonBox->addButton(addLocationButton_, QDialogButtonBox::ActionRole);
    listButtonBox->addButton(removeLocationButton_, QDialogButtonBox::ActionRole);

    QHBoxLayout* locationLayout = new QHBoxLayout(locationGroup);
    locationLayout->addWidget(libLocationsList_);
    locationLayout->addWidget(listButtonBox);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, this);

    // Setup the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(locationGroup);
    layout->addStretch(1);
    layout->addWidget(buttonBox);

    // Connect the signals.
    connect(addLocationButton_, SIGNAL(clicked()), this, SLOT(addLocation()));
    connect(removeLocationButton_, SIGNAL(clicked()), this, SLOT(removeLocation()));
    connect(libLocationsList_, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(onSelectLocation(QListWidgetItem*, QListWidgetItem*)));
    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(accept()), Qt::UniqueConnection);
    connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()), Qt::UniqueConnection);

    setFixedHeight(sizeHint().height());
    resize(500, sizeHint().height());

    loadSettings();
}

//-----------------------------------------------------------------------------
// Function: ~LibrarySettingsDialog()
//-----------------------------------------------------------------------------
LibrarySettingsDialog::~LibrarySettingsDialog()
{
}

//-----------------------------------------------------------------------------
// Function: validate()
//-----------------------------------------------------------------------------
bool LibrarySettingsDialog::validate()
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: apply()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::apply()
{
}

//-----------------------------------------------------------------------------
// Function: addLocation()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::addLocation()
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
void LibrarySettingsDialog::removeLocation()
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
void LibrarySettingsDialog::loadSettings()
{
    QString defaultLocation = settings_.value("Library/DefaultLocation", QString()).toString();

    // Load the library locations.
    QStringList locations = settings_.value("Library/Locations", QStringList()).toStringList();

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
void LibrarySettingsDialog::onSelectLocation(QListWidgetItem* cur, QListWidgetItem*)
{
    removeLocationButton_->setEnabled(cur != 0);
}

void LibrarySettingsDialog::onItemClicked( QListWidgetItem* item ) {

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

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::accept()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::accept()
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

    settings_.setValue("Library/Locations", locations);

    // save the default location is one was set
    settings_.setValue("Library/DefaultLocation", defaultLocation);

    if (changed_) {
        emit scanLibrary();
    }

    QDialog::accept();
}
