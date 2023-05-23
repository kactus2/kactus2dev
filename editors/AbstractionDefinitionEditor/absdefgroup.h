//-----------------------------------------------------------------------------
// File: absdefgroup.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 15.8.2011
//
// Description:
// Editor for the logical ports of an abstraction definition.
//-----------------------------------------------------------------------------

#ifndef ABSDEFGROUP_H
#define ABSDEFGROUP_H

#include "AbstractionPortsEditor.h"

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <QWidget>
#include <QPushButton>
#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QPlainTextEdit>
#include <QModelIndexList>
#include <QTabWidget>

class VLNVDisplayer;
class VLNVEditor;
class LibraryInterface;
class AbstractionDefinitionPortsSortFilter;
class PortAbstractionInterface;

//-----------------------------------------------------------------------------
//! Editor for the logical ports of an abstraction definition.
//-----------------------------------------------------------------------------
class AbsDefGroup : public QWidget
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] libraryHandler  Allows access to the library.
     *      @param [in] portInterface   Interface for accssing port abstractions.
	 *      @param [in] parent          The owner of the editor.
	 */
	AbsDefGroup(LibraryInterface* libraryHandler, PortAbstractionInterface* portInterface, 
        PortAbstractionInterface* extendInterface, QWidget *parent);
	
	/*!
     *  The destructor.
     */
	~AbsDefGroup() override = default;

    //! No copying. No assignment.
    AbsDefGroup(const AbsDefGroup& other) = delete;
    AbsDefGroup& operator=(const AbsDefGroup& other) = delete;

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
     *  Handler for changing the description of the abstraction definition.
     */
    void onDescriptionChanged();

    /*!
     *  Handle the change in abstraction definition extend.
     */
    void onExtendChanged();

private:
    
    /*!
     *  Check if the abstraction definition contains transactional ports.
     *
     *      @return True, if the abstraction definition contains any transactional ports.
     */
    bool abstractionContainsTransactionalPorts() const;

    /*!
     *  Setup the extended abstraction definition.
     */
    void setupExtendedAbstraction();

    /*!
     *  Get the extended abstraction definition.
     *
     *       @return The extended abstraction definition.
     */
    QSharedPointer<const AbstractionDefinition> getExtendedAbstraction() const;

    /*!
     *  Remove extend port abstractions.
     */
    void removeSignalsFromExtendedDefinition();

    /*!
     *  Extend the contained signals.
     *
     *       @param [in] extendAbstraction  The extended abstraction definition.
     */
    void extendSignals(QSharedPointer<const AbstractionDefinition> extendAbstraction);

    /*!
     *  Sets the editor layout.
     */
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Displays the VLNV of the abstraction definition.
    VLNVDisplayer* vlnvDisplay_;

    //! Editor for the abstraction definition extend.
    VLNVEditor* extendEditor_;

    //! Displays the VLNV of the referenced bus definition.
    VLNVDisplayer* busDisplay_;

    //! Editor for the description of the abstraction definition.
    QPlainTextEdit* descriptionEditor_;

    //! Contains tabs for wire and transactional ports.
    QTabWidget portTabs_;

    //! Interface for accessing port abstractions.
    PortAbstractionInterface* portInterface_;

    //! Interface for accessing extened port abstractions.
    PortAbstractionInterface* extendInterface_;

    //! The abstraction definition ports model.
    AbstractionPortsModel* portModel_;

    //! Editor for wire ports.
    AbstractionPortsEditor* wirePortsEditor_;

    //! Editor for transactional ports.
    AbstractionPortsEditor* transactionalPortsEditor_;

    //! The edited abstraction definition.
    QSharedPointer<AbstractionDefinition> abstraction_ = nullptr;

    //! The library interface.
    LibraryInterface* libraryHandler_;
 
};

#endif // ABSDEFGROUP_H
