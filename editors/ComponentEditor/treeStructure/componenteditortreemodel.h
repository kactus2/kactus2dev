//-----------------------------------------------------------------------------
// File: componenteditortreemodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 07.05.2012
//
// Description:
// The tree model that manages the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORTREEMODEL_H
#define COMPONENTEDITORTREEMODEL_H

#include "componenteditorrootitem.h"

#include <QAbstractItemModel>
#include <QWidget>
#include <QSharedPointer>

class LibraryInterface;
class PluginManager;
class Component;
class VLNV;

//-----------------------------------------------------------------------------
//! The tree model that manages the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorTreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] libHandler  Pointer to the instance that manages the library.
	 *      @param [in] parent      Pointer to the instance that owns this model.
	 */
	ComponentEditorTreeModel(QObject* parent);
	
	//! The destructor.
	virtual ~ComponentEditorTreeModel();

	/*!
     *  Set the root item.
	 *
     *      @param [in] root    Pointer to the root item.
	 */
    void setRootItem(QSharedPointer<ComponentEditorRootItem> root);

	/*!
     *  Get the number of rows an item contains.
	 *
	 *      @param [in] parent  Identifies the parent that's row count is requested.
	 *
	 *      @return Number of rows the item has.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
     *  Does the specified item have any child items.
     *
     *      @param [in] parent  Model index of the item.
     *
     *      @return True if the item has any child items.
     */
    virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const;

	/*!
     *  Get the number of columns the item has to be displayed.
	 *
	 *      @param [in] parent  Identifies the parent that's column count is requested.
	 *
	 *      @return The number of columns to be displayed.
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
     *  Get the data for specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is requested.
	 *      @param [in] role    The role that defines what kind of data is requested.
	 *
	 *       @return QVariant Contains the data for the item.
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*!
     *  Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index   Model index that identifies the item.
	 *
	 *      @return Qt::ItemFlags specify the possible operations for the item.
	 */
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/*!
     *  Get index that identifies the given item.
	 *
	 *      @param [in] row     The row of the item.
	 *      @param [in] column  The column of the item.
	 *      @param [in] parent  The parent index of the item.
	 *
	 *      @return QModelIndex The model index that identifies the item.
	 */
	virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

	/*!
     *  Get the index that identifies the given item.
	 *
	 *      @param [in] item    Pointer to the item that's index is wanted.
	 *
	 *      @return QModelIndex The model index that identifies the item.
	 */
	virtual QModelIndex index(ComponentEditorItem* item) const;

	/*!
     *  Get the index of the parent of given index.
	 *
	 *      @param [in] index   The index that identifies the item that's parent is wanted.
	 *
	 *      @return QModelIndex The model index that identifies the parent.
	 */
	virtual QModelIndex parent(const QModelIndex& index) const;

public slots:

	/*!
     *  Handler for component editor item's content changed signal.
	 *
	 *      @param [in] item    Pointer to the item that changed.
	 */
	void onContentChanged(ComponentEditorItem* item);

	/*!
     *  Add a new item to the tree under parent item.
	 *
	 *      @param [in] parentItem  Pointer to the item to add the child into.
	 *      @param [in] childIndex  The index to add the child to.
	 */
	void addItem(ComponentEditorItem* parentItem, int childIndex);

	/*!
     *  Remove a child from parent item.
	 *
	 *      @param [in] parentItem  Pointer to the item to remove the child from.
	 *      @param [in] childIndex  The index of the child to remove.
	 */
	void removeItem(ComponentEditorItem* parentItem, int childIndex);

	/*!
     *  Move a child from one position to another.
	 *
	 *      @param [in] parentItem      Pointer to the item that owns the child.
	 *      @param [in] childSource     The index of the child to move. 
	 *      @param [in] childtarget     The index to move the child into.
	 */
	void moveItem(ComponentEditorItem* parentItem, int childSource, int childtarget);

	/*!
     *  Set the locked state of the editors and visualizers.
	 *
	 *      @param [in] locked  True to set the editor's to view only mode.
	 */
	void setLocked(bool locked);

signals:
	
	//! Emitted when an error message should be shown to user.
	void errorMessage(const QString& msg);

    /*!
     *  Opens the specific C source file of the given component.
     *
     *      @param [in] filename   The name of the file to open.
     *      @param [in] component  The component containing the file.
     */
    void openCSource(QString const& filename, QSharedPointer<Component> component);

    //! Emitted when a help page should be changed in the context help window.
    void helpUrlRequested(QString const& url);

	//! Emitted when a tree item with given index should be selected.
	void selectItem(const QModelIndex& index);

	//! Emitted when an item should be expanded in the navigation view.
	void expandItem(const QModelIndex& index);

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

    //! Emitted when content change.
    void contentChanged();

private slots:

	/*!
     *  Open bus interface editor for named bus interface.
	 * 
	 *      @param [in] interfaceName   The name of the bus interface to open.
	 */
	void onSelectBusInterface(const QString& interfaceName);

	/*!
     *  Select the given item to be displayed.
	 *
	 *      @param [in] item    Pointer to the item which should be displayed.
	 */
	void onSelectItem(ComponentEditorItem* item);

private:

	//! No copying or assignment.
	ComponentEditorTreeModel(const ComponentEditorTreeModel& other);
	ComponentEditorTreeModel& operator=(const ComponentEditorTreeModel& other);

	//! Pointer to the root item of the tree.
	QSharedPointer<ComponentEditorRootItem> rootItem_;
};

#endif // COMPONENTEDITORTREEMODEL_H
