//-----------------------------------------------------------------------------
// File: InterconnectionConfigurationValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.01.2016
//
// Description:
// Validator for the ipxact:interconnectionConfiguration.
//-----------------------------------------------------------------------------

#ifndef INTERCONNECTIONCONFIGURATIONVALIDATOR_H
#define INTERCONNECTIONCONFIGURATIONVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>
#include <QVector>

class InterconnectionConfiguration;
class MultipleAbstractorInstances;
class AbstractorInstance;
class InterfaceRef;

class ExpressionParser;

class LibraryInterface;

class Design;

class Interconnection;
class MonitorInterconnection;
class ComponentInstance;
class BusInterface;

//-----------------------------------------------------------------------------
//! Validator for the ipxact:interconnectionConfiguration.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT InterconnectionConfigurationValidator
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parser      The used expression parser.
     *      @param [in] library     The used library interface.
     */
    InterconnectionConfigurationValidator(QSharedPointer<ExpressionParser> parser, LibraryInterface* library);

	//! The destructor.
	~InterconnectionConfigurationValidator();
    
    /*!
     *  Change the available elements contained in a design.
     *
     *      @param [in] newDesign   The new design.
     */
    void designChanged(QSharedPointer<Design> newDesign);

    /*!
     *  Validates the given interconnection configuration.
     *
     *      @param [in] configuration   The selected interconnection configuration.
     *
     *      @return True, if the interconnection configuration is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<InterconnectionConfiguration> configuration);

    /*!
     *  Check if the interconnection reference is valid.
     *
     *      @param [in] configuration   The selected interconnection configuration.
     *
     *      @return True, if the interconnection reference is valid, otherwise false.
     */
    bool hasValidInterconnectionReference(QSharedPointer<InterconnectionConfiguration> configuration) const;

    /*!
     *  Check if the is present value is valid.
     *
     *      @param [in] isPresent   The selected is present value.
     *
     *      @return True, if the is present value is valid, otherwise false.
     */
    bool hasValidIsPresent(QString const& isPresent) const;

    /*!
     *  Check if the multiple abstractor instances are valid.
     *
     *      @param [in] configuration   The selected interconnection configuration.
     *
     *      @return True, if the multiple abstractor instances are valid, otherwise false.
     */
    bool hasValidMultipleAbstractorInstances(QSharedPointer<InterconnectionConfiguration> configuration);

    /*!
     *  Locate errors within an interconnection configuration.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] configuration   The selected interconnection configuration.
     *      @param [in] context         Context to help locate the errors.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<InterconnectionConfiguration> configuration,
        QString const& context);

private:

	// Disable copying.
	InterconnectionConfigurationValidator(InterconnectionConfigurationValidator const& rhs);
	InterconnectionConfigurationValidator& operator=(InterconnectionConfigurationValidator const& rhs);

    /*!
     *  Check if a multiple abstractor instance is valid.
     *
     *      @param [in] multipleInstance    The selected multiple abstractor instance.
     *
     *      @return True, if the multiple abstractor instance is valid, otherwise false.
     */
    bool multipleAbstractorInstanceIsValid(QSharedPointer<MultipleAbstractorInstances> multipleInstance);

    /*!
     *  Check if a multiple abstractor instance interface references are valid.
     *
     *      @param [in] multipleInstance    The selected multiple abstractor instance.
     *
     *      @return True, if the multiple abstractor instance interface references are valid, otherwise false.
     */
    bool hasValidInterfaceReferences(QSharedPointer<MultipleAbstractorInstances> multipleInstance);

    /*!
     *  Check if an interface reference component reference valid.
     *
     *      @param [in] interfaceReference  The selected interface reference.
     *
     *      @return True, if the interface reference component reference is valid, otherwise false.
     */
    bool interfaceReferenceComponentReferenceIsValid(QSharedPointer<InterfaceRef> interfaceReference) const;

    /*!
     *  Check if an interface reference bus reference is valid.
     *
     *      @param [in] interfaceReference  The selected interface reference.
     *
     *      @return True, if the interface reference bus reference is valid, otherwise false.
     */
    bool interfaceReferenceBusReferenceIsValid(QSharedPointer<InterfaceRef> interfaceReference);

    /*!
     *  Change the currently available bus interfaces.
     *
     *      @param [in] interfaceReference  The selected interface reference.
     */
    void changeAvailableBusInterfaces(QSharedPointer<InterfaceRef> interfaceReference);

    /*!
     *  Check if an abstractor instance is valid.
     *
     *      @param [in] instance    The selected abstractor instance.
     *
     *      @return True, if the abstractor instance is valid, otherwise false.
     */
    bool abstractorInstanceIsValid(QSharedPointer<AbstractorInstance> instance) const;

    /*!
     *  Check if a name is valid.
     *
     *      @param [in] name    The selected name.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QString const& name) const;

    /*!
     *  Find errors in interconnection reference.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] configuration   The selected interconnection configuration.
     *      @param [in] context         Context to help locate the errors.
     */
    void findErrorsInInterconnectionReference(QVector<QString>& errors,
        QSharedPointer<InterconnectionConfiguration> configuration, QString const& context) const;

    /*!
     *  Find errors in is present value.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] isPresent   The selected is present value.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInIsPresent(QVector<QString>& errors, QString const& isPresent,
        QString const& innerContext, QString const& context) const;

    /*!
     *  Find errors in multiple abstractor instances.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] configuration   The selected interconnection configuration.
     *      @param [in] innerContext    Context of the interconnection configuration to help locate the errors.
     *      @param [in] context         Context to help locate the errors.
     */
    void findErrorsInMultipleAbstractorInstances(QVector<QString>& errors,
        QSharedPointer<InterconnectionConfiguration> configuration, QString const& innerContext,
        QString const& context);

    /*!
     *  Find errors in a single multiple abstractor instance.
     *
     *      @param [in] errors              List of found errors.
     *      @param [in] multipleInstance    The selected multiple abstractor instance.
     *      @param [in] context             Context to help locate the errors.
     */
    void findErrorsInOneMultipleAbstractorInstance(QVector<QString>& errors,
        QSharedPointer<MultipleAbstractorInstances> multipleInstance, QString const& context);

    /*!
     *  Find errors in an abstractor instance.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] instance    The selected abstractor instance.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInAbstractorInstance(QVector<QString>& errors, QSharedPointer<AbstractorInstance> instance,
        QString const& context) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The used expression parser.
    QSharedPointer<ExpressionParser> parser_;

    //! The used library interface.
    LibraryInterface* libraryHandler_;

    //! The currently available interconnections.
    QSharedPointer<QList<QSharedPointer<Interconnection> > > availableInterconnections_;
    
    //! The currently available monitor interconnections.
    QSharedPointer<QList<QSharedPointer<MonitorInterconnection> > > availableMonitorInterconnections_;

    //! The currently available component instances.
    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > availableComponentInstances_;

    //! The currently available bus interfaces.
    QSharedPointer<QList<QSharedPointer<BusInterface> > > availableBusInterfaces_;
};

#endif // INTERCONNECTIONCONFIGURATIONVALIDATOR_H
