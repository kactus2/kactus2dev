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
#include <designEditors/MemoryDesigner/ComponentInstanceLocator.h>
#include <designEditors/MemoryDesigner/ConnectivityInterface.h>
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
    expressionParser_(new SystemVerilogExpressionParser()), locator_(library), masterPaths_()
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
    foreach (VLNV const& designReference, getConfigurationsAndDesigns(topComponent))
    {
        QSharedPointer<DesignConfiguration> designConfiguration(0);
        QSharedPointer<Design> design(0);

        VLNV::IPXactType documentType = library_->getDocumentType(designReference);

        if (documentType == VLNV::DESIGNCONFIGURATION)
        {
            designConfiguration = library_->getModel(designReference).dynamicCast<DesignConfiguration>();
            design = library_->getDesign(designConfiguration->getDesignRef());
        }
        else if (documentType == VLNV::DESIGN)
        {
            design = library_->getDesign(designReference);
        }

        QSharedPointer<ConnectivityGraph> graph = locator_.createConnectivityGraph(design, designConfiguration);

        foreach (QSharedPointer<ConnectivityInterface> masterInterface, getMasterInterfaces(graph))
        {       
            findPaths(masterInterface, QSharedPointer<ConnectivityConnection>(0), 
                QVector<QSharedPointer<ConnectivityInterface> >(), graph);          
        }
    }

    writeFile(outputPath);
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
            QSharedPointer<DesignConfiguration> designConfiguration = 
                library_->getModel(hierarchyReference).dynamicCast<DesignConfiguration>();

            if (references.contains(designConfiguration->getDesignRef()))
            {
                references.removeAt(references.indexOf(designConfiguration->getDesignRef()));
            }
        }
    }

    return references;
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGenerator::getMasterInterfaces()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<ConnectivityInterface> > MemoryViewGenerator::getMasterInterfaces(
    QSharedPointer<ConnectivityGraph> graph) const
{
    QVector<QSharedPointer<ConnectivityInterface> > masterInterfaces;

    foreach (QSharedPointer<ConnectivityInterface> vertex, graph->getInterfaces())
    {
        if (vertex->getMode() == "master" && graph->getConnectionsFor(vertex).count() == 1)
        {
            masterInterfaces.append(vertex);
        }
    }

    return masterInterfaces;
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGenerator::findPaths()
//-----------------------------------------------------------------------------
void MemoryViewGenerator::findPaths(QSharedPointer<ConnectivityInterface> startVertex, 
    QSharedPointer<ConnectivityConnection> previousEdge, 
    QVector<QSharedPointer<ConnectivityInterface> > existingPath, QSharedPointer<ConnectivityGraph> graph)
{
    existingPath.append(startVertex);

    QVector<QSharedPointer<ConnectivityConnection> > connections = graph->getConnectionsFor(startVertex);

    if (connections.contains(previousEdge))
    {
        connections.remove(connections.indexOf(previousEdge));
    }
   
    if (connections.isEmpty())
    {
        masterPaths_.append(existingPath);
    }
    else
    {
        foreach (QSharedPointer<ConnectivityConnection> nextEdge, connections)
        {
            if (!existingPath.contains(nextEdge->getFirstInterface()))
            {
                findPaths(nextEdge->getFirstInterface(), nextEdge, existingPath, graph);
            }
            else if (!existingPath.contains(nextEdge->getSecondInterface()))
            {
                findPaths(nextEdge->getSecondInterface(), nextEdge, existingPath, graph);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGenerator::writeFile()
//-----------------------------------------------------------------------------
void MemoryViewGenerator::writeFile(QString const& outputPath)
{
    QFile outputFile(outputPath); 
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream outputStream(&outputFile);

    outputStream << "Identifier;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);" << endl;

    foreach (QVector<QSharedPointer<ConnectivityInterface> > path, masterPaths_)
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
        outputStream << item->getIdentifier() << ";" << addressOffset + item->getAddress().toInt() << ";" << 
            item->getRange() << ";" << item->getWidth() << ";" << item->getSize() << ";" <<
            item->getOffset() << ";" << endl;

        foreach (QSharedPointer<MemoryItem> child, item->getChildItems())
        {
            writeItem(child, addressOffset, outputStream);
        }
    }
}
