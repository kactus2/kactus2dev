//-----------------------------------------------------------------------------
// File: DocumentFileAccess.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 09.02.2018
//
// Description:
// Reading and writing of IP-XACT documents using a standard file system.
//-----------------------------------------------------------------------------

#include "DocumentFileAccess.h"

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/Catalog/Catalog.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/kactusExtensions/ComDefinition.h>
#include <IPXACTmodels/kactusExtensions/ApiDefinition.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinitionReader.h>
#include <IPXACTmodels/BusDefinition/BusDefinitionReader.h>
#include <IPXACTmodels/Catalog/CatalogReader.h>
#include <IPXACTmodels/Component/ComponentReader.h>
#include <IPXACTmodels/Design/DesignReader.h>
#include <IPXACTmodels/designConfiguration/DesignConfigurationReader.h>

#include <IPXACTmodels/kactusExtensions/ComDefinitionReader.h>
#include <IPXACTmodels/kactusExtensions/ApiDefinitionReader.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinitionWriter.h>
#include <IPXACTmodels/BusDefinition/BusDefinitionWriter.h>
#include <IPXACTmodels/Catalog/CatalogWriter.h>
#include <IPXACTmodels/Component/ComponentWriter.h>
#include <IPXACTmodels/Design/DesignWriter.h>
#include <IPXACTmodels/designConfiguration/DesignConfigurationWriter.h>

#include <IPXACTmodels/kactusExtensions/ComDefinitionWriter.h>
#include <IPXACTmodels/kactusExtensions/ApiDefinitionWriter.h>

#include <QObject>
#include <QDomElement>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// Function: DocumentFileAccess::DocumentFileAccess()
//-----------------------------------------------------------------------------
DocumentFileAccess::DocumentFileAccess(MessageMediator* messageChannel) :
    messageChannel_(messageChannel)
{

}

//-----------------------------------------------------------------------------
// Function: DocumentFileAccess::readDocument()
//-----------------------------------------------------------------------------
QSharedPointer<Document> DocumentFileAccess::readDocument(QString const& path)
{
    // Create file handle and use it to read the IP-XACT document into memory.
    QFile file(path);
    QDomDocument doc;
    if (!doc.setContent(&file))
    {
        file.close();
        //emit errorMessage(tr("The document %1 in file %2 could not be opened.").arg(toCreate.toString(), path));
        return QSharedPointer<Document>();
    }
    file.close();

    VLNV::IPXactType toCreate = getDocumentVLNV(doc);

    // Create correct type of object.
    if (toCreate == VLNV::ABSTRACTIONDEFINITION)
    {
        AbstractionDefinitionReader reader;
        return reader.createAbstractionDefinitionFrom(doc);
    }
    else if (toCreate == VLNV::BUSDEFINITION)
    {
        BusDefinitionReader reader;
        return reader.createBusDefinitionFrom(doc);
    }
    else if (toCreate == VLNV::CATALOG)
    {
        CatalogReader reader;
        return reader.createCatalogFrom(doc);
    }
    else if (toCreate == VLNV::COMPONENT)
    {
        ComponentReader reader;
        return reader.createComponentFrom(doc);
    }
    else if (toCreate == VLNV::DESIGN)
    {
        DesignReader reader;
        return reader.createDesignFrom(doc);
    }
    else if (toCreate == VLNV::DESIGNCONFIGURATION)
    {
        DesignConfigurationReader reader;
        return reader.createDesignConfigurationFrom(doc);
    }

    else if (toCreate == VLNV::APIDEFINITION)
    {
        ApiDefinitionReader reader;
        return reader.createApiDefinitionFrom(doc);
    }
    else if (toCreate == VLNV::COMDEFINITION)
    {
        ComDefinitionReader reader;
        return reader.createComDefinitionFrom(doc);
    }
    else
    {
        messageChannel_->showMessage(QObject::tr("Document was not supported type"));
        return QSharedPointer<Document>();
    }
}

//-----------------------------------------------------------------------------
// Function: DocumentFileAccess::writeDocument()
//-----------------------------------------------------------------------------
bool DocumentFileAccess::writeDocument(QSharedPointer<Document> model, QString const& path)
{
    QFile targetFile(path);
    if (!targetFile.open(QFile::WriteOnly | QFile::Truncate))
    {
        messageChannel_->showError(QObject::tr("Could not open file %1 for writing.").arg(path));
        return false;
    }

    // write the parsed model
    QXmlStreamWriter xmlWriter(&targetFile);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.setAutoFormattingIndent(-1);

    VLNV::IPXactType documentType = model->getVlnv().getType();
    if (documentType == VLNV::ABSTRACTIONDEFINITION)
    {
        AbstractionDefinitionWriter writer;
        QSharedPointer<AbstractionDefinition> absDef = model.dynamicCast<AbstractionDefinition>();
        writer.writeAbstractionDefinition(xmlWriter, absDef);
    }

    else if (documentType == VLNV::BUSDEFINITION)
    {
        BusDefinitionWriter writer;
        QSharedPointer<BusDefinition> busDef = model.dynamicCast<BusDefinition>();
        writer.writeBusDefinition(xmlWriter, busDef);
    }
    else if (documentType == VLNV::CATALOG)
    {
        CatalogWriter writer;
        QSharedPointer<Catalog> catalog = model.dynamicCast<Catalog>();
        writer.writeCatalog(xmlWriter, catalog);
    }
    else if (documentType == VLNV::COMPONENT)
    {
        ComponentWriter writer;
        QSharedPointer<Component> component = model.dynamicCast<Component>();
        writer.writeComponent(xmlWriter, component);
    }
    else if (documentType == VLNV::DESIGN)
    {
        DesignWriter designWriter;
        QSharedPointer<Design> design = model.dynamicCast<Design>();
        designWriter.writeDesign(xmlWriter, design);
    }

    else if (documentType == VLNV::DESIGNCONFIGURATION)
    {
        DesignConfigurationWriter designConfigurationWriter;
        QSharedPointer<DesignConfiguration> designConfiguration = model.dynamicCast<DesignConfiguration>();
        designConfigurationWriter.writeDesignConfiguration(xmlWriter, designConfiguration);
    }

    else if (documentType == VLNV::APIDEFINITION)
    {
        ApiDefinitionWriter apiDefinitionWriter;
        QSharedPointer<ApiDefinition> apiDefinition = model.dynamicCast<ApiDefinition>();
        apiDefinitionWriter.writeApiDefinition(xmlWriter, apiDefinition);
    }

    else if (documentType == VLNV::COMDEFINITION)
    {
        ComDefinitionWriter comDefinitionWriter;
        QSharedPointer<ComDefinition> comDefinition = model.dynamicCast<ComDefinition>();
        comDefinitionWriter.writeComDefinition(xmlWriter, comDefinition);
    }
    else
    {
        Q_ASSERT_X(false, "DocumentFileAccess::writeDocument().", "Trying to write unknown document type to file.");
        targetFile.close();
        return false;
    }

    targetFile.close();
    return true;
}

//-----------------------------------------------------------------------------
// Function: DocumentFileAccess::getDocumentVLNV()
//-----------------------------------------------------------------------------
VLNV::IPXactType DocumentFileAccess::getDocumentVLNV(QDomDocument const& doc)
{
    return VLNV::string2Type(doc.documentElement().nodeName());
}
