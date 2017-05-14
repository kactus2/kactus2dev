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

#include <library/VLNVDialer/filterwidget.h>

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

    LibraryHandler *getLibraryHandler();

    void setFilters(FilterWidget::FilterOptions filters);

    FilterWidget::FilterOptions getFilters() const;

signals:

public slots:

private:
    //! No copying
    LibraryWidget(const LibraryWidget& other);

    //! No assignment
    LibraryWidget& operator=(const LibraryWidget& other);

    void connectLibraryFilter(LibraryFilter *filter) const;

    void setupLayout();

    //! Widget with filtering options for the library.
    VLNVDialer* dialer_ ;

    //! The instance to manage library access.
    LibraryHandler* library_;

    //! The widget that contains the GUI items for the hierarchy tab
    HierarchyWidget* hierarchyWidget_;

    LibraryTreeWidget* treeWidget_;


};

#endif // LIBRARYWIDGET_H
