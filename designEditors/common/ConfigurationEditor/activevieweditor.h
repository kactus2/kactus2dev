/* 
 *  	Created on: 19.8.2011
 *      Author: Antti Kamppi
 * 		filename: activevieweditor.h
 *		Project: Kactus 2
 */

#ifndef ACTIVEVIEWEDITOR_H
#define ACTIVEVIEWEDITOR_H

#include "activeviewmodel.h"

#include <QGroupBox>
#include <QTableView>
#include <QSortFilterProxyModel>

class DesignWidget;

/*! \brief Editor to set the active views for component instances.
 *
 */
class ActiveViewEditor : public QGroupBox {
	Q_OBJECT

public:
	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	ActiveViewEditor(QWidget *parent);

	//! \brief The destructor
	virtual ~ActiveViewEditor();

	/*! \brief Set the configuration to be displayed.
	 *
	 * \param designWidget Pointer to the design widget used to edit the design.
	 *
	*/
    void setDesign(DesignWidget* designWidget);

	/*! \brief Clear the editor's contents.
	 *
	*/
	void clear();

	/*! \brief Lock/unlock the editor so user can/can't edit the active views.
	 *
	 * \param locked If true then editor is set to locked.
	 *
	*/
	void setLocked(bool locked);

signals:

	//! \brief Emitted when contents of the model change.
	void contentChanged();

private:
	//! \brief No copying
	ActiveViewEditor(const ActiveViewEditor& other);

	//! \brief No assignment
	ActiveViewEditor& operator=(const ActiveViewEditor& other);

	//! \brief The view to display the instance names and active views.
	QTableView view_;

	//! \brief The filter to do the sorting of items.
	QSortFilterProxyModel filter_;

	//! \brief The model that contains the instanceName-viewName pairs.
	ActiveViewModel model_;
};

#endif // ACTIVEVIEWEDITOR_H
