//-----------------------------------------------------------------------------
// File: EndpointEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.9.2011
//
// Description:
// Endpoint editor widget for the IP-XACT component editor.
//-----------------------------------------------------------------------------

#ifndef ENDPOINTEDITOR_H
#define ENDPOINTEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>

#include <models/modelparameter.h>
#include "EndpointModel.h"

#include <QTableView>
#include <QPushButton>
#include <QString>
#include <QMap>
#include <QList>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

class Component;

//-----------------------------------------------------------------------------
//! EndpointEditor class.
//-----------------------------------------------------------------------------
class EndpointEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param dataPointer Pointer to the QMap containing the endpoints (model parameters).
	 * \param parent Pointer to the owner of this widget.
	 * 
	*/
	EndpointEditor(QSharedPointer<Component> component, 
		           void* dataPointer, QWidget *parent);

	//! \brief The destructor
	virtual ~EndpointEditor();

	/*! \brief Check for the validity of the edited endpoints.
	*
	* \return True if all endpoints are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Make the changes from the widgets editors to the IPXact model.
	 *
	*/
	virtual void makeChanges();

private slots:

	//! \brief Remove the selected row from the model
	void onRemove();

private:

	//! \brief No copying
	EndpointEditor(const EndpointEditor& other);

	//! No assignment
	EndpointEditor& operator=(const EndpointEditor& other);
	
	//! \brief The button to add a new endpoint
	QPushButton addRowButton_;

	//! \brief The button to remove the selected endpoint
	QPushButton removeRowButton_;

	//! \brief The view that displays the parameters.
	QTableView view_;

	//! \brief The model that holds the data to be displayed to the user
	EndpointModel model_;

	//! \brief Pointer to the proxy that is used to sort the view
	QSortFilterProxyModel* proxy_;
};

//-----------------------------------------------------------------------------

#endif // ENDPOINTEDITOR_H
