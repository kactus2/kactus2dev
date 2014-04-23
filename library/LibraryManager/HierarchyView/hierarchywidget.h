/* 
 *  	Created on: 1.7.2011
 *      Author: Antti Kamppi
 * 		filename: hierarchywidget.h
 *		Project: Kactus 2
 */

#ifndef HIERARCHYWIDGET_H
#define HIERARCHYWIDGET_H

#include "hierarchyview.h"
#include "hierarchyfilter.h"

#include <library/LibraryManager/vlnv.h>

#include <QWidget>

class HierarchyModel;
class LibraryInterface;
class VLNVDialer;

/*! \brief The container for hierarchy view and hierarchy filter.
 *
 */
class HierarchyWidget : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor
	 * 
	 * \param dialer Pointer to the dialer that provides search options.
	 * \param parent Pointer to the owner of this widget.
	 * \param handler Pointer to the instance that manages library.
	 * \param dataModel Pointer to the model that provides data to view and filter.
	 *
	*/
	HierarchyWidget(VLNVDialer* dialer, 
		QWidget *parent, 
		LibraryInterface* handler,
		HierarchyModel* dataModel);
	
	//! \brief The destructor
	virtual ~HierarchyWidget();

	/*! \brief Select the library items that represent the given vlnv.
	 *
	 * \param vlnv Identifies the objects to select.
	 *
	*/
	void selectItems(const VLNV& vlnv);

signals:

	//! \brief Send an error message to be printed to user.
	void errorMessage(const QString& msg);

	//! \brief Send a notification to be printed to user.
	void noticeMessage(const QString& msg);

	//! \brief The specified component has been selected in the view.
	void componentSelected(const VLNV& vlnv);

private:
	//! \brief No copying
	HierarchyWidget(const HierarchyWidget& other);

	//! \brief No assignment
	HierarchyWidget& operator=(const HierarchyWidget& other);

	//! \brief Set up the connections between child widgets
	void setupConnections(HierarchyModel* dataModel);

	//! \brief The filter to sort and filter the components.
	HierarchyFilter filter_;

	//! \brief The view to display the library components.
	HierarchyView view_;

	//! \brief Pointer to the model that contains the items to display.
	HierarchyModel* model_;
};

#endif // HIERARCHYWIDGET_H
