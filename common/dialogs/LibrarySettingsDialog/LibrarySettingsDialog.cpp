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
#include "librarysettingsdelegate.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QDialogButtonBox>
#include <QStringList>
#include <QHeaderView>
#include <QCheckBox>

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::LibrarySettingsDialog()
//-----------------------------------------------------------------------------
LibrarySettingsDialog::LibrarySettingsDialog(QSettings& settings, QWidget* parent)
    : QDialog(parent),
      settings_(settings),
	  libLocationsTable_(0),
      addLocationButton_(new QPushButton(QIcon(":/icons/common/graphics/add.png"), QString(), this)),
      removeLocationButton_(new QPushButton(QIcon(":/icons/common/graphics/remove.png"), QString(), this)),
      changed_(false)
{
    setWindowTitle(tr("Configure Library"));

    // Create the library location group box.
    QGroupBox* locationGroup = new QGroupBox(tr("Library locations (check the default directory)"), this);

	libLocationsTable_ = new QTableWidget(0, 3, locationGroup);

	// the headers for the table
	QStringList headers;
	headers.append(tr("Default"));
	headers.append(tr("Active"));
	headers.append(tr("Library path"));
	libLocationsTable_->setHorizontalHeaderLabels(headers);

    // Cells are resized to match contents.
    libLocationsTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    libLocationsTable_->horizontalHeader()->setStretchLastSection(true);
    libLocationsTable_->setAlternatingRowColors(true);
    libLocationsTable_->verticalHeader()->hide();

	// Set the height of a row to be smaller than default.
	libLocationsTable_->verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 8);

	libLocationsTable_->setMinimumWidth(500);
    libLocationsTable_->setColumnWidth(LibrarySettingsDelegate::DEF_COLUMN, 50);
    libLocationsTable_->setColumnWidth(LibrarySettingsDelegate::ACTIVE_COL, 50);

	libLocationsTable_->setItemDelegate(new LibrarySettingsDelegate(this));

    removeLocationButton_->setEnabled(false);

    QDialogButtonBox* listButtonBox = new QDialogButtonBox(Qt::Vertical);
    listButtonBox->addButton(addLocationButton_, QDialogButtonBox::ActionRole);
    listButtonBox->addButton(removeLocationButton_, QDialogButtonBox::ActionRole);

    QHBoxLayout* locationLayout = new QHBoxLayout(locationGroup);
    locationLayout->addWidget(libLocationsTable_);
    locationLayout->addWidget(listButtonBox);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, this);
    // Setup the layout.
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(locationGroup, 1);
    layout->addWidget(buttonBox);

    resize(500, sizeHint().height());

    connect(libLocationsTable_, SIGNAL(itemClicked(QTableWidgetItem*)),
        this, SLOT(onItemClicked(QTableWidgetItem*)), Qt::UniqueConnection);
    connect(libLocationsTable_, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)),
        this, SLOT(onSelectLocation(QTableWidgetItem*, QTableWidgetItem*)), Qt::UniqueConnection);
    connect(addLocationButton_, SIGNAL(clicked()), this, SLOT(addLocation()));
    connect(removeLocationButton_, SIGNAL(clicked()), this, SLOT(removeLocation()));
    connect(buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), 
        this, SLOT(accept()), Qt::UniqueConnection);
    connect(buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), 
        this, SLOT(reject()), Qt::UniqueConnection);

    loadSettings();
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::~LibrarySettingsDialog()
//-----------------------------------------------------------------------------
LibrarySettingsDialog::~LibrarySettingsDialog()
{
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::validate()
//-----------------------------------------------------------------------------
bool LibrarySettingsDialog::validate()
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::apply()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::apply()
{
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::addLocation()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::addLocation()
{
    QString directory = QFileDialog::getExistingDirectory(this, tr("Select library location:"));

    if (!directory.isEmpty())
    {
        bool isDefault = libLocationsTable_->rowCount() == 0;
        createRowForDirectory(directory, true, isDefault);
        changed_ = true;
    }
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::removeLocation()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::removeLocation()
{
	int row = libLocationsTable_->currentRow();

	if (row >= 0)
    {
		QTableWidgetItem* defItem = libLocationsTable_->takeItem(row, LibrarySettingsDelegate::DEF_COLUMN);
		delete defItem;

		QTableWidgetItem* activeItem = libLocationsTable_->takeItem(row, LibrarySettingsDelegate::ACTIVE_COL);
		delete activeItem;

		QTableWidgetItem* pathItem = libLocationsTable_->takeItem(row, LibrarySettingsDelegate::PATH_COL);
		delete pathItem;

		libLocationsTable_->removeRow(row);
	}

    changed_ = true;
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::onSelectLocation()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::onSelectLocation(QTableWidgetItem* cur, QTableWidgetItem*)
{
    removeLocationButton_->setEnabled(cur != 0);
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::onItemClicked()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::onItemClicked(QTableWidgetItem* item)
{
	// If the item is the check box to select the default library.
	if (item->column() == LibrarySettingsDelegate::DEF_COLUMN)
	{
		changed_ = true;

		// Default library should be active.
		QTableWidgetItem* activeItem = libLocationsTable_->item(item->row(), LibrarySettingsDelegate::ACTIVE_COL);
		activeItem->setCheckState(Qt::Checked);

		// Uncheck other items because only one library can be default.
		for (int i = 0; i < libLocationsTable_->rowCount(); ++i)
		{
			QTableWidgetItem* temp = libLocationsTable_->item(i, LibrarySettingsDelegate::DEF_COLUMN);

			// If the item is not the clicked item.
			if (temp != item)
			{
				temp->setCheckState(Qt::Unchecked);
			}
		}

        // Do not allow unchecking.
        if (item->checkState() != Qt::Checked)
		{
            item->setCheckState(Qt::Checked);
        }           
    }
	// If the item is the check box to select the active libraries.
	else if (item->column() == LibrarySettingsDelegate::ACTIVE_COL)
	{
		changed_ = true;
	}
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::accept()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::accept()
{
    // Create a string list containing all the locations and save it to the settings.
    QStringList locations;

	// the active locations used to search for IP-XACT objects
	QStringList activeLocations;

    // the checked item in the list is the default location
    QString defaultLocation;

	for (int i = 0; i < libLocationsTable_->rowCount(); ++i)
    {
		QTableWidgetItem* defItem = libLocationsTable_->item(i, LibrarySettingsDelegate::DEF_COLUMN);
		QTableWidgetItem* activeItem = libLocationsTable_->item(i, LibrarySettingsDelegate::ACTIVE_COL);
		QTableWidgetItem* pathItem = libLocationsTable_->item(i, LibrarySettingsDelegate::PATH_COL);
		
		// if the row contains the default library
		if (defItem->checkState()  == Qt::Checked)
        {
			defaultLocation = pathItem->text();
		}

		if (activeItem->checkState() == Qt::Checked)
        {
			activeLocations.append(pathItem->text());
		}

		// add the library path to the known library locations
		locations.append(pathItem->text());
	}

    settings_.setValue("Library/Locations", locations);

    // save the default location is one was set
    settings_.setValue("Library/DefaultLocation", defaultLocation);

	// the active locations are saved
	settings_.setValue("Library/ActiveLocations", activeLocations);

    if (changed_)
    {
        emit scanLibrary();
    }

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::loadSettings()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::loadSettings()
{
    // Load the library locations.
    QString defaultLocation = settings_.value("Library/DefaultLocation", QString()).toString();

    QStringList activeLocations = settings_.value("Library/ActiveLocations", QStringList()).toStringList();

    QStringList locations = settings_.value("Library/Locations", QStringList()).toStringList();
    foreach (QString const& location, locations)
    {
        QString fullLocation = location;

        if (!QFileInfo(location).isAbsolute())
        {
            fullLocation = QFileInfo(location).absoluteFilePath();
        }

        createRowForDirectory(fullLocation, activeLocations.contains(location), location == defaultLocation);
    }

    libLocationsTable_->setCurrentIndex(QModelIndex());

    changed_ = false;
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::createRowForDirectory()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::createRowForDirectory(QString const& directory, bool isActive, bool isDefault)
{
    int rowNumber = libLocationsTable_->rowCount();

    libLocationsTable_->insertRow(rowNumber);

    // Create the item for default column.
     QTableWidgetItem* defaultItem = new QTableWidgetItem();
    defaultItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

    if (isDefault)
    {        
        defaultItem->setCheckState(Qt::Checked);
    }
    else
    {
        defaultItem->setCheckState(Qt::Unchecked);
    }

    libLocationsTable_->setItem(rowNumber, LibrarySettingsDelegate::DEF_COLUMN, defaultItem);

    // create the item for the active column
    QTableWidgetItem* activeItem = new QTableWidgetItem();
    activeItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);

    if (isActive)
    {
        activeItem->setCheckState(Qt::Checked);
    }
    else
    {
        activeItem->setCheckState(Qt::Unchecked);
    }

    libLocationsTable_->setItem(rowNumber, LibrarySettingsDelegate::ACTIVE_COL, activeItem);

    QTableWidgetItem* pathItem = new QTableWidgetItem(directory);
    pathItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    libLocationsTable_->setItem(rowNumber, LibrarySettingsDelegate::PATH_COL, pathItem);
}