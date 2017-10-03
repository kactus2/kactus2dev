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

#include <common/utils.h>

class LibraryHandler;

class VLNVDialer;
class HierarchyWidget;
class LibraryFilter;
class LibraryTreeWidget;

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
    LibraryWidget( QWidget* parent = 0);

    //! The destructor.
    virtual ~LibraryWidget();

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
};

#endif // LIBRARYWIDGET_H
