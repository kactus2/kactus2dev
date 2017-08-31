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

#include <common/views/EditableTableView/editabletableview.h>

#include <QGroupBox>
#include <QSortFilterProxyModel>
#include <QSharedPointer>

class Component;
class TransparentBridge;

/*! Editor to edit the transparent bridge-elements within a slave interface mode or indirect interface.
 *
 */
class BridgesEditor : public QGroupBox {
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] bridges     The bridges to edit in the editor.
	 *      @param [in] component   The component that contains the bridges.
	 *      @param [in] parent      The parent of this editor.
	 *
	*/
	BridgesEditor(QSharedPointer<QList<QSharedPointer<TransparentBridge> > > bridges,
        QSharedPointer<Component> component,
        QWidget* parent);
	
	//! The destructor.
	virtual ~BridgesEditor();

    /*!
     *  Refresh the bridges in the editor.
     *
     *      @param [in] transparentBridges   The bridges to edit.
     */
    void refresh(QSharedPointer<QList<QSharedPointer<TransparentBridge> > > transparentBridges);

signals:

	//! Emitted when the contents of the bridges change.
	void contentChanged();

private:
	
	//! No copying
	BridgesEditor(const BridgesEditor& other);

	//! No assignment
	BridgesEditor& operator=(const BridgesEditor& other);

	//! The view to display the bridges.
	EditableTableView view_;

	//! The proxy that does the sorting.
	QSortFilterProxyModel proxy_;

	//! Model that manages the bridges.
	BridgesModel model_;
};

#endif // BRIDGESEDITOR_H
