//-----------------------------------------------------------------------------
// File: SoftwareMappingsEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 31.10.2011
//
// Description:
// Editor for editing software mappings for a specific HW component.
//-----------------------------------------------------------------------------

#include "SoftwareMappingsEditor.h"

#include <models/design.h>

#include <LibraryManager/libraryinterface.h>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: SoftwareMappingsEditor()
//-----------------------------------------------------------------------------
SoftwareMappingsEditor::SoftwareMappingsEditor(LibraryInterface* libHandler,
                                               QSharedPointer<Component> component,
											   QWidget *parent,
											   QWidget* parentWnd):
ItemEditor(component, parent), 
libInterface_(libHandler), component_(component),
swDesignMapping_(VLNV::COMPONENT, libHandler, parentWnd, this)
{
    swDesignMapping_.setTitle(tr("Mapped SW Design"));
    swDesignMapping_.setImplementationFilter(true, KactusAttribute::KTS_SW);
    swDesignMapping_.updateFiltering();

	// the mapping is not mandatory element
	swDesignMapping_.setMandatory(false);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&swDesignMapping_);
    layout->addStretch(1);
    
    // Connect signals.
    connect(&swDesignMapping_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()));

    // Read the existing SW design if found.
    VLNV designVLNV = *component_->getVlnv();
    designVLNV.setType(VLNV::DESIGN);
    designVLNV.setName(designVLNV.getName().remove(".comp") + "_swmap.design");

    if (libInterface_->contains(designVLNV))
    {
        QSharedPointer<LibraryComponent> libComp = libInterface_->getModel(designVLNV);
        QSharedPointer<Design> design = libComp.staticCast<Design>();

        if (design->getComponentInstances().count() > 0)
        {
            Design::ComponentInstance const& instance =design->getComponentInstances().first();
            swDesignMapping_.setVLNV(instance.componentRef);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ~SoftwareMappingsEditor()
//-----------------------------------------------------------------------------
SoftwareMappingsEditor::~SoftwareMappingsEditor()
{
}

//-----------------------------------------------------------------------------
// Function: isValid()
//-----------------------------------------------------------------------------
bool SoftwareMappingsEditor::isValid() const
{
    // The mapped platform must be either totally empty or valid.
    if (!swDesignMapping_.isEmpty() && !swDesignMapping_.isValid())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: makeChanges()
//-----------------------------------------------------------------------------
void SoftwareMappingsEditor::makeChanges()
{
    VLNV designVLNV = *component_->getVlnv();
    designVLNV.setType(VLNV::DESIGN);
    designVLNV.setName(designVLNV.getName().remove(".comp") + "_swmap.design");

    // Create the SW design if there is something to save.
    if (swDesignMapping_.isValid())
    {
        // Create the view for the component if it does not exist.
        if (component_->findView("kts_sw_ref") == 0)
        {
            View* view = new View("kts_sw_ref");
            view->addEnvIdentifier("");
            view->setHierarchyRef(designVLNV);
            component_->addView(view);
        }

        QSharedPointer<Design> design(new Design(designVLNV));

        QList<Design::ComponentInstance> instances;
        Design::ComponentInstance instance(swDesignMapping_.getVLNV().getName().remove(".comp"),
                                           QString(), QString(), swDesignMapping_.getVLNV(), QPointF());
        instances.append(instance);

        design->setComponentInstances(instances);

        if (libInterface_->contains(designVLNV))
        {
            libInterface_->writeModelToFile(design);
        }
        else
        {
            QFileInfo fileInfo(libInterface_->getPath(*component_->getVlnv()));
            libInterface_->writeModelToFile(fileInfo.path(), design);
        }
    }
    else
    {
        if (libInterface_->contains(designVLNV))
        {
            libInterface_->removeObject(designVLNV);
        }

        if (component_->hasView("kts_sw_ref"))
        {
            component_->removeView("kts_sw_ref");
        }
    }
}