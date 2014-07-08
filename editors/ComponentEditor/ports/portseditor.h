/* 
 *
 *  Created on: 31.3.2011
 *      Author: Antti Kamppi
 * 		filename: portseditor.h
 */

#ifndef PORTSEDITOR_H
#define PORTSEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>
#include <common/views/EditableTableView/editabletableview.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/port.h>

#include "portsmodel.h"
#include "PortsView.h"

#include <QPushButton>
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
	 * \param handler Pointer to the instance that manages the library
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	PortsEditor(QSharedPointer<Component> component, 
		LibraryInterface* handler,
		QWidget *parent = 0);

	//! \brief The destructor
	virtual ~PortsEditor();

	/*! \brief Check for the validity of the edited ports.
	*
	* \return True if all ports are in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Reload the information from the model to the editor.
	*/
	virtual void refresh();

	/*! \brief Enable/disable the import/export csv functionality for the view.
	 *
	 * Method: 		setAllowImportExport
	 * Full name:	ModelParameterEditor::setAllowImportExport
	 * Access:		public 
	 *
	 * \param allow If true then import/export is enabled.
	 *
	*/
	void setAllowImportExport(bool allow);

signals:

    /*!
     *  Emitted when a locked port is removed from the model.
     */
    void lockedPortRemoved(QSharedPointer<Port> removedPort);

    /*!
     *  Emitted when a new interface should be added to the component editor tree view.
     */
    void createInterface();

public slots:

	/*! \brief Add a new port to the editor.
	 *
	 * Method: 		addPort
	 * Full name:	PortsEditor::addPort
	 * Access:		public 
	 *
	 * \param port The port to be added.
	 *
	*/
	void addPort(QSharedPointer<Port> port);

	/*! \brief Remove port from the editor.
	 *
	 * Method: 		removePort
	 * Full name:	PortsEditor::removePort
	 * Access:		public 
	 *
	 * \param portName The name of the port to be remove.
	 *
	*/
	void removePort(QSharedPointer<Port> port);

protected:
    //! Called when the editor is shown.
    virtual void showEvent(QShowEvent* event);

private slots:

    /*!
     *  Handler for new interface creation for new bus definition.
     *
     *      @param [in] selectedPorts   Ports to be mapped in the interface.
     */
    virtual void onCreateNewInteface(QStringList const& selectedPorts);

    /*!
     *  Handler for new interface creation for existing bus definition.
     *
     *      @param [in] selectedPorts   Ports to be mapped in the interface.
     */
    virtual void onCreateInterface(QStringList const& selectedPorts);

private:

	//! \brief No copying
	PortsEditor(const PortsEditor& other);

	//! No assignment
	PortsEditor& operator=(const PortsEditor& other);

	//! \brief The view that displays the parameters.
	PortsView view_;

	//! \brief The model that holds the data to be displayed to the user
	PortsModel model_;

	//! \brief Pointer to the proxy that is used to sort the view
	QSortFilterProxyModel proxy_;

    QSharedPointer<Component> component_; 

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;
};

#endif // PORTSEDITOR_H
