//-----------------------------------------------------------------------------
// File: LibraryWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.05.2017
//
// Description:
// Widget for holding all IP-XACT library widgets.
//-----------------------------------------------------------------------------

#ifndef LIBRARYWIDGET_H
#define LIBRARYWIDGET_H

#include <QWidget>
#include <QStatusBar>
#include <QProgressBar>

#include <KactusAPI/include/utils.h>

#include <IPXACTmodels/common/VLNV.h>

#include "TableViewDialog.h"

class LibraryHandler;

class VLNVDialer;
class HierarchyWidget;
class LibraryFilter;
class LibraryTreeWidget;
class MessageMediator;

//-----------------------------------------------------------------------------
//! Widget for holding all IP-XACT library widgets.
//-----------------------------------------------------------------------------
class LibraryWidget : public QWidget
{
    Q_OBJECT
public:

    /*! The constructor.
     *
     *      @param [in] library    The IP-XACT library interface.
     *      @param [in] parent     The parent widget.
    */
    LibraryWidget(LibraryHandler* library, MessageMediator* messageChannel, QWidget* parent = 0);

    //! The destructor.
    virtual ~LibraryWidget() = default;

    /*! Gets the handle for library access.
     *
     *      @return     The library access handle.
    */
    LibraryHandler* getLibraryHandler() const;

    /*! Set filters for the library navigation.
     *
     *      @param [in] filters    The filters to set.
    */
    void setFilters(Utils::FilterOptions filters);

    /*! Get the current filters for the library navigation.
     *
     *      @return     The current library filters.
    */
    Utils::FilterOptions getFilters() const;

    /*!
     *  Selects the given VLNV in all library views.
     *
     *      @param [in] componentVLNV   The VLVN to select.
     */
    void selectComponent(VLNV const& componentVLNV) const;

public slots:

    /*!
     *  Shows errors about the library item with the given VLNV.
     *
     *      @param [in] vlnv The VLNV of the library item.
     */
    void onShowErrors(VLNV const& vlnv);

    //!  Shows a report of all errors within the library items.
    void onGenerateIntegrityReport();

    void onCloseIntegrityReport();

private slots:

    void statusMessage(QString const& message);

private:
    //! No copying
    LibraryWidget(const LibraryWidget& other);

    //! No assignment
    LibraryWidget& operator=(const LibraryWidget& other);

    /*! Connects the given filter to the filtering widget.
     *
     *      @param [in} filter     The filter to connect.
    */
    void connectLibraryFilter(LibraryFilter* filter) const;

    //! Sets the widget layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    //! Data
    //-----------------------------------------------------------------------------

    //! Widget with filtering options for the library.
    VLNVDialer* dialer_ ;

    //! The instance to manage library access.
    LibraryHandler* library_;

    //! The widget that contains the GUI items for the hierarchy tab.
    HierarchyWidget* hierarchyWidget_;

    //! The widget containing the library items in a tree-like view.
    LibraryTreeWidget* treeWidget_;

    //! Widget for showing integrity report on-demand.
    TableViewDialog* integrityWidget_;

    QStatusBar* statusBar_;

};

#endif // LIBRARYWIDGET_H
