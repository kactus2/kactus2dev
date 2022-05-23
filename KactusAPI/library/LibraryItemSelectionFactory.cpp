#include "LibraryItemSelectionFactory.h"

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

//-----------------------------------------------------------------------------
// Function: LibraryItemSelectionFactory::constructItemsForSelectionDialog()
//-----------------------------------------------------------------------------
void LibraryItemSelectionFactory::constructItemsForSelectionDialog(LibraryInterface* library,
    ObjectSelectionDialog* dialog, QList<VLNV> const& vlvns)
{
    for (VLNV const& vlnv : vlvns)
    {
        if (library->contains(vlnv) == false)
        {
            continue;
        }

        dialog->createItem(vlnv.toString(), vlnv, true);

        VLNV::IPXactType vlnvType = vlnv.getType();
        if (vlnvType == VLNV::COMPONENT)
        {
            Details::addComponentFilesAndDesignItems(library, dialog, vlnv);
        }

        else if (vlnvType == VLNV::DESIGNCONFIGURATION)
        {
            Details::addDesignItemForConfiguration(library, dialog, vlnv);
        }

        else if (vlnvType == VLNV::BUSDEFINITION)
        {
            Details::addAbstractionDefinitionItems(library, dialog, vlnv);
        }

        else if (vlnvType == VLNV::ABSTRACTIONDEFINITION)
        {
            Details::addBusDefintionItemIfOnlyAbstraction(library, dialog, vlnv);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryItemSelectionFactory::addComponentFilesAndDesignItems()
//-----------------------------------------------------------------------------
void LibraryItemSelectionFactory::Details::addComponentFilesAndDesignItems(LibraryInterface* library,
    ObjectSelectionDialog* dialog,
    VLNV const& componentVLNV)
{
    QSharedPointer<const Component> component =
        library->getModelReadOnly(componentVLNV).staticCast<const Component>();

    for (VLNV const& ref : component->getHierRefs())
    {
        if (library->contains(ref))
        {
            dialog->createItem(ref.toString(), ref);

            if (library->getDocumentType(ref) == VLNV::DESIGNCONFIGURATION)
            {
                Details::addDesignItemForConfiguration(library, dialog, ref);
            }
        }
    }

    // Ask the component for all it's file references.
    QString componentPath = library->getPath(componentVLNV);
    for (QString const& relativeFilePath : component->getDependentFiles())
    {
        QString absoluteFilePath = General::getAbsolutePath(componentPath, relativeFilePath);
        if (!absoluteFilePath.isEmpty())
        {
            dialog->createItem(absoluteFilePath);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryItemSelectionFactory::addDesignItemForConfiguration()
//-----------------------------------------------------------------------------
void LibraryItemSelectionFactory::Details::addDesignItemForConfiguration(LibraryInterface* library,
    ObjectSelectionDialog* dialog, VLNV const& configurationVLNV)
{
    VLNV designVLNV =
        library->getModelReadOnly(configurationVLNV).staticCast<const DesignConfiguration>()->getDesignRef();
    if (library->contains(designVLNV))
    {
        dialog->createItem(designVLNV.toString(), designVLNV);
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryItemSelectionFactory::addAbstractionDefinitionItems()
//-----------------------------------------------------------------------------
void LibraryItemSelectionFactory::Details::addAbstractionDefinitionItems(LibraryInterface* library,
    ObjectSelectionDialog* dialog, VLNV const& busDefinitionVLNV)
{
    QList<VLNV> absDefVLNVs;
    library->getChildren(absDefVLNVs, busDefinitionVLNV);

    // If a bus definition is selected then ask to selected all it's abstraction definitions also.
    for (VLNV const& absDefVLNV : absDefVLNVs)
    {
        if (library->contains(absDefVLNV))
        {
            dialog->createItem(absDefVLNV.toString(), absDefVLNV);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryItemSelectionFactory::addBusDefintionItemIfOnlyAbstraction()
//-----------------------------------------------------------------------------
void LibraryItemSelectionFactory::Details::addBusDefintionItemIfOnlyAbstraction(LibraryInterface* library,
    ObjectSelectionDialog* dialog, VLNV const& abstractionVLNV)
{
    VLNV busVLNV =
        library->getModelReadOnly(abstractionVLNV).staticCast<const AbstractionDefinition>()->getBusType();

    if (library->contains(busVLNV))
    {
        QList<VLNV> absDefVLNVs;
        library->getChildren(absDefVLNVs, busVLNV);

        // if theres only this abs def for the bus def
        if (absDefVLNVs.size() == 1 && absDefVLNVs.first() == abstractionVLNV)
        {
            dialog->createItem(busVLNV.toString(), busVLNV);
        }
    }
}
