//-----------------------------------------------------------------------------
// File: portseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 31.03.2011
//
// Description:
// Editor to edit the ports of a component.
//-----------------------------------------------------------------------------

#ifndef PORTSEDITOR_H
#define PORTSEDITOR_H

#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <editors/ComponentEditor/itemeditor.h>
#include <editors/common/ExpressionSet.h>

#include <QTabWidget>

class BusInterface;
class BusInterfaceWizard;
class Component;
class LibraryInterface;
class PortValidator;
class MasterPortsEditor;
class PortsInterface;
class BusInterfaceInterface;
class PortsFilter;
class PortsModel;

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
	 *    @param [in] component               The component being edited.
	 *    @param [in] handler                 The instance that manages the library.
	 *    @param [in] parameterFinder         The parameter finder.
	 *    @param [in] expressionFormatter     The expression formatter.
     *    @param [in] portValidator           Validator used for ports.
     *    @param [in] busInterface            Interface for accessing bus interfaces.
	 *    @param [in] parent                  The owner of this widget.
	 */
	PortsEditor(QSharedPointer<Component> component,
        LibraryInterface* handler,
        ExpressionSet expressions,
        QSharedPointer<PortValidator> portValidator,
        QSharedPointer<PortsInterface> portsInterface,
        BusInterfaceInterface* busInterface,
        QWidget *parent = 0);

	/*!
     *  The destructor.
     */
	virtual ~PortsEditor() = default;

	/*!
     *  Check for the validity of the edited ports.
     *
     *    @return True if all ports are in valid state.
     */
	bool isValid() const;

	/*!
     *  Reload the information from the model to the editor.
	 */
	void refresh() final;

    /*!
     *  Sets the edited component.
     *
     *    @param [in] component   The component whose ports to edit.
     */
    virtual void setComponent(QSharedPointer<Component> component);

    //! No copying.
    PortsEditor(const PortsEditor& other) = delete;
    PortsEditor& operator=(const PortsEditor& other) = delete;


protected:

    /*!
     *  Called when the editor is shown.
     */
    virtual void showEvent(QShowEvent* event);

    /*!
     *  Get the associated ports model.
     *
     *    @return The ports model.
     */
    PortsModel* getModel() const;

private slots:

   

    /*!
     *  Change the vendor extensions of the vendor extensions editor.
     *
     *    @param [in] itemIndex   Index of the selected port.
     */
    void changeExtensionsEditorItem(QModelIndex const& itemIndex);


private:


    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component whose ports are being edited.
    QSharedPointer<Component> component_{ nullptr };

	//! The instance that manages the library.
	LibraryInterface* handler_{ nullptr };

    QSharedPointer<PortsInterface> portsInterface_{ nullptr };

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterface_{ nullptr };

    //! The ports model.
    PortsModel* model_{ nullptr };

    //! Filter for ports.
    PortsFilter* filter_{ nullptr };
};

#endif // PORTSEDITOR_H
