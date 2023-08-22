//-----------------------------------------------------------------------------
// File: WirePortsEditorConstructor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2019
//
// Description:
// Constructs wire ports editor items.
//-----------------------------------------------------------------------------

#ifndef WIREPORTSEDITORCONSTRUCTOR_H
#define WIREPORTSEDITORCONSTRUCTOR_H

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
class ParameterCompleter;
class PortsInterface;

//-----------------------------------------------------------------------------
//! Constructs wire ports editor items.
//-----------------------------------------------------------------------------
class WirePortsEditorConstructor : public PortsEditorConstructor
{
public:

    /*!
     *  The constructor.
     */
    WirePortsEditorConstructor() = default;

    /*!
     *  The destructor.
     */
    virtual ~WirePortsEditorConstructor() = default;

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
     *      @param [in] busInterface    Interface for accessing bus interface.
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
        QSharedPointer<PortValidator> portValidator, QObject* parent = 0) const override final;

    //! No copying
    WirePortsEditorConstructor(const WirePortsEditorConstructor& other) = delete;
    WirePortsEditorConstructor& operator=(const WirePortsEditorConstructor& other) = delete;
};

#endif // WIREPORTSEDITORCONSTRUCTOR_H
