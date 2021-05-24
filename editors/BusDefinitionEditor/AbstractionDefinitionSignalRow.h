//-----------------------------------------------------------------------------
// File: AbstractionDefinitionSignalRow.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 14.01.2020
//
// Description:
// Represents a row in the abstraction ports table by grouping the Port, Wire and Transactional elements.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONDEFINITIONSIGNALROW_H
#define ABSTRACTIONDEFINITIONSIGNALROW_H

#include <IPXACTmodels/generaldeclarations.h>

#include <QString>
#include <QSharedPointer>

class PortAbstraction;
class WirePort;
class TransactionalPort;

//-----------------------------------------------------------------------------
//! Represents a row in the abstraction ports table by grouping the Port, Wire and Transactional elements.
//-----------------------------------------------------------------------------
class AbstractionDefinitionSignalRow
{
public:

    /*!
     *  The constructor.
     */
    AbstractionDefinitionSignalRow();

    /*!
     *  The destructor.
     */
    virtual ~AbstractionDefinitionSignalRow() = default;

    /*!
     *  Copy constructor
     */
    AbstractionDefinitionSignalRow(AbstractionDefinitionSignalRow const& other) = default;

    /*!
     *  Comparison of two SignalRows.
     *
     *      @param [in] other   The SignalRow to compare this to.
     *
     *      @return True, if the SignalRows are equal, otherwise false.
     */
    bool operator==(AbstractionDefinitionSignalRow const& other) const;
    
    /*!
     *  Comparison of two SignalRows.
     *
     *      @param [in] other   The SignalRow to compare this to.
     *
     *      @return True, if the SignalRows are not equal, otherwise false.
     */
    bool operator!=(AbstractionDefinitionSignalRow const& other) const;
    
    /*!
     *  Less than comparison of two SignalRows for table ordering.
     *
     *      @param [in] other   The SignalRow to compare this to.
     *
     *      @return True, if this precedes other, otherwise false.
     */
    bool operator<(AbstractionDefinitionSignalRow const& other) const;
    
    /*!
     *  Get the current system group of the selected signal.
     *
     *      @param [in] signal  The selected signal.
     *
     *      @return The system group of the selected signal.
     */
    QString getSystemGroup() const;
    
    /*!
     *  Check if the compared signal is of the same type.
     *
     *      @param [in] comparisonSignal    The selected comparison signal.
     *
     *      @return True, if the signals are of the same type, false otherwise.
     */
    bool comparedSignalIsSameType(AbstractionDefinitionSignalRow const& comparisonSignal) const;

    /*!
     *  Get the current interface mode.
     *
     *      @return The current interface mode.
     */
    General::InterfaceMode getMode() const;

    /*!
     *  Set the interface mode.
     *
     *      @param [in] newMode     The new interface mode.
     */
    void setMode(General::InterfaceMode const& newMode);

    /*!
     *  Get the port abstraction.
     *
     *      @return The port abstraction.
     */
    QSharedPointer<PortAbstraction> getPortAbstraction() const;

    /*!
     *  Set the port abstraction.
     *
     *      @param [in] newAbstraction  The new port abstraction.
     */
    void setPortAbstraction(QSharedPointer<PortAbstraction> newAbstraction);

    /*!
     *  Get the wire port.
     *
     *      @return The wire port.
     */
    QSharedPointer<WirePort> getWire() const;

    /*!
     *  Set the wire port.
     *
     *      @param[in] newWire  The new wire port.
     */
    void setWire(QSharedPointer<WirePort> newWire);

    /*!
     *  Get the transactional port.
     *
     *      @return The transactional port.
     */
    QSharedPointer<TransactionalPort> getTransactional() const;

    /*!
     *  Set the transactional port.
     *
     *      @param [in] newTRansactional    The new transactional port.
     */
    void setTransactional(QSharedPointer<TransactionalPort> newTransactional);

    /*!
     *  Check if the extend data is locked.
     *
     *      @return True, if the extend data is locked, false otherwise.
     */
    bool isExtendDataLocked() const;

    /*!
     *  Set the lock flag for extend data.
     *
     *      @param [in] newLockStatus   New extend lock flag.
     */
    void lockExtendData(bool newLockStatus);

    /*!
     *  Check if the port data is locked.
     *
     *      @return True, if the port data is locked, false otherwise.
     */
    bool isPortDataLocked() const;

    /*!
     *  Set the lock flag for port data.
     *
     *      @param [in] newLockStatus   New port data lock flag.
     */
    void lockPortData(bool newLockStatus);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Defines the Port represented in the row.
    QSharedPointer<PortAbstraction> abstraction_;

    //! Defines the mode of the wirePort (master, slave or system).
    General::InterfaceMode mode_;

    //! Defines the wire port represented on the row.
    QSharedPointer<WirePort> wire_;

    //! Defines the transactional port represented on the row.
    QSharedPointer<TransactionalPort> transactional_;

    //! Flag for locking columns for extend signals.
    bool lockExtendData_;

    //! Flag for locking port abstraction columns.
    bool lockPortData_;
};

#endif // ABSTRACTIONDEFINITIONEXTENDPORTHANDLER_H
