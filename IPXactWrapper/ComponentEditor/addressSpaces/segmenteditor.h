/* 
 *  	Created on: 22.2.2012
 *      Author: Antti Kamppi
 * 		filename: segmenteditor.h
 *		Project: Kactus 2
 */

#ifndef SEGMENTEDITOR_H
#define SEGMENTEDITOR_H

#include <models/addressspace.h>
#include <models/segment.h>

#include <common/views/EditableTableView/editabletableview.h>
#include "segmentsmodel.h"
#include "segmentproxy.h"

#include <QSharedPointer>
#include <QGroupBox>

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
	SegmentEditor(QSharedPointer<AddressSpace> addrSpace, 
		QWidget *parent);
	
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
	void refresh();

signals:

	//! \brief Emitted when the contents of the editor change.
	void contentChanged();

	//! \brief Print an error message to the user.
	void errorMessage(const QString& msg);

	//! \brief Print a notification to the user.
	void noticeMessage(const QString& msg);

	//! \brief Emitted when a new segment is added to the address space.
	void segmentAdded(QSharedPointer<Segment> segment);

	//! \brief Emitted when a segment is removed from the address space.
	void segmentRemoved(const QString& segmentName);

	//! \brief Emitted when a segment is renamed.
	void segmentRenamed(const QString& oldName, const QString& newName);

	//! \brief Emitted when the range or offset of a segment has changed.
	void segmentChanged(QSharedPointer<Segment> segment);

private:
	//! \brief No copying
	SegmentEditor(const SegmentEditor& other);

	//! \brief No assignment
	SegmentEditor& operator=(const SegmentEditor& other);

	//! \brief The view to display the segments.
	EditableTableView view_;

	//! \brief The proxy to do the sorting of segments.
	SegmentProxy proxy_;

	//! \brief The model that contains the segments.
	SegmentsModel model_;
};

#endif // SEGMENTEDITOR_H
