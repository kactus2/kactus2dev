//-----------------------------------------------------------------------------
// File: bridgeseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.06.2012
//
// Description:
// Editor to edit the transparent bridge-elements within a slave interface mode or indirect interface.
//-----------------------------------------------------------------------------

#ifndef BRIDGESEDITOR_H
#define BRIDGESEDITOR_H

#include "bridgesmodel.h"

#include <IPXACTmodels/common/Document.h>

#include <common/views/EditableTableView/editabletableview.h>

#include <QGroupBox>
#include <QSortFilterProxyModel>
#include <QSharedPointer>

class Component;
class TransparentBridge;
class TransparentBridgeInterface;
class BusInterfaceInterface;
class TargetInterface;
class BusInterfaceValidator;
class IndirectInterface;
class IndirectInterfaceValidator;

 //-----------------------------------------------------------------------------
 // Editor to edit the transparent bridge-elements within a slave interface mode or indirect interface.
 //-----------------------------------------------------------------------------
class BridgesEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *    @param [in] busInterface        Interface for accessing bus interfaces.
	 *    @param [in] bridges             The bridges to edit in the editor.
     *    @param [in] currentRevision     Currently active IP-XACT revision.
	 *    @param [in] parent              The parent of this editor.
     */
    BridgesEditor(BusInterfaceInterface* busInterface,
        QSharedPointer<QList<QSharedPointer<TransparentBridge> > > bridges,
        Document::Revision currentRevision,
        QWidget* parent);

	/*!
     *  The destructor.
     */
	virtual ~BridgesEditor();

    /*!
     *  Refresh the bridges in the editor.
     */
    void refresh();

    /*!
     *  Setup the bridges for the editor.
     *
     *    @param [in] newBridges  The new bridges.
     */
    void setupBridges(QSharedPointer<QList<QSharedPointer<TransparentBridge> > > newBridges);

signals:

	/*!
     *  Emitted when the contents of the bridges change.
     */
	void contentChanged();

private:
	
    //! No copying. No assignment.
	BridgesEditor(const BridgesEditor& other);
	BridgesEditor& operator=(const BridgesEditor& other);

	//! The view to display the bridges.
	EditableTableView view_;

	//! The proxy that does the sorting.
	QSortFilterProxyModel proxy_;

	//! Model that manages the bridges.
	BridgesModel model_;

    //! Interface for accessing transparent bridges.
    TransparentBridgeInterface* bridgeInterface_;

    //! List of the available transparent bridges.
    QSharedPointer<QList<QSharedPointer<TransparentBridge> > > bridges_;
};

#endif // BRIDGESEDITOR_H
