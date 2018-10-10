#ifndef REGISTERFILEEXPRESSIONGATHERER_H
#define REGISTERFILEEXPRESSIONGATHERER_H

#include <QSharedPointer>
#include <QStringList>

class RegisterFile;

//-----------------------------------------------------------------------------
//! Gathers expressions from a register file and its registers
//-----------------------------------------------------------------------------
class RegisterFileExpressionsGatherer
{

public:

    /*!
     *  The constructor.
     */
    RegisterFileExpressionsGatherer();

    /*!
     *  The destructor.
     */
    virtual ~RegisterFileExpressionsGatherer();

    /*!
     *  Get the expressions from a given register.
     *
     *      @param [in] currentRegister     The given register.
     *
     *      @return A list containing all the expressions used in a register.
     */
    QStringList getExpressions(QSharedPointer<RegisterFile> currentRegisterFile) const;

private:

    //! No copying
    RegisterFileExpressionsGatherer(const RegisterFileExpressionsGatherer& other);

    //! No assignment
    RegisterFileExpressionsGatherer& operator=(const RegisterFileExpressionsGatherer& other);

};

#endif // REGISTERFILEEXPRESSIONGATHERER_H
