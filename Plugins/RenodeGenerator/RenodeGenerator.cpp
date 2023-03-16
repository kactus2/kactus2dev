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

#include <QFileInfo>
#include <QDir>

//-----------------------------------------------------------------------------
// Function: RenodeGenerator::RenodeGenerator()
//-----------------------------------------------------------------------------
RenodeGenerator::RenodeGenerator(LibraryInterface* library):
library_(library)
{

}

//-----------------------------------------------------------------------------
// Function: RenodeGenerator::generate()
//-----------------------------------------------------------------------------
void RenodeGenerator::generate(QSharedPointer<Component> topComponent, QString const& componentPath, QVector<QSharedPointer<RenodeCPUDetailRoutes>> const& cpuRoutes)
{
    QStringList cpuNames;
    for (auto cpuMasterRoute : cpuRoutes)
    {
        if (cpuMasterRoute->getRoutes()->empty() == false)
        {
            writeFile(topComponent, componentPath, cpuMasterRoute, cpuNames);
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
void RenodeGenerator::writeFile(QSharedPointer<Component> topComponent, QString const& componentPath, QSharedPointer<RenodeCPUDetailRoutes> cpuRoute, QStringList& fileNames)
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

        QString renodeFilePath = componentPath + "/" + fileName + "_peripherals";

        renodeFilePath += ".repl";

        QFile outputFile(renodeFilePath);
        if (!outputFile.open(QIODevice::WriteOnly))
        {
            return;
        }

        QTextStream stream(&outputFile);

        writePeripherals(stream, cpuRoute, componentPath);

        generatedFiles_.append(renodeFilePath);
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
void RenodeGenerator::writeCPU(QTextStream& stream, QSharedPointer<Cpu> currentCPU, QSharedPointer<RenodeCPUDetailRoutes> cpuContainer)
{
    stream << "cpu: ";
    stream << currentCPU->name() << ".";
    stream << "ClassX @ sysbus";
    stream << Qt::endl << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: RenodeGenerator::writePeripherals()
//-----------------------------------------------------------------------------
void RenodeGenerator::writePeripherals(QTextStream& stream, QSharedPointer<RenodeCPUDetailRoutes> renodeCollection, QString const& renodeFilePath)
{
    QString tab = "\t";
    for (auto peripheral : renodeCollection->getPeripherals())
    {
        writePeripheral(stream, peripheral, tab, renodeFilePath);

        if (peripheral != renodeCollection->getPeripherals().last())
        {
            stream << Qt::endl;
        }
    }
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

//     QString fileName = peripheral->filePath_ + "/" + peripheral->peripheralName_ + ".py";
    QString fileName = filePath + peripheral->peripheralName_ + ".py";

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
}
