//-----------------------------------------------------------------------------
// File: librarytreeview.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 20.12.2010
//
// Description:
// The widget to display the library in a tree.
//-----------------------------------------------------------------------------

#ifndef LIBRARYTREEVIEW_H
#define LIBRARYTREEVIEW_H

#include "LibraryTreeFilter.h"

#include <common/views/ExpandingTreeView/ExpandingTreeView.h>

#include <QTreeView>
#include <QContextMenuEvent>
#include <QAction>
#include <QPoint>
#include <QModelIndex>
#include <QMouseEvent>

class LibraryInterface;

//-----------------------------------------------------------------------------
//! The widget to display the library in a tree.
//-----------------------------------------------------------------------------
class LibraryTreeView : public ExpandingTreeView
{
	Q_OBJECT

public:

	/*! The constructor
	* 
	*    @param [in] handler  The instance that manages the library.
	*    @param [in] filter   The class that does the filtering for the view.
	*    @param [in] parent   The owner of this widget.
	*
	*/
	LibraryTreeView(LibraryInterface* handler, LibraryTreeFilter* filter, QWidget* parent = 0);

	//! The destructor.
	virtual ~LibraryTreeView() = default;

signals:

	//! Open the selected hierarchical design
	void openDesign(QModelIndex const& index, QString const& viewName);

	//! Open the SW design of a component
	void openSWDesign(QModelIndex const& index);

    //! Open the Memory design of a HW design.
    void openMemoryDesign(QModelIndex const& index, QString const& viewName);

	//! Open the system design of a component
	void openSystemDesign(QModelIndex const& index);

	//! Open the given object in the component editor
	void openComponent(QModelIndex const& index);

	//! Create new design
	void createNewDesign(QModelIndex const& index);

	//! Create new SW design
	void createNewSWDesign(QModelIndex const& index);

	//! Create new system design
	void createNewSystemDesign(QModelIndex const& index);

	//! Delete the selected item
	void deleteItem(QModelIndex const& index);

	//! Export the selected item and it's sub-items.
	void exportItem(QModelIndex const& index);

	//! Shows errors about the selected item.
	void showErrors(QModelIndex const& index);

	//! Open bus definition in an editor
	void openBus(QModelIndex const& index);

    //! Open catalog in an editor
    void openCatalog(QModelIndex const& index);

	//! Open COM definition in an editor
	void openComDef(QModelIndex const& index);

	//! Open API definition in an editor
	void openApiDef(QModelIndex const& index);

	//! Create a new bus definition
	void createBus(QModelIndex const& index);

	//! Create a new abstraction definition for given bus def.
	void createAbsDef(QModelIndex const& index);

	//! A drag operation has been initiated.
	void dragInitiated(QModelIndex const& index);

	//! Emitted when a library item identifying single vlnv is selected.
	void itemSelected(const VLNV& vlnv);

	//! Send error message to user.
	void errorMessage(QString const& message);

	//! Send notification to user.
	void noticeMessage(QString const& message);

public slots:

    //! Open a hierarchical component
    void onOpenDesign(QAction* viewAction);

    //! Open the SW design of a component
    void onOpenSWDesign();

    //! Open the Memory design of a HW design.
    void onOpenMemoryDesign(QAction* viewAction);

    //! Open the system design of a component
    void onOpenSystemDesign();

    //! Open component editor
    void onOpenComponent();

    //! Create new design
    void onCreateDesign();

    //! Create new SW design
    void onCreateSWDesign();

    //! Create new SW design
    void onCreateSystemDesign();

    //! Called when user selects "Delete item" in contextMenu
    void onDeleteAction();

    //! Called when user selects "Export item and sub-items" in contextMenu
    void onExportAction();

    //! Open bus.
    void onOpenBus();

    //! Open catalog.
    void onOpenCatalog();

    //! Open COM definition.
    void onOpenComDef();

    //! Open API definition.
    void onOpenApiDef();

