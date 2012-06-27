//-----------------------------------------------------------------------------
// File: apieditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 8.9.2011
//
// Description:
// API editor for SW Platform components.
//-----------------------------------------------------------------------------

#include "apieditor.h"

#include "ApiCreateDialog.h"

#include <LibraryManager/libraryinterface.h>

#include <models/component.h>
#include <models/businterface.h>
#include <models/busdefinition.h>

#include <QVBoxLayout>

namespace
{
    //! API type names.
    QString const API_TYPE_NAMES[] = 
    {
        "Upper-level (provider)",
        "Lower-level (requester)"
    };
}

//-----------------------------------------------------------------------------
// Function: APIEditor()
//-----------------------------------------------------------------------------
APIEditor::APIEditor(LibraryInterface* libHandler, QSharedPointer<Component> component, 
                     void* dataPointer, QWidget *parent) : ItemEditor(component, parent),
                                                           api_(static_cast<BusInterface*>(dataPointer)),
                                                           nameGroup_(this, tr("Name and description")),
                                                           attributes_(this),
                                                           busType_(VLNV::BUSDEFINITION, libHandler, this, this),
                                                           apiTypeGroup_(tr("API type"), this),
                                                           libHandler_(libHandler)
{
    Q_ASSERT_X(component, "APIEditor constructor",
        "Null Component-pointer given as parameter");
    Q_ASSERT_X(dataPointer, "APIEditor constructor",
        "Null dataPointer given as parameter");

    QVBoxLayout* typeLayout = new QVBoxLayout(&apiTypeGroup_);

    for (unsigned int i = 0; i < API_TYPE_COUNT; ++i)
    {
        apiTypeButtons_[i] = new QRadioButton(tr(API_TYPE_NAMES[i].toStdString().c_str()), &apiTypeGroup_);
        typeLayout->addWidget(apiTypeButtons_[i]);
    }

    apiTypeButtons_[0]->setChecked(true);

    connect(&nameGroup_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&nameGroup_, SIGNAL(nameChanged(const QString&)),
        this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
    connect(&attributes_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&busType_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(apiTypeButtons_[API_TYPE_UPPER], SIGNAL(clicked(bool)), this, SIGNAL(contentChanged()));
    connect(apiTypeButtons_[API_TYPE_LOWER], SIGNAL(clicked(bool)), this, SIGNAL(contentChanged()));
    
    busType_.setTitle(tr("API definition"));
    
    // create the layout of the general tab
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(&nameGroup_, 0, 0, 1, 1);
    layout->addWidget(&attributes_, 0, 1, 3, 1);
    layout->addWidget(&busType_, 1, 0, 1, 1);
    layout->addWidget(&apiTypeGroup_, 2, 0, 1, 1);
    layout->setRowStretch(3, 1);

    restoreChanges();
}

//-----------------------------------------------------------------------------
// Function: APIEditor()
//-----------------------------------------------------------------------------
APIEditor::~APIEditor()
{
}

//-----------------------------------------------------------------------------
// Function: isValid()
//-----------------------------------------------------------------------------
bool APIEditor::isValid() const
{
    if (!nameGroup_.isValid())
        return false;
    else if (!attributes_.isValid())
        return false;
    else if (!busType_.isValid())
        return false;
    // if specified bus type does not exist
    else if (!libHandler_->contains(busType_.getVLNV()))
        return false;

    // all was fine
    return true;
}

//-----------------------------------------------------------------------------
// Function: removeModel()
//-----------------------------------------------------------------------------
void APIEditor::removeModel()
{
    component()->removeBusInterface(api_);
}

//-----------------------------------------------------------------------------
// Function: makeChanges()
//-----------------------------------------------------------------------------
void APIEditor::makeChanges()
{
    api_->setName(nameGroup_.getName());
    api_->setDisplayName(nameGroup_.getDisplayName());
    api_->setDescription(nameGroup_.getDescription());
    api_->setAttributes(attributes_.getAttributes());
    api_->setBusType(busType_.getVLNV());

    if (apiTypeButtons_[API_TYPE_UPPER]->isChecked())
    {
        api_->setInterfaceMode(General::MASTER);
    }
    else
    {
        api_->setInterfaceMode(General::SLAVE);
    }
}

//-----------------------------------------------------------------------------
// Function: restoreChanges()
//-----------------------------------------------------------------------------
void APIEditor::restoreChanges()
{
    nameGroup_.setName(api_->getName());
    nameGroup_.setDisplayName(api_->getDisplayName());
    nameGroup_.setDescription(api_->getDescription());

    attributes_.setAttributes(api_->getAttributes());

    if (api_->getBusType().isValid())
        busType_.setVLNV(api_->getBusType());

    if (api_->getInterfaceMode() == General::MASTER)
    {
        apiTypeButtons_[API_TYPE_UPPER]->setChecked(true);
    }
    else
    {
        apiTypeButtons_[API_TYPE_LOWER]->setChecked(true);
    }
}

//-----------------------------------------------------------------------------
// Function: confirmEditorChange()
//-----------------------------------------------------------------------------
bool APIEditor::confirmEditorChange()
{
    // Check if the API definition is valid but does not exist in the library.
    VLNV vlnv = busType_.getVLNV();

    if (vlnv.isValid() && !libHandler_->contains(vlnv))
    {
        // Ask the user if he wants to create a new API definition.
        ApiCreateDialog dlg(vlnv, this);

        if (dlg.exec() == QDialog::Rejected)
        {
            return false;
        }

        // Create a new API definition.
        QSharedPointer<BusDefinition> apiDef(new BusDefinition());
        apiDef->setVlnv(vlnv);
        apiDef->setType(KactusAttribute::KTS_BUSDEF_API);

        libHandler_->writeModelToFile(dlg.getDirectory(), apiDef);
    }

    return true;
}
