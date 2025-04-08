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

#include <QVector>
#include <QSharedPointer>

class ConnectivityInterface;
class MemoryItem;

//-----------------------------------------------------------------------------
//! Calculates address ranges for connected memory items.
//-----------------------------------------------------------------------------
namespace MemoryConnectionAddressCalculator
{
    //! Calculated path variables for shortened path.
    struct CalculatedPathAddresses
    {
        //! Base address for the memory connection.
        quint64 connectionBaseAddress_ = 0;
        
        //! Last address of the memory connection.
        quint64 connectionLastAddress_ = 0;

        //! A connection can be created from this path.
        bool createConnection_ = false;
    };

    /*!
     *	Calculate the address range changes of the selected interface path.
     *
     *		@param [in]	startInterface  The first interface of the connection path.
     *		@param [in]	endInterface    The last interface of the connection path
     *		@param [in]	connectionPath  The connection path
     *
     *		@return Path variables containing data of the base address and last address for the connection in the connection path.
     */
    CalculatedPathAddresses calculatePathAddresses(QSharedPointer<const ConnectivityInterface> startInterface,
        QSharedPointer<const ConnectivityInterface> endInterface,
        QVector<QSharedPointer<ConnectivityInterface const> > connectionPath);

    /*!
     *	Get the memory map address ranges from the selected memory item.
     *
     *		@param [in]	mapItem     The selected memory item containing the memory map.
     *
     *		@return	QPair containing the base address and the last address of the selected memory map.
     */
    QPair<quint64, quint64> getMemoryMapAddressRanges(QSharedPointer<MemoryItem> mapItem);
};

//-----------------------------------------------------------------------------

#endif // MEMORYCONNECTIONADDRESSCALCULATOR_H
