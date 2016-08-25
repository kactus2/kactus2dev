//-----------------------------------------------------------------------------
// File: MemoryViewGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 26.04.2016
//
// Description:
// Creates a CSV listing of all slave memories within a design hierarchy.
//-----------------------------------------------------------------------------

#include "MemoryViewGenerator.h"

#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <designEditors/MemoryDesigner/ConnectivityConnection.h>
#include <designEditors/MemoryDesigner/ConnectivityGraph.h>
#include <designEditors/MemoryDesigner/ConnectivityGraphFactory.h>
#include <designEditors/MemoryDesigner/ConnectivityInterface.h>
#include <designEditors/MemoryDesigner/MasterSlavePathSearch.h>
#include <designEditors/MemoryDesigner/MemoryItem.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <QFileInfo>
#include <QTextStream>

//-----------------------------------------------------------------------------
// Function: MemoryViewGenerator::MemoryViewGenerator()
//-----------------------------------------------------------------------------
MemoryViewGenerator::MemoryViewGenerator(LibraryInterface* library): library_(library),
    expressionParser_(new SystemVerilogExpressionParser()), locator_(library)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryViewGenerator::~MemoryViewGenerator()
//-----------------------------------------------------------------------------
MemoryViewGenerator::~MemoryViewGenerator()
{
    delete expressionParser_;
    expressionParser_ = 0;
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGenerator::generate()
//-----------------------------------------------------------------------------
void MemoryViewGenerator::generate(QSharedPointer<Component> topComponent, QString const& outputPath)
{
    QVector<QVector<QSharedPointer<ConnectivityInterface> > > masterRoutes;

    foreach (VLNV const& designReference, getConfigurationsAndDesigns(topComponent))
    {
        QSharedPointer<const DesignConfiguration> designConfiguration(0);
        QSharedPointer<const Design> design(0);

        VLNV::IPXactType documentType = library_->getDocumentType(designReference);

        if (documentType == VLNV::DESIGNCONFIGURATION)
        {
            designConfiguration = library_->getModelReadOnly(designReference).dynamicCast<const DesignConfiguration>();
            design = library_->getModelReadOnly(designConfiguration->getDesignRef()).dynamicCast<const Design>();
        }
        else if (documentType == VLNV::DESIGN)
        {
            design = library_->getModelReadOnly(designReference).dynamicCast<const Design>();
        }

        QSharedPointer<ConnectivityGraph> graph = locator_.createConnectivityGraph(design, designConfiguration);
        MasterSlavePathSearch searchAlgorithm;
  
        writeFile(outputPath, searchAlgorithm.findMasterSlavePaths(graph));
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGenerator::getConfigurationsAndDesigns()
//-----------------------------------------------------------------------------
QVector<VLNV> MemoryViewGenerator::getConfigurationsAndDesigns(QSharedPointer<Component> component)
{
    QVector<VLNV> references = component->getHierRefs().toVector();

    foreach (VLNV const& hierarchyReference, references)
    {
        if (library_->getDocumentType(hierarchyReference) == VLNV::DESIGNCONFIGURATION)
        {
            QSharedPointer<const DesignConfiguration> designConfiguration = 
                library_->getModelReadOnly(hierarchyReference).dynamicCast<const DesignConfiguration>();

            if (references.contains(designConfiguration->getDesignRef()))
            {
                references.removeAt(references.indexOf(designConfiguration->getDesignRef()));
            }
        }
    }

    return references;
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGenerator::writeFile()
//-----------------------------------------------------------------------------
void MemoryViewGenerator::writeFile(QString const& outputPath, QVector<QVector<QSharedPointer<ConnectivityInterface> > > masterRoutes)
{
    QFile outputFile(outputPath); 
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream outputStream(&outputFile);

    outputStream << "Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);" << endl;

    foreach (QVector<QSharedPointer<ConnectivityInterface> > path, masterRoutes)
    {        
        int addressOffset = path.first()->getBaseAddress().toInt();
        foreach (QSharedPointer<ConnectivityInterface> inter, path)
        {
            if (inter->getMode() == "mirroredSlave")
            {
                addressOffset += inter->getRemapAddress().toInt();
            }
        }

        writeItem(path.last()->getConnectedMemory(), addressOffset, outputStream);
    }

    outputFile.close();
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGenerator::writeItem()
//-----------------------------------------------------------------------------
void MemoryViewGenerator::writeItem(QSharedPointer<MemoryItem> item, int addressOffset, QTextStream& outputStream)
{
    if (item)
    {
        outputStream << item->getIdentifier() << ";" << item->getType() << ";" <<
            addressOffset + item->getAddress().toInt() << ";" << 
            item->getRange() << ";" << item->getWidth() << ";" << item->getSize() << ";" <<
            item->getOffset() << ";" << endl;

        foreach (QSharedPointer<MemoryItem> child, item->getChildItems())
        {
            writeItem(child, addressOffset, outputStream);
        }
    }
}
