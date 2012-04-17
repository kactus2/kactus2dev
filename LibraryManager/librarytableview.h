/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#ifndef LIBRARYTABLEVIEW_H
#define LIBRARYTABLEVIEW_H

#include "librarysearchfilter.h"

#include <QTableView>
#include <QContextMenuEvent>
#include <QAction>
#include <QMouseEvent>

class LibraryHandler;

/*! \brief The widget to display the search results in search widget.
 *
 */
class LibraryTableView : public QTableView {

    Q_OBJECT

public:

    /*! \brief The constructor
     *
     * \param filter Pointer to the class that does the filtering of items
     * \param parent Pointer to the owner of this widget.
     *
    */
    LibraryTableView(LibraryHandler* handler, LibrarySearchFilter* filter, QWidget *parent = 0);

	//! \brief The destructor
    virtual ~LibraryTableView();

    /*! \brief The handler for context menu requests
     *
     * \param event Pointer to the contextMenu event
     *
    */
    virtual void contextMenuEvent(QContextMenuEvent* event);

signals:

	//! \brief Send error message to user.
	void errorMessage(const QString& message);

	//! \brief Send notification to user.
	void noticeMessage(const QString& message);

	//! \brief Open the selected hierarchical design
	void openDesign(const QModelIndex& index);

	//! \brief Open the given object in the component editor
	void openComponent(const QModelIndex& index);

	//! \brief Create new component
	void createNewComponent(const QModelIndex& index);

	//! \brief Create new design
	void createNewDesign(const QModelIndex& index);

	//! \brief Open bus definition in an editor
	void openBusDef(const QModelIndex& index);

	//! \brief Create a new bus definition
	void createBusDef(const QModelIndex& index);

    //! \brief Open COM definition in an editor
    void openComDef(const QModelIndex& index);

    //! \brief Create a new COM definition
    void createComDef(const QModelIndex& index);

    //! \brief Open API definition in an editor
    void openApiDef(const QModelIndex& index);

    //! \brief Create a new API definition
    void createApiDef(const QModelIndex& index);

	//! \brief Delete the selected item
	void deleteItem(const QModelIndex& index);

	//! \brief Export the selected item and it's sub-items
	void exportItem(const QModelIndex& index);

	//! \brief Hide the selected item 
	void hideItem(const QModelIndex& index);

	//! \brief Show  the selected item
	void showItem(const QModelIndex& index);

	//! \brief Refresh the library and check it's integrity
	void refreshLibrary();

public slots:

	//! \brief Open a hierarchical component
	void onOpenDesign();

	//! \brief Open component editor
	void onOpenComponent();

	//! \brief Create new component
	void onCreateComponent();

	//! \brief Create new design
	void onCreateDesign();

	//! \brief Called when user selects "Delete item" in contextMenu
	void onDeleteAction();

	//! \brief Called when user selects "Export item and sub-items" in contextMenu
	void onExportAction();

	//! \brief Called when user selects "Hide item" in contextMenu
	void onHideAction();

	//! \brief Called when user selects "Show item" in contextMenu
	void onShowAction();

	//! \brief Open bus definition.
	void onOpenBusDef();

	//! \brief Create new bus definition.
	void onCreateBusDef();

    //! \brief Open COM definition.
    void onOpenComDef();

    //! \brief Create new COM definition.
    void onCreateComDef();

    //! \brief Open API definition.
    void onOpenApiDef();

    //! \brief Create new API definition.
    void onCreateApiDef();

private:

	//! \brief No copying
	LibraryTableView(const LibraryTableView& other);

	//! \brief No assignment
	LibraryTableView& operator=(const LibraryTableView& other);

	//! \brief Set up the actions for the context menu
	void setupActions();

	//! \brief Pointer to the instance that does the sorting of items.
	LibrarySearchFilter* filter_;

	//! \brief Pointer to the library handler instance.
	LibraryHandler* handler_;

	/**********************************************************************/
	// The actions for the contextMenu

	//! \brief Open the hierarchical design of a component
	QAction* openDesignAction_;

	//! \brief Open the component editor
	QAction* openCompAction_;

	//! \brief Create a new empty object, only copy the vlnv structure.
	QAction* createNewComponentAction_;

	//! \brief Create new design for a component
	QAction* createNewDesignAction_;

	//! \brief Delete the item from the library
	QAction* deleteAction_;

	//! \brief Export the item(s) to a new location in file system
	QAction* exportAction_;

	//! \brief Hide the selected item
	QAction* hideAction_;

	//! \brief Set the selected item as unhidden.
	QAction* showAction_;

	//! \brief Refresh the view
	QAction* refreshAction_;

	//! \brief Open a bus definition
	QAction* openBusDefAction_;

	//! \brief Create a bus definition
	QAction* createBusDefAction_;

    //! \brief Open a COM definition
    QAction* openComDefAction_;

    //! \brief Create a COM definition
    QAction* createComDefAction_;

    //! \brief Open an API definition
    QAction* openApiDefAction_;

    //! \brief Create an API definition
    QAction* createApiDefAction_;
};

#endif // LIBRARYTABLEVIEW_H
