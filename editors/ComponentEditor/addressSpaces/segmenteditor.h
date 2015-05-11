/* 
 *  	Created on: 22.2.2012
 *      Author: Antti Kamppi
 * 		filename: segmenteditor.h
 *		Project: Kactus 2
 */

#ifndef SEGMENTEDITOR_H
#define SEGMENTEDITOR_H

#include <IPXACTmodels/addressspace.h>
#include <IPXACTmodels/segment.h>
#include <IPXACTmodels/component.h>
#include <common/views/EditableTableView/editabletableview.h>
#include "segmentsmodel.h"
#include "segmentproxy.h"

#include <QSharedPointer>
#include <QGroupBox>

class LibraryInterface;
class ExpressionParser;
class ExpressionFormatter;
/*! The editor to edit the segments of an address space.
 *
 */
class SegmentEditor : public QGroupBox {
	Q_OBJECT

public:

    /*! The constructor
     *
     *      @param [in] addrSpace           The address space whose segments are edited.
     *      @param [in] component           The component being edited.
     *      @param [in] componentPath       The path to component xml file.
     *      @param [in] parameterFinder     Finder for available parameter names.
     *      @param [in] expressionParser    Parser for expressions.
     *      @param [in] expressionFormatter Formatter for expressions.
     *      @param [in] parent              Pointer to the owner of this editor.
     *
	 */
	SegmentEditor(QSharedPointer<AddressSpace> addrSpace, 
		QSharedPointer<Component> component,
        QString const& componentPath,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
            QSharedPointer<ExpressionFormatter> expressionFormatter,
		QWidget *parent);
	
	//! The destructor
	virtual ~SegmentEditor();

	/*! Check if the editor is in valid state.
	 *
	 *      @return True if the editor is in valid state.
	*/
	bool isValid() const;

	/*! Read the settings from the address space to the editor.
	 *
	*/
	void refresh();

signals:

	//! Emitted when the contents of the editor change.
	void contentChanged();

	//! Print an error message to the user.
	void errorMessage(const QString& msg);

	//! Print a notification to the user.
	void noticeMessage(const QString& msg);

	//! Emitted when a new segment is added to the address space.
	void segmentAdded(QSharedPointer<Segment> segment);

	//! Emitted when a segment is removed from the address space.
	void segmentRemoved(const QString& segmentName);

	//! Emitted when a segment is renamed.
	void segmentRenamed(const QString& oldName, const QString& newName);

	//! Emitted when the range or offset of a segment has changed.
	void segmentChanged(QSharedPointer<Segment> segment);

private:
	//! No copying
	SegmentEditor(const SegmentEditor& other);
	SegmentEditor& operator=(const SegmentEditor& other);

	//! The view to display the segments.
	EditableTableView view_;

	//! The proxy to do the sorting of segments.
	SegmentProxy proxy_;

	//! The model that contains the segments.
	SegmentsModel model_;

	//! Pointer to the component being edited.
	QSharedPointer<Component> component_;
};

#endif // SEGMENTEDITOR_H
