//-----------------------------------------------------------------------------
// File: ActiveViewEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 19.08.2011
//
// Description:
// Editor to set the active views for component instances.
//-----------------------------------------------------------------------------

#ifndef ACTIVEVIEWEDITOR_H
#define ACTIVEVIEWEDITOR_H

#include "activeviewmodel.h"

#include <QGroupBox>
#include <QTableView>
#include <QSortFilterProxyModel>

class DesignWidget;

//-----------------------------------------------------------------------------
//! Editor to set the active views for component instances.
//-----------------------------------------------------------------------------
class ActiveViewEditor : public QGroupBox
{
	Q_OBJECT

public:

    /*!
     *  The constructor
	 *
	 *      @param [in] parent  Pointer to the owner of this widget.
	 */
	ActiveViewEditor(QWidget *parent);

	//! The destructor.
	virtual ~ActiveViewEditor();

	/*!
     *  Set the configuration to be displayed.
	 *
	 *      @param [in] designWidget    Pointer to the design widget used to edit the design.
	 */
    void setDesign(DesignWidget* designWidget);

	/*!
     *  Clear the editor's contents.
	 */
	void clear();

	/*!
     *  Lock/unlock the editor so user can/can't edit the active views.
	 *
	 *      @param [in] locked  If true then editor is set to locked.
	 */
	void setLocked(bool locked);

signals:

	//! Emitted when contents of the model change.
	void contentChanged();

private:
	//! No copying.
	ActiveViewEditor(const ActiveViewEditor& other);

	//! No assignment.
	ActiveViewEditor& operator=(const ActiveViewEditor& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The view to display the instance names and active views.
	QTableView view_;

	//! The filter to do the sorting of items.
	QSortFilterProxyModel filter_;

	//! The model that contains the instanceName-viewName pairs.
	ActiveViewModel model_;
};

#endif // ACTIVEVIEWEDITOR_H
