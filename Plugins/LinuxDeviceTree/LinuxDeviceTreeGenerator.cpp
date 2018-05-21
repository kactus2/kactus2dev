//-----------------------------------------------------------------------------
// File: LinuxDeviceTreePlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Copyright: Tampere University of Technology
// Author: Mikko Teuho
// Date: 08.05.2018
//
// Description:
// Creates a Linux Device Tree from the selected document.
//-----------------------------------------------------------------------------

#include "LinuxDeviceTreeGenerator.h"

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/AddressSpace.h>

#include <editors/MemoryDesigner/ConnectivityGraph.h>
#include <editors/MemoryDesigner/MasterSlavePathSearch.h>
#include <editors/MemoryDesigner/ConnectivityInterface.h>
#include <editors/MemoryDesigner/ConnectivityComponent.h>

#include <QTextStream>

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::LinuxDeviceTreeGenerator()
//-----------------------------------------------------------------------------
LinuxDeviceTreeGenerator::LinuxDeviceTreeGenerator(LibraryInterface* library):
// library_(library),
graphFactory_(library)
{

}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::~LinuxDeviceTreeGenerator()
//-----------------------------------------------------------------------------
LinuxDeviceTreeGenerator::~LinuxDeviceTreeGenerator()
{

}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::generate()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::generate(QSharedPointer<Component> topComponent, QString const& activeView,
    QString const& outputPath)
{
    QSharedPointer<ConnectivityGraph> graph = graphFactory_.createConnectivityGraph(topComponent, activeView);
    MasterSlavePathSearch searchAlgorithm;

    writeFile(outputPath, topComponent, searchAlgorithm.findMasterSlavePaths(graph));
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeFile()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeFile(QString const& outputPath, QSharedPointer<Component> topComponent,
    QVector<QVector<QSharedPointer<const ConnectivityInterface> > > masterRoutes)
{
    QFile outputFile(outputPath); 
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream outputStream(&outputFile);

/*
    outputStream << "/dts-v1/;" << endl << endl << "/ {" << endl;

    foreach (QVector<QSharedPointer<const ConnectivityInterface> > path, masterRoutes)
    {
        writePath(path);
    }

    writeCPUs(topComponent, outputStream);

    outputStream << "};" << endl;
*/

	writePaths(masterRoutes);

    outputFile.close();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writeCPUs()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writeCPUs(QSharedPointer<Component> containingComponent, QTextStream& outputStream)
{
    QSharedPointer<QList<QSharedPointer<Cpu> > > cpus = containingComponent->getCpus();
    if (cpus->isEmpty())
    {
        return;
    }

    outputStream << "\tcpus {" << endl;

    outputStream << "\t\taddress-cells = <1>;" << endl;
    outputStream << "\t\tsize-cells = <1>;" << endl;

    for (int i = 0; i < cpus->size(); ++i)
    {
        outputStream << "\t\tcpu@0" << " {" << endl;
        outputStream << "\t\t\treg = <" << QString::number(i) << ">;" << endl;
        outputStream << "\t\t};" << endl;
    }

    outputStream << "\t};" << endl;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::getCPUAddressSpace()
//-----------------------------------------------------------------------------
QSharedPointer<AddressSpace> LinuxDeviceTreeGenerator::getCPUAddressSpace(
    QSharedPointer<Component> containingComponent, QString const& spaceReference) const
{
    foreach (QSharedPointer<AddressSpace> space, *containingComponent->getAddressSpaces())
    {
        if (space->name().compare(spaceReference) == 0)
        {
            return space;
        }
    }

    return QSharedPointer<AddressSpace>();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writePath()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writePath(QVector<QSharedPointer<const ConnectivityInterface> > path)
{
    QSharedPointer<const ConnectivityInterface> firstInterface = path.first();
    if (firstInterface->getConnectedMemory() && firstInterface->getInstance())
    {
        VLNV::IPXactType vlnvType = VLNV::COMPONENT;
        QString vlnvString = firstInterface->getInstance()->getVlnv();
        VLNV componentVLNV(vlnvType, vlnvString);


    }
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeGenerator::writePaths()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeGenerator::writePaths(QVector<QVector<QSharedPointer<const ConnectivityInterface> > > masterPaths)
{
/*
	for (QVector<QSharedPointer<const ConnectivityInterface> > singlePath : paths)
	{
	}
*/

	foreach (auto path, masterPaths)
	{

	}
}
