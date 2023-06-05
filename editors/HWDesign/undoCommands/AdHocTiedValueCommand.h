//-----------------------------------------------------------------------------
// File: AdHocTiedValueCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.05.2017
//
// Description:
// Parent undo command for ad hoc tied value commands.
//-----------------------------------------------------------------------------

#ifndef ADHOCTIEDVALUECOMMAND_H
#define ADHOCTIEDVALUECOMMAND_H

#include <QUndoCommand>
#include <QSharedPointer>

class AdHocItem;
class AdHocConnection;
class PortReference;
class Design;
class HWDesignDiagram;

//-----------------------------------------------------------------------------
//! Parent undo command for ad hoc tied value commands.
//-----------------------------------------------------------------------------
class AdHocTiedValueCommand
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] portItem                The selected port item.
     *      @param [in] tiedValueConnection     The tie off connection.
     *      @param [in] designDiagram           Diagram containing the HW design.
     */
    AdHocTiedValueCommand(AdHocItem* portItem, QSharedPointer<AdHocConnection> tiedValueConnection,
        HWDesignDiagram* designDiagram);

    /*!
     *  The destructor.
     */
    ~AdHocTiedValueCommand();

protected:

    /*!
     *  Get the tied value connection.
     *
     *      @return The connection containing the tied values.
     */
    QSharedPointer<AdHocConnection> getTiedValueConnection() const;

    /*!
     *  Get the referenced port of the tied value connection.
     *
     *      @return The referenced port.
     */
    QSharedPointer<PortReference> getReferencedPort() const;

    /*!
     *  Add or remove the tied value connection.
     */
    void addOrRemoveTiedValueConnection();

private:
    
    // Disable copying.
    AdHocTiedValueCommand(AdHocTiedValueCommand const& rhs);
    AdHocTiedValueCommand& operator=(AdHocTiedValueCommand const& rhs);

    /*!
     *  Create an ad hoc connection for the selected port item.
     *
     *      @param [in] portItem    The selected port item.
     *
     *      @return The ad hoc connection for containing the tie off value.
     */
    QSharedPointer<AdHocConnection> createConnectionForTiedValue(AdHocItem* portItem);

    /*!
     *  Create a name for a tied value connection.
     *
     *      @param [in] portItem    The port item whose connection is being created.
     *
     *      @return Name of the tied value connection, <InstanceName>_<portName>_to_tiedValue
     */
    QString createNameForTiedValueConnection(AdHocItem* portItem) const;

    /*!
     *  Setup the referenced port.
     *
     *      @param [in] portName    Name of the ad hoc port item.
     */
    void setupReferencedPort(QString const& portName);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The tie off connection.
    QSharedPointer<AdHocConnection> tiedValueConnection_;

    //! The containing design.
    QSharedPointer<Design> containingDesign_;

    //! The referenced port.
    QSharedPointer<PortReference> referencedPort_;
};

#endif // ADHOCTIEDVALUECOMMAND_H
