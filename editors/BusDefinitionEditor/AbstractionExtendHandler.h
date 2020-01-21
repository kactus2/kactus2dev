//-----------------------------------------------------------------------------
// File: AbstractionExtendHandler.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 14.01.2020
//
// Description:
// Handles the extend ports of abstraction definition.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONEXTENDHANDLER_H
#define ABSTRACTIONEXTENDHANDLER_H

#include <editors/BusDefinitionEditor/AbstractionDefinitionSignalRow.h>

#include <QSharedPointer>

class PortAbstraction;
class AbstractionDefinition;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Handles the extend ports of abstraction definition.
//-----------------------------------------------------------------------------
namespace AbstractionExtendHandler
{
    /*!
     *  Remove the selected extend signal from the containing abstraction definition.
     *
     *      @param [in] signal          The selected extend signal.
     *      @param [in] extendPorts     List of extend ports.
     *      @param [in] abstraction     Containing abstraction definition.
     *
     *      @return True, if the extend signal was removed, false otherwise.
     */
    bool removeExtendSignalFromAbstraction(AbstractionDefinitionSignalRow const& signal,
        QList<QSharedPointer<PortAbstraction>> const& extendPorts,
        QSharedPointer<AbstractionDefinition> abstraction);

    /*!
     *  Check if the selected signal is part of an extend port.
     *
     *      @param [in] signal          The selected signal.
     *      @param [in] extendPorts     List of the containing extend ports.
     *
     *      @return True, if the signal is extended, false otherwise.
     */
    bool signalRowIsExtend(AbstractionDefinitionSignalRow const& signal,
        QList<QSharedPointer<PortAbstraction>> const& extendPorts);
    
    /*!
     *  Get a list of the extend signals edited to contain new port abstraction data.
     *
     *      @param [in] oldExtendSignals    Signal rows of the previous extend signals.
     *      @param [in] newExtends          The new extend port abstractions.
     *
     *      @return List of extend signals containing modified port data.
     */
    QVector<AbstractionDefinitionSignalRow> getEditedExtendSignals(
        QVector<AbstractionDefinitionSignalRow> const& oldExtendSignals,
        QList<QSharedPointer<PortAbstraction> > const& newExtends);

    /*!
     *  Get the row count of the selected list of port abstractions.
     *
     *      @param [in] extendPorts     The selected port abstraction list.
     *
     *      @return Number of rows that can be created from the selected port abstraction list.
     */
    int getPortAbstractionRowCount(QList<QSharedPointer<PortAbstraction>> const& extendPorts);

    /*!
     *  Add the selected signal row to abstraction definition.
     *
     *      @param [in] signal          The selected signal row.
     *      @param [in] abstraction     Target abstraction definition.
     */
    void addSignalRowToPortAbstraction(AbstractionDefinitionSignalRow& signal,
        QSharedPointer<AbstractionDefinition> abstraction);

    /*!
     *  Remove the selected port abstractions from the abstraction definition.
     *
     *      @param [in] abstraction     The selected abstraction definition.
     *      @param [in] extendPorts     List of the removed port abstractions.
     */
    void removeExtendedPortsFromAbstraction(QSharedPointer<AbstractionDefinition> abstraction,
        QList<QSharedPointer<PortAbstraction>> const& extendPorts);

    /*!
     *  Get all the extend ports from the selected abstraction definition.
     *
     *      @param [in] abstraction     The selected abstraction definition.
     *      @param [in] libraryAccess   The library interface.
     *
     *      @return List of the extend port abstractions.
     */
    QList<QSharedPointer<PortAbstraction> > getAllExtendPorts(QSharedPointer<AbstractionDefinition> abstraction,
        LibraryInterface* libraryAccess);

    /*!
     *  Get the extend abstraction definition of the selected abstraction definition.
     *
     *      @param [in] abstraction     The selected abstraction definition.
     *      @param [in] libraryAccess   Library interface.
     *
     *      @return Pointer to the extend abstraction definition of the selected abstraction definition.
     */
    QSharedPointer<const AbstractionDefinition> getExtendAbstraction(
        QSharedPointer<AbstractionDefinition> abstraction, LibraryInterface* libraryAccess);
};

#endif // ABSTRACTIONEXTENDHANDLER_H
