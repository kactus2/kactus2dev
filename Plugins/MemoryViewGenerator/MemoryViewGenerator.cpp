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

#include <KactusAPI/include/LibraryInterface.h>

#include <KactusAPI/include/SystemVerilogExpressionParser.h>

#include <editors/MemoryDesigner/ConnectivityConnection.h>
#include <editors/MemoryDesigner/ConnectivityGraph.h>
#include <editors/MemoryDesigner/ConnectivityGraphFactory.h>
#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/MasterSlavePathSearch.h>
#include <editors/MemoryDesigner/MemoryItem.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <QFileInfo>
#include <QTextStream>

//-----------------------------------------------------------------------------
// Function: MemoryViewGenerator::MemoryViewGenerator()
//-----------------------------------------------------------------------------
MemoryViewGenerator::MemoryViewGenerator(LibraryInterface* library): library_(library),
    graphFactory_(library),
    expressionParser_(new SystemVerilogExpressionParser())
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
void MemoryViewGenerator::generate(QSharedPointer<Component> topComponent, QString const& activeView, 
    QString const& outputPath)
{
    QSharedPointer<ConnectivityGraph> graph = graphFactory_.createConnectivityGraph(topComponent, activeView);
    MasterSlavePathSearch searchAlgorithm;

    writeFile(outputPath, searchAlgorithm.findMasterSlavePaths(graph));
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGenerator::writeFile()
//-----------------------------------------------------------------------------
void MemoryViewGenerator::writeFile(QString const& outputPath, 
    QVector<QVector<QSharedPointer<const ConnectivityInterface> > > masterRoutes)
{
    QFile outputFile(outputPath); 
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream outputStream(&outputFile);

    outputStream << "Identifier;Type;Address;Range (AUB);Width (bits);Size (bits);Offset (bits);" << Qt::endl;

    foreach (QVector<QSharedPointer<const ConnectivityInterface> > path, masterRoutes)
    {        
        int addressOffset = path.first()->getBaseAddress().toInt();
        foreach (QSharedPointer<const ConnectivityInterface> inter, path)
        {
            if (inter->getMode() == General::MIRRORED_SLAVE)
            {
                addressOffset += inter->getRemapAddress().toInt();
            }
        }

        writeItem(path.first()->getConnectedMemory(), addressOffset, outputStream);

        if (path.first() != path.last())
        {
            writeItem(path.last()->getConnectedMemory(), addressOffset, outputStream);
        }
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
            "0x" + QString::number(addressOffset + item->getAddress().toInt(), 16) << ";" << 
            item->getRange() << ";" << item->getWidth() << ";" << item->getSize() << ";" <<
            item->getOffset() << ";" << Qt::endl;

        foreach (QSharedPointer<MemoryItem> child, item->getChildItems())
        {
            writeItem(child, addressOffset, outputStream);
        }
    }
}
