//-----------------------------------------------------------------------------
// File: vlnvdisplayer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.03.2011
//
// Description:
// VLNVDisplayer is a widget to display a vlnv tag in a GUI item.
//-----------------------------------------------------------------------------

#include "vlnvdisplayer.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QSizePolicy>

//-----------------------------------------------------------------------------
// Function: vlnvdisplayer::VLNVDisplayer()
//-----------------------------------------------------------------------------
VLNVDisplayer::VLNVDisplayer(QWidget* parent, VLNV const& vlnv, bool compact):
QGroupBox(parent),
    vendorLabel_(tr("Vendor:"), this),
    libraryLabel_(tr("Library:"), this),
    nameLabel_(tr("Name:"), this),
    versionLabel_(tr("Version:"), this),
    vendor_(this), library_(this),
    name_(this),
    version_(this),
    layout_(new QFormLayout()),
    compactLayout_(new QHBoxLayout())
{
	layout_->addRow(&vendorLabel_, &vendor_);
	layout_->addRow(&libraryLabel_, &library_);
	layout_->addRow(&nameLabel_, &name_);
	layout_->addRow(&versionLabel_, &version_);

    compactLayout_->setSpacing(1);
    compactLayout_->addWidget(&vendor_);
    compactLayout_->addWidget(&library_);
    compactLayout_->addWidget(&name_);
    compactLayout_->addWidget(&version_);
    compactLayout_->addStretch(1);

	setVLNV(vlnv, compact);
    setFlat(true);
}

//-----------------------------------------------------------------------------
// Function: vlnvdisplayer::VLNVDisplayer()
//-----------------------------------------------------------------------------
VLNVDisplayer::VLNVDisplayer(QWidget* parent):
QGroupBox(parent),
    vendorLabel_(tr("Vendor:"), this),
    libraryLabel_(tr("Library:"), this),
    nameLabel_(tr("Name:"), this),
    versionLabel_(tr("Version:"), this),
    vendor_(this),
    library_(this),
    name_(this),
    version_(this),
    layout_(new QFormLayout()),
    compactLayout_(new QHBoxLayout())
{
    layout_->addRow(&vendorLabel_, &vendor_);
    layout_->addRow(&libraryLabel_, &library_);
    layout_->addRow(&nameLabel_, &name_);
    layout_->addRow(&versionLabel_, &version_);

    compactLayout_->setSpacing(1);
    compactLayout_->addWidget(&vendor_);
    compactLayout_->addWidget(&library_);
    compactLayout_->addWidget(&name_);
    compactLayout_->addWidget(&version_);
    compactLayout_->addStretch(1);

    setFlat(true);
}

//-----------------------------------------------------------------------------
// Function: vlnvdisplayer::~VLNVDisplayer()
//-----------------------------------------------------------------------------
VLNVDisplayer::~VLNVDisplayer()
{

}

//-----------------------------------------------------------------------------
// Function: vlnvdisplayer::setVLNV()
//-----------------------------------------------------------------------------
void VLNVDisplayer::setVLNV(const VLNV& vlnv, bool compact)
{
    vendor_.setText(vlnv.getVendor());
    library_.setText(vlnv.getLibrary());
    name_.setText(vlnv.getName());
    version_.setText(vlnv.getVersion());

    if (!vlnv.isValid())
    {
        if (vlnv.getVendor().isEmpty())
        {
            vendor_.setText("UNSET");
        }
        if (vlnv.getLibrary().isEmpty())
        {
            library_.setText("UNSET");
        }
        if (vlnv.getName().isEmpty())
        {
            name_.setText("UNSET");
        }
        if (vlnv.getVersion().isEmpty())
        {
            version_.setText("UNSET");
        }
    }

    // Show the labels for non-compact layout.
    vendorLabel_.setVisible(!compact);
    libraryLabel_.setVisible(!compact);
    nameLabel_.setVisible(!compact);
    versionLabel_.setVisible(!compact);

    if (compact)
    {
        vendor_.setStyleSheet("border: 1px solid grey");
        vendor_.setMinimumWidth(20);
        library_.setStyleSheet("border: 1px solid grey");
        library_.setMinimumWidth(20);
        name_.setStyleSheet("border: 1px solid grey");
        version_.setStyleSheet("border: 1px solid grey");

        // Set the compact layout.
        setLayout(compactLayout_);
    }
    else
    {
        vendor_.setMinimumWidth(vendor_.sizeHint().width());
        library_.setMinimumWidth(library_.sizeHint().width());
        
        // Set the non-compact layout.
        setLayout(layout_);
    }
}

//-----------------------------------------------------------------------------
// Function: vlnvdisplayer::setPath()
//-----------------------------------------------------------------------------
void VLNVDisplayer::setPath(QString const& path)
{
	QLabel* pathLabel = new QLabel(path, this);
    pathLabel->setMinimumWidth(20);
    pathLabel->setToolTip(path);
    pathLabel->setWordWrap(true);

    layout_->addRow(tr("Path:"), pathLabel);
}
