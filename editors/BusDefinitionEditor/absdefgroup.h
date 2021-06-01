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

#include <editors/BusDefinitionEditor/AbstractionPortsEditor.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <QGroupBox>
#include <QPushButton>
#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QPlainTextEdit>
#include <QModelIndexList>
#include <QTabWidget>

class BusDefinition;
class VLNVDisplayer;
class VLNVEditor;
class LibraryInterface;
class AbstractionDefinitionPortsSortFilter;
class PortAbstractionInterface;

//-----------------------------------------------------------------------------
//! Editor for the logical ports of an abstraction definition.
//-----------------------------------------------------------------------------
class AbsDefGroup : public QGroupBox
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
	virtual ~AbsDefGroup();

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
     *  Handler for changing the description of the abstraction definition.
     */
    void onDescriptionChanged();

    /*!
     *  Handle the change in abstraction definition extend.
     */
    void onExtendChanged();

private:
	//! No copying. No assignment.
	AbsDefGroup(const AbsDefGroup& other);
	AbsDefGroup& operator=(const AbsDefGroup& other);

    /*!
     *  Sets the editor layout.
     */
	void setupLayout();
    
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
     *  Check if a port abstractions is within a list of port abstractions.
     *
     *      @param [in] logicalPort     The selected port abstraction.
     *      @param [in] currentPorts    List of port abstractions.
     *
     *      @return True, if the selected port is within the list of port abstractions, false otherwise.
     */
    bool portIsWithinPorts(QSharedPointer<PortAbstraction> logicalPort,
        QList<QSharedPointer<PortAbstraction> > const& currentPorts);

    /*!
     *  Extend the contained signals.
     *
     *       @param [in] extendAbstraction  The extended abstraction definition.
     */
    void extendSignals(QSharedPointer<const AbstractionDefinition> extendAbstraction);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Displays the VLNV of the abstraction definition.
    VLNVDisplayer* vlnvDisplay_;

    //! Editor for the abstraction definition extend.
    VLNVEditor* extendEditor_;

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
    QSharedPointer<AbstractionDefinition> abstraction_;

    //! The library interface.
    LibraryInterface* libraryHandler_;


 
};

#endif // ABSDEFGROUP_H
