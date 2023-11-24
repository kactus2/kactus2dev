//-----------------------------------------------------------------------------
// File: PortsEditorConstructor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2019
//
// Description:
// Constructs ports editor items.
//-----------------------------------------------------------------------------

#ifndef PORTSEDITORCONSTRUCTOR_H
#define PORTSEDITORCONSTRUCTOR_H

#include <QSortFilterProxyModel>
#include <QSharedPointer>
#include <QWidget>

#include <QCompleter>

class ParameterFinder;
class ExpressionFormatter;
class ExpressionParser;
class Component;
class PortsModel;
class PortsView;
class PortValidator;
class PortsDelegate;
class PortsFilter;
class PortsInterface;
class BusInterfaceInterface;
class PortAbstractionInterface;

//-----------------------------------------------------------------------------
//! Constructs ports editor items.
//-----------------------------------------------------------------------------
class PortsEditorConstructor
{
public:

    PortsEditorConstructor(QSharedPointer<Component> component,
        QAbstractItemModel* completionModel, QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<PortValidator> portValidator, 
        QSharedPointer<PortsInterface> portsInterface,
        QSharedPointer<PortAbstractionInterface> signalInterface,
        BusInterfaceInterface* busInterface,
        QString const& defaultPath):
        component_(component),
        completionModel_(completionModel),
        parameterFinder_(parameterFinder),
        portValidator_(portValidator),
        portsInterface_(portsInterface),
        signalInterface_(signalInterface),
        busInterface_(busInterface),
        defaultPath_(defaultPath)
    {
    }

    ~PortsEditorConstructor() = default;

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
    virtual PortsModel* constructModel(QObject* parent = 0) const = 0;

    /*!
     *  Construct a filter.
     *
     *      @param [in] portsInterface      Interface for accessing the component ports.
     *      @param [in] parent              Owner of the filter.
     *
     *      @return The created ports filter.
     */
    virtual PortsFilter* createFilter(QObject* parent) const = 0;

    /*!
     *  Construct a view.
     *
     *      @param [in] defaultPath     Default import / export path.
     *      @param [in] busInterface    Interface for accessing bus interfaces.
     *      @param [in] parent          Owner of the view.
     *
     *      @return The created view.
     */
    virtual PortsView* createView(QWidget* parent) const = 0;

    /*!
     *  Construct a delegate.
     *
     *      @param [in] component           The component being edited.
     *      @param [in] parameterCompleter  Completer for expressions.
     *      @param [in] parameterFinder     The parameter finder.
     *      @param [in] portValidator       Validator used for ports.
	 *      @param [in] parent              The owner of the delegate.
     *
     *      @return The created delegate.
     */
    virtual PortsDelegate* constructDelegate(QObject* parent) const = 0;

protected:

    QSharedPointer<Component> component_;
    QAbstractItemModel* completionModel_;
    QSharedPointer<ParameterFinder> parameterFinder_;
    QSharedPointer<PortValidator> portValidator_;
    QSharedPointer<PortsInterface> portsInterface_;
    QSharedPointer<PortAbstractionInterface> signalInterface_;
    BusInterfaceInterface* busInterface_;
    QString defaultPath_;


};

#endif // PORTSEDITORCONSTRUCTOR_H
