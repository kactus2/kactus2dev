//-----------------------------------------------------------------------------
// File: RegisterExpressionGatherer.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.04.2015
//
// Description:
// Gathers expressions from a register and its fields.
//-----------------------------------------------------------------------------

#ifndef REGISTEREXPRESSIONSGATHERER_H
#define REGISTEREXPRESSIONSGATHERER_H

#include <QSharedPointer>
#include <QStringList>

class Register;

//-----------------------------------------------------------------------------
//! Gathers expressions from a register and its fields.
//-----------------------------------------------------------------------------
class RegisterExpressionsGatherer
{

public:

    /*!
     *  The constructor.
     */
    RegisterExpressionsGatherer();

    /*!
     *  The destructor.
     */
    virtual ~RegisterExpressionsGatherer();

    /*!
     *  Get the expressions from a given register.
     *
     *    @param [in] currentRegister     The given register.
     *
     *    @return A list containing all the expressions used in a register.
     */
    QStringList getExpressions(QSharedPointer<Register> currentRegister) const;

private:
	
	//! No copying
    RegisterExpressionsGatherer(const RegisterExpressionsGatherer& other);

	//! No assignment
    RegisterExpressionsGatherer& operator=(const RegisterExpressionsGatherer& other);

};

#endif // REGISTEREXPRESSIONSGATHERER_H
