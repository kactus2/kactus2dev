//-----------------------------------------------------------------------------
// File: RegisterFileValidator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Dan Chianucci
// Date: 04.06.2018
//
// Description:
// Validator for ipxact:RegisterFile.
//-----------------------------------------------------------------------------


#ifndef REGISTERFILEVALIDATOR_H
#define REGISTERFILEVALIDATOR_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/Component/validators/RegisterBaseValidator.h>
#include <QSharedPointer>
#include <QString>


class ExpressionParser;
class RegisterBase;
class RegisterFile;
class RegisterValidator;
class ParameterValidator;
//-----------------------------------------------------------------------------
//! Validator for ipxact:Register.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT RegisterFileValidator : public RegisterBaseValidator
{
public:


    RegisterFileValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<RegisterValidator> registerValidator,
        QSharedPointer<ParameterValidator> parameterValidator);

    virtual ~RegisterFileValidator();

    QSharedPointer<RegisterValidator> getRegisterValidator() const;

    bool validate(QSharedPointer<RegisterFile> selectedRegisterFile) const;

    bool hasValidRange(QSharedPointer<RegisterFile> selectedRegisterFile) const;

    bool hasValidRegisterData(QSharedPointer<RegisterFile> selectedRegisterFile) const;

    void findErrorsIn(QVector<QString>& errors,
                      QSharedPointer<RegisterFile> selectedRegisterFile,
                      QString const& context) const;


private:

    // Disable copying.
    RegisterFileValidator(RegisterFileValidator const& rhs);
    RegisterFileValidator& operator=(RegisterFileValidator const& rhs);


    void findErrorsInRange(QVector<QString>& errors,
                           QSharedPointer<RegisterFile> selectedRegisterFile,
                           QString const& context) const;

    void findErrorsInRegisterData(QVector<QString>& errors,
                                  QSharedPointer<RegisterFile> selectedRegisterFile,
                                  QString const& context) const;
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    //! The validator used for fields.
    QSharedPointer<RegisterValidator> registerValidator_;
};
#endif // REGISTERFILEVALIDATOR_H
