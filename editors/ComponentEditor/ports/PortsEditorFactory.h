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

#ifndef PORTSEDITOR_FACTORY_H
#define PORTSEDITOR_FACTORY_H

#include <editors/common/ExpressionSet.h>

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
class PortsEditorFactory
{
public:

    PortsEditorFactory(QSharedPointer<Component> component,
        ExpressionSet expressions,
        QSharedPointer<PortValidator> portValidator, 
        QSharedPointer<PortsInterface> portsInterface,
        QSharedPointer<PortAbstractionInterface> signalInterface,
        BusInterfaceInterface* busInterface,
        QString const& defaultPath):
        component_(component),
        expressions_(expressions),
        portValidator_(portValidator),
        portsInterface_(portsInterface),
        signalInterface_(signalInterface),
        busInterface_(busInterface),
        defaultPath_(defaultPath)
    {
    }

    /*!
     *  The destructor.
     */
    ~PortsEditorFactory() = default;

    //! No copying.
    PortsEditorFactory(const PortsEditorFactory& other) = delete;
    PortsEditorFactory& operator=(const PortsEditorFactory& other) = delete;

    /*!
     *  Construct a ports model.
     *
     *      @param [in] parent              The owner of the constructed model.
     *
     *      @return The created ports model.
     */
    virtual PortsModel* createModel(QObject* parent = 0) const = 0;

    /*!
     *  Construct a filter.
     *
     *      @param [in] parent              Owner of the filter.
     *
     *      @return The created ports filter.
     */
    virtual PortsFilter* createFilter(QObject* parent) const = 0;

    /*!
     *  Construct a view.
     *
     *      @param [in] parent          Owner of the view.
     *
     *      @return The created view.
     */
    virtual PortsView* createView(QWidget* parent) const = 0;

    /*!
     *  Construct a delegate.
     *
	 *      @param [in] parent              The owner of the delegate.
     *
     *      @return The created delegate.
     */
    virtual PortsDelegate* createDelegate(QObject* parent) const = 0;

protected:

    QSharedPointer<Component> component_;
    ExpressionSet expressions_;
    QSharedPointer<PortValidator> portValidator_;
    QSharedPointer<PortsInterface> portsInterface_;
    QSharedPointer<PortAbstractionInterface> signalInterface_;
    BusInterfaceInterface* busInterface_;
    QString defaultPath_;

};

#endif // PORTSEDITOR_FACTORY_H
