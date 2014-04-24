/* 
 *  	Created on: 7.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditortreemodel.h
 *		Project: Kactus 2
 */

#ifndef COMPONENTEDITORTREEMODEL_H
#define COMPONENTEDITORTREEMODEL_H

#include "componenteditorrootitem.h"
#include <IPXACTmodels/component.h>

#include <QAbstractItemModel>
#include <QWidget>
#include <QSharedPointer>

class LibraryInterface;
class PluginManager;

/*! \brief The tree model that manages the component editor's navigation tree.
 *
 */
class ComponentEditorTreeModel : public QAbstractItemModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param parent Pointer to the instance that owns this model.
	 *
	*/
	ComponentEditorTreeModel(LibraryInterface* libHandler,
		PluginManager& pluginMgr, QObject* parent, QWidget* parentWnd);
	
	//! \brief The destructor
	virtual ~ComponentEditorTreeModel();

	/*! \brief Set the component to be edited.
	 *
	 * \param component Pointer to the component to edit.
	 *
	*/
	void setComponent(QSharedPointer<Component> component);

	/*! \brief Get the number of rows an item contains.
	 *
	 * \param parent Identifies the parent that's row count is requested.
	 *
	 * \return Number of rows the item has.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Does the specified item have any child items
     *
     * \param parent Model index of the item.
     *
     * \return True if the item has any child items. This function is implemented
     * for performance reasons.
    */
    virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the number of columns the item has to be displayed.
	 *
	 * \param parent Identifies the parent that's column count is requested.
	 *
	 * \return The number of columns to be displayed.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the data for specified item.
	 *
	 * \param index Specifies the item that's data is requested.
	 * \param role The role that defines what kind of data is requested.
	 *
	 * \return QVariant Contains the data for the item.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*! \brief Get the item flags that defines the possible operations for the item.
	 *
	 * \param index Model index that identifies the item.
	 *
	 * \return Qt::ItemFlags specify the possible operations for the item.
	*/
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Get index that identifies the given item.
	 *
	 * \param row The row of the item.
	 * \param column The column of the item.
	 * \param parent The parent index of the item.
	 *
	 * \return QModelIndex The model index that identifies the item.
	*/
	virtual QModelIndex index(int row, int column, 
		const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the index that identifies the given item.
	 *
	 * \param item Pointer to the item that's index is wanted.
	 *
	 * \return QModelIndex The model index that identifies the item.
	*/
	virtual QModelIndex index(ComponentEditorItem* item) const;

	/*! \brief Get the index of the parent of given index.
	 *
	 * \param index The index that identifies the item that's parent is wanted.
	 *
	 * \return QModelIndex The model index that identifies the parent.
	*/
	virtual QModelIndex parent(const QModelIndex& index) const;

public slots:

	/*! \brief Handler for component editor item's content changed signal.
	 *
	 * \param item Pointer to the item that changed.
	 *
	*/
	void onContentChanged(ComponentEditorItem* item);

	/*! \brief Add a new item to the tree under parent item.
	 *
	 * \param parentItem Pointer to the item to add the child into.
	 * \param childIndex The index to add the child to.
	 *
	*/
	void addItem(ComponentEditorItem* parentItem, int childIndex);

	/*! \brief Remove a child from parent item.
	 *
	 * \param parentItem Pointer to the item to remove the child from.
	 * \param childIndex The index of the child to remove.
	 *
	*/
	void removeItem(ComponentEditorItem* parentItem, int childIndex);

	/*! \brief Move a child from one position to another.
	 *
	 * \param parentItem Pointer to the item that owns the child.
	 * \param childSource The index of the child to move. 
	 * \param childtarget The index to move the child into.
	 *
	*/
	void moveItem(ComponentEditorItem* parentItem, int childSource, int childtarget);

	/*! \brief Set the locked state of the editors and visualizers.
	 *
	 * \param locked True to set the editor's to view only mode.
	 *
	*/
	void setLocked(bool locked);

signals:
	
	//! \brief Emitted when an error message should be shown to user.
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

	//! \brief Emitted when a tree item with given index should be selected.
	void selectItem(const QModelIndex& index);

	//! \brief Emitted when an item should be expanded in the navigation view.
	void expandItem(const QModelIndex& index);

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

    void contentChanged();

private slots:

	/*! \brief Open bus interface editor for named bus interface.
	 * 
	 * If interface is not found then nothing is done.
	 * 
	 * \param interfaceName The name of the bus interface to open.
	 *
	*/
	void onSelectBusInterface(const QString& interfaceName);

	/*! \brief Select the given item to be displayed.
	 *
	 * \param item Pointer to the item which should be displayed.
	 *
	*/
	void onSelectItem(ComponentEditorItem* item);

private:
	//! \brief No copying
	ComponentEditorTreeModel(const ComponentEditorTreeModel& other);

	//! \brief No assignment
	ComponentEditorTreeModel& operator=(const ComponentEditorTreeModel& other);

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* libHandler_;

    //! The plugin manager.
    PluginManager& pluginMgr_;

	//! \brief Pointer to the root item of the tree.
	QSharedPointer<ComponentEditorRootItem> rootItem_;

    //! The parent window.
    QWidget* parentWnd_;
};

#endif // COMPONENTEDITORTREEMODEL_H
