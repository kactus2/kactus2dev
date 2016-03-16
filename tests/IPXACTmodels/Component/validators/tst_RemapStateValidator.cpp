//-----------------------------------------------------------------------------
// File: tst_RemapStateValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 27.11.2015
//
// Description:
// Unit test for class RemapStateValidator.
//-----------------------------------------------------------------------------

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <IPXACTmodels/Component/validators/RemapStateValidator.h>

#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/RemapPort.h>
#include <IPXACTmodels/Component/Port.h>

#include <QtTest>

class tst_RemapStateValidator : public QObject
{
    Q_OBJECT

public:
    tst_RemapStateValidator();

private slots:

    void testNameIsValid();
    void testNameIsValid_data();

    void testHasValidRemapPorts();
    void testHasValidRemapPorts_data();
    void testRemapPortHasValidValue();
    void testRemapPortHasValidValue_data();
    void testRemapPortHasValidPortIndex();
    void testRemapPortHasValidPortIndex_data();

private:

    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);
};

//-----------------------------------------------------------------------------
// Function: tst_RemapStateValidator::tst_RemapStateValidator()
//-----------------------------------------------------------------------------
tst_RemapStateValidator::tst_RemapStateValidator()
{
}

//-----------------------------------------------------------------------------
// Function: tst_RemapStateValidator::testNameIsValid()
//-----------------------------------------------------------------------------
void tst_RemapStateValidator::testNameIsValid()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<RemapState> testState (new RemapState(name));

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    RemapStateValidator validator(parser, QSharedPointer<QList<QSharedPointer<Port> > > ());
    QCOMPARE(validator.hasValidName(testState), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testState, "test");

        QString expectedError = QObject::tr("Invalid name specified for remap state %1 within %2").
            arg(testState->name(), "test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RemapStateValidator::testNameIsValid_data()
//-----------------------------------------------------------------------------
void tst_RemapStateValidator::testNameIsValid_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_RemapStateValidator::testHasValidRemapPorts()
//-----------------------------------------------------------------------------
void tst_RemapStateValidator::testHasValidRemapPorts()
{
    QFETCH(QString, portRef);
    QFETCH(bool, createPort);
    QFETCH(bool, isValid);

    QSharedPointer<RemapPort> testRemapPort (new RemapPort(portRef));
    testRemapPort->setValue("3");

    QSharedPointer<QList<QSharedPointer<Port> > > ports (new QList<QSharedPointer<Port> > ());
    if (createPort)
    {
        if (portRef.isEmpty())
        {
            portRef = "camploo";
        }
        QSharedPointer<Port> newPort (new Port(portRef));
        ports->append(newPort);
    }

    QSharedPointer<RemapState> testState (new RemapState("testState"));
    testState->getRemapPorts()->append(testRemapPort);
    
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    RemapStateValidator validator(parser, ports);
    QCOMPARE(validator.hasValidRemapPorts(testState), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testState, "test");

        QString expectedError = QObject::tr("Invalid port %1 set for remap state %2")
            .arg(testRemapPort->getPortNameRef()).arg(testState->name());
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RemapStateValidator::testHasValidRemapPorts_data()
//-----------------------------------------------------------------------------
void tst_RemapStateValidator::testHasValidRemapPorts_data()
{
    QTest::addColumn<QString>("portRef");
    QTest::addColumn<bool>("createPort");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Remap port with a port reference to existing port is valid") << "reference" << true << true;
    QTest::newRow("Remap port with a portRef to non-existing port is not valid") << "reference" << false << false;
    QTest::newRow("Remap port without a portRef is not valid") << "" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_RemapStateValidator::testRemapPortHasValidValue()
//-----------------------------------------------------------------------------
void tst_RemapStateValidator::testRemapPortHasValidValue()
{
    QFETCH(QString, remapPortValue);
    QFETCH(bool, isValid);

    QSharedPointer<QList<QSharedPointer<Port> > > ports (new QList<QSharedPointer<Port> > ());
    QSharedPointer<Port> newPort (new Port(QLatin1String("Champloo")));
    ports->append(newPort);

    QSharedPointer<RemapPort> testRemapPort (new RemapPort(newPort->name()));
    testRemapPort->setValue(remapPortValue);

    QSharedPointer<RemapState> testState (new RemapState("testState"));
    testState->getRemapPorts()->append(testRemapPort);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    RemapStateValidator validator(parser, ports);
    QCOMPARE(validator.hasValidRemapPorts(testState), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testState, "test");

        QString expectedError = QObject::tr("Invalid value set for remap port %1 within remap state %2")
            .arg(testRemapPort->getPortNameRef()).arg(testState->name());
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RemapStateValidator::testRemapPortHasValidValue_data()
//-----------------------------------------------------------------------------
void tst_RemapStateValidator::testRemapPortHasValidValue_data()
{
    QTest::addColumn<QString>("remapPortValue");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("0 is valid for remap port value") << "0" << true;
    QTest::newRow("12*2+13-5 is valid for remap port value") << "12*2+13-5" << true;
    QTest::newRow("-20 is not valid for remap port value") << "-20" << false;
    QTest::newRow("14-5*3 is not valid for remap port value") << "14-5*3" << false;
    QTest::newRow("Text is not valid for remap port value") << "text" << false;
    QTest::newRow("String is not valid for remap port value") << "\"text\"" << false;
    QTest::newRow("Empty value is not valid for remap port value") << "" << false;

    QTest::newRow("Long value is valid") << "40000000000" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_RemapStateValidator::testRemapPortHasValidPortIndex()
//-----------------------------------------------------------------------------
void tst_RemapStateValidator::testRemapPortHasValidPortIndex()
{
    QFETCH(QString, remapPortIndex);
    QFETCH(QString, portLeftBound);
    QFETCH(QString, portRightBound);
    QFETCH(bool, isValid);

    QSharedPointer<Port> newPort (new Port("testPort"));
    newPort->setLeftBound(portLeftBound);
    newPort->setRightBound(portRightBound);

    QSharedPointer<QList<QSharedPointer<Port> > > ports (new QList<QSharedPointer<Port> > ());
    ports->append(newPort);

    QSharedPointer<RemapPort> testRemapPort (new RemapPort(newPort->name()));
    testRemapPort->setValue("3");
    testRemapPort->setPortIndex(remapPortIndex);

    QSharedPointer<RemapState> testState (new RemapState("testRemapState"));
    testState->getRemapPorts()->append(testRemapPort);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    RemapStateValidator validator(parser, ports);
    QCOMPARE(validator.hasValidRemapPorts(testState), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator.findErrorsIn(foundErrors, testState, "test");

        QString expectedError = QObject::tr("Invalid port index set for remap port %1 within remap state %2")
            .arg(testRemapPort->getPortNameRef()).arg(testState->name());
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_RemapStateValidator::testRemapPortHasValidPortIndex_data()
//-----------------------------------------------------------------------------
void tst_RemapStateValidator::testRemapPortHasValidPortIndex_data()
{
    QTest::addColumn<QString>("remapPortIndex");
    QTest::addColumn<QString>("portLeftBound");
    QTest::addColumn<QString>("portRightBound");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("0 is valid for remap port index with referenced port size 2") << "0" << "0" << "1" << true;
    QTest::newRow("String is not valid for remap port index") << "\"text\"" << "0" << "1" << false;
    QTest::newRow("Test is not valid for remap port index") << "text" << "0" << "1" << false;
    QTest::newRow("Negative index is not valid") << "-113" << "0" << "1" << false;

    QTest::newRow("Index is not valid for remap port with non vectored port") << "12" << "1" << "1" << false;
    QTest::newRow("Index is not valid if index is greater than port size") << "10" << "0" << "8" << false;
    QTest::newRow("Empty index with referenced port size > 0 is valid") << "" << "0" << "10" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_RemapStateValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_RemapStateValidator::errorIsNotFoundInErrorList(QString const& expectedError,
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

QTEST_APPLESS_MAIN(tst_RemapStateValidator)

#include "tst_RemapStateValidator.moc"
