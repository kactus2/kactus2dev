//-----------------------------------------------------------------------------
// File: hierarchywidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 01.07.2011
//
// Description:
// The container for hierarchy view and hierarchy filter.
//-----------------------------------------------------------------------------

#ifndef HIERARCHYWIDGET_H
#define HIERARCHYWIDGET_H

#include "hierarchyview.h"
#include "hierarchyfilter.h"

#include <IPXACTmodels/common/VLNV.h>

#include <QWidget>

class HierarchyModel;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! The container for hierarchy view and hierarchy filter.
//-----------------------------------------------------------------------------
class HierarchyWidget : public QWidget
{
	Q_OBJECT

public:

	/*! The constructor
	 * 
	 *      @param [in] parent      The owner of this widget.
	 *      @param [in] handler     The instance that manages library.
	 *      @param [in] dataModel   The model that provides data to view and filter.
	 */
    HierarchyWidget(LibraryInterface* handler, HierarchyModel* dataModel, QWidget* parent);
	
	//! The destructor
	virtual ~HierarchyWidget();

	/*! Select the library items that represent the given vlnv.
	 *
	 *      @param [in] vlnv Identifies the objects to select.
	 */
	void selectItems(const VLNV& vlnv);
    	
    /*!
     *  Gets the filter for the tree model.
     *
     *      @return The filter in use.
     */
    LibraryFilter* getFilter() const;

signals:

	//! Send an error message to be printed to user.
	void errorMessage(const QString& msg);

	//! Send a notification to be printed to user.
	void noticeMessage(const QString& msg);

	//! The specified component has been selected in the view.
	void componentSelected(const VLNV& vlnv);

private:
	//! No copying
	HierarchyWidget(const HierarchyWidget& other);

	//! No assignment
	HierarchyWidget& operator=(const HierarchyWidget& other);

	//! Set up the connections between child widgets
	void setupConnections(HierarchyModel* dataModel);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The filter to sort and filter the components.
	HierarchyFilter* filter_;

	//! The view to display the library components.
	HierarchyView view_;

	//! The model that contains the items to display.
	HierarchyModel* model_;
};

#endif // HIERARCHYWIDGET_H
