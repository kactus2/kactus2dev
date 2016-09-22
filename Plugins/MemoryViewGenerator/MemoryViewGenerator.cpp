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
void MemoryViewGenerator::generate(QSharedPointer<Component> topComponent, QString const& activeView, QString const& outputPath)
{
    QVector<QVector<QSharedPointer<ConnectivityInterface> > > masterRoutes;

    VLNV designReference = getConfigurationOrDesign(topComponent, activeView);

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

//-----------------------------------------------------------------------------
// Function: MemoryViewGenerator::getConfigurationOrDesign()
//-----------------------------------------------------------------------------
VLNV MemoryViewGenerator::getConfigurationOrDesign(QSharedPointer<Component> component, QString const& activeView)
{
    QSharedPointer<View> view = component->getModel()->findView(activeView);
    if (view)
    {
        if (!view->getDesignConfigurationInstantiationRef().isEmpty())
        {
            QSharedPointer<DesignConfigurationInstantiation> instantiation = 
                component->getModel()->findDesignConfigurationInstantiation(
                view->getDesignConfigurationInstantiationRef());

            if (instantiation)
            {
                return *instantiation->getDesignConfigurationReference();
            }
        }
        else if (!view->getDesignInstantiationRef().isEmpty())
        {
            QSharedPointer<DesignInstantiation> instantiation = component->getModel()->findDesignInstantiation(
                view->getDesignInstantiationRef());

            if (instantiation)
            {
                return *instantiation->getDesignReference();
            }
        }
    }
    
    if (component->getDesignConfigurationInstantiations()->count() == 1)
    {
        return *component->getDesignConfigurationInstantiations()->first()->getDesignConfigurationReference();
    }

    if (component->getDesignInstantiations()->count() == 1)
    {
        return *component->getDesignInstantiations()->first()->getDesignReference();
    }

    return VLNV();
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
            item->getOffset() << ";" << endl;

        foreach (QSharedPointer<MemoryItem> child, item->getChildItems())
        {
            writeItem(child, addressOffset, outputStream);
        }
    }
}
