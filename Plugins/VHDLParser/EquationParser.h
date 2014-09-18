//-----------------------------------------------------------------------------
// File: EquationParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.09.2014
//
// Description:
// Parser for simple math equations.
//-----------------------------------------------------------------------------

#ifndef EQUATIONPARSER_H
#define EQUATIONPARSER_H

#include <QList>
#include <QSharedPointer>

class ModelParameter;

//-----------------------------------------------------------------------------
//! Parser for simple math equations.
//-----------------------------------------------------------------------------
class EquationParser 
{
public:

	//! The constructor.
	EquationParser(QList<QSharedPointer<ModelParameter> > modelParameters);

	//! The destructor.
	~EquationParser();

    /*!
     *  Parses a given equation.
     *
     *      @param [in] equation   The equation to parse.
     *
     *      @return The result of the equation. If equation cannot be solved, -1 is returned.
     */
    int parse(QString const& equation) const;

private:

	// Disable copying.
	EquationParser(EquationParser const& rhs);
	EquationParser& operator=(EquationParser const& rhs);
   
    QStringList toStringList(QString const& equation) const;

    /*!
     *  Checks if the given equation contains power operations.
     *
     *      @param [in] listedEquation   The equation to check.
     *
     *      @return True, if the equation has power operations.
     */
    bool hasPower(QStringList& listedEquation) const;

    /*!
     *  Solves a power operation from the given equation.
     *
     *      @param [in] listedEquation   The equation containing a power operation to solve.
     *
     *      @return The equation where power operation has been replaced with the result.
     */
    QStringList solvePower(QStringList& listedEquation) const;

    /*!
     *  Finds a value for a given string.
     *
     *      @param [in] term      The string whose value to find.
     *
     *      @return The value for the term.
     */
    int valueOf(QString const& term) const;

    /*!
     *  Finds the value for a given string from the known model parameters.
     *
     *      @param [in] term   The term whose value to find.
     *
     *      @return The value for the term.
     */
    int valueFromModelParameter(QString const& term) const;

    /*!
     *  Checks if the given equation contains multiply or division operations.
     *
     *      @param [in] listedEquation   The equation to check.
     *
     *      @return True, if the equation has multiply or division operations.
     */
    bool hasMultiplyOrDivide(QStringList& listedEquation) const;

    /*!
     *  Solves a multiply or division operation from the given equation.
     *
     *      @param [in] listedEquation   The equation containing a multiply or division operation to solve.
     *
     *      @return The equation where operation has been replaced with the result.
     */
    QStringList solveMultiplyAndDivide(QStringList& listedEquation) const;

    /*!
     *  Solves an add or subtract operation from the given equation.
     *
     *      @param [in] listedEquation   The equation containing an add or subtract operation to solve.
     *
     *      @return The equation where operation has been replaced with the result.
     */
    QStringList solveAddAndSubtract(QStringList& listedEquation) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The known model parameter values to use in equations.
    QList<QSharedPointer<ModelParameter> > knownValues_;
};

#endif // EQUATIONPARSER_H
