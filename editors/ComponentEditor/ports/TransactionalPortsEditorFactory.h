//-----------------------------------------------------------------------------
// File: TransactionalPortsEditorFactory.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2019
//
// Description:
// Constructs transactional ports editor items.
//-----------------------------------------------------------------------------

#ifndef TRANSACTIONAL_PORTS_EDITOR_FACTORY_H
#define TRANSACTIONAL_PORTS_EDITOR_FACTORY_H

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
//! Constructs transactional ports editor items.
//-----------------------------------------------------------------------------
class TransactionalPortsEditorFactory : public PortsEditorFactory
{
public:

    using PortsEditorFactory::PortsEditorFactory;

    /*!
     *  The destructor.
     */
    ~TransactionalPortsEditorFactory() = default;

    /*!
     *  Construct a ports model.
     *
    *      @param [in] parent              The owner of the constructed model.
     *
     *      @return The created ports model.
     */
    PortsModel* createModel(QObject* parent = 0) const  final;

    /*!
     *  Construct a filter.
     *
     *      @param [in] parent          Owner of the filter.
     *
     *      @return The created ports filter.
     */
    PortsFilter* createFilter(QObject* parent = 0) const final;

    /*!
     *  Construct a view.
     *
     *      @param [in] parent          Owner of the view.
     *
     *      @return The created view.
     */
    PortsView* createView(QWidget* parent) const final;

    /*!
     *  Construct a delegate.
     *
     *      @param [in] parent              The owner of the delegate.
     *
     *      @return The created delegate.
     */
    PortsDelegate* createDelegate(QObject* parent) const final;

};

#endif // TRANSACTIONAL_PORTS_EDITOR_FACTORY_H
