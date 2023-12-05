//-----------------------------------------------------------------------------
// File: tst_ParameterValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.11.2014
//
// Description:
// Unit test for class ParameterValidator.
//-----------------------------------------------------------------------------

#include <QtTest>
#include <QSharedPointer>

#include <KactusAPI/include/SystemVerilogExpressionParser.h>

#include <IPXACTmodels/common/Choice.h>
#include <IPXACTmodels/common/Enumeration.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

class tst_ParameterValidator : public QObject
{
    Q_OBJECT

public:
    tst_ParameterValidator();

private slots:

    void testResolve();
    void testResolve_data();

    void testIdIsSpecifiedForResolveUserAndGenerated();
    void testIdIsSpecifiedForResolveUserAndGenerated_data();

    void testValue();
    void testValue_data();

    void testChoiceReference();
    void testChoiceReference_data();

    void testValueUsingChoice();
    void testValueUsingChoice_data();

    void testValidityWithMinimumValue();
    void testValidityWithMinimumValue_data();

    void testValidityWithMaximumValue();
    void testValidityWithMaximumValue_data();

private:
        
    QSharedPointer<Parameter> createParameterWithName();
    ParameterValidator* createValidator(QSharedPointer<QList<QSharedPointer<Choice> > > choices =
        QSharedPointer<QList<QSharedPointer<Choice> > >(new QList<QSharedPointer<Choice> >()));

    QStringList findErrors(ParameterValidator* validator, QSharedPointer<Parameter> parameter);

    bool errorIsNotFoundInErrorlist(QString const& expectedError, QStringList const& errorlist) const;


};

