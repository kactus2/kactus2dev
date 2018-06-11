//-----------------------------------------------------------------------------
// File: AbstractionTransactionalPortsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 05.06.2018
//
// Description:
// Editor for the transactional ports of an abstraction definition.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIOTRANSACTIONALPORTSEDITOR_H
#define ABSTRACTIOTRANSACTIONALPORTSEDITOR_H

#include <editors/BusDefinitionEditor/AbstractionPortsView.h>
#include <editors/BusDefinitionEditor/AbstractionTransactionalPortsModel.h>
#include <editors/BusDefinitionEditor/AbstractionTransactionalPortsDelegate.h>

#include <QWidget>
#include <QObject>
#include <QModelIndexList>

class AbstractionDefinitionPortsSortFilter;
class AbstractionDefinition;
class BusDefinition;

//-----------------------------------------------------------------------------
//! Editor for the transactional ports of an abstraction definition.
//-----------------------------------------------------------------------------
class AbstractionTransactionalPortsEditor : public QWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent          The owner of the editor.
     */
    AbstractionTransactionalPortsEditor(QWidget *parent);

    /*!
     *  The destructor.
     */
    virtual ~AbstractionTransactionalPortsEditor() = default;

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
    AbstractionTransactionalPortsEditor(const AbstractionTransactionalPortsEditor& other);
    AbstractionTransactionalPortsEditor& operator=(const AbstractionTransactionalPortsEditor& other);

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

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The table view to display the logical signals.
    AbstractionPortsView portView_;

    //! Proxy model for sorting abstract ports.
    AbstractionDefinitionPortsSortFilter* portProxy_;

    //! The model that contains the logical signals of Abstraction Definition.
    AbstractionTransactionalPortsModel portModel_;

    //! The item delegate for portView_.
    AbstractionTransactionalPortsDelegate portDelegate_;
};

#endif // ABSTRACTIOTRANSACTIONALPORTSEDITOR_H
