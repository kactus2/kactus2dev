//-----------------------------------------------------------------------------
// File: LinuxDeviceTreeCPUDetails.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.10.2021
//
// Description:
// Details for Linux device tree CPU declarations.
//-----------------------------------------------------------------------------

#ifndef LINUXDEVICETREECPUDETAILS_H
#define LINUXDEVICETREECPUDETAILS_H

#include <QVector>
#include <QSharedPointer>
#include <QMultiMap>

#include <Plugins/LinuxDeviceTree/CPUSelection/LinuxDeviceTreeCpuRoutesContainer.h>

class Cpu;
class ConnectivityInterface;
class Component;
class LibraryInterface;

namespace LinuxDeviceTreeCPUDetails
{
    //! Container for memory data.
    struct CpuMemory
    {
        //! Interface containing the memory item.
        QSharedPointer<ConnectivityInterface const> memoryInterface_;

        //! Base address of the memory location.
        quint64 itemBaseAddress_;

        //! Base address of the memory.
        quint64 mapBaseAddress_;

        //! Range of the memory.
        quint64 range_;
    };

    /*!
     *  Get containers for combined CPUs and interfaces.
     *
     *    @param [in] fileName        Default file name for the containers.
     *    @param [in] topComponent    The top level component.
     *    @param [in] viewName        Name of the top level view.
     *    @param [in] library         The library interface.
     *
     *    @return Containers containing combined CPUs and interfaces.
     */
    QVector<QSharedPointer<LinuxDeviceTreeCpuRoutesContainer> > getCPUContainers(QString const& fileName,
        QSharedPointer<Component> topComponent,
        QString const& viewName,
        LibraryInterface* library);

    /*!
     *  Get the component containing the selected interface.
     *
     *    @param [in] interfaceNode   The selected interface.
     *    @param [in] library         The library interface.
     *
     *    @return Component containing the selected interface.
     */
    QSharedPointer<Component const> getComponentContainingInterface(
        QSharedPointer<const ConnectivityInterface> interfaceNode, LibraryInterface* library);


    /*!
     *  Get the memory data of the selected interface.
     *
     *    @param [in] deviceTreeContainer     Container for the tree structure of a device tree interface.
     *    @param [in] baseAddress             Currently active base address.
     *    @param [in] memoryItemRange         Currently active range.
     *
     *    @return List of memory data connected to the selected interface.
     */
    QVector<CpuMemory> getMemories(QSharedPointer<DeviceTreeUtilities::CpuDeviceTree> deviceTreeContainer,
        quint64 baseAddress,
        quint64 const& memoryItemRange);

    /*!
     *  Check if the selected interface contains memory data.
     *
     *    @param [in] memoryInterface     The selected interface.
     *
     *    @return True, if the selected interface contains memory data, false otherwise.
     */
    bool interfacedItemIsMemory(QSharedPointer<const ConnectivityInterface> memoryInterface);
}

#endif // LINUXDEVICETREECPUDETAILS_H
