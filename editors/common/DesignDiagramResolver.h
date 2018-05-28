//-----------------------------------------------------------------------------
// File: DesignDiagramResolver.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.05.2016
//
// Description:
// Resolver class containing functionality for calculating expressions within design diagrams.
//-----------------------------------------------------------------------------

#ifndef DESIGNDIAGRAMRESOLVER_H
#define DESIGNDIAGRAMRESOLVER_H

#include <IPXACTmodels/common/validators/ValueFormatter.h>

#include <QSharedPointer>

class ExpressionParser;
class ComponentParameterFinder;
class AdHocItem;
class Component;
class ExpressionFormatter;
class MultipleParameterFinder;
class ListParameterFinder;

//-----------------------------------------------------------------------------
//! Resolver class containing functionality for calculating expressions within design diagrams.
//-----------------------------------------------------------------------------
class DesignDiagramResolver
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] designParameterFinder   Locates the design parameters.
     */
    DesignDiagramResolver(QSharedPointer<ListParameterFinder> designParameterFinder);

    // Disable copying.
    DesignDiagramResolver(DesignDiagramResolver const& rhs) = delete;
    DesignDiagramResolver& operator=(DesignDiagramResolver const& rhs) = delete;

	//! The destructor.
    ~DesignDiagramResolver() = default;

    void setContext(QSharedPointer<Component const> component);

    QString parseToConstant(QString const& expression) const;

    /*!
     *  Resolve and draw a symbol for tie off in an ad hoc port item.
     *
     *      @param [in] tieOff      The tie off of the selected port.
     *      @param [in] tieOffPort  The selected port item.
     */
    void resolveAdhocTieOff(QString const& tieOff, AdHocItem* tieOffPort);

private:


    /*!
     *  Get the parsed value from a tie off value.
     *
     *      @param [in] tieOffValue     The unparsed tie off value.
     *      @param [in] portName        The ad hoc port name.
     *      @param [in] ownerComponent  The encompassing component of the ad hoc port containing the tie off value.     
     *
     *      @return Parsed tie off value.
     */
    QString getParsedTieOffValue(QString const& tieOffValue,
        QString const& portName, QSharedPointer<Component const> ownerComponent) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The used design parameter finder.
    QSharedPointer<ListParameterFinder> designParameterFinder_;

    //! The used design and component instance parameter finder.
    QSharedPointer<MultipleParameterFinder> designAndInstanceParameterFinder_;

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! Used to format expressions from using referenced UUIDs to using referenced parameter names.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;

    //! Formats the value to the base number of the expression.
    ValueFormatter valueFormatter_;
};

#endif // DESIGNDIAGRAMRESOLVER_H