//-----------------------------------------------------------------------------
// Function: tst_Parameter::tst_Parameter()
//-----------------------------------------------------------------------------
tst_ParameterValidator::tst_ParameterValidator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testResolve()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testResolve()
{
    QFETCH(QString, resolve);
    QFETCH(bool, expectedValid);

    QSharedPointer<Parameter> parameter = createParameterWithName();
    parameter->setValueResolve(resolve);

    QScopedPointer<ParameterValidator> validator(createValidator());
    QVERIFY(validator->hasValidResolve(parameter) == expectedValid);

    if (!expectedValid)
    {
        QStringList errorlist =  findErrors(validator.data(), parameter);

        QString expectedError = "Invalid resolve " + resolve + " specified for parameter param within test";
        if (errorIsNotFoundInErrorlist(expectedError, errorlist))
        {
            QFAIL("No error message found.");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testResolve_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testResolve_data()
{
    QTest::addColumn<QString>("resolve");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("empty resolve is valid") << "" << true;
    QTest::newRow("immediate resolve is valid") << "immediate" << true;
    QTest::newRow("user resolve is valid") << "user" << true;
    QTest::newRow("generated resolve is valid") << "generated" << true;

    QTest::newRow("other resolve is invalid") << "other" << false;
    QTest::newRow("number resolve is invalid") << "1" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testIdIsSpecifiedForResolveUserAndGenerated()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testIdIsSpecifiedForResolveUserAndGenerated()
{
    QFETCH(QString, id);
    QFETCH(QString, resolve);
    QFETCH(bool, expectedValid);

    QSharedPointer<Parameter> parameter = createParameterWithName();
    parameter->setValueId(id);
    parameter->setValueResolve(resolve);

    QScopedPointer<ParameterValidator> validator(createValidator());
    QVERIFY(validator->hasValidValueId(parameter) == expectedValid);

   if (!expectedValid)
    {
        QStringList errorlist =  findErrors(validator.data(), parameter);

        QString expectedError = "No identifier specified for parameter param with resolve " + resolve + " within test";
        if (errorIsNotFoundInErrorlist(expectedError, errorlist))
        {
            QFAIL("No error message found.");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testIdIsSpecifiedForResolveUserAndGenerated()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testIdIsSpecifiedForResolveUserAndGenerated_data()
{
    QTest::addColumn<QString>("id");
    QTest::addColumn<QString>("resolve");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("No id for unspecified resolve is valid") << "" << "" << true;
    QTest::newRow("Id for unspecified resolve is valid") << "1" << "" << true;

    QTest::newRow("No id for resolve user is invalid") << "" << "user" << false;
    QTest::newRow("Id for resolve user is valid") << "1" << "user" << true;

    QTest::newRow("No id for resolve generated is invalid") << "" << "generated" << false;
    QTest::newRow("Id for resolve generated is valid") << "1" << "generated" << true;

    QTest::newRow("No id for resolve immediate is valid") << "" << "immediate" << true;
    QTest::newRow("Id for resolve immediate is valid") << "1" << "immediate" << true;

    QTest::newRow("No id for resolve dependent is valid") << "" << "dependent" << true;
    QTest::newRow("Id for resolve dependent is valid") << "1" << "dependent" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testValue()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValue()
{
    QFETCH(QString, value);
    QFETCH(bool, expectedValid);

    QSharedPointer<Parameter> parameter = createParameterWithName();
    parameter->setValue(value);
    parameter->setType("bit");

    QScopedPointer<ParameterValidator> validator(createValidator());
    QVERIFY(validator->hasValidValue(parameter) == expectedValid);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testValue_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValue_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("Empty value is invalid") << "" << false;
    QTest::newRow("Integer is valid") << "1" << true;
    QTest::newRow("Array is valid") << "{1,1}" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testChoiceReference()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testChoiceReference()
{
    QFETCH(QStringList, choiceNames);
    QFETCH(QString, choiceRef);
    QFETCH(bool, expectedValid);

    QSharedPointer<Parameter> parameter = createParameterWithName();
    parameter->setChoiceRef(choiceRef);

    QSharedPointer<QList<QSharedPointer<Choice> > > choices(new QList<QSharedPointer<Choice> >());

    foreach(QString choiceName, choiceNames)
    {
        QSharedPointer<Choice> choice(new Choice());
        choice->setName(choiceName);
        choices->append(choice);
    }

    QScopedPointer<ParameterValidator> validator(createValidator(choices));
    QVERIFY(validator->hasValidChoice(parameter) == expectedValid);

    if (!expectedValid)
    {
        QStringList errorlist =  findErrors(validator.data(), parameter);

        QString expectedError = "Choice choice1 referenced in parameter param is not specified within test";

        if (errorIsNotFoundInErrorlist(expectedError, errorlist))
        {
            QFAIL("No error message found.");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testChoiceReference_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testChoiceReference_data()
{
    QTest::addColumn<QStringList>("choiceNames");
    QTest::addColumn<QString>("choiceRef");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("Empty choice is valid") << QStringList() << "" << true;
    QTest::newRow("Empty choice is valid if choices available") << QStringList("choice1") << "" << true;

    QTest::newRow("No choices available but reference set is not valid") << QStringList() << "choice1" << false;
    QTest::newRow("Reference to unavailable choice is not valid") << QStringList("choice0") << "choice1" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testValueUsingChoice()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValueUsingChoice()
{
    QFETCH(QString, choiceRef);
    QFETCH(QStringList, enumerations);
    QFETCH(QString, value);
    QFETCH(bool, expectedValid);

    QSharedPointer<Parameter> parameter = createParameterWithName();
    parameter->setChoiceRef(choiceRef);
    parameter->setValue(value);

    QSharedPointer<QList<QSharedPointer<Choice> > > choices(new QList<QSharedPointer<Choice> > ());

    QSharedPointer<Choice> referencedChoice(new Choice());
    referencedChoice->setName(choiceRef);
    foreach(QString enumerationValue, enumerations)
    {
        QSharedPointer<Enumeration> enumeration(new Enumeration());
        enumeration->setValue(enumerationValue);
        referencedChoice->enumerations()->append(enumeration);
    }
    choices->append(referencedChoice);

    QScopedPointer<ParameterValidator> validator(createValidator(choices));
    QVERIFY(validator->hasValidValue(parameter) == expectedValid);

    if (!expectedValid && !value.isEmpty())
    {
        QStringList errorlist =  findErrors(validator.data(), parameter);

        QString expectedError = "Value '" + value + "' references unknown enumeration "
            "for choice " + choiceRef + " in parameter param within test";

        if (errorIsNotFoundInErrorlist(expectedError, errorlist))
        {
            QFAIL("No error message found.");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValueUsingChoice_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValueUsingChoice_data()
{
    QTest::addColumn<QString>("choiceRef");
    QTest::addColumn<QStringList>("enumerations");
    QTest::addColumn<QString>("value");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("Empty choice and numeric value is valid") << "" << QStringList() << "1" << true;
    QTest::newRow("Choice and empty value is not valid") << "choice1" << QStringList() << "" << false;

    QTest::newRow("Choice with no enumerations is not valid") << "choice1" << QStringList() << "1" << false;
    QTest::newRow("Choice and value not in enumerations is not valid") 
        << "choice1" << QStringList("0") << "1" << false;

    QTest::newRow("Choice and value in enumeration is valid") << "choice1" << QStringList("1") << "1" << true;
    QTest::newRow("Choice and value in enumerations is valid") 
        << "choice1" << QString("0,1,2").split(',') << "1" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testValidityWithMinimumValue()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValidityWithMinimumValue()
{
    QFETCH(QString, value);
    QFETCH(QString, minimum);
    QFETCH(bool, expectedValid);

    QSharedPointer<Parameter> parameter = createParameterWithName();    
    parameter->setValue(value);
    parameter->setType(QStringLiteral("int"));
    parameter->setMinimumValue(minimum);

    QScopedPointer<ParameterValidator> validator(createValidator());
    QVERIFY(validator->hasValidValue(parameter) == expectedValid);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testValidityWithMinimumValue_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValidityWithMinimumValue_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<QString>("minimum");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("Empty minimum is valid") << "1" << "" << true;
    QTest::newRow("Value equal to minimum is valid") << "1" << "1" << true; 
    QTest::newRow("Value greater than minimum is valid") << "2" << "1" << true;
    
    QTest::newRow("Value less than minimum is not valid") << "1" << "2" << false;
    QTest::newRow("Value less than negative minimum is not valid") << "-2" << "-1" << false;

    QTest::newRow("Empty minimum is valid for array") << "{1,1}" << "" << true;
    QTest::newRow("Value greater than minimum is valid for array") << "{2,3}" << "1" << true;

    QTest::newRow("Every value in array must be greater than minimum") << "{0,1,2}" << "1" << false;
    QTest::newRow("Empty array is not valid") << "{}" << "1" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testValidityWithMaximumValue()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValidityWithMaximumValue()
{
    QFETCH(QString, value);
    QFETCH(QString, maximum);
    QFETCH(bool, expectedValid);

    QSharedPointer<Parameter> parameter = createParameterWithName();
    parameter->setValue(value);
    parameter->setType(QStringLiteral("int"));
    parameter->setMaximumValue(maximum);

    QScopedPointer<ParameterValidator> validator(createValidator());
    QVERIFY(validator->hasValidValue(parameter) == expectedValid);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testValidityWithMaximumValue_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValidityWithMaximumValue_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<QString>("maximum");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("Empty maximum is valid") << "1" << "" << true;
    QTest::newRow("Value equal to maximum is valid") << "1" << "1" << true;
    QTest::newRow("Value less than maximum is valid") << "1" << "2" << true;

    QTest::newRow("Value greater than maximum is not valid") << "2" << "1" << false;

    QTest::newRow("Empty maximum is valid for array") << "{1,1}" << "" << true;
    QTest::newRow("Value less than maximum is valid for array") << "{1,2}" << "3" << true;

    QTest::newRow("Every value in array must be less than maximum") << "{0,1,2}" << "1" << false;
    QTest::newRow("Empty array is not valid") << "{}" << "1" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::createParameterWithName()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> tst_ParameterValidator::createParameterWithName()
{
    QSharedPointer<Parameter> parameter(new Parameter());
    parameter->setName("param");

    return parameter;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::createValidator()
//-----------------------------------------------------------------------------
ParameterValidator* tst_ParameterValidator::createValidator(QSharedPointer<QList<QSharedPointer<Choice> > > choices)
{
    QSharedPointer<SystemVerilogExpressionParser> basicParser(new SystemVerilogExpressionParser());
   
    return new ParameterValidator(basicParser, choices, Document::Revision::Std14);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::verifyErrorIsFoundInErrorlist()
//-----------------------------------------------------------------------------
bool tst_ParameterValidator::errorIsNotFoundInErrorlist(QString const& expectedError, QStringList const& errorlist) const
{
    if (!errorlist.contains(expectedError))
    {
        qDebug() << "The following error:" << Qt::endl << expectedError << Qt::endl << "was not found in errorlist:";
        for (QString const& error : errorlist)
        {
            qDebug() << error; 
        }
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::findErrors()
//-----------------------------------------------------------------------------
QStringList tst_ParameterValidator::findErrors(ParameterValidator* validator, QSharedPointer<Parameter> parameter)
{
    QVector<QString> errors; 
    validator->findErrorsIn(errors, parameter, "test");
    return errors.toList();
}

QTEST_APPLESS_MAIN(tst_ParameterValidator)

#include "tst_ParameterValidator.moc"
