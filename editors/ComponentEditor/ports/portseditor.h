//-----------------------------------------------------------------------------
// File: portseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 31.03.2011
//
// Description:
// Editor to edit the ports of a component.
//-----------------------------------------------------------------------------

#ifndef PORTSEDITOR_H
#define PORTSEDITOR_H

#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/itemeditor.h>

#include <common/views/EditableTableView/editabletableview.h>

#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QSharedPointer>

class Component;
class LibraryInterface;
class Port;
class PortsModel;
class PortsView;
class PortValidator;
//-----------------------------------------------------------------------------
//! Editor to edit the ports of a component.
//-----------------------------------------------------------------------------
class PortsEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component               The component being edited.
	 *      @param [in] handler                 The instance that manages the library.
	 *      @param [in] parameterFinder         The parameter finder.
	 *      @param [in] expressionFormatter     The expression formatter.
     *      @param [in] portValidator           Validator used for ports.
	 *      @param [in] parent                  The owner of this widget.
	 */
	PortsEditor(QSharedPointer<Component> component,
                LibraryInterface* handler,
                QSharedPointer<ParameterFinder> parameterFinder,
                QSharedPointer<ExpressionFormatter> expressionFormatter,
                QSharedPointer<PortValidator> portValidator,
                QWidget *parent = 0);

	//! The destructor
	virtual ~PortsEditor();

	/*! Check for the validity of the edited ports.
	*
	*      @return True if all ports are in valid state.
	*/
	virtual bool isValid() const;

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

	/*!
     *  Enable/disable the import/export csv functionality for the view.
	 *
	 *      @param [in] allow If true then import/export is enabled.
	 */
	void setAllowImportExport(bool allow);

    /*!
     *  Sets the edited component.
     *
     *      @param [in] component   The component whose ports to edit.
     */
    void setComponent(QSharedPointer<Component> component);

signals:

    /*!
     *  Emitted when a new interface should be added to the component editor tree view.
     */
    void createInterface();

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

	//! No copying
	PortsEditor(const PortsEditor& other);
	PortsEditor& operator=(const PortsEditor& other);

	//! The view that displays the parameters.
	PortsView* view_;

	//! The model that holds the data to be displayed to the user
	PortsModel* model_;

	//! The proxy that is used to sort the view
	QSortFilterProxyModel proxy_;

    //! The component whose ports are being edited.
    QSharedPointer<Component> component_; 

	//! The instance that manages the library.
	LibraryInterface* handler_;
};

#endif // PORTSEDITOR_H
