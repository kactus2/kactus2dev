//-----------------------------------------------------------------------------
// File: componenteditoritem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 07.05.2012
//
// Description:
// ComponentEditorItem is one item in the navigation tree in component editor.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORITEM_H
#define COMPONENTEDITORITEM_H

#include <editors/ComponentEditor/referenceCounter/ReferenceCounter.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <QAction>
#include <QObject>
#include <QSharedPointer>
#include <QFont>
#include <QList>
#include <QGraphicsItem>

class LibraryInterface;
class ComponentEditorTreeModel;
class ItemVisualizer;
class ItemEditor;
class Component;
class VLNV;

//-----------------------------------------------------------------------------
//! ComponentEditorItem is one item in the navigation tree in component editor.
//-----------------------------------------------------------------------------
class ComponentEditorItem : public QObject
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] model       Pointer to the model that owns the items.
	 *      @param [in] libHandler  Pointer to the instance that manages the library.
	 *      @param [in] component   Pointer to the component being edited.
	 *      @param [in] parent      Pointer to the parent item.
	 */
	ComponentEditorItem(ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);
	
	/*!
     *  The constructor for root item.
	 *
	 *      @param [in] libHandler  Pointer to the instance that manages the library.
	 *      @param [in] component   Pointer to the component being edited.
	 *      @param [in] parent      Pointer to the parent object.
	 */
	ComponentEditorItem(LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorTreeModel* parent);

	//! The destructor.
	virtual ~ComponentEditorItem();

	/*! \brief Get the row of this item under the parent.
	 *
	 * \return The row number of this item.
	*/
	int row() const;

	/*!
     *  Get the index of the child item under the parent.
	 *
	 *      @param [in] child   Pointer to the child that's row is wanted.
	 *
	 *      @return The index of the child item. Returns -1 if child is not found.
	 */
	int getIndexOf(const ComponentEditorItem* child) const;

	/*!
     *  Get number of children the item contains.
	 *
	 *      @return The number of child items this item contains.
	 */
	virtual int rowCount() const;

	/*!
     *  Returns true if item contains child items.
	 *
	 *      @return True if child items exist.
	 */
	bool hasChildren() const;

	/*!
     *  Get pointer to the child in given index.
	 *
	 *      @param [in] index   The index identifying the child.
	 *
	 *      @return Pointer to the indexed child. Null pointer is returned if index is invalid.
	 */
	QSharedPointer<ComponentEditorItem> child(const int index);

	/*!
     *  Get pointer to the parent of this item.
	 *
	 *      @return Pointer to the parent item.
	 */
	ComponentEditorItem* parent();

	/*!
     *  Move child from source index to target index under the same parent.
	 *
	 *      @param [in] sourceIndex     The index identifying which item to move.
	 *      @param [in] targetIndex     The index identifying the target to move to. If under 0 then item is
	 *                                  prepended to the list. If greater than number of children then item is
     *                                  appended to the list.
	 */
	void moveChild(const int sourceIndex, int targetIndex);

	/*!
     *  Get the font to be used for text of this item.
	 *
	 *      @return QFont instance that defines the font to be used.
	 */
	virtual QFont getFont() const;

	/*!
     *  Set the locked state of the editor and possible visualizer.
	 *
	 *      @param [in] locked  True to set the editor and visualizer in read only mode.
	 */
	virtual void setLocked(bool locked);

	/*!
     *  Add a new child to the item.
	 *
	 *      @param [in] index   The index to add the child into.
	 */
	virtual void createChild(int index);

	/*!
     *  Remove the child from the given index.
	 *
	 *      @param [in] index Identifies the child to remove.
	 */
	virtual void removeChild(int index);

	/*!
     *  Get the tool tip for the item.
	 * 
	 *      @return The text for the tool tip to print to user.
	 */
	virtual QString getTooltip() const = 0;

	/*!
     *  Get the text to be displayed to user in the tree for this item.
	 *
	 *      @return QString Contains the text to display.
	 */
	virtual QString text() const = 0;

	/*!
     *  Check the validity of this item and sub items.
	 *
	 *      @return bool True if item is in valid state.
	 */
	virtual bool isValid() const;

    /*!
     *  Set the highlight.
     *
     *      @param [in] highlight   The highlight status.
     */
    virtual void setHighlight(bool highlight);

    /*!
     *  Get the highlight status.
     *
     *      @return True, if the editor item is highlighted, false otherwise.
     */
    virtual bool highlight() const;

	/*!
     *  Get pointer to the editor of this item.
	 *
	 *      @return Pointer to the editor to use for this item.
	 */
	virtual ItemEditor* editor() = 0;

	/*!
     *  Get pointer to the visualizer of this item.
	 * 
	 *      @return Pointer to the visualizer to use for this item.
	 */
	virtual ItemVisualizer* visualizer();

	/*!
     *  Refresh the contents of the editor associated with this item.
	 */
	virtual void refreshEditor();

	/*!
     *  Tells if the item can be opened or not.
	 * 
	 * The base class implementation always returns false.
	 * Reimplement this if the item can be opened to some other editor.
	 * 
	 * Note: If item can be opened then also reimplement actions() and openItem() to perform the opening.
	 */
	virtual bool canBeOpened() const;

    /*!
     *  Returns the possible actions for opening the item.
     *
     *      @return The actions to open the item.
     */
    virtual QList<QAction* > actions() const;

	/*!
     *  Get the visualizer graphics item for the element.
	 *
	 * The default implementation returns a null pointer, reimplement this for
	 * elements that contain a visualization element.
	 *
	 *      @return QGraphicsItem* Pointer to the graphics item.
	 */
	virtual QGraphicsItem* getGraphicsItem();

	/*!
     *  Update the graphics item of the element.
	 *
	 * The default implementation does nothing, reimplement for elements which
	 * contain a graphics item.
	 */
	virtual void updateGraphics();

	/*!
     *  Remove the graphics item of the element.
	 *
	 * The default implementation does nothing, reimplement for elements which
	 * contain a graphics item.
	 */
	virtual void removeGraphicsItem();

	/*!
     *  Tells if the item has a decoration icon or not.
	 * 
	 *      @return True if the item has an icon, otherwise false (default).
	 */
    virtual bool hasIcon() const;

	/*!
     *  Gets the decoration icon for the item.
	 */
    virtual QIcon getIcon() const;

    /*!
     *  Set the reference counter for this item.
     *
     *      @param [in] newReferenceCounter   The counter for references.
     */
    void setReferenceCounter(QSharedPointer<ReferenceCounter> newReferenceCounter);

    /*!
     *  Set the parameter finder for this item.
     *
     *      @param [in] parameterFinder     The parameter finder.
     */
    void setParameterFinder(QSharedPointer<ParameterFinder> parameterFinder);

    /*!
     *  Set the expression formatter.
     *
     *      @param [in] expressionFormatter     The expression formatter.
     */
    void setExpressionFormatter(QSharedPointer<ExpressionFormatter> expressionFormatter);