    //! Create new bus.
    void onCreateBus();

    //! Create new abstraction definition for selected bus definition.
    void onAddSignals();

    /*! Set the index to be the current index and select and expand it.
    *
    *    @param [in] index Specified the index to be set as the current index.
    */
    virtual void setCurrentIndex(QModelIndex const& index);

    //! Open the xml of the selected IP-Xact element.
    void onOpenXml();

    //! Opens the folder that contains the IP-XACT object.
    void onOpenContainingFolder();

    //! Called when the user chooses Show Errors from the context menu.
    void onShowErrors();

protected:

    //! The event handler for mouse double clicks
    virtual void mouseDoubleClickEvent(QMouseEvent * event);

    //! The event handler for mousePress events
    virtual void mousePressEvent(QMouseEvent *event);

    //! The event handler for mouseMove events
    virtual void mouseMoveEvent(QMouseEvent *event);

    //! The event handler for mouse release events
    virtual void mouseReleaseEvent(QMouseEvent* event);

    /*! Handler for the context menu events.
    *
    *    @param [in] event The contextMenuEvent to be handled.
    */
    virtual void contextMenuEvent(QContextMenuEvent* event);

private slots:
    
    //! Called when all the children of the current index should be expanded.
    void onExpandBranches();

    //! Called when all the children of the current index should be collapsed.
    void onCollapseBranches();

private:

    //! No assignment
	LibraryTreeView(const LibraryTreeView& other);

	//! No copying
	LibraryTreeView& operator=(const LibraryTreeView& other);

    //! Expands/collapses all child indexes recursively.
    void setChildrenExpandStates(QModelIndex index, bool expanded);

    //! Set up the actions for the context menu
    void setupActions();

	//! The instance that gives the xml objects.
	LibraryInterface* handler_;

	//! The filter that is used to transform model indexes 
	LibraryTreeFilter* filter_;

	//! The starting position for drag
	QPoint startPos_;

	//! The index that is selected to be dragged
	QModelIndex dragIndex_;

	/**********************************************************************/
	// The actions for the contextMenu

	//! Open the hierarchical design of a component. Single action if only one view available, otherwise a menu.
    QAction* openHWDesignAction_;
	QMenu* openHWDesignMenu_;

	//! Open the SW design of a component
	QAction* openSWDesignAction_;

    //! Action for opening a memory design. Single action if only one view is available, otherwise a menu.
    QAction* openMemoryDesignAction_;
    QMenu* openMemoryDesignMenu_;

	//! Open the component editor
	QAction* openComponentAction_;

	//! Create new design for a component
	QAction* createNewDesignAction_;

	//! Create new SW design for a component.
	QAction* createNewSWDesignAction_;

	//! Create new system design for a component.
	QAction* createNewSystemDesignAction_;

	//! Delete the item from the library
	QAction* deleteAction_;

	//! Export the item(s) to a new location in file system
	QAction* exportAction_;

	//! Show errors about the item.
	QAction* showErrorsAction_;

	//! Open a bus definition
	QAction* openBusAction_;
   
    //! Open a catalog.
    QAction* openCatalogAction_;

	//! Create new abstraction definition for bus definition.
	QAction* addSignalsAction_;

	//! Open a COM definition
	QAction* openComDefAction_;

	//! Open an API definition
	QAction* openApiDefAction_;

	//! Open an existing system for editing.
	QAction* openSystemAction_;

	//! Open the xml file to be viewed by the user.
	QAction* openXmlAction_;

    //! Action to open the containing folder.
    QAction* openContainingFolderAction_;

    //! Action for recursively expand all child indexes of the selected index.
    QAction* expandChilds_;

    //! Action for recursively collapse all child indexes of the selected index.
    QAction* collapseChilds_;

    //! Action to expand the whole library tree.
    QAction* expandAllAction_;

    //! Action to collapse the whole library tree
    QAction* collapseAllAction_;
};

#endif // LIBRARYTREEVIEW_H
