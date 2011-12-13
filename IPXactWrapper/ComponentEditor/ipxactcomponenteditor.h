/* 
 *  Created on: 21.1.2011
 *      Author: Antti Kamppi
 */

#ifndef COMPONENTGENERATOR_H
#define COMPONENTGENERATOR_H

#include <common/widgets/tabDocument/TabDocument.h>

#include "componenttreenavigator.h"
#include <LibraryManager/libraryinterface.h>
#include <models/component.h>

#include <QWidget>
#include <QStackedWidget>
#include <QMap>
#include <QModelIndex>
#include <QSharedPointer>
#include <QFileInfo>
#include <QSplitter>
#include <QVBoxLayout>

class ComponentTreeItem;
class ItemEditor;

/*! \brief IPXactComponentEditor is widget for editing IP-Xact component.
 * 
 * This widget is used to create/edit an IP-Xact component document.
 */
class IPXactComponentEditor : public TabDocument {
    Q_OBJECT

public:

	/*! \brief The constructor
	* 
	* Component given to this editor must not be null. If a new component is
	* created then it must be done before calling this editor and then giving
	* pointer to that empty component to this widget.
	* 
    * \param lh The library handler.
	* \param xmlLocation QFileInfo instance that represents the xml-file in the
	* file system. This is needed so relative file paths are set correctly.
	* \param component Pointer to the Component-instance that is being edited.
	* \param parent Pointer to the owner of this widget.
	*/
    IPXactComponentEditor(LibraryInterface* lh, 
		const QFileInfo& xmlLocation, 
		QSharedPointer<Component> component, 
		QWidget *parent = 0);

	//! \brief The destructor
    virtual ~IPXactComponentEditor();

    /*!
     *  Sets the protection state of the document.
     *
     *      @param [in] locked True for locked state; false for unlocked.
     */
    virtual void setProtection(bool locked);

	/*! \brief Get the vlnv of the current bus definition.
	 *
	 * \return VLNV of the bus definition being edited.
	*/
	virtual VLNV getComponentVLNV() const;

	/*! \brief Check if the editor is editing a hardware implementation or not.
	 *
	 * \return True if the edited object is hardware implementation.
	*/
	virtual bool isHWImplementation() const;

	//! \brief Refreshes the editor to display the changes made.
    virtual void refresh();

public slots:

	//! Saves the document and resets the modified state.
	virtual bool save();

	//! \brief Saves the document as new object and resets modifies state
	virtual bool saveAs();

	/*! \brief Run vhdl generator for the component.
	 *
	 *
	 * \return bool True if the metadata was changed and the editor should be re-opened.
	*/
	bool onVhdlGenerate();

	/*! \brief Run modelsim generator for the component.
	 *
	 *
	 * \return bool True if the metadata was changed and the editor should be re-opened.
	*/
	bool onModelsimGenerate();

private slots:

	/*! \brief Should be called when another item is selected for editing.
	*
	* This function opens a new editor or an existing editor that matches the
	* new selected item.
	*/
	void selectedItemChanged(const QModelIndex& newIndex);

	/*! \brief The name of the item currently being edited changed.
	*
	* If no item is currently selected or the new name is empty then nothing is
	* changed.
	*
	* \param newName The new name for the item.
	*/
	void onItemNameChanged(const QString& newName);

	/*! \brief Remove the editors and IPXact elements for specified tree item.
	 * 
	 * If the given index doesn't yet have an editor created for it then the
	 * editor is created for it and instantly destroyed. This happens because
	 * the editor takes care of removing the matching IP-Xact model.
	 *
	 * \param index QModelIndex that specifies the editor to remove
	 *
	*/
	void onItemRemoved(const QModelIndex& index);

	//! \brief Called when user clicks ok in widget.
	virtual void onOk();

	//! \brief Called when user clicks apply changes in widget.
	virtual void onApply();

private:

	//! No copying
	IPXactComponentEditor(const IPXactComponentEditor& other);

	//! No assignment
	IPXactComponentEditor& operator=(const IPXactComponentEditor& other);

	/*! \brief Create an empty editor and add it to the widget stack.
	*
	* This function creates an empty editor widget that is used for the 
	* tree items that don't have a direct representation in IP-Xact models.
	* 
	* The null editor is set at index 0.
	*/
	void setupNullEditor();

	/*! \brief Creates a new editor and adds it to the widget stack
	*
	* \param item Pointer to the ComponentTreeItem instance that holds the info
	* on the type of the IP-Xact element and pointer to it that should be
	* edited with the editor.
	*/
	void createNewEditor(ComponentTreeItem* item);
	
	/*! \brief Set the data pointer for given editor and add it to widget stack
	 *
	 * \param item Pointer to the ComponentTreeItem that the editor is created 
	 * for
	 * \param editor Pointer to the editor that is added.
	 *
	*/
	void addEditor(ComponentTreeItem* item, ItemEditor* editor);

	//! \brief Update the previous ComponentTreeItem when a new one is selected.
	bool updateOldIndexItem();

	/*! \brief Save the contents of valid editors and close them.
	 *
	 * \return True if all editors were valid and all contents is saved
	*/
	bool saveEditors();

	//! \brief Refresh the editors and the navigator tree.
	void refreshEditors();

	//! \brief Represents the xml-file in the file system.
	QFileInfo xmlFile_;

	//! \brief Contains the navigator and widgetStack.
	QSplitter* splitter_;

	//! \brief The parent library handler.
	LibraryInterface* handler_;

	//! \brief Pointer to the original Component-instance
	QSharedPointer<Component> component_;

	//! \brief Pointer to the component being edited.
	//QSharedPointer<Component> editableComponent_;

	//! \brief The tree navigator widget on the left side
	ComponentTreeNavigator* navigator_;

	//! \brief The stack that contains the editors to display.
	QStackedWidget* widgetStack_;

	/*! \brief Contains the pointers used to select the correct editor.
	*
	* Key = Pointer to the model-element that is to be edited.
	* Value = Pointer to the correct editor in the widgetStack.
	*/
	QMap<void*, ItemEditor*> indexes_;

	//! \brief The model index that is currently selected in the navigation tree.
	QModelIndex currentIndex_;

	//! \brief The layout of the editor
	QVBoxLayout* layout_;
};

#endif // COMPONENTGENERATOR_H
