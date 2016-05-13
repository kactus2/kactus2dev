//-----------------------------------------------------------------------------
// File: DesignDiagramResolver.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.05.2016
//
// Description:
// Resolver class containing functionality for calculating expressions within design diagrams.
//-----------------------------------------------------------------------------

#ifndef DESIGNDIAGRAMRESOLVER_H
#define DESIGNDIAGRAMRESOLVER_H

#include <QSharedPointer>

class ExpressionParser;
class ComponentParameterFinder;
class AdHocItem;
class Component;

//-----------------------------------------------------------------------------
//! Resolver class containing functionality for calculating expressions within design diagrams.
//-----------------------------------------------------------------------------
class DesignDiagramResolver
{

public:

    /*!
     *  The constructor.
     */
    DesignDiagramResolver();

	/*
     *  The destructor.
     */
    ~DesignDiagramResolver();

    /*!
     *  Resolve and draw a symbol for tie off in an ad hoc port item.
     *
     *      @param [in] tieOff      The tie off of the selected port.
     *      @param [in] tieOffPort  The selected port item.
     */
    void resolveAdhocTieOff(QString const& tieOff, AdHocItem* tieOffPort);

private:

    // Disable copying.
    DesignDiagramResolver(DesignDiagramResolver const& rhs);
    DesignDiagramResolver& operator=(DesignDiagramResolver const& rhs);

    /*!
     *  Get the parsed value from a tie off value.
     *
     *      @param [in] tieOffValue     The unparsed tie off value.
     *      @param [in] ownerComponent  The encompassing component of the ad hoc port containing the tie off value.
     *      @param [in] portItem        The ad hoc port item.
     *
     *      @return Parsed tie off value.
     */
    QString getParsedTieOffValue(QString const& tieOffValue, QSharedPointer<Component> ownerComponent,
        AdHocItem* portItem) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The used component parameter finder.
    QSharedPointer<ComponentParameterFinder> componentFinder_;

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // DESIGNDIAGRAMRESOLVER_H
