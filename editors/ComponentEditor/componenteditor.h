//-----------------------------------------------------------------------------
// File: componenteditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 16.5.2012
//
// Description:
// The editor to edit/packet IP-Xact components.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITOR_H
#define COMPONENTEDITOR_H

#include <common/widgets/tabDocument/TabDocument.h>

#include <IPXACTmodels/component.h>

#include <editors/ComponentEditor/treeStructure/componenttreeview.h>
#include <editors/ComponentEditor/treeStructure/componenteditortreemodel.h>
#include <editors/ComponentEditor/treeStructure/componenteditorgroupslot.h>
#include <editors/ComponentEditor/treeStructure/ComponentEditorTreeSortProxyModel.h>

#include <QSharedPointer>
#include <QSplitter>
#include <QSettings>

class LibraryInterface;
class PluginManager;

//-----------------------------------------------------------------------------
//! The editor to edit/packet IP-Xact components.
//-----------------------------------------------------------------------------
class ComponentEditor : public TabDocument
{
	Q_OBJECT

public:

	/*! The constructor.
	 *
	 *      @param [in] libHandler  The instance that manages the component library.
     *      @param [in] pluginMgr   The instance that manages the plugins.
	 *      @param [in] component   The component being edited.
	 *      @param [in] parent      Parent widget of the editor.
	 *
	 */
	ComponentEditor(LibraryInterface* libHandler, 
        PluginManager& pluginMgr,
		QSharedPointer<Component> component, 
		QWidget *parent);

	//! The destructor
	~ComponentEditor();

	/*! Get the VLNV that can be used to identify the component.
	 *
	 *      @return The VLNV of the component.
	*/
	virtual VLNV getIdentifyingVLNV() const;

	/*! Get the vlnv of the current component.
	 *
	 *      @return VLNV of the component being edited.
	*/
	virtual VLNV getDocumentVLNV() const;

	/*! Check if the editor is editing a hardware implementation or not.
	 *
	 *      @return True if the component is a hardware component.
	*/
	virtual bool isHWImplementation() const;

	//! Refreshes the editor to display the changes made.
    virtual void refresh();

	/*!
	 *  Applies the current settings into use.
	 *
	 *      @param [in] settings   The settings file.
	 */
	void applySettings(QSettings& settings);

public slots:

    /*! Validates the document against the IP-XACT standard.
     *
     *      @param [in] errorList Error message list for reporting standard violations.
     *
     *      @return True if the document is valid. False if there were any violations.
    */
    virtual bool validate(QStringList& errorList);

	//! Saves the document and resets the modified state.
	virtual bool save();

	//! Saves the document as new object and resets modifies state
	virtual bool saveAs();

	/*! Run vhdl generator for the component.
	 *
	 *      @return bool True if the metadata was changed and the editor should be refreshed.
	*/
	bool onVhdlGenerate();

	/*! Run modelsim generator for the component.
	 *
	 *      @return bool True if the metadata was changed and the editor should be refreshed.
	*/
	bool onModelsimGenerate();

	/*! Set the protection state of the document.
	 *
	 *      @param [in] locked True for locked state, false for unlocked.
	 *
	*/
	virtual void setProtection(bool locked);

	/*!
	 *  Set the visibility of rows in tree view.
	 */
	void setRowVisibility(QSettings& settings);
    
private slots:

	/*! This slot is called when an item is selected in the navigation tree.
	 *
	 *      @param [in] index The index identifying the item.
	 *
	*/
	void onItemActivated(const QModelIndex& index);

	/*! This slot is called when navigation tree sends a selectItem signal.
	 * 
	 * This function tells navigation view to select the index in the tree and then
	 * calls ComponentEditor::onItemActivated() slot.
	 * 
	 *      @param [in] index Model index that identifies the item to select.
	 *
	*/
	void onNavigationTreeSelection(const QModelIndex& index);

	/*! This slot is called when navigation tree sends an expandItem signal.
	 * 
	 * This function tells navigation view to expand the index in the tree.
	 * 
	 *      @param [in] index Model index that identifies the item to expand.
	 *
	*/
    void onExpand(const QModelIndex& index);

signals:
    /*!
     *  Opens the specific C source file of the given component.
     *
     *      @param [in] filename   The name of the file to open.
     *      @param [in] component  The component containing the file.
     */
    void openCSource(QString const& filename, QSharedPointer<Component> component);

private:

	//! No copying
	ComponentEditor(const ComponentEditor& other);

	//! No assignment
	ComponentEditor& operator=(const ComponentEditor& other);

    /*!
     *  Creates the root item for the navigation model for the given component.
     *
     *      @param [in] component   The component for which to create the root item.
     *
     *      @return The root item for the navigation model for the component.
     */
    QSharedPointer<ComponentEditorRootItem> createNavigationRootForComponent(QSharedPointer<Component> component);
    
    /*!
     *  Creates the navigation model root item and its children for a HW component.
     *
     *      @param [in] component   The component for which to create the root item.
     *
     *      @return The root item for the navigation model for the component.
     */
    QSharedPointer<ComponentEditorRootItem> createHWRootItem(QSharedPointer<Component> component);
        
    /*!
     *  Creates the navigation model root item and its children for a SW component.
     *
     *      @param [in] component   The component for which to create the root item.
     *
     *      @return The root item for the navigation model for the component.
     */
    QSharedPointer<ComponentEditorRootItem> createSWRootItem(QSharedPointer<Component> component);

    //! Setups the editor layout.
    void setupLayout();

	//-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

    //! The instance managing all the plugins.
    PluginManager& pluginManager_;

	//! The component being edited.
	QSharedPointer<Component> component_;

	//! The splitter to contain the navigation tree.
	QSplitter navigationSplitter_;

	//! The splitter to display the editors and visualizers.
	QSplitter editorVisualizerSplitter_;

	//! The model to control the navigation view.
	ComponentEditorTreeModel navigationModel_;

	//! The tree view to navigate in the editor.
	ComponentTreeView navigationView_;

    //! The proxy model for sorting the tree items.
    ComponentEditorTreeProxyModel proxy_;

	//! The slot to display the editors in.
	ComponentEditorGroupSlot editorSlot_;

	//! The slot to display the visualizers in.
	ComponentEditorGroupSlot visualizerSlot_;
};

#endif // COMPONENTEDITOR_H