public slots:

	/*!
     *  Open the item in an editor.
	 * 
	 * The base class implementation does nothing. 
	 * Reimplement this in sub class to make it possible for item to be opened.
	 * 
	 * Note: If item can be opened then also reimplement actions() and canBeOpened() to return true.
	 */
    virtual void openItem();

signals:
	
	//! Emitted when a help page should be changed in the context help window.
    void helpUrlRequested(QString const& url);

	//! This signal is emitted when the item should be updated in view.
	void contentChanged(ComponentEditorItem* item);

    //! Emitted when the component should be marked as modified.
    void contentChanged();

	//! Emitted when a error message should be shown to user.
	void errorMessage(const QString& msg);

	/*!
     *  Emitted when a child should be added to this item.
	 *
	 *      @param [in] item        Pointer to this item.
	 *      @param [in] childIndex  The index the child should be added to.
	 */
	void createChild(ComponentEditorItem* item, int childIndex);

	/*!
     *  Emitted when a child should be removed from the item.
	 *
	 *      @param [in] item        Pointer to this item.
	 *      @param [in] childIndex  The index of the child that should be removed.
	 */
	void removeChild(ComponentEditorItem* item, int childIndex);

	/*!
     *  Emitted when a child should be moved from one place to another.
	 *
	 *      @param [in] item            Pointer to this item.
	 *      @param [in] sourceIndex     The index of the child item to move.
	 *      @param [in] targetIndex     The index to move the child into.
	 */
	void moveChild(ComponentEditorItem* item, int sourceIndex, int targetIndex);

    /*!
     *  Opens the specific C source file of the given component.
     *
     *      @param [in] filename   The name of the file to open.
     *      @param [in] component  The component containing the file.
     */
    void openCSource(QString const& filename, QSharedPointer<Component> component);

	//! Emitted when a design editor should be opened for given hierarchical view.
	void openDesign(const VLNV& componentVLNV, const QString& viewName);

	//! Emitted when a bus editor should be opened for given bus.
	void openBus(const VLNV& busdefVLNV, const VLNV& absdefVLNV);

	//! Emitted when a COM definition should be opened in an editor.
	void openComDefinition(const VLNV& comdefVLNV);

	//! Emitted when a SW design should be opened in an editor.
	void openSWDesign(const VLNV& componentVLNV, const QString& viewName);

	//! Emitted when a system design should be opened in an editor.
	void openSystemDesign(const VLNV& componentVLNV, const QString& viewName);

	/*!
     *  Emitted when a tree item should be selected in component editor.
	 *
	 *      @param [in] item    Pointer to the item to select.
	 */
	void selectItem(ComponentEditorItem* item);

    /*!
     *  Open the reference tree of the selected parameter.
     *
     *      @param [in] id   The id of the parameter.
     */
    void openReferenceTree(QString const& id) const;

