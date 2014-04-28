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
#include <IPXACTmodels/generaldeclarations.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDir>
#include <QGroupBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QCoreApplication>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QStringList>
#include <QHeaderView>
#include <QFontMetrics>

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog()
//-----------------------------------------------------------------------------
LibrarySettingsDialog::LibrarySettingsDialog(QSettings& settings, QWidget* parent)
    : QDialog(parent),
      settings_(settings),
	  libLocationsTable_(0),
      addLocationButton_(0),
      removeLocationButton_(0),
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

	// cells are resized to match contents 
	libLocationsTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

	// vertical headers are not visible
	libLocationsTable_->verticalHeader()->hide();

	// set the height of a row to be smaller than default
	libLocationsTable_->verticalHeader()->setDefaultSectionSize(fontMetrics().height() + 8);

	// easies to see the different rows from one another
	libLocationsTable_->setAlternatingRowColors(true);

	libLocationsTable_->setColumnWidth(LibrarySettingsDelegate::DEF_COLUMN, 50);
	libLocationsTable_->setColumnWidth(LibrarySettingsDelegate::ACTIVE_COL, 50);
	//last column is stretched take the available space in the widget
	libLocationsTable_->horizontalHeader()->setStretchLastSection(true);
	libLocationsTable_->setMinimumWidth(500);

	libLocationsTable_->setItemDelegate(new LibrarySettingsDelegate(this));

	connect(libLocationsTable_, SIGNAL(itemClicked(QTableWidgetItem*)),
		this, SLOT(onItemClicked(QTableWidgetItem*)), Qt::UniqueConnection);
	connect(libLocationsTable_, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)),
		this, SLOT(onSelectLocation(QTableWidgetItem*, QTableWidgetItem*)), Qt::UniqueConnection);

    addLocationButton_ = new QPushButton(QIcon(":/icons/common/graphics/add.png"), QString(), this);
    removeLocationButton_ = new QPushButton(QIcon(":/icons/common/graphics/remove.png"), QString(), this);
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
    layout->addWidget(locationGroup);
    layout->addStretch(1);
    layout->addWidget(buttonBox);

    // Connect the signals.
    connect(addLocationButton_, SIGNAL(clicked()), this, SLOT(addLocation()));
    connect(removeLocationButton_, SIGNAL(clicked()), this, SLOT(removeLocation()));
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
void LibrarySettingsDialog::apply() {
}

//-----------------------------------------------------------------------------
// Function: addLocation()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::addLocation() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select library location:"));

    if (!dir.isEmpty()) {

		int rows = libLocationsTable_->rowCount();

		libLocationsTable_->insertRow(rows);

		// create the item for default column
		QTableWidgetItem* defCheckItem = new QTableWidgetItem();
		
		// if this is the only row on the list
		if (rows == 0) {
			defCheckItem->setCheckState(Qt::Checked);
		}
		else {
			defCheckItem->setCheckState(Qt::Unchecked);
		}
		libLocationsTable_->setItem(rows, LibrarySettingsDelegate::DEF_COLUMN, defCheckItem);

		// create the item for the active column
		QTableWidgetItem* activeItem = new QTableWidgetItem();
		
		// new directory is active by default
		activeItem->setCheckState(Qt::Checked);

		libLocationsTable_->setItem(rows, LibrarySettingsDelegate::ACTIVE_COL, activeItem);

		QTableWidgetItem* pathItem = new QTableWidgetItem(dir);
		libLocationsTable_->setItem(rows, LibrarySettingsDelegate::PATH_COL, pathItem);
    }

    changed_ = true;
}

//-----------------------------------------------------------------------------
// Function: removeLocation()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::removeLocation() {

	int row = libLocationsTable_->currentRow();

	if (row >= 0) {
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
// Function: loadSettings()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::loadSettings()
{
    QString defaultLocation = settings_.value("Library/DefaultLocation", QString()).toString();

    // Load the library locations.
    QStringList locations = settings_.value("Library/Locations", QStringList()).toStringList();

	QStringList activeLocations = settings_.value("Library/ActiveLocations", QStringList()).toStringList();

    for (int i = 0; i < locations.size(); ++i) {
		QString location = locations.at(i);
        QString fullLocation = location;

        if (!QFileInfo(location).isAbsolute()) {
            fullLocation = QFileInfo(location).absoluteFilePath();
        }

		libLocationsTable_->insertRow(i);

		// mark the default library
		QTableWidgetItem* defItem = new QTableWidgetItem();
		if (location == defaultLocation) {
			defItem->setCheckState(Qt::Checked);
		}
		else {
			defItem->setCheckState(Qt::Unchecked);
		}
		defItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		libLocationsTable_->setItem(i, LibrarySettingsDelegate::DEF_COLUMN, defItem);

		QTableWidgetItem* activeItem = new QTableWidgetItem();
		// if the location is set as active
		if (activeLocations.contains(location)) {
			activeItem->setCheckState(Qt::Checked);
		}
		// if location is inactive
		else {
			activeItem->setCheckState(Qt::Unchecked);
		}
		activeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		libLocationsTable_->setItem(i, LibrarySettingsDelegate::ACTIVE_COL, activeItem);

		QTableWidgetItem* pathItem = new QTableWidgetItem(fullLocation);
		pathItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		libLocationsTable_->setItem(i, LibrarySettingsDelegate::PATH_COL, pathItem);
    }

	libLocationsTable_->setCurrentIndex(QModelIndex());

	changed_ = false;
}

void LibrarySettingsDialog::onSelectLocation( QTableWidgetItem* cur, QTableWidgetItem*) {
	removeLocationButton_->setEnabled(cur != 0);
}

void LibrarySettingsDialog::onItemClicked( QTableWidgetItem* item ) {
	
	// if the item is the check box to select the default library
	if (item->column() == LibrarySettingsDelegate::DEF_COLUMN) {

		changed_ = true;

		// if item was checked then uncheck other items because only one library can be default
		if (item->checkState() == Qt::Checked) {

			// default library should be active
			QTableWidgetItem* activeItem = libLocationsTable_->item(item->row(), LibrarySettingsDelegate::ACTIVE_COL);
			activeItem->setCheckState(Qt::Checked);

			for (int i = 0; i < libLocationsTable_->rowCount(); ++i) {
				QTableWidgetItem* temp = libLocationsTable_->item(i, LibrarySettingsDelegate::DEF_COLUMN);

				// if the item is not the clicked item
				if (temp != item) {
					temp->setCheckState(Qt::Unchecked);
				}
			}
        }
        // Do not allow unchecking
        else
        {
            item->setCheckState(Qt::Checked);
        }           
    }

	// if the item is the check box to select the active libraries
	else if (item->column() == LibrarySettingsDelegate::ACTIVE_COL) {
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

	for (int i = 0; i < libLocationsTable_->rowCount(); ++i) {
		QTableWidgetItem* defItem = libLocationsTable_->item(i, LibrarySettingsDelegate::DEF_COLUMN);
		QTableWidgetItem* activeItem = libLocationsTable_->item(i, LibrarySettingsDelegate::ACTIVE_COL);
		QTableWidgetItem* pathItem = libLocationsTable_->item(i, LibrarySettingsDelegate::PATH_COL);
		
		// if the row contains the default library
		if (defItem->checkState() == Qt::Checked) {
			defaultLocation = pathItem->text();
		}

		if (activeItem->checkState() == Qt::Checked) {
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

    if (changed_) {
        emit scanLibrary();
    }

    QDialog::accept();
}
