//-----------------------------------------------------------------------------
// File: MemoryConnectionAddressCalculator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.05.2018
//
// Description:
// Calculates address ranges for connected memory items.
//-----------------------------------------------------------------------------

#ifndef MEMORYCONNECTIONADDRESSCALCULATOR_H
#define MEMORYCONNECTIONADDRESSCALCULATOR_H

class ConnectivityInterface;
class MemoryItem;

#include <QVector>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Calculates address ranges for connected memory items.
//-----------------------------------------------------------------------------
namespace MemoryConnectionAddressCalculator
{
    //! Contains information of the connected address space interface.
    struct ChainedSpace
    {
        //! Interface containing the connected address space.
        QSharedPointer<const ConnectivityInterface> spaceInterface_;

        //! Base address of the address space connection.
        quint64 spaceConnectionBaseAddress_;
    };

    //! Path variables determined during connection path examination.
    struct ConnectionPathVariables
    {
        //! The base address number of the path interfaces.
        quint64 baseAddressNumber_;

        //! Address change caused by mirrored slave bus interfaces.
        quint64 mirroredSlaveAddressChange_;

        //! End address of the memory map modified by mirrored slave interfaces.
        quint64 memoryMapEndAddress_;

        //! Base address modified by the interface bridges.
        quint64 spaceChainBaseAddress_;

        //! Flag for paths that have gone through a mirrored slave with a remapped address range.
        bool hasRemapRange_;

        //! Flag for paths that have gone through a bridge.
        bool isChainedSpaceConnection_;

        //! Address space items referenced by the bridged master bus interfaces of the path.
        QVector<ChainedSpace> spaceChain_;

        //! Base address of the connected memory map interface.
        quint64 memoryMapBaseAddress_;

        //! Last address of the connected memory map interface.
        quint64 endAddressNumber_;

        //! The remapped base address. (where the mapped memory map starts in the initiator address space)
        quint64 remappedAddress_;

        //! The remapped end address. (where the mapped memory map ends in the initiator address space)
        quint64 remappedEndAddress_;
    };

    /*!
     *	Calculate the address range changes of the selected interface path.
     *
     *		@param [in]	startInterface  The first interface of the connection path.
     *		@param [in]	endInterface    The last interface of the connection path
     *		@param [in]	connectionPath  The connection path
     *
     *		@return Path variables containing data of the address range changes in the connection path.
     */
    ConnectionPathVariables calculatePathAddresses(QSharedPointer<const ConnectivityInterface> startInterface,
        QSharedPointer<const ConnectivityInterface> endInterface,
        QVector<QSharedPointer<ConnectivityInterface const> > connectionPath);

    /*!
     *  Get the starting base address of the memory connection.
     *
     *      @param [in] startInterface  Start interface of the memory connection path.
     *      @param [in] endInterface    End interface of the memory connection path.
     *
     *      @return Starting base address of the memory connection.
     */
    quint64 getStartingBaseAddress(QSharedPointer<ConnectivityInterface const> startInterface,
        QSharedPointer<ConnectivityInterface const> endInterface);

    /*!
     *	Get the memory map address ranges from the selected memory item.
     *
     *		@param [in]	mapItem     The selected memory item containing the memory map.
     *
     *		@return	QPair containing the base address and the last address of the selected memory map.
     */
    QPair<quint64, quint64> getMemoryMapAddressRanges(QSharedPointer<MemoryItem> mapItem);

    /*!
     *  Calculate the remapped base address for the memory connection.
     *
     *      @param [in] memoryMapBaseAddress                Base address of the memory map item.
     *      @param [in] baseAddressNumber                   Base address of the memory connection.
     *      @param [in] spaceChainConnectionBaseAddress     Base address modifier caused by the bridged interfaces.
     *      @param [in] mirroredSlaveAddressChange          Address change caused by the mirrored slave interfaces.
     *      @param [in] hasRemapRange                       Flag for remapped connection range.
     *
     *      @return The remapped base address of the memory connection.
     */
    quint64 getRemappedBaseAddress(quint64 memoryMapBaseAddress, quint64 baseAddressNumber,
        quint64 spaceChainConnectionBaseAddress, quint64 mirroredSlaveAddressChange, bool hasRemapRange);
};

//-----------------------------------------------------------------------------

#endif // MEMORYCONNECTIONADDRESSCALCULATOR_H
