//-----------------------------------------------------------------------------
// File: DocumentTreeBuilder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.05.2015
//
// Description:
// Creates an object tree from IP-XACT documents.
//-----------------------------------------------------------------------------

#include "DocumentTreeBuilder.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

//-----------------------------------------------------------------------------
// Function: DocumentTreeBuilder::DocumentTreeBuilder()
//-----------------------------------------------------------------------------
DocumentTreeBuilder::DocumentTreeBuilder(LibraryInterface* library):
library_(library)
{

}

//-----------------------------------------------------------------------------
// Function: DocumentTreeBuilder::~DocumentTreeBuilder()
//-----------------------------------------------------------------------------
DocumentTreeBuilder::~DocumentTreeBuilder()
{

}

//-----------------------------------------------------------------------------
// Function: DocumentTreeBuilder::createFrom()
//-----------------------------------------------------------------------------
QObject* DocumentTreeBuilder::createFrom(VLNV const& root) const
{
    QObject* rootNode;

    if (library_->contains(root))
    {
        if (library_->getDocumentType(root) == VLNV::COMPONENT)
        { 
            rootNode = createFromComponent(root);
        }
        else if (library_->getDocumentType(root) == VLNV::DESIGN)
        {
            rootNode = createFromDesign(root);
        }
        else if (library_->getDocumentType(root) == VLNV::DESIGNCONFIGURATION)
        {
            rootNode = createFromDesignConfiguration(root);
        }
        else
        {
            rootNode = new QObject();
            rootNode->setObjectName(root.toString());
        }
    }
    else
    {
        rootNode = new QObject();
        rootNode->setObjectName(root.toString());
    }

    return rootNode;
}

//-----------------------------------------------------------------------------
// Function: DocumentTreeBuilder::createFromComponent()
//-----------------------------------------------------------------------------
QObject* DocumentTreeBuilder::createFromComponent(VLNV const& root) const
{
    QObject* componentNode = new QObject();
    componentNode->setObjectName(root.toString());
    componentNode->setProperty("VLNVType", "Component");

    QSharedPointer<Document const> component = library_->getModelReadOnly(root);
    QSharedPointer<Component const> rootComponent = component.dynamicCast<Component const>();

    if (rootComponent->getImplementation() == KactusAttribute::SW)
    {
        componentNode->setProperty("implementation", "SW");
    }
    else
    {
        componentNode->setProperty("implementation", "HW");
    }

    QList<VLNV> hierarchyReferences = rootComponent->getHierRefs();
    foreach (VLNV reference, hierarchyReferences)
    {
        if (library_->contains(reference))
        {
            if (library_->getDocumentType(reference) == VLNV::DESIGNCONFIGURATION)
            {
                QObject* designConfigNode = createFromDesignConfiguration(reference);
                designConfigNode->setParent(componentNode);
            }
            else if (library_->getDocumentType(reference) == VLNV::DESIGN)
            {
                QObject* designNode = createFromDesign(reference);
                designNode->setParent(componentNode);
            }
        }
    }

    return componentNode;
}

//-----------------------------------------------------------------------------
// Function: DocumentTreeBuilder::createFromDesignConfiguration()
//-----------------------------------------------------------------------------
QObject* DocumentTreeBuilder::createFromDesignConfiguration(VLNV const& designConfiguration) const
{
    QObject* designConfigNode = new QObject(0);
    designConfigNode->setObjectName(designConfiguration.toString());
    designConfigNode->setProperty("VLNVType", "DesignConfiguration");

    QSharedPointer<Document const> configModel = library_->getModelReadOnly(designConfiguration);
    QSharedPointer<DesignConfiguration const> configuration = configModel.dynamicCast<DesignConfiguration const>();

    VLNV designRef = configuration->getDesignRef();

    if (library_->contains(designRef))
    {
        QObject* designNode = createFromDesign(designRef);
        designNode->setParent(designConfigNode);
    }

    return designConfigNode;
}

//-----------------------------------------------------------------------------
// Function: DocumentTreeBuilder::createFromDesign()
//-----------------------------------------------------------------------------
QObject* DocumentTreeBuilder::createFromDesign(VLNV const& designRef) const
{
    QObject* designNode = new QObject(0);
    designNode->setObjectName(designRef.toString());
    designNode->setProperty("VLNVType", "Design");

    QSharedPointer<Document const> designModel = library_->getModelReadOnly(designRef);
    QSharedPointer<Design const> design = designModel.dynamicCast<Design const>();

    if (design->getDesignImplementation() == KactusAttribute::SW)
    {
        designNode->setProperty("implementation", "SW");
    }
    else if (design->getDesignImplementation() == KactusAttribute::SYSTEM)
    {
        designNode->setProperty("implementation", "System");
    }
    else
    {
        designNode->setProperty("implementation", "HW");
    }

    foreach (QSharedPointer<ComponentInstance> hwInstance, *design->getComponentInstances())
    {
        QObject* instanceNode = createFrom(*hwInstance->getComponentRef());
        instanceNode->setParent(designNode);

        instanceNode->setProperty("instanceName", hwInstance->getInstanceName());
    }

    foreach (QSharedPointer<SWInstance> swInstance, design->getSWInstances())
    {
        QObject* swInstanceNode = createFrom(*swInstance->getComponentRef());
        swInstanceNode->setParent(designNode);

        swInstanceNode->setProperty("instanceName", swInstance->getInstanceName());
    }

    return designNode;
}
