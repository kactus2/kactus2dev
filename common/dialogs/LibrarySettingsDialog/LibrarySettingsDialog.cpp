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

#include <common/dialogs/LibrarySettingsDialog/LibrarySettingsColumns.h>

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QStringList>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::LibrarySettingsDialog()
//-----------------------------------------------------------------------------
LibrarySettingsDialog::LibrarySettingsDialog(QSettings& settings, QWidget* parent):
QDialog(parent),
settings_(settings),
iconProvider_(),
libLocationsTable_(0),
addLocationButton_(new QPushButton(QIcon(":/icons/common/graphics/add.png"), QString(), this)),
removeLocationButton_(new QPushButton(QIcon(":/icons/common/graphics/remove.png"), QString(), this)),
changed_(false),
checkMarkIcon_(":/icons/common/graphics/checkMark.png"),
locations_(),
activeLocations_(),
defaultLocation_()
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Configure Library"));

    // Create the library location group box.
    QGroupBox* locationGroup = new QGroupBox(tr("Library locations"), this);

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
    libLocationsTable_->setColumnWidth(LibrarySettingsColumns::DEFAULT, 50);
    libLocationsTable_->setColumnWidth(LibrarySettingsColumns::ACTIVE, 50);

    libLocationsTable_->setItemDelegate(new LibrarySettingsDelegate(this));

    removeLocationButton_->setEnabled(false);

    QWidget* introWidget = new QWidget(this);
    QHBoxLayout* introLayout = new QHBoxLayout(introWidget);
    introLayout->addWidget(new QLabel(tr("<b>Set library locations on disk</b><br>"
        "Active directories and their sub-directories will be searched for IP-XACT files and "
        "read into the library."), this));

    QPalette introPalette = introWidget->palette();
    introPalette.setColor(QPalette::Background, Qt::white);
    introWidget->setPalette(introPalette);
    introWidget->setAutoFillBackground(true);

    QDialogButtonBox* listButtonBox = new QDialogButtonBox(Qt::Vertical);
    listButtonBox->addButton(addLocationButton_, QDialogButtonBox::ActionRole);
    listButtonBox->addButton(removeLocationButton_, QDialogButtonBox::ActionRole);

    QHBoxLayout* locationLayout = new QHBoxLayout(locationGroup);
    locationLayout->addWidget(libLocationsTable_);
    locationLayout->addWidget(listButtonBox);

    QGroupBox* separator = new QGroupBox(this);
    separator->setFlat(true);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                       Qt::Horizontal, this);
    // Setup the layout.
    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(introWidget);
    topLayout->addWidget(locationGroup, 1);
    topLayout->addWidget(separator);
    topLayout->addWidget(buttonBox);

    resize(700, sizeHint().height() + 50);

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
// Function: LibrarySettingsDialog::getLocations()
//-----------------------------------------------------------------------------
QStringList LibrarySettingsDialog::getLocations() const
{
    return locations_;
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::getActiveLocations()
//-----------------------------------------------------------------------------
QStringList LibrarySettingsDialog::getActiveLocations() const
{
    return activeLocations_;
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::getDefaultLocation()
//-----------------------------------------------------------------------------
QString LibrarySettingsDialog::getDefaultLocation() const
{
    return defaultLocation_;
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
        QTableWidgetItem* defItem = libLocationsTable_->takeItem(row, LibrarySettingsColumns::DEFAULT);
        delete defItem;

        QTableWidgetItem* activeItem = libLocationsTable_->takeItem(row, LibrarySettingsColumns::ACTIVE);
        delete activeItem;

        QTableWidgetItem* pathItem = libLocationsTable_->takeItem(row, LibrarySettingsColumns::PATH);
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
    if (item->column() == LibrarySettingsColumns::DEFAULT || item->column() == LibrarySettingsColumns::ACTIVE)
    {
        changed_ = true;
        if (item->column() == LibrarySettingsColumns::DEFAULT)
        {
            changeDefaultPath(item);
        }
        else if (item->column() == LibrarySettingsColumns::ACTIVE)
        {
            changeActivePathStatus(item);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::changeDefaultPath()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::changeDefaultPath(QTableWidgetItem* item)
{
    if (!item->icon().isNull())
    {
        return;
    }

    item->setIcon(checkMarkIcon_);
    for (int index = 0; index < libLocationsTable_->rowCount(); ++index)
    {
        QTableWidgetItem* tableItem = libLocationsTable_->item(index, LibrarySettingsColumns::DEFAULT);
        if (tableItem != item)
        {
            if (!tableItem->icon().isNull())
            {
                tableItem->setIcon(QIcon());
            }
        }
        else
        {
            QTableWidgetItem* activeTableItem = libLocationsTable_->item(index, LibrarySettingsColumns::ACTIVE);
            if (activeTableItem && activeTableItem->icon().isNull())
            {
                activeTableItem->setIcon(checkMarkIcon_);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::changeActivePathStatus()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::changeActivePathStatus(QTableWidgetItem* item)
{
    if (!item->icon().isNull())
    {
        QTableWidgetItem* defaultItem = libLocationsTable_->item(item->row(), LibrarySettingsColumns::DEFAULT);
        if (defaultItem && defaultItem->icon().isNull())
        {
            item->setIcon(QIcon());
        }
    }
    else
    {
        item->setIcon(checkMarkIcon_);
    }
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::accept()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::accept()
{
    for (int i = 0; i < libLocationsTable_->rowCount(); ++i)
    {
        QTableWidgetItem* defItem = libLocationsTable_->item(i, LibrarySettingsColumns::DEFAULT);
        QTableWidgetItem* activeItem = libLocationsTable_->item(i, LibrarySettingsColumns::ACTIVE);
        QTableWidgetItem* pathItem = libLocationsTable_->item(i, LibrarySettingsColumns::PATH);
        
        // if the row contains the default library
        if (!defItem->icon().isNull())
        {
            defaultLocation_ = pathItem->text();
        }
        if (!activeItem->icon().isNull())
        {
            activeLocations_.append(pathItem->text());
        }

        // add the library path to the known library locations
        locations_.append(pathItem->text());
    }

    QDialog::accept();
}

//-----------------------------------------------------------------------------
// Function: LibrarySettingsDialog::loadSettings()
//-----------------------------------------------------------------------------
void LibrarySettingsDialog::loadSettings()
{
    // Load the library locations.
    defaultLocation_ = settings_.value(QStringLiteral("Library/DefaultLocation")).toString();
    activeLocations_ = settings_.value(QStringLiteral("Library/ActiveLocations")).toStringList();
    locations_ = settings_.value(QStringLiteral("Library/Locations")).toStringList();

    for (QString const& location : locations_)
    {
        QString fullLocation = location;

        if (QFileInfo(location).isRelative())
        {
            fullLocation = QFileInfo(location).absoluteFilePath();
        }

        bool isDefaultLocation = location.compare(defaultLocation_) == 0;
        createRowForDirectory(fullLocation, activeLocations_.contains(location), isDefaultLocation);
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
    defaultItem->setFlags(Qt::ItemIsEnabled);

    if (isDefault)
    {        
        defaultItem->setIcon(checkMarkIcon_);
    }

    libLocationsTable_->setItem(rowNumber, LibrarySettingsColumns::DEFAULT, defaultItem);

    // create the item for the active column
    QTableWidgetItem* activeItem = new QTableWidgetItem();
    activeItem->setFlags(Qt::ItemIsEnabled);

    if (isActive)
    {
        activeItem->setIcon(checkMarkIcon_);
    }

    libLocationsTable_->setItem(rowNumber, LibrarySettingsColumns::ACTIVE, activeItem);

    QTableWidgetItem* pathItem = new QTableWidgetItem(directory);
    pathItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QFileInfo directoryInfo(directory);
    if (directoryInfo.exists())
    {
         pathItem->setIcon(iconProvider_.icon(directoryInfo));
    }
    else
    {
        pathItem->setIcon(QIcon(QStringLiteral(":/icons/common/graphics/exclamation--frame.png")));
    }

    libLocationsTable_->setItem(rowNumber, LibrarySettingsColumns::PATH, pathItem);
}
