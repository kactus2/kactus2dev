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
pathLabel_(this)
{
    setupLayout(compact);
    setVLNV(vlnv);
}

//-----------------------------------------------------------------------------
// Function: vlnvdisplayer::setupLayout()
//-----------------------------------------------------------------------------
void VLNVDisplayer::setupLayout(bool compact)
{
    pathLabel_.hide();

    // Show the labels for non-compact layout.
    vendorLabel_.setVisible(!compact);
    libraryLabel_.setVisible(!compact);
    nameLabel_.setVisible(!compact);
    versionLabel_.setVisible(!compact);

    if (compact)
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

        QHBoxLayout* compactLayout = new QHBoxLayout(this);
        compactLayout->setSpacing(1);
        compactLayout->addWidget(&vendor_);
        compactLayout->addWidget(firstLine);
        compactLayout->addWidget(&library_);
        compactLayout->addWidget(secondLine);
        compactLayout->addWidget(&name_);
        compactLayout->addWidget(thirdLine);
        compactLayout->addWidget(&version_);
        compactLayout->addStretch(1);
    }
    else
    {
        QFormLayout* layout_ = new QFormLayout(this);
        layout_->addRow(&vendorLabel_, &vendor_);
        layout_->addRow(&libraryLabel_, &library_);
        layout_->addRow(&nameLabel_, &name_);
        layout_->addRow(&versionLabel_, &version_);
    }
}

//-----------------------------------------------------------------------------
// Function: vlnvdisplayer::setVLNV()
//-----------------------------------------------------------------------------
void VLNVDisplayer::setVLNV(const VLNV& vlnv)
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


//     if (compact_)
//     {
//         // Set the compact layout.
//         setLayout(compactLayout_);
//     }
//     else
//     {
//         vendor_.setMinimumWidth(vendor_.sizeHint().width());
//         library_.setMinimumWidth(library_.sizeHint().width());
//         
//         // Set the non-compact layout.
//         setLayout(layout_);
//     }
}

//-----------------------------------------------------------------------------
// Function: vlnvdisplayer::setPath()
//-----------------------------------------------------------------------------
void VLNVDisplayer::setPath(QString const& path)
{
    pathLabel_.setText(path);
    pathLabel_.setToolTip(path);

    pathLabel_.setVisible(path.isEmpty() == false);

    if (auto activeLayout = dynamic_cast<QFormLayout*>(layout()); 
        activeLayout)
    {
        pathLabel_.setWordWrap(true);
        pathLabel_.setMinimumWidth(20);

        activeLayout->addRow(QString("Path:"), &pathLabel_);
    }
}
