//-----------------------------------------------------------------------------
// File: TransactionalPortsEditorConstructor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2019
//
// Description:
// Constructs transactional ports editor items.
//-----------------------------------------------------------------------------

#ifndef TRANSACTIONALPORTSEDITORCONSTRUCTOR_H
#define TRANSACTIONALPORTSEDITORCONSTRUCTOR_H

#include <editors/ComponentEditor/ports/PortsEditorConstructor.h>

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
class TransactionalPortsEditorConstructor : public PortsEditorConstructor
{
public:

    /*!
     *  The constructor.
     */
    TransactionalPortsEditorConstructor() = default;

    /*!
     *  The destructor.
     */
    virtual ~TransactionalPortsEditorConstructor() = default;

    /*!
     *  Construct a ports model.
     *
     *      @param [in] parameterFinder     Locates the different parameters of the containing component.
     *      @param [in] portsInterface      Interface for accessing the component ports.
     *      @param [in] signalInterface     Interface for accessing abstraction signals.
     *      @param [in] filter              Filter for ports model.
     *      @param [in] parent              The owner of the constructed model.
     *
     *      @return The created ports model.
     */
    virtual PortsModel* constructModel(QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<PortsInterface> portsInterface,
        QSharedPointer<PortAbstractionInterface> signalInterface,
        QSortFilterProxyModel* filter,
        QObject* parent = 0)
        const override final;

    /*!
     *  Construct a filter.
     *
     *      @param [in] portsInterface  Interface for accessing the component ports.
     *      @param [in] parent          Owner of the filter.
     *
     *      @return The created ports filter.
     */
    virtual PortsFilter* constructFilter(QSharedPointer<PortsInterface> portsInterface, QObject* parent = 0) const
        override final;

    /*!
     *  Construct a view.
     *
     *      @param [in] defaultPath     Default import / export path.
     *      @param [in] busInterface    Interface for accessing bus interfaces.
     *      @param [in] parent          Owner of the view.
     *
     *      @return The created view.
     */
    virtual PortsView* constructView(QString const& defaultPath,
        BusInterfaceInterface* busInterface,
        QWidget* parent) const override final;

    /*!
     *  Construct a delegate.
     *
     *      @param [in] component           The component being edited.
     *      @param [in] completionModel     Model containing the completions used in expression editor.
     *      @param [in] parameterFinder     The parameter finder.
     *      @param [in] portValidator       Validator used for ports.
     *      @param [in] parent              The owner of the delegate.
     *
     *      @return The created delegate.
     */
    virtual PortsDelegate* constructDelegate(QSharedPointer<Component> component,
        QAbstractItemModel* completionModel, QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<PortValidator> portValidator, QObject* parent = 0) const final;

    //! No copying.
    TransactionalPortsEditorConstructor(const TransactionalPortsEditorConstructor& other) = delete;
    TransactionalPortsEditorConstructor& operator=(const TransactionalPortsEditorConstructor& other) = delete;
};

#endif // TRANSACTIONALPORTSEDITORCONSTRUCTOR_H
