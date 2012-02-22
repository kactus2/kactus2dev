/* 
 *  	Created on: 22.2.2012
 *      Author: Antti Kamppi
 * 		filename: segmenteditor.h
 *		Project: Kactus 2
 */

#ifndef SEGMENTEDITOR_H
#define SEGMENTEDITOR_H

#include <models/addressspace.h>

#include <common/views/EditableTableView/editabletableview.h>
#include "segmentsmodel.h"

#include <QGroupBox>
#include <QSortFilterProxyModel>

/*! \brief The editor to edit the segments of an address space.
 *
 */
class SegmentEditor : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param addrSpace Pointer to the address space being edited.
	 * \param parent Pointer to the owner of this editor.
	 *
	*/
	SegmentEditor(AddressSpace* addrSpace, QWidget *parent);
	
	//! \brief The destructor
	virtual ~SegmentEditor();

	/*! \brief Check if the editor is in valid state.
	 *
	 * \return True if the editor is in valid state.
	*/
	bool isValid() const;

	/*! \brief Read the settings from the address space to the editor.
	 *
	*/
	void restore();

	/*! \brief Write the settings from the editor to the address space.
	 *
	*/
	void makeChanges();

signals:

	//! \brief Emitted when the contents of the editor change.
	void contentChanged();

private:
	//! \brief No copying
	SegmentEditor(const SegmentEditor& other);

	//! \brief No assignment
	SegmentEditor& operator=(const SegmentEditor& other);

	//! \brief The view to display the segments.
	EditableTableView view_;

	//! \brief The proxy to do the sorting of segments.
	QSortFilterProxyModel proxy_;

	//! \brief The model that contains the segments.
	SegmentsModel model_;
};

#endif // SEGMENTEDITOR_H
