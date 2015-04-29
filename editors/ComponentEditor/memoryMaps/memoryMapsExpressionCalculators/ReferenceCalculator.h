//-----------------------------------------------------------------------------
// File: ReferenceCalculator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.04.2015
//
// Description:
// Calculates the usage of parameters in a given list of expressions.
//-----------------------------------------------------------------------------

#ifndef REFERENCECALCULATOR_H
#define REFERENCECALCULATOR_H

#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <QStringList>
#include <QMap>

//-----------------------------------------------------------------------------
//! Calculates the usage of parameters in a given list of expressions.
//-----------------------------------------------------------------------------
class ReferenceCalculator
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parameterFinder     Pointer to the instance for finding component parameters.
     */
    ReferenceCalculator(QSharedPointer<ParameterFinder> parameterFinder);

    /*!
     *  The destructor.
     */
    virtual ~ReferenceCalculator();

    /*!
     *  Calculates the parameters used in a given list of expressions.
     *
     *      @param [in] listOfPossibleReferences    A list of expressions with possible references.
     *
     *      @return A map containing pairs of referenced ids and the number of references made to them.
     */
    QMap<QString, int> getReferencedParameters(QStringList listOfPossibleReferences) const;

private:
	
	//! No copying
    ReferenceCalculator(const ReferenceCalculator& other);

	//! No assignment
    ReferenceCalculator& operator=(const ReferenceCalculator& other);

    //! The parameter finder.
    QSharedPointer<ParameterFinder> parameterFinder_;
};

#endif // REFERENCECALCULATOR_H
