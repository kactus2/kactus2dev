//-----------------------------------------------------------------------------
// File: AbstractionPortsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.06.2018
//
// Description:
// Editor for the ports of an abstraction definition.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONPORTSEDITOR_H
#define ABSTRACTIONPORTSEDITOR_H

#include <IPXACTmodels/generaldeclarations.h>

#include "AbstractionPortsView.h"
#include "LogicalPortColumns.h"

#include <QWidget>
#include <QObject>

class AbstractionPortsModel;
class AbstractionDefinitionPortsSortFilter;
class AbstractionDefinition;
class AbstractionPortsDelegate;
class BusDefinition;
class PortAbstractionInterface;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Editor for the ports of an abstraction definition.
//-----------------------------------------------------------------------------
class AbstractionPortsEditor : public QWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *     
     *      @param [in] libraryAccess   Interface to the library.
     *      @param [in] portInterface   Interface for accessing port abstractions.
     *      @param [in] portsModel      Model for this editor.
     *      @param [in] type            The type of ports the editor is targeted for: wire or transactional.
     *      @param [in] parent          The owner of the editor.
     */
    AbstractionPortsEditor(LibraryInterface* libraryAccess, PortAbstractionInterface* portInterface,
        AbstractionPortsModel* portModel,
        LogicalPortColumns::AbstractionType type, QWidget* parent);

    /*!
     *  The destructor.
     */
    virtual ~AbstractionPortsEditor() = default;

    /*!
     *  Reset the port abstraction model.
     */
    void resetPortModel();

    /*!
     *  Set the bus definition referenced by the abstraction definition.
     *
     *      @param [in] busDefinition   The referenced bus definition.
     */
    void setBusDef(QSharedPointer<BusDefinition> busDefinition);

signals:

    /*!
     *  Emitted when contents of this editor changes.
     */
    void contentChanged();

    /*!
     *  Emitted when an error should be printed to user.
     */
    void errorMessage(QString const& message);

    /*!
     *  Emitted when a notification should be printed to user.
     */
    void noticeMessage(QString const& message);

    /*!
     *  Inform that a port abstraction has been removed.
     *
     *      @param [in] portName    Name of the removed port abstraction.
     *      @param [in] mode        Mode of the removed port abstraction.
     */
    void portRemoved(QString const& portName, General::InterfaceMode const mode);

private slots:

    /*!
     *  Handles the adding of new master signals.
     */
    void onAddMaster();

    /*!
     *  Handles the adding of new slave signals.
     */
    void onAddSlave();

    /*!
     *  Handles the adding of new system signals.
     */
    void onAddSystem();

    /*!
     *  Handles the adding of missing system group signals.
     */
    void onAddAllSystems();

private:
    //! No copying. No assignment.
    AbstractionPortsEditor(const AbstractionPortsEditor& other);
    AbstractionPortsEditor& operator=(const AbstractionPortsEditor& other);

    /*!
     *  Sets the editor layout.
     */
    void setupLayout();

   
    /*!
     *  Get a list of the selected indexes.
     *
     *      @return List of the selected indexes.
     */
    QModelIndexList getSelectedIndexes();

    //! Hide columns specific to transactional ports.
    void hideTransactionalColumns();

    //! Hide columns specific to wire ports.
    void hideWireColumns();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The table view to display the logical signals
    AbstractionPortsView portView_;

    //! Proxy model for sorting abstract ports.
    AbstractionDefinitionPortsSortFilter* portProxy_;

    //! The model that contains the logical signals of Abstraction Definition.
    AbstractionPortsModel* portModel_;

    //! The item delegate for portView_.
    AbstractionPortsDelegate* portDelegate_;

};

#endif // ABSTRACTIONPORTSEDITOR_H
