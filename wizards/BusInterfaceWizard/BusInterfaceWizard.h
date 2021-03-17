//-----------------------------------------------------------------------------
// File: BusInterfaceWizard.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// Wizard for generating bus interface from a set of ports.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEWIZARD_H
#define BUSINTERFACEWIZARD_H

#include <IPXACTmodels/common/VLNV.h>

#include <QWizard>
#include <QSharedPointer>

class BusInterface;
class BusInterfaceValidator;
class Component;
class LibraryInterface;
class ExpressionParser;
class ParameterValidator;
class IPXactSystemVerilogParser;
class BusInterfaceInterface;
class FileSetInterface;
class MemoryMapInterface;
class AbstractionTypeInterface;
class PortMapInterface;
class TransparentBridgeInterface;
class PortMapValidator;
class ParameterFinder;
class ExpressionFormatter;

//-----------------------------------------------------------------------------
//! Bus wizard dialog.
//-----------------------------------------------------------------------------
class BusInterfaceWizard : public QWizard
{
    Q_OBJECT

public:
    
    //! Enumeration for the wizard pages.
    enum Pages
    {
        PAGE_INTRO = 0,
        PAGE_GENERALOPTIONS,
        PAGE_BUSDEFINITION,
        PAGE_PORTMAPS,
        PAGE_SUMMARY
	 };    

    /*!
     *  The constructor.
     *
     *      @param [in] component                   The component for which the wizard is run.
     *      @param [in] busIf                       The bus interface for which the wizard is run.
     *      @param [in] handler                     Pointer to the instance which manages the library.
     *      @param [in] portNames                   Names of the physical ports used to generate the interface.
     *      @param [in] parent                      The parent widget.
     *      @param [in] absDefVLNV                  VLNV of the fixed abstraction definition of the bus interface.
     *                                              If invalid, VLNV can be chosen freely on general page.
     *      @param [in] descriptionAsLogicalName    Logical signal naming selection.
     */
    BusInterfaceWizard(QSharedPointer<Component> component,  
        QSharedPointer<BusInterface> busIf,
        LibraryInterface* handler, QStringList portNames,         
        QWidget* parent = 0, 
        VLNV absDefVLNV = VLNV(), 
        bool descriptionAsLogicalName = false);

	 /*!
	 *  Destructor.
	 */
	 ~BusInterfaceWizard();

signals:

     /*!
      *  Increase the number of references made to a specific parameter.
      *
      *      @param [in] id  The id of the parameter whose reference count is to be increased.
      */
     void increaseReferences(QString id);

     /*!
      *  Decrease the number of references made to a specific parameter.
      *
      *      @param [in] id  The id of the parameter whose reference count is to be decreased.
      */
     void decreaseReferences(QString id);

private:
    // Disable copying.
    BusInterfaceWizard(BusInterfaceWizard const& rhs);
    BusInterfaceWizard& operator=(BusInterfaceWizard const& rhs);

    /*!
     *  Create the interface for accessing bus interfaces.
     *
     *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] component               The containing component.
     *      @param [in] library                 Interface for accessing the library.
     *
     *      @return Interface for accessing bus interfaces.
     */
    BusInterfaceInterface* createBusInterface(QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<IPXactSystemVerilogParser> expressionParser, QSharedPointer<Component> component,
        LibraryInterface* library) const;

    /*!
     *  Create the interface for accessing file sets.
     *
     *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] component               The containing component.
     *
     *      @return Interface for accessing file sets.
     */
    FileSetInterface* createFileSetInterface(QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<IPXactSystemVerilogParser> expressionParser, QSharedPointer<Component> component) const;

    /*!
     *  Create the interface for accessing memory maps.
     *
     *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] parameterValidator      Validator for parameters.
     *      @param [in] component               The containing component.
     *
     *      @return Interface for accessing memory maps.
     */
    MemoryMapInterface* createMapInterface(QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<IPXactSystemVerilogParser> expressionParser,
        QSharedPointer<ParameterValidator> parameterValidator, QSharedPointer<Component> component) const;

    /*!
     *  Create the interface for accessing abstraction types.
     *
     *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] portMapValidator        Validator for port maps.
     *      @param [in] component               The containing component.
     *      @param [in] library                 Interface for accessing the library.
     *
     *      @return Interface for accessing abstraction types.
     */
    AbstractionTypeInterface* creaetAbstractionTypeInterface(QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<IPXactSystemVerilogParser> expressionParser,
        QSharedPointer<PortMapValidator> portMapValidator, QSharedPointer<Component> component,
        LibraryInterface* library) const;

    /*!
     *  Create the interface for accessing port maps.
     *
     *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] expressionParser        Parser for expressions.
     *      @param [in] portMapValidator        Validator for port maps.
     *      @param [in] component               The containing component.
     *
     *      @return Interface for accessing port maps.
     */
    PortMapInterface* createPortMapInterface(QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<IPXactSystemVerilogParser> expressionParser,
        QSharedPointer<PortMapValidator> portMapValidator, QSharedPointer<Component> component) const;

    /*!
     *  Create the interface for accessing transparent bridges.
     *
     *      @param [in] expressionFormatter     Formatter for expressions.
     *      @param [in] expressionParser        Parser for expressions.
     *
     *      @return Interface for accessing transparent bridges.
     */
    TransparentBridgeInterface* createBridgeInterface(QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<IPXactSystemVerilogParser> expressionParser) const;
};

#endif // BUSINTERFACEWIZARD_H
