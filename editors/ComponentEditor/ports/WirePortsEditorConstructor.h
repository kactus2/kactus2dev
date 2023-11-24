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
class PortsInterface;

//-----------------------------------------------------------------------------
//! Constructs wire ports editor items.
//-----------------------------------------------------------------------------
class WirePortsEditorConstructor : public PortsEditorConstructor
{
public:

    using PortsEditorConstructor::PortsEditorConstructor;

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
    PortsModel* constructModel(QObject* parent = 0) const final;

    /*!
     *  Construct a filter.
     *
     *      @param [in] portsInterface  Interface for accessing the component ports.
     *      @param [in] parent          Owner of the filter.
     *
     *      @return The created ports filter.
     */
    PortsFilter* createFilter(QObject* parent) const final;

    /*!
     *  Construct a view.
     *
     *      @param [in] busInterface    Interface for accessing bus interface.
     *      @param [in] parent          Owner of the view.
     *
     *      @return The created view.
     */
    PortsView* createView(QWidget* parent) const final;

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
    PortsDelegate* constructDelegate(QObject* parent) const  final;
};

#endif // WIREPORTSEDITORCONSTRUCTOR_H
