//-----------------------------------------------------------------------------
// File: AbstractionTypeValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.01.2018
//
// Description:
// Validator for the ipxact:abstractionType.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONTYPEVALIDATOR_H
#define ABSTRACTIONTYPEVALIDATOR_H

#include <IPXACTmodels/generaldeclarations.h>

class LibraryInterface;

class View;
class Port;
class PortMap;
class PortAbstraction;

class AbstractionType;
class BusInterface;
class AbstractionDefinition;
class ConfigurableVLNVReference;

class PortMapValidator;

class ExpressionParser;

#include <QList>

//-----------------------------------------------------------------------------
//! Validator for the ipxact:abstractionType.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AbstractionTypeValidator
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] expressionParser    Expression parser.
     *      @param [in] views               Views contained within the component.
     *      @param [in] portMapValidator    Validator for port maps.
     *      @param [in] library             Interface for the VLNV library.
     */
    AbstractionTypeValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<QList<QSharedPointer<View> > > views, QSharedPointer<PortMapValidator> portMapValidator,
        LibraryInterface* library);

    /*!
     *  The destructor.
     */
    virtual ~AbstractionTypeValidator();

    /*!
     *  Change the currently selected component.
     *
     *      @param [in] newViews    Views contained within the new component.
     *      @param [in] newPorts    Ports contained within the new component.
     */
    void changeComponent(QSharedPointer<QList<QSharedPointer<View> > > newViews,
        QSharedPointer<QList<QSharedPointer<Port> > > newPorts);

    /*!
     *  Get the validator for port maps.
     *
     *      @return The port map validator.
     */
    QSharedPointer<PortMapValidator> getPortMapValidator() const;

    /*!
     *  Validates the given abstraction type.
     *
     *      @param [in] abstraction         The selected abstraction type.
     *      @param [in] abstractionList     List of the abstraction types contained within the bus interface.
     *      @param [in] containingBus       Bus interface containing the selected abstraction type.
     *
     *      @return True, if the selected abstraction type is valid, false otherwise.
     */
    virtual bool validate(QSharedPointer<AbstractionType> abstraction,
        QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionList,
        QSharedPointer<BusInterface> containingBus) const;

    /*!
     *  Check if the reference to the abstraction definition of the selected abstraction type is valid.
     *
     *      @param [in] abstraction     The selected abstraction type.
     *
     *      @return True, if the abstraction reference is valid, false otherwise.
     */
    bool hasValidAbstractionReference(QSharedPointer<AbstractionType> abstraction) const;

    /*!
     *  Check if the view references contained within the selected abstraction type are valid.
     *
     *      @param [in] abstraction         The selected abstraction type.
     *      @param [in] abstractionList     List of the abstraction types contained within the bus interface.
     *
     *      @return True, if the view references are valid, false otherwise.
     */
    bool hasValidViewReferences(QSharedPointer<AbstractionType> abstraction,
        QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionList) const;

    /*!
     *  Check if the selected view reference is valid.
     *
     *      @param [in] viewReference       The selected view reference.
     *      @param [in] abstractionList     List of the abstraction types contained within the bus interface.
     *
     *      @return True, if the selected view reference is valid, false otherwise.
     */
    bool referencedViewIsValid(QString const& viewReference,
        QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionList) const;

    /*!
     *  Check if the port maps contained within the selected abstraction type are valid.
     *
     *      @param [in] abstraction     The selected abstraction type.
     *      @param [in] containingBus   Bus interface containing the selected abstraction type.
     *
     *      @return True, if the port maps are valid, false otherwise.
     */
    bool hasValidPortMaps(QSharedPointer<AbstractionType> abstraction, QSharedPointer<BusInterface> containingBus)
        const;

    /*!
     *  Find errors within the selected abstraction type.
     *
     *      @param [in/out] errors          List of the found errors.
     *      @param [in] abstraction         The selected abstraction type.
     *      @param [in] abstractionList     List of the abstraction types contained within the bus interface.
     *      @param [in] containingBus       Bus interface containing the selected abstraction type.
     *      @param [in] context             Context of the errors.
     */
    virtual void findErrorsIn(QVector<QString>& errors, QSharedPointer<AbstractionType> abstraction,
        QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionList,
        QSharedPointer<BusInterface> containingBus, QString const& context) const;

