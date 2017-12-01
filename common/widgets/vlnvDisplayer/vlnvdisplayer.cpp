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
compactLayout_(new QHBoxLayout()),
pathLabel_(new QLabel(this))
{
    setupLayout();
	setVLNV(vlnv, compact);
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
compactLayout_(new QHBoxLayout()),
pathLabel_(new QLabel(this))
{
    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: vlnvdisplayer::~VLNVDisplayer()
//-----------------------------------------------------------------------------
VLNVDisplayer::~VLNVDisplayer()
{

}

//-----------------------------------------------------------------------------
// Function: vlnvdisplayer::setupLayout()
//-----------------------------------------------------------------------------
void VLNVDisplayer::setupLayout()
{
    pathLabel_->hide();

    layout_->addRow(&vendorLabel_, &vendor_);
    layout_->addRow(&libraryLabel_, &library_);
    layout_->addRow(&nameLabel_, &name_);
    layout_->addRow(&versionLabel_, &version_);

    setupCompactLayout();
}

//-----------------------------------------------------------------------------
// Function: vlnvdisplayer::setupCompactLayout()
//-----------------------------------------------------------------------------
void VLNVDisplayer::setupCompactLayout()
{
    QFrame* firstLine = new QFrame();
    firstLine->setFrameShape(QFrame::VLine);
    firstLine->setLineWidth(1);
    QFrame* secondLine = new QFrame();
    secondLine->setFrameShape(QFrame::VLine);
    secondLine->setLineWidth(1);
    QFrame* thirdLine = new QFrame();
    thirdLine->setFrameShape(QFrame::VLine);
    thirdLine->setLineWidth(1);

    compactLayout_->setSpacing(1);
    compactLayout_->addWidget(&vendor_);
    compactLayout_->addWidget(firstLine);
    compactLayout_->addWidget(&library_);
    compactLayout_->addWidget(secondLine);
    compactLayout_->addWidget(&name_);
    compactLayout_->addWidget(thirdLine);
    compactLayout_->addWidget(&version_);
    compactLayout_->addStretch(1);
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
            vendor_.setText("unassigned");
        }
        if (vlnv.getLibrary().isEmpty())
        {
            library_.setText("unassigned");
        }
        if (vlnv.getName().isEmpty())
        {
            name_.setText("unassigned");
        }
        if (vlnv.getVersion().isEmpty())
        {
            version_.setText("unassigned");
        }
    }

    // Show the labels for non-compact layout.
    vendorLabel_.setVisible(!compact);
    libraryLabel_.setVisible(!compact);
    nameLabel_.setVisible(!compact);
    versionLabel_.setVisible(!compact);

    if (compact)
    {
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
    pathLabel_->setText(path);
    pathLabel_->setToolTip(path);

    if (path.isEmpty() && pathLabel_->isVisible())
    {
        pathLabel_->hide();
    }
    if (!pathLabel_->isVisible())
    {
        pathLabel_->setWordWrap(true);
        pathLabel_->setMinimumWidth(20);

        pathLabel_->show();
        layout_->addRow(QString("Path:"), pathLabel_);
    }
}
