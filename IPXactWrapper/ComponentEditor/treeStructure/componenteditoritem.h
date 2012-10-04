/* 
 *  	Created on: 7.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditoritem.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORITEM_H
#define COMPONENTEDITORITEM_H

#include <LibraryManager/libraryinterface.h>
#include <models/component.h>

#include <QObject>
#include <QSharedPointer>
#include <QFont>
#include <QList>
#include <QWidget>

class LibraryInterface;
class ItemEditor;
class ComponentEditorTreeModel;
class ItemVisualizer;

/*! \brief ComponentEditorItem is one item in the navigation tree in component editor.
 *
 */
class ComponentEditorItem : public QObject {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);
	
	/*! \brief The constructor for root item.
	 *
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent object.
	 *
	*/
	ComponentEditorItem(LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorTreeModel* parent);

	//! \brief The destructor
	virtual ~ComponentEditorItem();

	/*! \brief Get the row of this item under the parent.
	 *
	 * \return The row number of this item.
	*/
	int row() const;

	/*! \brief Get the index of the child item under the parent.
	 *
	 * \param child Pointer to the child that's row is wanted.
	 *
	 * \return The index of the child item. Returns -1 if child is not found.
	*/
	int getIndexOf(const ComponentEditorItem* child) const;

	/*! \brief Get number of children the item contains.
	 *
	 * \return The number of child items this item contains.
	*/
	int rowCount() const;

	/*! \brief Returns true if item contains child items.
	 *
	 * \return True if child items exist.
	*/
	bool hasChildren() const;

	/*! \brief Get pointer to the child in given index.
	 *
	 * \param index The index identifying the child.
	 *
	 * \return Pointer to the indexed child. Null pointer is returned if index is invalid.
	*/
	QSharedPointer<ComponentEditorItem> child(const int index);

	/*! \brief Get pointer to the parent of this item.
	 *
	 * \return Pointer to the parent item.
	*/
	ComponentEditorItem* parent();

	/*! \brief Move child from source index to target index under the same parent.
	 *
	 * \param sourceIndex The index identifying which item to move. If not valid then nothing is done.
	 * \param targetIndex The index identifying the target to move to. If under 0 then item is
	 * prepended to the list. If greater than number of children then item is appended to the list.
	 *
	*/
	void moveChild(const int sourceIndex, int targetIndex);

	/*! \brief Get the font to be used for text of this item.
	*
	* The default implementation is for the base elements that can not be removed.
	*
	* \return QFont instance that defines the font to be used.
	*/
	virtual QFont getFont() const;

	/*! \brief Set the locked state of the editor and possible visualizer.
	 *
	 * \param locked True to set the editor and visualizer in read only mode.
	 *
	*/
	virtual void setLocked(bool locked);

	/*! \brief Add a new child to the item.
	 *
	 * The base class implementation does nothing.
	 * 
	 * \param index The index to add the child into.
	 *
	*/
	virtual void createChild(int index);

	/*! \brief Remove the child from the given index.
	 *
	 * \param index Identifies the child to remove.
	 *
	*/
	virtual void removeChild(int index);

	/*! \brief Get the tool tip for the item.
	 * 
	 * \return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const = 0;

	/*! \brief Get the text to be displayed to user in the tree for this item.
	 *
	 * \return QString Contains the text to display.
	*/
	virtual QString text() const = 0;

	/*! \brief Check the validity of this item and sub items.
	 *
	 * \return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor() = 0;

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual const ItemEditor* editor() const = 0;

	/*! \brief Get pointer to the visualizer of this item.
	 * 
	 * The default implementation returns a null pointer.
	 * 
	 * \return Pointer to the visualizer to use for this item.
	*/
	virtual ItemVisualizer* visualizer();

	/*! \brief Refresh the contents of the editor associated with this item.
	 *
	*/
	virtual void refreshEditor();

	/*! \brief Tells if the item can be opened or not.
	 * 
	 * The base class implementation always returns false.
	 * Reimplement this if the item can be opened to some other editor.
	 * 
	 * Note: If item can be opened then also reimplement openItem() to perform the opening.
	 *
	*/
	virtual bool canBeOpened() const;

    /*!
     *  Returns true if the item has a built-in editor available.
     */
    virtual bool hasBuiltinEditor() const;

