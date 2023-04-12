//-----------------------------------------------------------------------------
// File: RenodeGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.02.2023
//
// Description:
// Creates a Renode file.
//-----------------------------------------------------------------------------

#include "RenodeGenerator.h"

#include <IPXACTmodels/generaldeclarations.h>

#include <Plugins/RenodeGenerator/CPUDialog/RenodeCPUDetailRoutes.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeUtilities.h>

#include <QFileInfo>
#include <QDir>

//-----------------------------------------------------------------------------
// Function: RenodeGenerator::RenodeGenerator()
//-----------------------------------------------------------------------------
RenodeGenerator::RenodeGenerator(LibraryInterface* library):
library_(library),
generatedFiles_()
{

}

//-----------------------------------------------------------------------------
// Function: RenodeGenerator::generate()
//-----------------------------------------------------------------------------
void RenodeGenerator::generate(QSharedPointer<Component> topComponent, QString const& componentPath,
    QVector<QSharedPointer<RenodeCPUDetailRoutes>> const& cpuRoutes, bool createCpuFile, bool createMemoryFile, bool createPeripheralFile)
{
    QStringList cpuNames;
    for (auto cpuMasterRoute : cpuRoutes)
    {
        if (cpuMasterRoute->getRoutes()->empty() == false)
        {
            if (createCpuFile || createMemoryFile || createPeripheralFile)
            {
                writeFile(topComponent, componentPath, cpuMasterRoute, cpuNames, createCpuFile, createMemoryFile, createPeripheralFile);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RenodeGenerator::getGeneratedFiles()
//-----------------------------------------------------------------------------
QStringList RenodeGenerator::getGeneratedFiles() const
{
    return generatedFiles_;
}

//-----------------------------------------------------------------------------
// Function: RenodeGenerator::writeFile()
//-----------------------------------------------------------------------------
void RenodeGenerator::writeFile(QSharedPointer<Component> topComponent, QString const& componentPath,
    QSharedPointer<RenodeCPUDetailRoutes> cpuRoute, QStringList& fileNames, bool createCpuFile, bool createMemoryFile, bool createPeripheralFile)
{
    QSharedPointer<const ConnectivityInterface> cpuInterface = cpuRoute->getCPUInterface();
    QSharedPointer<const ConnectivityComponent> routeComponent = cpuInterface->getInstance();
    QSharedPointer<const Component> interfaceComponent =
        ConnectivityGraphUtilities::getInterfacedComponent(library_, routeComponent);
    if (interfaceComponent)
    {
        QSharedPointer<Cpu> interfaceCPU = ConnectivityGraphUtilities::getInterfacedCPU(interfaceComponent, cpuInterface);

        QString fileName = topComponent->getVlnv().getName() + "_" + interfaceCPU->name();
        if (fileNames.contains(interfaceCPU->name()))
        {
            fileName = fileName + "_" + QString::number(getFileNumberExtension(fileNames, fileName));
        }

        QString renodeFilePath = componentPath + "/" + fileName;

        if (createCpuFile)
        {
            writeCPU(renodeFilePath, cpuRoute);
        }
        if (createMemoryFile)
        {
            writeMemories(renodeFilePath, cpuRoute);
        }
        if (createPeripheralFile)
        {
            writePeripherals(renodeFilePath, componentPath, cpuRoute);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::getFileNumberExtension()
//-----------------------------------------------------------------------------
int RenodeGenerator::getFileNumberExtension(QStringList const& fileNames, QString const& fileName) const
{
    int extensionNumber = -1;
    for (auto file : fileNames)
    {
        if (file.contains(fileName))
        {
            extensionNumber++;
        }
    }

    return extensionNumber;
}

//-----------------------------------------------------------------------------
// Function: RenodeGenerator::writeCPU()
//-----------------------------------------------------------------------------
void RenodeGenerator::writeCPU(QString const& filePath, QSharedPointer<RenodeCPUDetailRoutes> cpuContainer)
{
    QString cpuFilePath = filePath + "_" + RenodeConstants::RENODECPUFILENAME + "." + RenodeConstants::RENODEFILETYPE;

    QFile outputFile(cpuFilePath);
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream stream(&outputFile);

    QString tab = "\t";
    stream << "cpu: CPU." << cpuContainer->getClassName() << " @ sysbus" << Qt::endl;
    stream << tab << "cpuType: \"" << cpuContainer->getCpuType() << "\"" << Qt::endl;
    stream << tab << "timeProvider: " << cpuContainer->getTimeProvider() << Qt::endl;

    generatedFiles_.append(cpuFilePath);
}

//-----------------------------------------------------------------------------
// Function: RenodeGenerator::writePeripherals()
//-----------------------------------------------------------------------------
void RenodeGenerator::writePeripherals(QString const& filePath, QString const& componentPath, QSharedPointer<RenodeCPUDetailRoutes> renodeCollection)
{
    if (renodeCollection->getPeripherals().isEmpty())
    {
        return;
    }

    QString peripheralFilePath = filePath + "_" + RenodeConstants::RENODEPERIPHERALFILENAME + "." + RenodeConstants::RENODEFILETYPE;

    QFile outputFile(peripheralFilePath);
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream stream(&outputFile);

    QString tab = "\t";
    for (auto peripheral : renodeCollection->getPeripherals())
    {
        writePeripheral(stream, peripheral, tab, componentPath);

        if (peripheral != renodeCollection->getPeripherals().last())
        {
            stream << Qt::endl;
        }
    }

    generatedFiles_.append(peripheralFilePath);
}

//-----------------------------------------------------------------------------
// Function: RenodeGenerator::writePeripheral()
//-----------------------------------------------------------------------------
void RenodeGenerator::writePeripheral(QTextStream& stream, QSharedPointer<RenodeStructs::cpuPeripherals> peripheral, QString const& tab, QString const& renodeFilePath)
{
    QString filePath = peripheral->filePath_;
    if (filePath.last(1) != "/")
    {
        filePath.append("/");
    }

    QString fileName = filePath + peripheral->peripheralName_ + "." + RenodeConstants::PYTHONFILETYPE;

    stream << peripheral->peripheralName_ << ": " << peripheral->className_ << " @ sysbus " << peripheral->baseAddress_ << Qt::endl;
    stream << tab << "size: " << peripheral->size_ << Qt::endl;
    stream << tab << "initable: " << QVariant(peripheral->initable_).toString() << Qt::endl;
    stream << tab << "filename: " << fileName << Qt::endl;

    QString directoryPath = renodeFilePath + "/" + peripheral->filePath_;
    QDir pathDirectory(directoryPath);
    if (!pathDirectory.exists())
    {
        QDir currentDirectory(renodeFilePath);
        currentDirectory.mkdir(peripheral->filePath_);
    }

    QString peripheralFilePath = renodeFilePath + "/" + fileName;
    QFile peripheralOutputFile(peripheralFilePath);
    peripheralOutputFile.open(QIODevice::WriteOnly);

    generatedFiles_.append(peripheralFilePath);
}

//-----------------------------------------------------------------------------
// Function: RenodeGenerator::writeMemories()
//-----------------------------------------------------------------------------
void RenodeGenerator::writeMemories(QString const& filePath, QSharedPointer<RenodeCPUDetailRoutes> renodeCollection)
{
    if (renodeCollection->getMemories().isEmpty())
    {
        return;
    }

    QString memoryFilePath = filePath + "_" + RenodeConstants::RENODEMEMORYFILENAME + "." + RenodeConstants::RENODEFILETYPE;
    QFile outputFile(memoryFilePath);
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream stream(&outputFile);

    QString tab = "\t";
    for (auto memory : renodeCollection->getMemories())
    {
        writeMemory(stream, memory, tab);

        if (memory != renodeCollection->getMemories().last())
        {
            stream << Qt::endl;
        }
    }

    generatedFiles_.append(memoryFilePath);
}

//-----------------------------------------------------------------------------
// Function: RenodeGenerator::writeMemory()
//-----------------------------------------------------------------------------
void RenodeGenerator::writeMemory(QTextStream& stream, QSharedPointer<RenodeStructs::cpuMemories> memory, QString const& tab)
{
    stream << memory->memoryName_ << ": " << memory->className_ << " @ sysbus " << memory->baseAddress_ << Qt::endl;
    stream << tab << "size: " << memory->size_ << Qt::endl;
}
