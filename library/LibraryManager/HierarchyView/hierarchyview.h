//-----------------------------------------------------------------------------
// File: hierarchyview.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 01.07.2011
//
// Description:
// HierarchyView displays the filtered contents from hierarchy filter.
//-----------------------------------------------------------------------------

#ifndef HIERARCHYVIEW_H
#define HIERARCHYVIEW_H

#include <IPXACTmodels/common/VLNV.h>

#include <QTreeView>
#include <QPoint>
#include <QAction>
#include <QContextMenuEvent>
#include <QMouseEvent>

class LibraryInterface;
class HierarchyFilter;

//-----------------------------------------------------------------------------
//! HierarchyView displays the filtered contents from hierarchy filter.
//-----------------------------------------------------------------------------
class HierarchyView : public QTreeView
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent      The owner of this widget.
	 *      @param [in] handler     The instance that manages the library.
	 *      @param [in] filter      The instance that does sorting and filtering.
	*/
	HierarchyView(QWidget *parent, LibraryInterface* handler, HierarchyFilter* filter);
	
	//! The destructor
	virtual ~HierarchyView();

	/*! Select the item with given indexes in the view.
	 *
	 *      @param [in] indexes List in model indexes to select.
	*/
	void setSelectedIndexes(const QModelIndexList& indexes);

public slots:

	//! Open a hierarchical component
	void onOpenDesign();

    //! Open a memory design of a hierarchical component
    void onOpenMemoryDesign();

    //! Open the SW design of a component
    void onOpenSWDesign();

    //! Open the system design of a component.
    void onOpenSystemDesign();

	//! Open component editor
	void onOpenComponent();

	//! Create new component
	void onCreateComponent();

	//! Create new design
	void onCreateDesign();

    //! Create new SW design
    void onCreateSWDesign();

    //! Create new system design
    void onCreateSystemDesign();

	//! Called when user selects "Export item and sub-items" in contextMenu
	void onExportAction();

	/*! Set the index to be the current index and select and expand it.
	 *
	 *      @param [in] index Specified the index to be set as the current index.
	 *
	*/
	virtual void setCurrentIndex(QModelIndex const& index);

	//! Open bus in bus editor
	void onOpenBus();

	//! Create new abstraction definition for selected bus definition.
	void onAddSignals();

	//! Create new bus definition and abstraction definition.
	void onCreateBus();

    //! Open COM definition in editor
    void onOpenComDef();

    //! Create new COM definition.
    void onCreateComDef();

    //! Open API definition in editor
    void onOpenApiDef();

    //! Create new API definition.
    void onCreateApiDef();

	//! Open the xml of the selected IP-Xact element.
	void onOpenXml();

    //! Opens the folder that contains the IP-XACT object.
    void onOpenContainingFolder();

    //! Called when the user chooses Show Errors from the context menu.
    void onShowErrors();

signals:
	//! Open the selected hierarchical design
	void openDesign(const QModelIndex& index);

    //! Open the memory design of a hierarchical component
    void openMemoryDesign(const QModelIndex& index);

    //! Open the selected SW design
    void openSWDesign(const QModelIndex& index);

    //! Open the selected system design
    void openSystemDesign(const QModelIndex& index);

	//! Open the given object in the component editor
	void openComponent(const QModelIndex& index);

	//! Create new component
	void createNewComponent(const QModelIndex& index);

	//! create new bus
	void createNewBus(const QModelIndex& index);

	//! Create new abstraction definition for given bus definition.
	void createNewAbsDef(const QModelIndex& index);

    //! create new COM definition
    void createNewComDef(const QModelIndex& index);

    //! create new API definition
    void createNewApiDef(const QModelIndex& index);

	//! Create new design
	void createNewDesign(const QModelIndex& index);

    //! Create new SW design.
    void createNewSWDesign(QModelIndex const& index);

    //! Create new system design.
    void createNewSystemDesign(QModelIndex const& index);

	//! Export the selected item and it's sub-items.
	void exportItem(const QModelIndex& index);

    //! Shows errors about the selected item.
    void showErrors(QModelIndex const& index);

	//! Signal is emitted when component is selected in the view.
	void componentSelected(const VLNV& vlnv);

	//! A drag operation has been initiated.
	void dragInitiated(const QModelIndex& index);

	//! Send error message to user.
	void errorMessage(const QString& message);

	//! Send notification to user.
	void noticeMessage(const QString& message);

protected:

	/*! Handler for the context menu events.
     *
     *      @param [in] event The contextMenuEvent to be handled.
    */
    virtual void contextMenuEvent(QContextMenuEvent* event);

	//! The event handler for mouse double clicks
	virtual void mouseDoubleClickEvent(QMouseEvent * event);

	//! The event handler for mousePress events
	virtual void mousePressEvent(QMouseEvent *event);

	//! The event handler for mouseMove events
	virtual void mouseMoveEvent(QMouseEvent *event);

	//! The event handler for mouse release events
	virtual void mouseReleaseEvent(QMouseEvent * event);

private:
	//! No copying
	HierarchyView(const HierarchyView& other);

	//! No assignment
	HierarchyView& operator=(const HierarchyView& other);

	//! Set up the actions for the context menu
	void setupActions();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The instance that manages the library.
	LibraryInterface* handler_;

	//! The instance that does the filtering and sorting
	HierarchyFilter* filter_;

	//! The starting position for drag
	QPoint startPos_;

	//! The index that is selected to be dragged
	QModelIndex dragIndex_;

    //-----------------------------------------------------------------------------
    //  The actions for the contextMenu
    //-----------------------------------------------------------------------------

	//! Open the hierarchical design of a component
	QAction* openDesignAction_;

    //! Open the memory design of a component
    QAction* openMemoryDesignAction_;

    //! Open the SW design of a component
    QAction* openSWDesignAction_;

	//! Open the component editor
	QAction* openComponentAction_;

	//! Create a new empty object, only copy the vlnv structure.
	QAction* createNewComponentAction_;

	//! Create new design for a component
	QAction* createNewDesignAction_;

    //! Create new SW design for a component
    QAction* createNewSWDesignAction_;

    //! Create new system design for a component
    QAction* createNewSystemDesignAction_;

	//! Export the item(s) to a new location in file system
	QAction* exportAction_;

    //! Show errors about the item.
    QAction* showErrorsAction_;

	//! Open the bus editor
	QAction* openBusAction_;

	//! Create new abstraction definition for bus definition.
	QAction* addSignalsAction_;

	//! Open the COM definition editor
    QAction* openComDefAction_;

    //! Open the API definition editor
    QAction* openApiDefAction_;

    //! Open an existing system for editing.
	QAction* openSystemAction_;

	//! Open an existing software platform.
	QAction* openPlatformAction_;

	//! Open a software application
	QAction* openApplicationAction_;

    //! Open end point to be edited
    QAction* openEndpointAction_;

    //! Open the xml file to be viewed by the user.
	QAction* openXmlAction_;

    //! Action to open the containing folder.
    QAction* openContainingFolderAction_;
};

#endif // HIERARCHYVIEW_H
