//-----------------------------------------------------------------------------
// File: InterconnectionValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.01.2016
//
// Description:
// Validator for the ipxact:interconnection and ipxact:monitorInterconnection.
//-----------------------------------------------------------------------------

#ifndef INTERCONNETIONVALIDATOR_H
#define INTERCONNETIONVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>
#include <QVector>

class Interconnection;
class MonitorInterconnection;

class LibraryInterface;
class ExpressionParser;

class ActiveInterface;
class MonitorInterface;

class Component;
class ComponentInstance;
class BusInterface;

//-----------------------------------------------------------------------------
//! Validator for the ipxact:interconnection and ipxact:monitorInterconnection.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT InterconnectionValidator
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parser      The used expression parser.
     *      @param [in] library     The used library interface.
     */
    InterconnectionValidator(QSharedPointer<ExpressionParser> parser, LibraryInterface* library);

	//! The destructor.
	~InterconnectionValidator();
    
    /*!
     *  Change the available component instances.
     *
     *      @param [in] newInstances    The new component instances.
     */
    void changeComponentInstances(QSharedPointer<QList<QSharedPointer<ComponentInstance> > > newInstances);

    /*!
     *  Validates the given interconnection.
     *
     *      @param [in] interconnection     The selected interconnection.
     *
     *      @return True, if the interconnection is valid IP-XACT, otherwise false.
     */
    bool validateInterconnection(QSharedPointer<Interconnection> interconnection) const;

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] name    The selected name.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name) const;

    /*!
     *  Check if the isPresent is valid.
     *
     *      @param [in] isPresent   The selected isPresent value.
     *
     *      @return True, if the isPresent is valid, otherwise false.
     */
    bool hasValidIsPresent(QString const& isPresent) const;

    /*!
     *  Check if the start interface is valid.
     *
     *      @param [in] interconnection     The selected interconnection.
     *
     *      @return True, if the start interface is valid, otherwise false.
     */
    bool hasValidStartInterface(QSharedPointer<Interconnection> interconnection) const;

    /*!
     *  Check if the interfaces are valid.
     *
     *      @param [in] interconnection     The selected interconnection.
     *
     *      @return True, if the interfaces are valid, otherwise false.
     */
    bool hasValidInterfaces(QSharedPointer<Interconnection> interConnection) const;

    /*!
     *  Validates the given monitor interconnection.
     *
     *      @param [in] connection  The selected monitor interconnection.
     *
     *      @return True, if the monitor interconnection is valid, otherwise false.
     */
    bool validateMonitorInterconnection(QSharedPointer<MonitorInterconnection> connection) const;

    /*!
     *  Check if the monitored active interface is valid.
     *
     *      @param [in] connection  The selected monitor interconnection.
     *
     *      @return True, if the monitored active interface is valid, otherwise false.
     */
    bool hasValidMonitoredActiveInterface(QSharedPointer<MonitorInterconnection> connection) const;

    /*!
     *  Check if the monitor interfaces are valid.
     *
     *      @param [in] connection  The selected monitor interconnection.
     *
     *      @return True, if the monitor interfaces are valid, otherwise false.
     */
    bool hasValidMonitorInterfaces(QSharedPointer<MonitorInterconnection> connection) const;

    /*!
     *  Locate errors within an interconnection.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] interconnection     The selected interconnection.
     *      @param [in] context             Context to help locate the errors.
     */
    void findErrorsInInterconnection(QVector<QString>& errors, QSharedPointer<Interconnection> interconnection,
        QString const& context) const;

    /*!
     *  Locate errors within a monitor interconnection.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] connection  The selected monitor interconnection.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInMonitorInterconnection(QVector<QString>& errors,
        QSharedPointer<MonitorInterconnection> connection, QString const& context) const;

private:

	// Disable copying.
	InterconnectionValidator(InterconnectionValidator const& rhs);
	InterconnectionValidator& operator=(InterconnectionValidator const& rhs);

    /*!
     *  Check if a reference combination of component and bus interface is valid.
     *
     *      @param [in] referenceCombinations   The used component / bus interface combinations.
     *      @param [in] componentReference      The selected component reference.
     *      @param [in] busReference            The selected bus reference.
     *
     *      @return True, if the component / bus interface reference pair is unique, otherwise false.
     */
    bool referenceCombinationIsUnique(QMap<QString, QString> referenceCombinations,
        QString const& componentReference, QString const& busReference) const;

    /*!
     *  Check if an active interface is valid.
     *
     *      @param [in] activeInterface     The selected active interface.
     *
     *      @return True, if the active interface is valid, otherwise false.
     */
    bool activeInterfaceIsValid(QSharedPointer<ActiveInterface> activeInterface) const;

    /*!
     *  Get the reference component instance.
     *
     *      @param [in] instanceReference   The selected component instance reference.
     *
     *      @return The referenced component instance.
     */
    QSharedPointer<ComponentInstance> getReferencedComponentInstance(QString const& instanceReference) const;

    /*!
     *  Get the component referenced by the component instance.
     *
     *      @param [in] referencingInstance     The selected component instance.
     *
     *      @return The referenced component.
     */
    QSharedPointer<const Component> getReferencedComponent(QSharedPointer<ComponentInstance> referencingInstance) const;

    /*!
     *  Check if the bus interface reference is valid.
     *
     *      @param [in] component       The referenced component.
     *      @param [in] busReference    The selected bus interface reference.
     *
     *      @return True, if the bus interface reference is valid, otherwise false.
     */
    bool busReferenceIsValid(QSharedPointer<const Component> component, QString const& busReference) const;

    /*!
     *  Check if the exclude ports of an active interface are valid.
     *
     *      @param [in] component           The referenced component.
     *      @param [in] activeInterface     The selected active interface.
     *
     *      @return True, if the exclude ports are valid, otherwise false.
     */
    bool excludePortsAreValid(QSharedPointer<const Component> component, QSharedPointer<ActiveInterface> activeInterface)
        const;

    /*!
     *  Check if a single exclude port is valid.
     *
     *      @param [in] portReference   The selected port reference.
     *      @param [in] currentBus      The referenced bus interfaces.
     *
     *      @return True, if the exclude port is valid, otherwise false.
     */
    bool singleExcludePortIsValid(QString const& portReference, QSharedPointer<BusInterface> currentBus) const;

    /*!
     *  Check if a monitor interface is valid.
     *
     *      @param [in] monitorInterface    The selected monitor interface.
     *
     *      @return True, if the monitor interface is valid, otherwise false.
     */
    bool monitorInterfaceIsValid(QSharedPointer<MonitorInterface> monitorInterface) const;

    /*!
     *  Find errors in name.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] name        The selected name.
     *      @param [in] element     The name of the containing element.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInName(QVector<QString>& errors, QString const& name, QString const& element,
        QString const& context) const;

    /*!
     *  Find errors in isPresent.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] isPresent       The selected isPresent value.
     *      @param [in] innerContext    The item context to help locate the errors.
     *      @param [in] context         Context to help locate the errors.
     */
    void findErrorsInIsPresent(QVector<QString>& errors, QString const& isPresent, QString const& innerContext,
        QString const& context) const;

    /*!
     *  Find errors in the interconnection start interface.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] interconnection     The selected interconnection.
     *      @param [in] innerContext        The item context to help locate the errors.
     *      @param [in] context             Context to help locate the errors.
     */
    void findErrorsInStartInterface(QVector<QString>& errors, QSharedPointer<Interconnection> interconnection,
        QString const& innerContext, QString const& context) const;

    /*!
     *  Find errors in an active interface.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] activeInterface     The selected active interface.
     *      @param [in] innerContext        The item context to help locate the errors.
     *      @param [in] context             Context to help locate the errors.
     */
    void findErrorsInActiveInterface(QVector<QString>& errors, QSharedPointer<ActiveInterface> activeInterface,
        QString const& innerContext, QString const& context) const;

    /*!
     *  Find errors in component reference.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] componentReference  The selected component reference.
     *      @param [in] referencedInstance  The referenced component instance.
     *      @param [in] elementName         The name of the containing element.
     *      @param [in] innerContext        The item context to help locate the errors.
     *      @param [in] context             Context to help locate the errors.
     */
    void findErrorsInComponentReference(QVector<QString>& errors, QString const& componentReference,
        QSharedPointer<ComponentInstance> referencedInstance, QString const& elementName,
        QString const& innerContext, QString const& context) const;

    /*!
     *  Find errors in bus interface reference.
     *
     *      @param [in] errors                  List of found errors.
     *      @param [in] busReference            The selected bus interface reference.
     *      @param [in] referencedComponent     The referenced component.
     *      @param [in] elementName             The containing element.
     *      @param [in] innerContext            The item context to help locate the errors.
     *      @param [in] context                 Context to help locate the errors.
     */
    void findErrorsInBusReference(QVector<QString>& errors, QString const& busReference,
        QSharedPointer<const Component> referencedComponent, QString const& elementName, QString const& innerContext,
        QString const& context) const;

    /*!
     *  Find errors in exclude ports.
     *
     *      @param [in] errors                  List of found errors.
     *      @param [in] activeInterface         The selected active interface.
     *      @param [in] referencedComponent     The referenced component
     *      @param [in] innerContext            The item context to help locate the errors.
     *      @param [in] context                 Context to help locate the errors.
     */
    void findErrorsInExcludePorts(QVector<QString>& errors, QSharedPointer<ActiveInterface> activeInterface,
        QSharedPointer<const Component> referencedComponent, QString const& innerContext) const;

    /*!
     *  Find errors in interfaces.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] interconnection     The selected interconnection.
     *      @param [in] innerContext        The item context to help locate the errors.
     *      @param [in] context             Context to help locate the errors.
     */
    void findErrorsInInterfaces(QVector<QString>& errors, QSharedPointer<Interconnection> interConnection,
        QString const& innerContext, QString const& context) const;

    /*!
     *  Find errors in monitored active interface.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] connection      The selected connection.
     *      @param [in] innerContext    The item context to help locate the errors.
     *      @param [in] context         Context to help locate the errors.
     */
    void findErrorsInMonitoredActiveInterface(QVector<QString>& errors,
        QSharedPointer<MonitorInterconnection> connection, QString const& innerContext, QString const& context)
        const;

    /*!
     *  Find errors in a monitor interface.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] monitorInterface    The selected monitor interface.
     *      @param [in] innerContext        The item context to help locate the errors.
     *      @param [in] context             Context to help locate the errors.
     */
    void findErrorsInSingleMonitorInterface(QVector<QString>& errors,
        QSharedPointer<MonitorInterface> monitorInterface, QString const& elementName, QString const& innerContext,
        QString const& context) const;

    /*!
     *  Find errors in monitor interfaces.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] connection      The selected monitor interconnection.
     *      @param [in] innerContext    The item context to help locate the errors.
     *      @param [in] context         Context to help locate the errors.
     */
    void findErrorsInMonitorInterfaces(QVector<QString>& errors, QSharedPointer<MonitorInterconnection> connection,
        QString const& innerContext, QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The used expression parser.
    QSharedPointer<ExpressionParser> parser_;

    //! The containing library.
    LibraryInterface* libraryHandler_;

    //! The currently available component instances.
    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > availableComponentInstances_;
};

#endif // INTERCONNETIONVALIDATOR_H
