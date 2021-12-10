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

    //! Container for CPUs and the connected interfaces.
    struct CPUContainer
    {
        //! FLag for creating a device tree file.
        bool createFile_;

        //! The device tree file name.
        QString fileName_;

        //! The device tree file path.
        QString filePath_;

        //! List of CPUs connected to the same interfaces.
        QVector<QSharedPointer<const Cpu> > interfacedCPUs_;

        //! List of interfaces connected to the CPUs.
        QVector<QSharedPointer<ConnectivityInterface> > rootInterfaces_;
    };

    /*!
     *  Get containers for combined CPUs and interfaces.
     *
     *      @param [in] fileName    Default file name for the containers.
     *      @param [in] roots       Root interfaces.
     *      @param [in] library     The library interface.
     *
     *      @return Containers containing combined CPUs and interfaces.
     */
    QVector<QSharedPointer<CPUContainer> > getCPUContainers(QString const& fileName,
        QVector<QSharedPointer<ConnectivityInterface> > roots, LibraryInterface* library);

    /*!
     *  Get CPU interface pairs from the selected interfaces.
     *
     *      @param [in] roots               The selected interfaces.
     *      @param [in] visitedInterfaces   List of examined interfaces.
     *      @param [in] library             The library interface.
     *
     *      @return CPU interface pairs in a multimap.
     */
    QMultiMap<QSharedPointer<const Cpu>, QSharedPointer<ConnectivityInterface> > getCPUInterfacePairs(
        QVector<QSharedPointer<ConnectivityInterface> > roots,
        QVector<QSharedPointer<ConnectivityInterface> >& visitedInterfaces, LibraryInterface* library);

    /*!
     *  Get the component containing the selected interface.
     *
     *      @param [in] interfaceNode   The selected interface.
     *      @param [in] library         The library interface.
     *
     *      @return Component containing the selected interface.
     */
    QSharedPointer<Component const> getComponentContainingInterface(
        QSharedPointer<const ConnectivityInterface> interfaceNode, LibraryInterface* library);


    /*!
     *  Get a list of the CPUs connected to the selected interface.
     *
     *      @param [in] containingComponent     Component containing the selected interface.
     *      @param [in] interfaceNode           The selected interface.
     *
     *      @return List of CPUs connected to the selected interface.
     */
    QVector<QSharedPointer<const Cpu> > getInterfacedCPUs(QSharedPointer<const Component> containingComponent,
        QSharedPointer<const ConnectivityInterface> interfaceNode);

    /*!
     *  Get the container for the selected CPU.
     *
     *      @param [in] singleCPU   The selected CPU.
     *      @param [in] containers  List of available containers.
     *
     *      @return The container for the selected CPU.
     */
    QSharedPointer<CPUContainer> getContainerForCPU(QSharedPointer<const Cpu> singleCPU,
        QVector<QSharedPointer<LinuxDeviceTreeCPUDetails::CPUContainer> > containers);

    /*!
     *  Get the memory data of the selected interface.
     *
     *      @param [in] previousInterface   The previously checked interface.
     *      @param [in] interfaceNode       The current interface.
     *      @param [in] baseAddress         Currently active base address.
     *      @param [in] memoryItemRange     Currently active range.
     *
     *      @return List of memory data connected to the selected interface.
     */
    QVector<CpuMemory> getMemories(QSharedPointer<const ConnectivityInterface> previousInterface,
        QSharedPointer<ConnectivityInterface> interfaceNode, quint64 baseAddress,
        quint64 const& memoryItemRange);

    /*!
     *  Check if the selected interface contains memory data.
     *
     *      @param [in] memoryInterface     The selected interface.
     *
     *      @return True, if the selected interface contains memory data, false otherwise.
     */
    bool interfacedItemIsMemory(QSharedPointer<const ConnectivityInterface> memoryInterface);
}

#endif // LINUXDEVICETREECPUDETAILS_H
