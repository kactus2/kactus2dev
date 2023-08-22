//-----------------------------------------------------------------------------
// File: MasterPortsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2019
//
// Description:
// Master editor for component ports.
//-----------------------------------------------------------------------------

#ifndef MASTERPORTSEDITOR_H
#define MASTERPORTSEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>

#include <QAbstractItemModel>
#include <QSharedPointer>
#include <QWidget>

class ExpressionParser;
class ParameterFinder;
class ExpressionFormatter;
class LibraryInterface;
class ParameterCompleter;
class Component;
class Port;
class PortsModel;
class PortsView;
class PortValidator;
class PortsDelegate;
class PortsFilter;
class PortsEditorConstructor;
class PortsInterface;
class BusInterfaceInterface;
class PortAbstractionInterface;

//-----------------------------------------------------------------------------
//! Master editor for component ports.
//-----------------------------------------------------------------------------
class MasterPortsEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component           The component being edited.
	 *      @param [in] handler             The instance that manages the library.
     *      @param [in] portsInterface      Interface for accessing the component ports.
     *      @param [in] signalInterface     Interface for accessing logical ports.
     *      @param [in] editorConstructor   Constructor for required modules.
     *      @param [in] parameterFinder     Locates the different parameters of the containing component.
     *      @param [in] portValidator       Validator used for ports.
     *      @param [in] completionModel     Model containing the completions used in expression editor.
     *      @param [in] defaultPath         The default import / export path.
     *      @param [in] busInterface        Interface for accessing bus interfaces.
	 *      @param [in] parent              The owner of this widget.
	 */
    MasterPortsEditor(QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<PortsInterface> portsInterface,
        QSharedPointer<PortAbstractionInterface> signalInterface,
        PortsEditorConstructor* editorConstructor,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<PortValidator> portValidator,
        QAbstractItemModel* completionModel,
        QString const& defaultPath,
        BusInterfaceInterface* busInterface,
        QWidget *parent = 0);

	/*!
     *  The destructor.
     */
    virtual ~MasterPortsEditor() = default;

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
	 *      @param [in] allow   If true then import/export is enabled.
	 */
	void setAllowImportExport(bool allow);

    /*!
     *  Sets the edited component.
     *
     *      @param [in] component   The component whose ports to edit.
     */
    void setComponent(QSharedPointer<Component> component);

    /*!
     *  Get the port for the selected index.
     *
     *      @param [in] portIndex   The selected index.
     *
     *      @return Port from the selected index.
     */
    QSharedPointer<Port> getIndexedPort(QModelIndex const& portIndex) const;

    /*!
     *  Get the current amount of visible ports in the editor.
     *
     *      @return The amount of visible ports.
     */
    int getAmountOfPorts() const;

    //! No copying.
    MasterPortsEditor(const MasterPortsEditor& other) = delete;
    MasterPortsEditor& operator=(const MasterPortsEditor& other) = delete;

signals:

    /*!
     *  Handler for new interface creation for new bus definition.
     *
     *      @param [in] selectedPorts   Ports to be mapped in the interface.
     */
    void createNewInteface(QStringList const& selectedPorts);

    /*!
     *  Handler for new interface creation for existing bus definition.
     *
     *      @param [in] selectedPorts   Ports to be mapped in the interface.
     */
    void createInterface(QStringList const& selectedPorts);

    /*!
     *  Change the vendor extensions of the vendor extensions editor.
     *
     *      @param [in] itemIndex   Index of the selected port.
     */
    void changeExtensionsEditorItem(QModelIndex const& itemIndex);

    /*!
     *  Invalidate the filter from the other ports editor.
     */
    void invalidateOtherFilter();

    /*!
     *  Invalidate the filter from this editor.
     */
    void ivalidateThisFilter();

    /*!
     *  Inform that the port count has changed.
     */
    void portCountChanged();

private slots:

    /*!
     *  Handle port creation from the selected abstraction definition of a bus interface.
     *
     *      @param [in] busName             Name of the selected bus interface.
     *      @param [in] abstractionString   VLNV string of the abstraction definition.
     */
    void onCreatePortsFromAbstraction(std::string const& busName, QString const& abstractionString);

private:

    /*!
     *  Connect the editor signals.
     */
    void connectSignals();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The view that displays the ports.
	PortsView* view_;

	//! The model that holds the data to be displayed to the user.
	PortsModel* model_;

	//! The proxy that is used to sort the view.
	PortsFilter* proxy_;

    //! The delegate for ports.
    PortsDelegate* delegate_;

    //! Interface for accessing ports.
    QSharedPointer<PortsInterface> portInterface_;

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterface_;
};

#endif // MASTERPORTSEDITOR_H
