//-----------------------------------------------------------------------------
// File: AbstractionWirePortsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.06.2018
//
// Description:
// Editor for the wire ports of an abstraction definition.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONWIREPORTSEDITOR_H
#define ABSTRACTIONWIREPORTSEDITOR_H

#include <IPXACTmodels/generaldeclarations.h>

#include <editors/BusDefinitionEditor/AbstractionWirePortsDelegate.h>
#include <editors/BusDefinitionEditor/AbstractionPortsView.h>

#include <QWidget>
#include <QObject>

class AbstractionPortsModel;
class AbstractionWirePortsSortFilter;
class AbstractionDefinition;
class BusDefinition;

//-----------------------------------------------------------------------------
//! Editor for the wire ports of an abstraction definition.
//-----------------------------------------------------------------------------
class AbstractionWirePortsEditor : public QWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] portsModel      Model for this editor.
     *      @param [in] libraryAccess   Interface to the library.
     *      @param [in] parent          The owner of the editor.
     */
    AbstractionWirePortsEditor(AbstractionPortsModel* portsModel, LibraryInterface* libaryAccess, QWidget *parent);

    /*!
     *  The destructor.
     */
    virtual ~AbstractionWirePortsEditor() = default;

    /*!
     *  Saves the changes made in the editor.
     */
    void save();

    /*!
     *  Set the abstraction definition for the editor.
     *
     *      @param [in] absDef  The Abstraction definition to edit.
     */
    void setAbsDef(QSharedPointer<AbstractionDefinition> absDef);

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
    AbstractionWirePortsEditor(const AbstractionWirePortsEditor& other);
    AbstractionWirePortsEditor& operator=(const AbstractionWirePortsEditor& other);

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

    /*!
     *  Hide the transactional columns
     */
    void hideTransactionalColumns();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The table view to display the logical signals
    AbstractionPortsView portView_;

    //! Proxy model for sorting abstract ports.
    AbstractionWirePortsSortFilter* portProxy_;

    //! The model that contains the logical signals of Abstraction Definition.
    AbstractionPortsModel* portModel_;

    //! The item delegate for portView_.
    AbstractionWirePortsDelegate portDelegate_;
};

#endif // ABSTRACTIONWIREPORTSEDITOR_H
