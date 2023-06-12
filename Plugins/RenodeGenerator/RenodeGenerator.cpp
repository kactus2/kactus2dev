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

#include <Plugins/RenodeGenerator/CPUDialog/RenodeCpuRoutesContainer.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeUtilities.h>

#include <QCoreApplication>
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
void RenodeGenerator::generate(QString const& componentPath, QSharedPointer<RenodeCpuRoutesContainer> const& cpuContainer,
    bool createCpuFile, QString const& cpuFileName, bool createMemoryFile, QString const& memoryFileName,
    bool createPeripheralFile, QString const& peripheralFileName)
{
    QStringList fileNames;
    if (cpuContainer->getRoutes().empty() == false)
    {
        if (createCpuFile || createMemoryFile || createPeripheralFile)
        {
            writeFiles(componentPath, cpuContainer, fileNames, createCpuFile, cpuFileName,
                createMemoryFile, memoryFileName, createPeripheralFile, peripheralFileName);
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
// Function: RenodeGenerator::writeFiles()
//-----------------------------------------------------------------------------
void RenodeGenerator::writeFiles(QString const& componentPath, QSharedPointer<RenodeCpuRoutesContainer> cpuRoute,
    QStringList& fileNames, bool createCpuFile, QString const& cpuFileName, bool createMemoryFile,
    QString const& memoryFileName, bool createPeripheralFile, QString const& peripheralFileName)
{
    QSharedPointer<const ConnectivityInterface> cpuInterface = cpuRoute->getRoutes().first()->cpuInterface_;
    QSharedPointer<const ConnectivityComponent> routeComponent = cpuInterface->getInstance();
    QSharedPointer<const Component> interfaceComponent =
        ConnectivityGraphUtilities::getInterfacedComponent(library_, routeComponent);
    if (interfaceComponent)
    {
        QString renodeFilePath = componentPath + "/";

        if (createCpuFile)
        {
            writeCPU(renodeFilePath, cpuRoute, getUniqueFileName(fileNames, cpuFileName));
        }
        if (createMemoryFile)
        {
            writeMemories(renodeFilePath, cpuRoute, getUniqueFileName(fileNames, memoryFileName));
        }
        if (createPeripheralFile)
        {
            writePeripherals(renodeFilePath, componentPath, cpuRoute, getUniqueFileName(fileNames, peripheralFileName));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RenodeGenerator::getUniqueFileName()
//-----------------------------------------------------------------------------
QString RenodeGenerator::getUniqueFileName(QStringList& fileNames, QString const& currentFileName) const
{
    QString modifiedFileName = currentFileName;
    if (fileNames.contains(modifiedFileName))
    {
        modifiedFileName = currentFileName + "_" + QString::number(getFileNumberExtension(fileNames, currentFileName));
    }

    fileNames.append(modifiedFileName);
    return modifiedFileName;
}

//-----------------------------------------------------------------------------
// Function: SVDGenerator::getFileNumberExtension()
//-----------------------------------------------------------------------------
int RenodeGenerator::getFileNumberExtension(QStringList const& fileNames, QString const& fileName) const
{
    int extensionNumber = -1;
    for (QString file : fileNames)
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
void RenodeGenerator::writeCPU(QString const& filePath, QSharedPointer<RenodeCpuRoutesContainer> cpuContainer, QString const& fileName)
{
    QString cpuFilePath = filePath + fileName + "." + RenodeConstants::RENODEFILETYPE;

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
void RenodeGenerator::writePeripherals(QString const& filePath, QString const& peripheralPath,
    QSharedPointer<RenodeCpuRoutesContainer> renodeCollection, QString const& fileName)
{
    if (renodeCollection->getPeripherals().isEmpty())
    {
        return;
    }

    QString peripheralFilePath = filePath + fileName + "." + RenodeConstants::RENODEFILETYPE;

    QFile outputFile(peripheralFilePath);
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream stream(&outputFile);

    QString tab = "\t";
    for (auto peripheral : renodeCollection->getPeripherals())
    {
        writePeripheral(stream, peripheral, tab, peripheralPath);

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
void RenodeGenerator::writePeripheral(QTextStream& stream, QSharedPointer<RenodeStructs::cpuPeripheral> peripheral, QString const& tab, QString const& renodeFilePath)
{
    stream << peripheral->peripheralName_ << ": " << peripheral->className_ << " @ sysbus " << peripheral->baseAddress_ << Qt::endl;
    stream << tab << "size: " << peripheral->size_ << Qt::endl;

    if (peripheral->className_ == RenodeConstants::PYTHONPERIPHERAL)
    {
        QString filePath = peripheral->filePath_;
        if (filePath.last(1) != "/")
        {
            filePath.append("/");
        }

        QString fileName = filePath + peripheral->peripheralName_ + "." + RenodeConstants::PYTHONFILETYPE;

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

        if (peripheral->template_)
        {
            QString templatePath = QCoreApplication::applicationDirPath() + "/" + peripheral->template_->template_;
            QFile templateFile(templatePath);
            if (templateFile.open(QIODevice::ReadOnly))
            {
                QTextStream templateStream(&templateFile);
                QTextStream peripheralStream(&peripheralOutputFile);

                while (!templateStream.atEnd())
                {
                    peripheralStream << templateStream.readAll();
                }
            }
        }

        generatedFiles_.append(peripheralFilePath);
    }
}

//-----------------------------------------------------------------------------
// Function: RenodeGenerator::writeMemories()
//-----------------------------------------------------------------------------
void RenodeGenerator::writeMemories(QString const& filePath, QSharedPointer<RenodeCpuRoutesContainer> renodeCollection, QString const& fileName)
{
    if (renodeCollection->getMemories().isEmpty())
    {
        return;
    }

    QString memoryFilePath = filePath + fileName + "." + RenodeConstants::RENODEFILETYPE;
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
