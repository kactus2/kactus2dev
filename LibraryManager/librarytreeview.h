/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#ifndef LIBRARYTREEVIEW_H
#define LIBRARYTREEVIEW_H

#include "librarytreefilter.h"

#include <QTreeView>
#include <QContextMenuEvent>
#include <QAction>
#include <QPoint>
#include <QModelIndex>
#include <QMouseEvent>

class LibraryInterface;

/*! \brief The widget to display the library hierarchically.
 *
 */
class LibraryTreeView : public QTreeView {

	Q_OBJECT

public:

    /*! \brief The constructor
     * 
     * \param handler Pointer to the instance that manages the library.
     * \param filter Pointer to the class that does the filtering for the view.
     * \param parent Pointer to the owner of this widget.
     *
    */
    LibraryTreeView(LibraryInterface* handler, 
		LibraryTreeFilter* filter, 
		QWidget *parent = 0);

	//! \brief The destructor.
    virtual ~LibraryTreeView();

signals:

	//! \brief Open the selected hierarchical design
	void openDesign(const QModelIndex& index);

    //! \brief Open the SW design of a component
    void openSWDesign(const QModelIndex& index);

    //! \brief Open the system design of a component
    void openSystemDesign(const QModelIndex& index);

	//! \brief Open the given object in the component editor
	void openComponent(const QModelIndex& index);

	//! \brief Create new component
	void createNewComponent(const QModelIndex& index);

	//! \brief Create new design
	void createNewDesign(const QModelIndex& index);

    //! \brief Create new SW design
    void createNewSWDesign(const QModelIndex& index);

	//! \brief Delete the selected item
	void deleteItem(const QModelIndex& index);

	//! \brief Export the selected item and it's sub-items.
	void exportItem(const QModelIndex& index);

	//! \brief Open bus definition in an editor
	void openBus(const QModelIndex& index);

    //! \brief Open COM definition in an editor
    void openComDef(const QModelIndex& index);

    //! \brief Open API definition in an editor
    void openApiDef(const QModelIndex& index);

	//! \brief Create a new bus definition
	void createBus(const QModelIndex& index);

	//! \brief Create a new abstraction definition for given bus def.
	void createAbsDef(const QModelIndex& index);

    //! \brief Create a new COM definition
    void createComDef(const QModelIndex& index);

    //! \brief Create a new API definition
    void createApiDef(const QModelIndex& index);

	//! \brief A drag operation has been initiated.
	void dragInitiated(const QModelIndex& index);

	//! \brief Emitted when a library item identifying single vlnv is selected.
	void itemSelected(const VLNV& vlnv);

	//! \brief Send error message to user.
	void errorMessage(const QString& message);

	//! \brief Send notification to user.
	void noticeMessage(const QString& message);

public slots:

	//! \brief Open a hierarchical component
	void onOpenDesign();

    //! \brief Open the SW design of a component
    void onOpenSWDesign();

    //! \brief Open the system design of a component
    void onOpenSystemDesign();

	//! \brief Open component editor
	void onOpenComponent();

	//! \brief Create new component
	void onCreateComponent();

	//! \brief Create new design
	void onCreateDesign();

    //! \brief Create new SW design
    void onCreateSWDesign();

	//! \brief Called when user selects "Delete item" in contextMenu
	void onDeleteAction();

	//! \brief Called when user selects "Export item and sub-items" in contextMenu
	void onExportAction();

	//! \brief Open bus.
	void onOpenBus();

    //! \brief Open COM definition.
    void onOpenComDef();

    //! \brief Open API definition.
    void onOpenApiDef();

	//! \brief Create new bus.
	void onCreateBus();

    //! \brief Create new COM definition.
    void onCreateComDef();

    //! \brief Create new API definition.
    void onCreateApiDef();

	//! \brief Create new abstraction definition for selected bus definition.
	void onAddSignals();

	/*! \brief Set the index to be the current index and select and expand it.
	 *
	 * \param index Specified the index to be set as the current index.
	 *
	*/
	virtual void setCurrentIndex(const QModelIndex& index);

	//! \brief Open the xml of the selected IP-Xact element.
	void onOpenXml();

protected:

	//! \brief The event handler for mouse double clicks
	virtual void mouseDoubleClickEvent(QMouseEvent * event);

	//! \brief The event handler for mousePress events
	virtual void mousePressEvent(QMouseEvent *event);

	//! \brief The event handler for mouseMove events
	virtual void mouseMoveEvent(QMouseEvent *event);

	//! \brief The event handler for mouse release events
	virtual void mouseReleaseEvent(QMouseEvent* event);

	/*! \brief Handler for the context menu events.
     *
     * \param event Pointer to the contextMenuEvent to be handled.
    */
    virtual void contextMenuEvent(QContextMenuEvent* event);

private:

	//! \brief No assignment
	LibraryTreeView(const LibraryTreeView& other);

	//! \brief No copying
	LibraryTreeView& operator=(const LibraryTreeView& other);

	//! \brief Set up the actions for the context menu
	void setupActions();

	//! \brief Pointer to the instance that gives the xml objects.
	LibraryInterface* handler_;

	//! \brief Pointer to the filter that is used to transform model indexes 
	LibraryTreeFilter* filter_;

	//! \brief The starting position for drag
	QPoint startPos_;

	//! \brief The index that is selected to be dragged
	QModelIndex dragIndex_;

	/**********************************************************************/
	// The actions for the contextMenu

	//! \brief Open the hierarchical design of a component
	QAction* openDesignAction_;

    //! \brief Open the SW design of a component
    QAction* openSWDesignAction_;

	//! \brief Open the component editor
	QAction* openCompAction_;

	//! \brief Create a new empty object, only copy the vlnv structure.
	QAction* createNewComponentAction_;

	//! \brief Create new design for a component
	QAction* createNewDesignAction_;

    //! Create new SW design for a component.
    QAction* createNewSWDesignAction_;

	//! \brief Delete the item from the library
	QAction* deleteAction_;

	//! \brief Export the item(s) to a new location in file system
	QAction* exportAction_;

	//! \brief Open a bus definition
	QAction* openBusAction_;

	//! \brief Create a bus definition
	QAction* createBusAction_;

	//! \brief Create new abstraction definition for bus definition.
	QAction* addSignalsAction_;

    //! \brief Open a COM definition
    QAction* openComDefAction_;

    //! \brief Create a COM definition
    QAction* createComDefAction_;

    //! \brief Open an API definition
    QAction* openApiDefAction_;

    //! \brief Create a API definition
    QAction* createApiDefAction_;

	//! \brief Open an existing system for editing.
	QAction* openSystemAction_;

	//! \brief Open the xml file to be viewed by the user.
	QAction* openXmlAction_;
};

#endif // LIBRARYTREEVIEW_H
