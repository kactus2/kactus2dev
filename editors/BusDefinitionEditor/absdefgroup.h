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

#include "busportsview.h"
#include "busportsmodel.h"
#include "busportsdelegate.h"

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <QGroupBox>
#include <QPushButton>
#include <QSharedPointer>
#include <QSortFilterProxyModel>
#include <QPlainTextEdit>

class BusDefinition;
class VLNVDisplayer;
class VLNVEditor;
class LibraryInterface;
class AbstractionDefinitionPortsSortFilter;

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
	 *      @param [in] parent          The owner of the editor.
	 */
	AbsDefGroup(LibraryInterface* libraryHandler, QWidget *parent);
	
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
     *  Inform that a port abstraction has been renamed.
     *
     *      @param [in] oldName     Old name of the port abstraction.
     *      @param [in] newName     New name for the port abstraction.
     */
    void portRenamed(QString const& oldName, QString const& newName);

    /*!
     *  Inform that a port abstraction has been removed.
     *
     *      @param [in] portName    Name of the removed port abstraction.
     *      @param [in] mode        Mode of the removed port abstraction.
     */
    void portRemoved(QString const& portName, General::InterfaceMode const mode);

private slots:

	/*!
     *  Handler for add new signal options button clicks.
     */
	void onAddSignalOptions();

	/*!
     *  Handler for changing the description of the abstraction definition.
     */
    void onDescriptionChanged();

private:
	//! No copying. No assignment.
	AbsDefGroup(const AbsDefGroup& other);
	AbsDefGroup& operator=(const AbsDefGroup& other);

    /*!
     *  Sets the editor layout.
     */
	void setupLayout();
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The table view to display the logical signals
	BusPortsView portView_;

    //! Proxy model for sorting abstract ports.
    AbstractionDefinitionPortsSortFilter* portProxy_;

	//! The model that contains the logical signals of Abstraction Definition.
	BusPortsModel portModel_;

    //! The item delegate for portView_.
    BusPortsDelegate portDelegate_;

    //! Displays the VLNV of the abstraction definition.
    VLNVDisplayer* vlnvDisplay_;

    //! Editor for the abstraction definition extend.
    VLNVEditor* extendVLNVEditor_;

    //! Editor for the description of the abstraction definition.
    QPlainTextEdit* descriptionEditor_;

    //! The edited abstraction definition.
    QSharedPointer<AbstractionDefinition> abstraction_;
};

#endif // ABSDEFGROUP_H
