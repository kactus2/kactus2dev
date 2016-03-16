/* 
 *  	Created on: 21.6.2012
 *      Author: Antti Kamppi
 * 		filename: bridgeseditor.h
 *		Project: Kactus 2
 */

#ifndef BRIDGESEDITOR_H
#define BRIDGESEDITOR_H

#include "bridgesmodel.h"

#include <common/views/EditableTableView/editabletableview.h>

#include <IPXACTmodels/Component/SlaveInterface.h>

#include <QGroupBox>
#include <QSortFilterProxyModel>
#include <QSharedPointer>

class Component;
/*! Editor to edit the bridge-elements within a slave interface mode.
 *
 */
class BridgesEditor : public QGroupBox {
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] slave The slave element being edited.
	 *      @param [in] component The component that owns the slave interface.
	 *      @param [in] parent The owner of this editor.
	 *
	*/
	BridgesEditor(QSharedPointer<SlaveInterface> slave,	QSharedPointer<Component> component, QWidget* parent);
	
	//! The destructor.
	virtual ~BridgesEditor();

	//! Check if the editor is in valid state.
	bool isValid() const;

	/*! Refresh the editor to match the given slave interface.
	 *
	 *      @param [in] slave The slave interface being edited.
	 *
	*/
	void refresh(QSharedPointer<SlaveInterface> slave);

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
