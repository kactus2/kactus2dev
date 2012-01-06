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
class LibraryInterface;

//-----------------------------------------------------------------------------
//! EndpointEditor class.
//-----------------------------------------------------------------------------
class EndpointEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
     *  Constructor.
	 *
     *      @param [in]     lh           The library interface.
	 *      @param [in]     component    Pointer to the component being edited.
	 *      @param [in,out] dataPointer  Pointer to the QMap containing the endpoints (model parameters).
     *      @param [in]     parentWnd    The parent window.
     *      @param [in]     parent       Pointer to the owner of this widget.
     *      @param [in]     showRemote   If true, the remote endpoint names are visible and editable in the editor.
	 */
	EndpointEditor(LibraryInterface* lh, QSharedPointer<Component> component,
		           void* dataPointer, QWidget* parentWnd, QWidget* parent, bool showRemote = false);

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
	/*!
     *  Removes the selected row from the model.
     */
	void onRemove();

    /*!
     *  Imports endpoints by analyzing the source code.
     */
    void onAnalyzeSource();

    /*!
     *  Imports endpoints from an endpoint component.
     */
    void onImportEndpoints();

private:

	//! \brief No copying
	EndpointEditor(const EndpointEditor& other);

	//! No assignment
	EndpointEditor& operator=(const EndpointEditor& other);

    //! The library interface.
    LibraryInterface* lh_;

    //! The parent window.
    QWidget* parentWnd_;
	
	//! \brief The button to add a new endpoint
	QPushButton addRowButton_;

	//! \brief The button to remove the selected endpoint
	QPushButton removeRowButton_;

    //! Button to analyze endpoints from source code.
    QPushButton analyzeSourceButton_;

    //! Button to import endpoints from an endpoint component.
    QPushButton importEndpointsButton_;

	//! \brief The view that displays the parameters.
	QTableView view_;

	//! \brief The model that holds the data to be displayed to the user
	EndpointModel model_;

	//! \brief Pointer to the proxy that is used to sort the view
	QSortFilterProxyModel* proxy_;
};

//-----------------------------------------------------------------------------

#endif // ENDPOINTEDITOR_H
