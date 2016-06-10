//-----------------------------------------------------------------------------
// File: absdefgroup.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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

class BusDefinition;

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
	 *      @param [in] parent      The owner of the editor.
	 */
	AbsDefGroup(QWidget *parent);
	
	//! The destructor.
	virtual ~AbsDefGroup();

	//! Saves the changes made in the editor.
	void save();

	/*! Set the abstraction definition for the editor.
	 *
	 *      @param [in] absDef  The Abstraction definition to edit.
	*/
	void setAbsDef(QSharedPointer<AbstractionDefinition> absDef);
    
	/*! Set the bus definition referenced by the abstraction definition.
	 *
	 *      @param [in] busDefinition  The referenced bus definition.
	*/
    void setBusDef(QSharedPointer<BusDefinition> busDefinition);

signals:

	//! Emitted when contents of this editor changes.
	void contentChanged();

	//! Emitted when an error should be printed to user.
	void errorMessage(QString const& message);

	//! Emitted when a notification should be printed to user.
	void noticeMessage(QString const& message);

    //! Inform that a port abstraction has been renamed.
    void portRenamed(QString const& oldName, QString const& newName);

    //! Inform that a port abstraction has been removed.
    void portRemoved(QString const& portName, General::InterfaceMode const mode);

private slots:

	//! Handler for add new signal options button clicks
	void onAddSignalOptions();

private:
	//! No copying
	AbsDefGroup(const AbsDefGroup& other);

	//! No assignment
	AbsDefGroup& operator=(const AbsDefGroup& other);

    //! Sets the editor layout.
	void setupLayout();
    
	//! The table view to display the logical signals
	BusPortsView portView_;

	//! The model that contains the logical signals of Abstraction Definition.
	BusPortsModel portModel_;

    //! The item delegate for portView_.
    BusPortsDelegate portDelegate_;

};

#endif // ABSDEFGROUP_H