private:

    // Disable copying.
    AbstractionTypeValidator(AbstractionTypeValidator const& rhs);
    AbstractionTypeValidator& operator=(AbstractionTypeValidator const& rhs);

    /*!
     *  Check if the selected view reference exists.
     *
     *      @param [in] viewReference   The selected view reference.
     *
     *      @return True, if the view reference exists, false otherwise.
     */
    bool referencedViewExists(QString const& viewReference) const;

    /*!
     *  Check if the selected view reference is used only once in the containing bus interface.
     *
     *      @param [in] viewReference       The selected view reference.
     *      @param [in] abstractionList     List of the abstraction types contained within the bus interface.
     *
     *      @return True, if the selected view is referenced once, false otherwise.
     */
    bool viewIsReferencedOnce(QString const& viewReference,
        QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionList) const;

    /*!
     *  Get the abstraction definition referenced by the abstraction type.
     *
     *      @param [in] abstractionReference    The VLNV reference of the selected abstraction type.
     *
     *      @return Abstraction definition matching the selected VLNV reference.
     */
    QSharedPointer<AbstractionDefinition const> getAbstractionDefinition(
        QSharedPointer<ConfigurableVLNVReference> abstractionReference) const;

    /*!
     *  Check if the required logical ports have been mapped.
     *
     *      @param [in] containingBus           Bus interface containing the abstraction type.
     *      @param [in] abstractionDefinition   Abstraction definition referenced by the abstraction type.
     *      @param [in] portMaps                List of the port maps contained within the abstraction type.
     *
     *      @return True, if the required port abstractions have been mapped, false otherwise.
     */
    bool requiredPortAbstractionsAreMapped(QSharedPointer<BusInterface> containingBus,
        QSharedPointer<AbstractionDefinition const> abstractionDefinition,
        QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps) const;

    /*!
     *  Check if the selected logical port has been referenced in any port maps.
     *
     *      @param [in] portName    Name of the selected logical port.
     *      @param [in] portMaps    List of the contained port maps.
     *
     *      @return True, if the logical port has been mapped, false otherwise.
     */
    bool logicalPortHasReferencingPortMaps(QString const& portName,
        QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps) const;

    /*!
     *  Check if the logical port has valid presence.
     *
     *      @param [in] abstractionDefinition   Abstraction definition of the abstraction type.
     *      @param [in] logicalPortName         Name of the selected logical port.
     *      @param [in] mode                    Interface mode of the containing bus interface.
     *      @param [in] systemGroup             System group used in the containing bus interface.
     *
     *      @return True, if the logical port has valid presence, false otherwise.
     */
    bool logicalPortHasValidPresence(QSharedPointer<AbstractionDefinition const> abstractionDefinition,
        QString const& logicalPortName, General::InterfaceMode mode, QString const& systemGroup) const;

    /*!
     *  Get the selected logical port from the selected abstraction definition.
     *
     *      @param [in] portName                Name of the selected logical port.
     *      @param [in] abstractionDefinition   The selected abstraction definition.
     *
     *      @return The logical port with the selected name.
     */
    QSharedPointer<PortAbstraction> getLogicalPort(QString const& portName,
        QSharedPointer<AbstractionDefinition const> abstractionDefinition) const;

    /*!
     *  Find errors within the view references of the selected abstraction type.
     *
     *      @param [in/out] errors          List of found errors.
     *      @param [in] abstraction         The selected Abstraction type.
     *      @param [in] abstractionList     List of the abstraction types contained within the bus interface.
     *      @param [in] context             Context of the errors.
     */
    void findErrorsInReferencedViews(QVector<QString>& errors, QSharedPointer<AbstractionType> abstraction,
        QSharedPointer<QList<QSharedPointer<AbstractionType> > > abstractionList, QString const& context) const;

    /*!
     *  Find errors within the abstraction definition reference of the selected abstraction type.
     *
     *      @param [in/out] errors      List of found errors.
     *      @param [in] abstraction     The selected abstraction type.
     *      @param [in] context         Context of the errors.
     */
    void findErrorsInAbstractionReference(QVector<QString>& errors, QSharedPointer<AbstractionType> abstraction,
        QString const& context) const;

    /*!
     *  Find errors within the port maps of the selected abstraction type.
     *
     *      @param [in/out] errors      List of found errors.
     *      @param [in] abstraction     The selected abstraction type.
     *      @param [in] containingBus   Bus interface containing the selected abstraction type.
     *      @param [in] context         Context of the errors.
     */
    void findErrorsInPortMaps(QVector<QString>& errors, QSharedPointer<AbstractionType> abstraction,
        QSharedPointer<BusInterface> containingBus, QString const& context) const;

    /*!
     *  Find errors within the required port map logical ports.
     *
     *      @param [in/out] errors              List of found errors.
     *      @param [in] context                 Context of the errors.
     *      @param [in] busInterface            Bus interface containing the abstraction definition reference.
     *      @param [in] abstractionDefinition   The selected abstraction definition.
     *      @param [in] portMaps                List of the port maps.
     */
    void findErrorsInRequiredPortAbstractions(QVector<QString>& errors, QString const& context,
        QSharedPointer<BusInterface> busInterface,
        QSharedPointer<AbstractionDefinition const> abstractionDefinition,
        QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for the VLNV library.
    LibraryInterface* library_;

    //! List of the view contained within the containing component.
    QSharedPointer<QList<QSharedPointer<View> > > availableViews_;

    //! Validator for port maps.
    QSharedPointer<PortMapValidator> portMapValidator_;

    //! The expression validator.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // ABSTRACTIONTYPEVALIDATOR_H
