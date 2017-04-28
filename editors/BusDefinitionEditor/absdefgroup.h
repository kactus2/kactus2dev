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

#include "AbsDefTreeView.h"
#include "AbsDefTreeModel.h"

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <QGroupBox>
#include <QPushButton>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

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
	AbsDefTreeView portView_;

    //! Proxy model for sorting abstract ports.
    QSortFilterProxyModel portProxy_;

	//! The model that contains the logical signals of Abstraction Definition.
	AbsDefTreeModel portModel_;

    //! The item delegate for portView_.
    BusPortsDelegate portDelegate_;

};

#endif // ABSDEFGROUP_H
