//-----------------------------------------------------------------------------
// File: PortSummaryEditorFactory.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 28.11.2023
//
// Description:
// Creates the components of port summary editor.
//-----------------------------------------------------------------------------

#ifndef PORT_SUMMARY_EDITOR_FACTORY_H
#define PORT_SUMMARY_EDITOR_FACTORY_H

#include <editors/ComponentEditor/ports/PortsEditorFactory.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>
#include <QWidget>
#include <QAbstractItemModel>

class Component;
class PortsModel;
class PortsView;
class PortValidator;
class PortsDelegate;
class ExpressionParser;

//-----------------------------------------------------------------------------
//! Creates the components of port summary editor.
//-----------------------------------------------------------------------------
class PortSummaryEditorFactory : public PortsEditorFactory
{
public:

    using PortsEditorFactory::PortsEditorFactory;

    /*!
     *  The destructor.
     */
    ~PortSummaryEditorFactory() = default;

    /*!
     *  Construct a ports model.
     *
    *    @param [in] parent              The owner of the constructed model.
     *
     *    @return The created ports model.
     */
    PortsModel* createModel(QObject* parent) const  final;

    /*!
     *  Construct a filter.
     *
     *    @param [in] parent          Owner of the filter.
     *
     *    @return The created ports filter.
     */
    PortsFilter* createFilter(QObject* parent) const final;

    /*!
     *  Construct a view.
     *
     *    @param [in] parent          Owner of the view.
     *
     *    @return The created view.
     */
    PortsView* createView(QWidget* parent) const final;

    /*!
     *  Construct a delegate.
     *
     *    @param [in] parent              The owner of the delegate.
     *
     *    @return The created delegate.
     */
    PortsDelegate* createDelegate(QObject* parent) const final;

};

#endif // PORT_SUMMARY_EDITOR_FACTORY_H
