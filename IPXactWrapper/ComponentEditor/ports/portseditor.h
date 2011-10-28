/* 
 *
 *  Created on: 31.3.2011
 *      Author: Antti Kamppi
 * 		filename: portseditor.h
 */

#ifndef PORTSEDITOR_H
#define PORTSEDITOR_H

#include <IPXactWrapper/ComponentEditor/itemeditor.h>

#include "portsmodel.h"

#include <QPushButton>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QSharedPointer>

class LibraryInterface;

/*! \brief Editor to edit the ports of a component.
 *
 */
class PortsEditor : public ItemEditor {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component being edited.
	 * \param dataPointer Pointer to the QMap containing pointers to the ports.
	 * \param handler Pointer to the instance that manages the library
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	PortsEditor(QSharedPointer<Component> component, 
		void* dataPointer,
		LibraryInterface* handler,
		QWidget *parent);

	//! \brief The destructor
	virtual ~PortsEditor();

	/*! \brief Check for the validity of the edited model parameters.
	*
	* \return True if all model parameters are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Make the changes from the widgets editors to the IPXact model.
	 *
	*/
	virtual void makeChanges();

private slots:

	//! \brief Remove the selected row from the model
	void onRemove();

	//! \brief Import a csv file to ports
	void onImport();

	//! \brief Export ports to a csv file
	void onExport();

private:

	//! \brief No copying
	PortsEditor(const PortsEditor& other);

	//! No assignment
	PortsEditor& operator=(const PortsEditor& other);

	//! \brief The button to add a new port
	QPushButton addRowButton_;

	//! \brief The button to remove the selected port
	QPushButton removeRowButton_;

	//! \brief The button to import a csv file
	QPushButton importButton_;

	//! \brief The button to export a csv file
	QPushButton exportButton_;

	//! \brief The view that displays the parameters.
	QTableView view_;

	//! \brief The model that holds the data to be displayed to the user
	PortsModel model_;

	//! \brief Pointer to the proxy that is used to sort the view
	QSortFilterProxyModel* proxy_;

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;
};

#endif // PORTSEDITOR_H
