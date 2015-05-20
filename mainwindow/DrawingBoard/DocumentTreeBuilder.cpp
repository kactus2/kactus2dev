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

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>

#include <IPXACTmodels/librarycomponent.h>

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
            rootNode = createFromDesign(root, QSharedPointer<DesignConfiguration const>());
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

    QSharedPointer<LibraryComponent const> component = library_->getModelReadOnly(root);
    QSharedPointer<Component const> rootComponent = component.dynamicCast<Component const>();

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
                QObject* designNode = createFromDesign(reference, QSharedPointer<DesignConfiguration const>());
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

    QSharedPointer<LibraryComponent const> configModel = library_->getModelReadOnly(designConfiguration);
    QSharedPointer<DesignConfiguration const> configuration = configModel.dynamicCast<DesignConfiguration const>();
    
    VLNV designRef = configuration->getDesignRef();

    if (library_->contains(designRef))
    {
        QObject* designNode = createFromDesign(designRef, configuration);
        designNode->setParent(designConfigNode);
    }

    return designConfigNode;
}

//-----------------------------------------------------------------------------
// Function: DocumentTreeBuilder::createFromDesign()
//-----------------------------------------------------------------------------
QObject* DocumentTreeBuilder::createFromDesign(VLNV const& designRef, 
    QSharedPointer<DesignConfiguration const> configuration) const
{
    QObject* designNode = new QObject(0);
    designNode->setObjectName(designRef.toString());

    QSharedPointer<LibraryComponent const> designModel = library_->getModelReadOnly(designRef);
    QSharedPointer<Design const> design = designModel.dynamicCast<Design const>();

    foreach (ComponentInstance instance, design->getComponentInstances())
    {
        QObject* instanceNode = createFrom(instance.getComponentRef());
        instanceNode->setParent(designNode);

        instanceNode->setProperty("instanceName", instance.getInstanceName());
        if (!configuration.isNull())
        {
            instanceNode->setProperty("activeView", configuration->getActiveView(instance.getInstanceName()));
        }
    }

    return designNode;
}
