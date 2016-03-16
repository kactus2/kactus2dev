//-----------------------------------------------------------------------------
// File: tst_ComponentGeneratorValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.01.2016
//
// Description:
// Unit test for class ComponentGeneratorValidator.
//-----------------------------------------------------------------------------

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <IPXACTmodels/Component/ComponentGenerator.h>

#include <IPXACTmodels/Component/validators/ComponentGeneratorValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

#include <QtTest>

class tst_ComponentGeneratorValidator : public QObject
{
    Q_OBJECT

public:
    tst_ComponentGeneratorValidator();

private slots:
    
    void testHasValidName();
    void testHasValidName_data();

    void testHasValidPhase();
    void testHasValidPhase_data();

    void testHasValidGeneratorExe();
    void testHasValidGeneratorExe_data();

private:

    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);

    QSharedPointer<ComponentGeneratorValidator> createComponentGeneratorValidator();
};

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorValidator::tst_ComponentGeneratorValidator()
//-----------------------------------------------------------------------------
tst_ComponentGeneratorValidator::tst_ComponentGeneratorValidator()
{
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorValidator::testHasValidName()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorValidator::testHasValidName()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<ComponentGenerator> testGenerator (new ComponentGenerator());
    testGenerator->setName(name);

    QSharedPointer<ComponentGeneratorValidator> validator = createComponentGeneratorValidator();

    QCOMPARE(validator->hasValidName(testGenerator), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testGenerator, "test");

        QString expectedError = QObject::tr("Invalid name specified for component generator %1 within %2").
            arg(testGenerator->name(), "test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorValidator::testHasValidName_data()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorValidator::testHasValidName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorValidator::testHasValidPhase()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorValidator::testHasValidPhase()
{
    QFETCH(QString, phaseValue);
    QFETCH(bool, isValid);

    QSharedPointer<ComponentGenerator> testGenerator (new ComponentGenerator());
    testGenerator->setName("Phasor");
    testGenerator->setPhase(phaseValue);

    QSharedPointer<ComponentGeneratorValidator> validator = createComponentGeneratorValidator();

    QCOMPARE(validator->hasValidPhase(testGenerator), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testGenerator, "test");

        QString expectedError = QObject::tr("Invalid phase value set for component generator '%1' within %2")
            .arg(testGenerator->name()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorValidator::testHasValidPhase_data()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorValidator::testHasValidPhase_data()
{
    QTest::addColumn<QString>("phaseValue");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Phase value 1+1.12 is valid") << "1+1.12" << true;
    QTest::newRow("Phase value 0 is valid") << "0" << true;
    QTest::newRow("Phase value -4.12 is valid") << "-4.12" << true;
    QTest::newRow("Empty phase value is valid") << "" << true;
    QTest::newRow("Text is not valid for phase value") << "text" << false;
    QTest::newRow("String is not valid for phase value") << "\"text\"" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorValidator::testHasValidGeneratorExe()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorValidator::testHasValidGeneratorExe()
{
    QFETCH(QString, generatorExe);
    QFETCH(bool, isValid);

    QSharedPointer<ComponentGenerator> testGenerator (new ComponentGenerator());
    testGenerator->setName("Phazon");
    testGenerator->setGeneratorExe(generatorExe);

    QSharedPointer<ComponentGeneratorValidator> validator = createComponentGeneratorValidator();

    QCOMPARE(validator->hasValidGeneratorExe(testGenerator), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testGenerator, "test");

        QString expectedError = QObject::tr("Invalid generator exe set for component generator '%1' within %2")
            .arg(testGenerator->name()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorValidator::testHasValidGeneratorExe_data()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorValidator::testHasValidGeneratorExe_data()
{
    QTest::addColumn<QString>("generatorExe");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Generator exe ../bin/myGenerator.pl is valid") << "../bin/myGenerator.pl" << true;
    QTest::newRow("Empty generator exe is not valid") << "" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_ComponentGeneratorValidator::errorIsNotFoundInErrorList(QString const& expectedError,
    QVector<QString> errorList)
{
    if (!errorList.contains(expectedError))
    {
        qDebug() << "The following error:" << endl << expectedError << endl << "was not found in error list:";
        foreach(QString error, errorList)
        {
            qDebug() << error;
        }
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorValidator::createGeneratorValidator()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentGeneratorValidator> tst_ComponentGeneratorValidator::createComponentGeneratorValidator()
{
    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());

    QSharedPointer<ParameterValidator2014> parameterValidator(new ParameterValidator2014(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > ()));

    QSharedPointer<ComponentGeneratorValidator> generatorValidator (
        new ComponentGeneratorValidator(parser, parameterValidator));

    return generatorValidator;
}

QTEST_APPLESS_MAIN(tst_ComponentGeneratorValidator)

#include "tst_ComponentGeneratorValidator.moc"