public slots:

	/*! \brief Open the item in an editor.
	 * 
	 * The base class implementation does nothing. 
	 * Reimplement this in sub class to make it possible for item to be opened.
	 * 
	 * Note: If item can be opened then also reimplement canBeOpened() to return true.
	*/
	virtual void openItem(bool builtinEditor);

signals:
	
	//! Emitted when a help page should be changed in the context help window.
    void helpUrlRequested(QString const& url);

	//! \brief This signal is emitted when the item should be updated in view.
	void contentChanged(ComponentEditorItem* item);

	//! \brief Emitted when a error message should be shown to user.
	void errorMessage(const QString& msg);

	/*! \brief Emitted when a child should be added to this item.
	 *
	 * \param item Pointer to this item.
	 * \param childIndex The index the child should be added to.
	 *
	*/
	void createChild(ComponentEditorItem* item, int childIndex);

	/*! \brief Emitted when a child should be removed from the item.
	 *
	 * \param item Pointer to this item.
	 * \param childIndex The index of the child that should be removed.
	 *
	*/
	void removeChild(ComponentEditorItem* item, int childIndex);

	/*! \brief Emitted when a child should be moved from one place to another.
	 *
	 * \param item Pointer to this item.
	 * \param sourceIndex The index of the child item to move.
	 * \param targetIndex The index to move the child into.
	 *
	*/
	void moveChild(ComponentEditorItem* item, int sourceIndex, int targetIndex);

    /*!
     *  Opens the specific C source file of the given component.
     *
     *      @param [in] filename   The name of the file to open.
     *      @param [in] component  The component containing the file.
     */
    void openCSource(QString const& filename, QSharedPointer<Component> component);

	//! \brief Emitted when a design editor should be opened for given hierarchical view.
	void openDesign(const VLNV& componentVLNV, const QString& viewName);

	//! \brief Emitted when a bus editor should be opened for given bus.
	void openBus(const VLNV& busdefVLNV, const VLNV& absdefVLNV);

	//! \brief Emitted when a COM definition should be opened in an editor.
	void openComDefinition(const VLNV& comdefVLNV);

	//! \brief Emitted when a SW design should be opened in an editor.
	void openSWDesign(const VLNV& componentVLNV, const QString& viewName);

	//! \brief Emitted when a system design should be opened in an editor.
	void openSystemDesign(const VLNV& componentVLNV, const QString& viewName);

	/*! \brief Emitted when a tree item should be selected in component editor.
	 *
	 * \param item Pointer to the item to select.
	 *
	*/
	void selectItem(const QSharedPointer<ComponentEditorItem> item);

protected:

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

	//! \brief Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! \brief Pointer to the model that owns this item.
	ComponentEditorTreeModel* model_;

	//! \brief Contains the children of this item.
	QList<QSharedPointer<ComponentEditorItem> > childItems_;

protected slots:

	/*! \brief Handler for editor's contentChanged signal.
	 *
	*/
	virtual void onEditorChanged();

	/*! \brief Handler for ItemEditor's childAdded(int) signal.
	 *
	 * \param index The index to add the child to.
	 *
	*/
	virtual void onAddChild(int index);

	/*! \brief Handler for ItemEditor's childRemoved(int) signal.
	 *
	 * \param index The index of the child to remove.
	 *
	*/
	virtual void onRemoveChild(int index);

	/*! \brief Handler for ItemEditor's childMoved(int, int) signal.
	 *
	 * \param source The index of the child to move.
	 * \param target The position to move the child into.
	 *
	*/
	virtual void onMoveChild(int source, int target);

private:
	//! \brief No copying
	ComponentEditorItem(const ComponentEditorItem& other);

	//! \brief No assignment
	ComponentEditorItem& operator=(const ComponentEditorItem& other);

	//! \brief Pointer to the parent item.
	ComponentEditorItem* parent_;
};

#endif // COMPONENTEDITORITEM_H
