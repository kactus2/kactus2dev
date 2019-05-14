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

#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/itemeditor.h>

class Component;
class LibraryInterface;
class PortValidator;
class MasterPortsEditor;

#include <QTabWidget>

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

	/*!
     *  The destructor.
     */
	virtual ~PortsEditor() = default;

	/*!
     *  Check for the validity of the edited ports.
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

    //! No copying.
    PortsEditor(const PortsEditor& other) = delete;
    PortsEditor& operator=(const PortsEditor& other) = delete;

signals:

    /*!
     *  Emitted when a new interface should be added to the component editor tree view.
     */
    void createInterface();

protected:

    /*!
     *  Called when the editor is shown.
     */
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

    /*!
     *  Change the vendor extensions of the vendor extensions editor.
     *
     *      @param [in] itemIndex   Index of the selected port.
     */
    void changeExtensionsEditorItem(QModelIndex const& itemIndex);

    /*!
     *  Handles the changing of the help file.
     */
    void changeHelpFile();

    /*!
     *  Handles the redefinition of tab text.
     */
    void redefineTabText();

private:

    /*!
     *  Connect the signals.
     */
    void connectSignals();

    /*!
     *  Get the name for the tab.
     *
     *      @param [in] tabName     Basic name of the editor.
     *      @param [in] portCount   Current number of editor ports.
     *
     *      @return The name of the tab combined with the number of ports.
     */
    QString getTabNameWithPortCount(QString const& tabName, int const& portCount) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component whose ports are being edited.
    QSharedPointer<Component> component_; 

	//! The instance that manages the library.
	LibraryInterface* handler_;

    //! Editor for wire ports.
    MasterPortsEditor* wireEditor_;

    //! Editor for transactional ports.
    MasterPortsEditor* transactionalEditor_;

    //! Tabs for the port editors.
    QTabWidget* portTabs_;
};

#endif // PORTSEDITOR_H
