/* 
 *  	Created on: 21.6.2012
 *      Author: Antti Kamppi
 * 		filename: bridgeseditor.h
 *		Project: Kactus 2
 */

#ifndef BRIDGESEDITOR_H
#define BRIDGESEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>
#include <models/slaveinterface.h>
#include "bridgesmodel.h"

#include <QGroupBox>
#include <QSortFilterProxyModel>

/*! \brief Editor to edit the bridge-elements within a slave interface mode.
 *
 */
class BridgesEditor : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param slave Pointer to the slave element being edited.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	BridgesEditor(QSharedPointer<SlaveInterface> slave,
		QWidget *parent);
	
	//! \brief The destructor.
	virtual ~BridgesEditor();

	//! \brief Check if the editor is in valid state.
	bool isValid() const;

	/*! \brief Refresh the editor to match the given slave interface.
	 *
	 * \param slave Pointer to the slave interface being edited.
	 *
	*/
	void refresh(QSharedPointer<SlaveInterface> slave);

private:
	
	//! \brief No copying
	BridgesEditor(const BridgesEditor& other);

	//! \brief No assignment
	BridgesEditor& operator=(const BridgesEditor& other);

	//! \brief The view to display the bridges.
	EditableTableView view_;

	//! \brief The proxy that does the sorting.
	QSortFilterProxyModel proxy_;

	//! \brief Model that manages the bridges.
	BridgesModel model_;
};

#endif // BRIDGESEDITOR_H
