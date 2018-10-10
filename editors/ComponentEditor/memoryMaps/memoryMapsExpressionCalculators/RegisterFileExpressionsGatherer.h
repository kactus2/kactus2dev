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
    virtual ~RegisterFileExpressionsGatherer() = default;

    //! No copying
    RegisterFileExpressionsGatherer(const RegisterFileExpressionsGatherer& other) = delete;

    //! No assignment
    RegisterFileExpressionsGatherer& operator=(const RegisterFileExpressionsGatherer& other) = delete;

    /*!
     *  Get the expressions from a given register file.
     *
     *      @param [in] currentRegister     The given register file.
     *
     *      @return A list containing all the expressions used in a register file.
     */
    QStringList getExpressions(QSharedPointer<RegisterFile> currentRegisterFile) const;

};

#endif // REGISTERFILEEXPRESSIONGATHERER_H
