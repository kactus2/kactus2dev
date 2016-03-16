//-----------------------------------------------------------------------------
// File: AdHocConnectionValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.01.2016
//
// Description:
// Validator for the ipxact:adHocConnection.
//-----------------------------------------------------------------------------

#ifndef ADHOCCONNECTIONVALIDATOR_H
#define ADHOCCONNECTIONVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QString>
#include <QVector>

class LibraryInterface;

class ExpressionParser;

class AdHocConnection;
class PortReference;
class ComponentInstance;
class PartSelect;

class Component;
class Port;

//-----------------------------------------------------------------------------
//! Validator for the ipxact:adHocConnection.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AdHocConnectionValidator
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parser      The used expression parser.
     *      @param [in] library     The used library interface.
     */
    AdHocConnectionValidator(QSharedPointer<ExpressionParser> parser, LibraryInterface* library);

	//! The destructor.
	~AdHocConnectionValidator();
    
    /*!
     *  Change the used component instances.
     *
     *      @param [in] newInstances    A list of the new component instances.
     */
    void changeComponentInstances(QSharedPointer<QList<QSharedPointer<ComponentInstance> > > newInstances);

    /*!
     *  Validates the given ad hoc connection.
     *
     *      @param [in] connection  The selected ad hoc connection.
     *
     *      @return True, if the ad hoc connection is valid IP-XACT, otherwise false.
     */
    bool validate(QSharedPointer<AdHocConnection> connection) const;

    /*!
     *  Check if the name is valid.
     *
     *      @param [in] connection  The selected ad hoc connection.
     *
     *      @return True, if the name is valid, otherwise false.
     */
    bool hasValidName(QSharedPointer<AdHocConnection> connection) const;

    /*!
     *  Check if the isPresent is valid.
     *
     *      @param [in] connection  The selected ad hoc connection.
     *
     *      @return True, if the isPresent is valid, otherwise false.
     */
    bool hasValidIsPresent(QString const& isPresent) const;

    /*!
     *  Check if the tied value is valid.
     *
     *      @param [in] connection  The selected ad hoc connection.
     *
     *      @return True, if the tied value is valid, otherwise false.
     */
    bool hasValidTiedValue(QSharedPointer<AdHocConnection> connection) const;

    /*!
     *  Check if the port references are valid.
     *
     *      @param [in] connection  The selected ad hoc connection.
     *
     *      @return True, if the port references are valid, otherwise false.
     */
    bool hasValidPortReferences(QSharedPointer<AdHocConnection> connection) const;

    /*!
     *  Locate errors within an ad hoc connection.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] connection  The selected ad hoc connection.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsIn(QVector<QString>& errors, QSharedPointer<AdHocConnection> connection, QString const& context)
        const;

private:

	// Disable copying.
	AdHocConnectionValidator(AdHocConnectionValidator const& rhs);
	AdHocConnectionValidator& operator=(AdHocConnectionValidator const& rhs);

    /*!
     *  Check if an internal port reference is valid.
     *
     *      @param [in] portReference   The selected internal port reference.
     *      @param [in] tiedValue       Tied value of the ad hoc connection.
     *
     *      @return True, if the internal port reference is valid, otherwise false.
     */
    bool internalPortReferenceIsValid(QSharedPointer<PortReference> portReference, QString const& tiedValue) const;

    /*!
     *  Get the referenced component instance.
     *
     *      @param [in] instanceReference   The name of the referenced component instance.
     *
     *      @return The referenced component instance, or a null pointer if the component instance is not found.
     */
    QSharedPointer<ComponentInstance> getReferencedComponentInstance(QString const& instanceReference) const;

    /*!
     *  Get the component referenced by the component instance.
     *
     *      @param [in] referencingInstance     The selected component instance.
     *
     *      @return The referenced component instance, or a null pointer if the component is not found.
     */
    QSharedPointer<const Component> getReferencedComponent(QSharedPointer<ComponentInstance> referencingInstance) const;

    /*!
     *  Get the port referenced by the port reference.
     *
     *      @param [in] component       The selected component.
     *      @param [in] portReference   The selected port reference.
     *
     *      @return The referenced port, or a null pointer if the port is not found.
     */
    QSharedPointer<Port> getReferencedPort(QSharedPointer<const Component> component,
        QSharedPointer<PortReference> portReference) const;

    /*!
     *  Check if the referenced port default value is valid.
     *
     *      @param [in] tiedValue       The tied value of the connection.
     *      @param [in] referencedPort  The referenced port.
     *
     *      @return True, if the port default value is valid, otherwise false.
     */
    bool tiedValueIsValidWithReferencedPort(QString const& tiedValue, QSharedPointer<Port> referencedPort) const;

    /*!
     *  Check if the external port reference is valid.
     *
     *      @param [in] externalPort    The selected external port.
     *
     *      @return True, if the external port reference is valid, otherwise false.
     */
    bool externalPortReferenceIsValid(QSharedPointer<PortReference> externalPort) const;

    /*!
     *  Check if the port reference part select is valid.
     *
     *      @param [in] partSelect  The selected part select.
     *
     *      @return True, if the port reference part select is valid, otherwise false.
     */
    bool portReferencePartSelectIsValid(QSharedPointer<PartSelect> partSelect) const;

    /*!
     *  Check if an expression is valid unsigned int expression.
     *
     *      @param [in] expression  The selected expression.
     *
     *      @return True, if the expression is valid, otherwise false.
     */
    bool unsignedIntExpressionIsValid(QString const& expression) const;

    /*!
     *  Find errors in name.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] connection  The selected ad hoc connection.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInName(QVector<QString>& errors, QSharedPointer<AdHocConnection> connection,
        QString const& context) const;

    /*!
     *  Find errors in isPresent.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] isPresent       The selected isPresent value.
     *      @param [in] innerContext    Context information of the ad hoc connection.
     *      @param [in] context         Context to help locate the errors.
     */
    void findErrorsInIsPresent(QVector<QString>& errors, QString const& isPresent, QString const& innerContext,
        QString const& context) const;

    /*!
     *  Find errors in tied value.
     *
     *      @param [in] errors      List of found errors.
     *      @param [in] connection  The selected ad hoc connection.
     *      @param [in] context     Context to help locate the errors.
     */
    void findErrorsInTiedValue(QVector<QString>& errors, QSharedPointer<AdHocConnection> connection,
        QString const& context) const;

    /*!
     *  Find errors in port references.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] connection      The selected ad hoc connection.
     *      @param [in] innerContext    Context information of the ad hoc connection.
     *      @param [in] context         Context to help locate the errors.
     */
    void findErrorsInPortReferences(QVector<QString>& errors, QSharedPointer<AdHocConnection> connection,
        QString const& innerContext, QString const& context) const;

    /*!
     *  Find errors in an internal port reference.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] internalPort    The selected internal port reference.
     *      @param [in] tiedValue       The tied value of the connection.
     *      @param [in] elementName     Element name of the internal port reference.
     *      @param [in] innerContext    Context information of the ad hoc connection.
     *      @param [in] context         Context to help locate the errors.
     */
    void findErrorsInInternalPortReference(QVector<QString>& errors, QSharedPointer<PortReference> internalPort,
        QString const& tiedValue, QString const& elementName, QString const& innerContext, QString const& context)
        const;

    /*!
     *  Find errors in an external port reference.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] externalPort    The selected external port reference.
     *      @param [in] elementName     Element name of the external port reference.
     *      @param [in] innerContext    Context information of the ad hoc connection.
     *      @param [in] context         Context to help locate the errors.
     */
    void findErrorsInExternalPortReference(QVector<QString>& errors, QSharedPointer<PortReference> externalPort,
        QString const& elementName, QString const& innerContext, QString const& context) const;

    /*!
     *  Find errors in port reference part select.
     *
     *      @param [in] errors          List of found errors.
     *      @param [in] partSelect      The selected part select.
     *      @param [in] elementName     The element name of the port reference.
     *      @param [in] innerContext    Context information of the ad hoc connection.
     *      @param [in] context         Context to help locate the errors.
     */
    void findErrorsInPortReferencePartSelect(QVector<QString>& errors, QSharedPointer<PartSelect> partSelect,
        QString const& elementName, QString const& innerContext, QString const& context) const;

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

#endif // ADHOCCONNECTIONVALIDATOR_H
