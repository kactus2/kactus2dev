/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditor.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITOR_H
#define COMPONENTEDITOR_H

#include <common/widgets/tabDocument/TabDocument.h>
#include <models/component.h>
#include "componenttreeview.h"
#include "componenteditortreemodel.h"
#include "componenteditorgroupslot.h"

#include <QSharedPointer>
#include <QSplitter>

class LibraryInterface;
class PluginManager;

/*! \brief The editor to edit/packet IP-Xact components.
 *
 */
class ComponentEditor : public TabDocument {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Parent widget of the editor.
	 *
	*/
	ComponentEditor(LibraryInterface* libHandler, 
        PluginManager& pluginMgr,
		QSharedPointer<Component> component, 
		QWidget *parent);

	//! \brief The destructor
	~ComponentEditor();

	/*! \brief Get the VLNV that can be used to identify the component.
	 *
	 * \return The VLNV of the component.
	*/
	virtual VLNV getIdentifyingVLNV() const;

	/*! \brief Get the vlnv of the current component.
	 *
	 * \return VLNV of the component being edited.
	*/
	virtual VLNV getDocumentVLNV() const;

	/*! \brief Check if the editor is editing a hardware implementation or not.
	 *
	 * \return True if the component is a hardware component.
	*/
	virtual bool isHWImplementation() const;

	//! \brief Refreshes the editor to display the changes made.
    virtual void refresh();

public slots:

    /*! \brief Validates the document against the IP-XACT standard.
     *
     * \param errorList Error message list for reporting standard violations.
     *
     * \return True if the document is valid. False if there were any violations.
    */
    virtual bool validate(QStringList& errorList);

	//! \brief Saves the document and resets the modified state.
	virtual bool save();

	//! \brief Saves the document as new object and resets modifies state
	virtual bool saveAs();

	/*! \brief Run vhdl generator for the component.
	 *
	 * \return bool True if the metadata was changed and the editor should be refreshed.
	*/
	bool onVhdlGenerate();

	/*! \brief Run modelsim generator for the component.
	 *
	 * \return bool True if the metadata was changed and the editor should be refreshed.
	*/
	bool onModelsimGenerate();

	/*! \brief Set the protection state of the document.
	 *
	 * \param locked True for locked state, false for unlocked.
	 *
	*/
	virtual void setProtection(bool locked);

private slots:

	/*! \brief This slot is called when an item is selected in the navigation tree.
	 *
	 * \param index The index identifying the item.
	 *
	*/
	void onItemActivated(const QModelIndex& index);

	/*! \brief This slot is called when navigation tree sends a selectItem signal.
	 * 
	 * This function tells navigation view to select the index in the tree and then
	 * calls ComponentEditor::onItemActivated() slot.
	 * 
	 * \param index Model index that identifies the item to select.
	 *
	*/
	void onNavigationTreeSelection(const QModelIndex& index);

	/*! \brief This slot is called when one of the component editors changes state.
	 *
	*/
	void onItemChanged();

signals:
    /*!
     *  Opens the specific C source file of the given component.
     *
     *      @param [in] filename   The name of the file to open.
     *      @param [in] component  The component containing the file.
     */
    void openCSource(QString const& filename, QSharedPointer<Component> component);

private:
	//! \brief No copying
	ComponentEditor(const ComponentEditor& other);

	//! \brief No assignment
	ComponentEditor& operator=(const ComponentEditor& other);

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

	//! \brief The component being edited.
	QSharedPointer<Component> component_;

	//! \brief The splitter to contain the navigation tree.
	QSplitter navigationSplitter_;

	//! \brief The splitter to display the editors and visualizers.
	QSplitter editorVisualizerSplitter_;

	//! \brief The model to control the navigation view.
	ComponentEditorTreeModel navigationModel_;

	//! \brief The tree view to navigate in the editor.
	ComponentTreeView navigationView_;

	//! \brief The slot to display the editors in.
	ComponentEditorGroupSlot editorSlot_;

	//! \brief The slot to display the visualizers in.
	ComponentEditorGroupSlot visualizerSlot_;
};

#endif // COMPONENTEDITOR_H
