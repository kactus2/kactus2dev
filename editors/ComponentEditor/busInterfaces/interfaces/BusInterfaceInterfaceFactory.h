//-----------------------------------------------------------------------------
// File: BusInterfaceInterfaceFactory.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.03.2021
//
// Description:
// Constructs the bus interface interface.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEINTERFACEFACTORY_H
#define BUSINTERFACEINTERFACEFACTORY_H

#include <QSharedPointer>

#include <common/Global.h>

class LibraryInterface;

class Component;

class ParameterValidator;
class PortMapValidator;

class ParameterFinder;
class ExpressionFormatter;
class ExpressionParser;

class BusInterfaceInterface;
class AbstractionTypeInterface;
class PortMapInterface;
class TransparentBridgeInterface;
class FileSetInterface;
class MemoryMapInterface;
class ParametersInterface;

//-----------------------------------------------------------------------------
//! Constructs the bus interface interface.
//-----------------------------------------------------------------------------
namespace BusInterfaceInterfaceFactory
{
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
    KACTUS2_API BusInterfaceInterface* createBusInterface(
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<Component> component,
        LibraryInterface* library);

    namespace Details
    {
        /*!
         *  Create the common items in an interface for accessing bus interfaces.
         *
         *      @param [in] parameterFinder         The parameter finder.
         *      @param [in] expressionFormatter     Formatter for expressions.
         *      @param [in] expressionParser        Parser for expressions.
         *      @param [in] component               The containing component.
         *      @param [in] library                 Interface for accessing the library.
         *      @param [in] portMapValidator        Validator for port maps.
         *      @param [in] abstractionInterface    Interface for accessing abstraction types.
         *
         *      @return Interface for accessing bus interfaces.
         */
        BusInterfaceInterface* createCommonBusInterfaceItems(
            QSharedPointer<ParameterFinder> parameterFinder,
            QSharedPointer<ExpressionFormatter> expressionFormatter,
            QSharedPointer<ExpressionParser> expressionParser,
            QSharedPointer<Component> component,
            LibraryInterface* library,
            QSharedPointer<PortMapValidator> portMapValidator,
            AbstractionTypeInterface* abstractionInterface);

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
            QSharedPointer<ExpressionParser> expressionParser,
            QSharedPointer<Component> component);

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
            QSharedPointer<ExpressionParser> expressionParser,
            QSharedPointer<ParameterValidator> parameterValidator,
            QSharedPointer<Component> component);

        /*!
         *  Create the interface for accessing abstraction types.
         *
         *      @param [in] parameterFinder         The parameter finder.
         *      @param [in] expressionFormatter     Formatter for expressions.
         *      @param [in] expressionParser        Parser for expressions.
         *      @param [in] portMapValidator        The VLNV library.
         *      @param [in] component               The containing component.
         *      @param [in] library                 Interface for accessing the library.
         *
         *      @return Interface for accessing abstraction types.
         */
        AbstractionTypeInterface* createAbstractionTypeInterface(
            QSharedPointer<ParameterFinder> parameterFinder,
            QSharedPointer<ExpressionFormatter> expressionFormatter,
            QSharedPointer<ExpressionParser> expressionParser,
            QSharedPointer<PortMapValidator> portMapValidator,
            QSharedPointer<Component> component,
            LibraryInterface* library);

        /*!
         *  Create the common properties of an abstraction type interface.
         *
         *      @param [in] expressionParser    Parser for expressions.
         *      @param [in] component           The containing component.
         *      @param [in] portMapValidator    Validator for port maps.
         *      @param [in] library             The VLNV library.
         *      @param [in] portMapInterface    Interface for accessing the port maps.
         *
         *      @return Interface for accessing the abstraction types.
         */
        AbstractionTypeInterface* createCommonAbstractionTypeInterface(
            QSharedPointer<ExpressionParser> expressionParser,
            QSharedPointer<Component> component,
            QSharedPointer<PortMapValidator> portMapValidator,
            LibraryInterface* library,
            PortMapInterface* portMapInterface);

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
        PortMapInterface* createPortMapInterface(
            QSharedPointer<ParameterFinder> parameterFinder,
            QSharedPointer<ExpressionFormatter> expressionFormatter,
            QSharedPointer<ExpressionParser> expressionParser,
            QSharedPointer<PortMapValidator> portMapValidator,
            QSharedPointer<Component> component);

        /*!
         *  Create the interface for accessing transparent bridges.
         *
         *      @param [in] expressionFormatter     Formatter for expressions.
         *      @param [in] expressionParser        Parser for expressions.
         *
         *      @return Interface for accessing transparent bridges.
         */
        TransparentBridgeInterface* createBridgeInterface(QSharedPointer<ExpressionFormatter> expressionFormatter,
            QSharedPointer<ExpressionParser> expressionParser);

        /*!
         *  Create the interface for accessing parameters.
         *
         *      @param [in] validator               Validator for parameters.
         *      @param [in] expressionFormatter     Formatter for expressions.
         *      @param [in] expressionParser        Parser for expressions.
         *
         *      @return The interface for accessing parameters.
         */
        ParametersInterface* createParameterInterface(QSharedPointer<ParameterValidator> validator,
            QSharedPointer<ExpressionParser> expressionParser,
            QSharedPointer<ExpressionFormatter> expressionFormatter);
    };
}

#endif /* BUSINTERFACEINTERFACEFACTORY_H */
