//-----------------------------------------------------------------------------
// File: ParametersView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.10.2017
//
// Description:
// View used to display parameters.
//-----------------------------------------------------------------------------

#ifndef PARAMETERSVIEW_H
#define PARAMETERSVIEW_H

#include <common/views/EditableTableView/editabletableview.h>

//-----------------------------------------------------------------------------
//! View used to display parameters.
//-----------------------------------------------------------------------------
class ParametersView : public EditableTableView
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] parent  Pointer to the owner of this view.
	 */
	ParametersView(QWidget* parent);

	/*!
     *  The destructor.
     */
    virtual ~ParametersView();

signals:

    /*!
     *  Recalculate references made to the parameters contained in the selected indexes.
     *
     *      @param [in] selectedIndexes     The selected indexes.
     */
    void recalculateReferenceToIndexes(QModelIndexList selectedIndexes);

protected:

	/*!
     *  Handler for context menu events
     */
	virtual void contextMenuEvent(QContextMenuEvent* event);

    /*!
     *  Set up the actions for the context menu
     */
    virtual void setupActions();

private slots:

    /*!
     *  Handles the action for recalculating the parameter references.
     */
    void onRecalculateReferencesAction();

private:

	//! No copying. No assignment.
	ParametersView(const ParametersView& other);
	ParametersView& operator=(const ParametersView& other);

    /*!
     *  Add the recalculate references action to the context menu.
     *
     *      @param [in] menu    The context menu for the actions.
     */
    void addRecalculateReferencesActionForContextMenu(QMenu& menu);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Action for recalculating references made to the selected parameter.
    QAction recalculateReferencesAction_;
};

#endif // PARAMETERSVIEW_H