protected:

    /*!
     *  Connects this component editor items item editor to its reference counter.
     */
    void connectItemEditorToReferenceCounter();

    /*!
     *  Connect the signals related to a reference tree.
     */
    void connectReferenceTree();

    //! Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

	//! Pointer to the component being edited.
	QSharedPointer<Component> component_;

	//! Pointer to the model that owns this item.
	ComponentEditorTreeModel* model_;

	//! Contains the children of this item.
	QList<QSharedPointer<ComponentEditorItem> > childItems_;

	//! The editor for the component editor item.
	ItemEditor* editor_;

	//! Defines the lock-status for editors.
	bool locked_;

    //! Flag for indicating highlight of the item.
    bool highlight_;

    //! Allows increasing and decreasing of parameter usage counts.
    QSharedPointer<ReferenceCounter> referenceCounter_;

    //! Parameter finder, finds the desired parameters.
    QSharedPointer<ParameterFinder> parameterFinder_;

    //! Expression formatter, formats the referencing expressions.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;

protected slots:

	/*!
     *  Handler for editor's contentChanged signal.
	 */
	virtual void onEditorChanged();

    /*!
     *  Handles the redrawing of the visualization of the item.
     */
    virtual void onGraphicsChanged();

	/*!
     *  Handler for ItemEditor's childAdded(int) signal.
	 *
	 *      @param [in] index   The index to add the child to.
	 */
	virtual void onAddChild(int index);

	/*!
     *  Handler for ItemEditor's childRemoved(int) signal.
	 *
	 *      @param [in] index   The index of the child to remove.
	 */
	virtual void onRemoveChild(int index);

	/*!
     *  Handler for ItemEditor's childMoved(int, int) signal.
	 *
	 *      @param [in] source  The index of the child to move.
	 *      @param [in] target  The position to move the child into.
	 */
	virtual void onMoveChild(int source, int target);

private slots:

	//! Slot for display requests of this item.
	void onSelectRequest();

private:

	//! No copying or assignment.
	ComponentEditorItem(const ComponentEditorItem& other);
	ComponentEditorItem& operator=(const ComponentEditorItem& other);

	//! Pointer to the parent item.
	ComponentEditorItem* parent_;
};

#endif // COMPONENTEDITORITEM_H
