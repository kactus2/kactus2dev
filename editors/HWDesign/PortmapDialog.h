//-----------------------------------------------------------------------------
// File: PortmapDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 10.6.2011
//
// Description:
// Port map dialog.
//-----------------------------------------------------------------------------

#ifndef PORTMAPDIALOG_H
#define PORTMAPDIALOG_H

#include <QDialog>

class BusInterface;
class Component;
class LibraryInterface;
class BusInterfacePortMapTab;
class BusInterfaceInterface;
class FileSetInterface;
class PortMapInterface;
class MemoryMapInterface;
class AbstractionTypeInterface;
class TransparentBridgeInterface;
class PortMapValidator;
class ParameterValidator;
class ParameterFinder;
class ExpressionFormatter;
class IPXactSystemVerilogParser;

//-----------------------------------------------------------------------------
//! PortmapDialog class.
//-----------------------------------------------------------------------------
class PortmapDialog : public QDialog
{
public:

    /*!
     *  Constructor.
     *
     *      @param [in] library     The library interface.
     *      @param [in] component   The parent component of the bus interface.
     *      @param [in] busIf       The bus interface whose port map will be edited.
     *      @param [in] otherBusIf  The bus interface in the other end of the connection.
     *      @param [in] parent      The owner of this dialog.
     */
    PortmapDialog(LibraryInterface* library, QSharedPointer<Component> component,
                  QSharedPointer<BusInterface> busIf, QSharedPointer<BusInterface> otherBusIf, QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    ~PortmapDialog();

public slots:
    void accept();

private:
    // Disable copying.
    PortmapDialog(PortmapDialog const& rhs);
    PortmapDialog& operator=(PortmapDialog const& rhs);

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

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Port map editor widget.
    BusInterfacePortMapTab* portmapWidget_;

    //! The bus interface whose port map to edit.
    QSharedPointer<BusInterface> busIf_;

    //! The bus interface in the other end of the connection (for validity checks).
    QSharedPointer<BusInterface> otherBusIf_;
};

//-----------------------------------------------------------------------------

#endif // PORTMAPDIALOG_H
