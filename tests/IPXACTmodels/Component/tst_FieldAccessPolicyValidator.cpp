//-----------------------------------------------------------------------------
// File: tst_FieldAccessPolicyvalidator_.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 2.8.2023
//
// Description:
// Unit test for class FieldAccessPolicyValidator>.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/validators/FieldAccessPolicyValidator.h>
#include <IPXACTmodels/Component/FieldAccessPolicy.h>

#include <KactusAPI/include/SystemVerilogExpressionParser.h>

#include <QtTest>
#include <QDomDocument>

class tst_FieldAccessPolicyValidator : public QObject
{
    Q_OBJECT

public:
    tst_FieldAccessPolicyValidator();

private slots:
    
    void init();
    void cleanup();
    void testDefinitionRefIsValid();

    void testAccessIsValid();
    void testAccessIsValid_data();

    void testWriteValueConstraintIsValid();
    void testWriteValueConstraintIsValid_data();

    void testReadResponseIsValid();
    void testReadResponseIsValid_data();
    
    void testBroadcastsAreValid();
    void testAccessRestrictionsAreValid();

    void testReserved();
    void testReserved_data();


private:
    QSharedPointer<FieldAccessPolicy> fieldAccessPolicy_;

    QSharedPointer<ExpressionParser> expressionParser_ = QSharedPointer<ExpressionParser>(new SystemVerilogExpressionParser());
    
    FieldAccessPolicyValidator validator_;

    bool errorIsNotFoundInErrorList(QString const& expectedError, QStringList errorList);
};

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyValidator::tst_FieldAccessPolicyValidator()
//-----------------------------------------------------------------------------
tst_FieldAccessPolicyValidator::tst_FieldAccessPolicyValidator() :
    validator_(expressionParser_)
{

}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyValidator::init()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyValidator::init()
{
    fieldAccessPolicy_ = QSharedPointer<FieldAccessPolicy>(new FieldAccessPolicy());
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyValidator::cleanup()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyValidator::cleanup()
{
    fieldAccessPolicy_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyValidator::testDefinitionRefIsValid()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyValidator::testDefinitionRefIsValid()
{
    fieldAccessPolicy_->setFieldAccessPolicyDefinitionRef("fieldAccessPolicy_AccessPolicyDefinition");

    QStringList errorList;

    validator_.findErrorsIn(errorList, fieldAccessPolicy_, "test");

    QCOMPARE(errorList.size(), 1); // No typedefinitions.
    QVERIFY(validator_.validate(fieldAccessPolicy_) == false);

    errorList.clear();
    fieldAccessPolicy_->setFieldAccessPolicyTypeDefinitionRef("testTypeDefinitions");

    validator_.findErrorsIn(errorList, fieldAccessPolicy_, "test");
    QCOMPARE(errorList.size(), 0);
    QVERIFY(validator_.validate(fieldAccessPolicy_));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyValidator::testAccessIsValid()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyValidator::testAccessIsValid()
{
    QFETCH(QString, accessValue);
    QFETCH(QString, modifiedWriteValue);
    QFETCH(QString, readAction);
    QFETCH(bool, isValid);

    fieldAccessPolicy_->setAccess(AccessTypes::str2Access(accessValue, AccessTypes::ACCESS_COUNT));
    fieldAccessPolicy_->setModifiedWrite(General::str2ModifiedWrite(modifiedWriteValue));
    fieldAccessPolicy_->setReadAction(General::str2ReadAction(readAction));

    QStringList errorList;

    QCOMPARE(validator_.hasValidAccess(fieldAccessPolicy_), isValid);

    if (!isValid)
    {
        QVector<QString> errorList;
        validator_.findErrorsIn(errorList, fieldAccessPolicy_, "test");

        if (fieldAccessPolicy_->getAccess() == AccessTypes::READ_ONLY &&
            fieldAccessPolicy_->getModifiedWrite() != General::MODIFIED_WRITE_COUNT)
        {
            QString expectedError = QObject::tr("In field access policy within %1, access type readOnly does not "
                "allow the field access policy to include a modified write value.").arg("test");
            if (errorIsNotFoundInErrorList(expectedError, errorList))
            {
                QFAIL("No error message found");
            }
        }
        else if ((fieldAccessPolicy_->getAccess() == AccessTypes::WRITE_ONLY ||
            fieldAccessPolicy_->getAccess() == AccessTypes::WRITEONCE) &&
            fieldAccessPolicy_->getReadAction() != General::READ_ACTION_COUNT)
        {
            QString expectedError = QObject::tr("In field access policy within %1, access type write only and write once "
                "do not allow the field access policy to include a read action value.").arg("test");
            if (errorIsNotFoundInErrorList(expectedError, errorList))
            {
                QFAIL("No error message found");
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyValidator::testAccessIsValid_data()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyValidator::testAccessIsValid_data()
{
    QTest::addColumn<QString>("accessValue");
    QTest::addColumn<QString>("modifiedWriteValue");
    QTest::addColumn<QString>("readAction");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Empty access, modified write value and read action are valid") << "" << "" << "" << true;
    QTest::newRow("Access read-write with a modified write value is valid") << "read-write" << "oneToClear" <<
        "set" << true;
    QTest::newRow("Access read-only with a modified write value is invalid") << "read-only" << "oneToClear" <<
        "set" << false;
    QTest::newRow("Access read-only without modified write value is valid") << "read-only" << "" << "set" << true;
    QTest::newRow("Access write-only with a read action is invalid") << "write-only" << "" << "set" << false;
    QTest::newRow("Access writeOnce with a read action is invalid") << "writeOnce" << "" << "clear" << false;
    QTest::newRow("Access write-only without a read action is valid") << "write-only" << "oneToClear" << "" << true;
    QTest::newRow("Access writeOnce without a read action is valid") << "writeOnce" << "oneToClear" << "" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyValidator::testWriteValueConstraintIsValid()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyValidator::testWriteValueConstraintIsValid()
{
    QFETCH(QString, minimum);
    QFETCH(QString, maximum);
    QFETCH(QString, constraintType);
    QFETCH(bool, isValid);

    QSharedPointer<WriteValueConstraint> testConstraint(new WriteValueConstraint());
    if (constraintType == QLatin1String("writeAsRead"))
    {
        testConstraint->setType(WriteValueConstraint::WRITE_AS_READ);
    }
    else if (constraintType == QLatin1String("useEnumeratedValues"))
    {
        testConstraint->setType(WriteValueConstraint::USE_ENUM);
    }
    else if (constraintType == QLatin1String("minMax"))
    {
        testConstraint->setMinimum(minimum);
        testConstraint->setMaximum(maximum);
    }

    fieldAccessPolicy_->setWriteValueConstraint(testConstraint);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    FieldAccessPolicyValidator validator(parser);
    QCOMPARE(validator_.hasValidWriteValueConstraint(fieldAccessPolicy_), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator_.findErrorsIn(foundErrors, fieldAccessPolicy_, "test");

        if (testConstraint->getType() == WriteValueConstraint::TYPE_COUNT)
        {
            QString expectedError = QObject::tr("Invalid write value constraint set for field access policy within %1")
                .arg("test");
            if (errorIsNotFoundInErrorList(expectedError, foundErrors))
            {
                QFAIL("No error message found");
            }
        }
        else if (testConstraint->getType() == WriteValueConstraint::MIN_MAX)
        {
            int minimumValue = parser->parseExpression(testConstraint->getMinimum()).toInt();
            int maximumValue = parser->parseExpression(testConstraint->getMaximum()).toInt();

            QString expectedError;
            if (minimumValue > maximumValue)
            {
                expectedError = (QObject::tr("Maximum value must be greater than or equal to the minimum value in "
                    "write value constraint set for field access policy within %1").arg("test"));
                if (errorIsNotFoundInErrorList(expectedError, foundErrors))
                {
                    QFAIL("No error message found");
                }
            }

            QRegularExpression bitExpression("^([0-9]+|[1-9]+[0-9]*'([bB][01_]+|[hH][0-9a-fA-F_]+))$");
            if (!bitExpression.match(testConstraint->getMinimum()).hasMatch() ||
                !bitExpression.match(parser->parseExpression(testConstraint->getMinimum())).hasMatch())
            {
                expectedError = QObject::tr(
                    "Invalid minimum value set for write value constraint in field access policy within %1")
                    .arg("test");
                if (errorIsNotFoundInErrorList(expectedError, foundErrors))
                {
                    QFAIL("No error message found");
                }
            }

            if (!bitExpression.match(testConstraint->getMaximum()).hasMatch() ||
                !bitExpression.match(parser->parseExpression(testConstraint->getMaximum())).hasMatch())
            {
                expectedError = QObject::tr(
                    "Invalid maximum value set for write value constraint in field access policy within %1")
                    .arg("test");
                if (errorIsNotFoundInErrorList(expectedError, foundErrors))
                {
                    QFAIL("No error message found");
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyValidator::testWriteValueConstraintIsValid_data()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyValidator::testWriteValueConstraintIsValid_data()
{
    QTest::addColumn<QString>("minimum");
    QTest::addColumn<QString>("maximum");
    QTest::addColumn<QString>("constraintType");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("No type for write value constraint is invalid") << "" << "" << "" << false;
    QTest::newRow("Write value constraint writeAsRead is valid") << "" << "" << "writeAsRead" << true;
    QTest::newRow("Write value constraint useEnumerated values is valid") << "" << "" << "useEnumeratedValues" << true;
    QTest::newRow("Write value constraint minMax is invalid for empty min and max") << "" << "" << "minMax" << false;
    QTest::newRow("8'b1001_0110 is valid for min and max") << "8'b1001_0110" << "8'b1001_0110" << "minMax" << true;
    QTest::newRow("Text is invalid for min and max") << "text" << "text" << "minMax" << false;
    QTest::newRow("String is invalid for min and max") << "\"test\"" << "\"test\"" << "minMax" << false;
    QTest::newRow("Values 14 and 2'h2 are invalid for min and max") << "14" << "2'h2" << "minMax" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyValidator::testReadResponseIsValid()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyValidator::testReadResponseIsValid()
{
    QFETCH(QString, readResponse);
    QFETCH(bool, isValid);

    fieldAccessPolicy_->setReadResponse(readResponse);

    QCOMPARE(validator_.hasValidReadResponse(fieldAccessPolicy_), isValid);
    
    if (!isValid)
    {
        QStringList errors;

        validator_.findErrorsIn(errors, fieldAccessPolicy_, "test");

        QString expectedError = QObject::tr("Invalid read response value for field access policy within test");

        if (errorIsNotFoundInErrorList(expectedError, errors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyValidator::testReadResponseIsValid_data()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyValidator::testReadResponseIsValid_data()
{
    QTest::addColumn<QString>("readResponse");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("8'b1001_0110 is valid") << "8'b1001_0110" << true;
    QTest::newRow("2'b10 is valid") << "2'b10" << true;
    QTest::newRow("2'h2 is valid") << "2'h2" << true;
    QTest::newRow("text is invalid") << "text" << false;
    QTest::newRow("string is invalid") << "\"text\"" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyValidator::testBroadcastsAreValid()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyValidator::testBroadcastsAreValid()
{
    QStringList errors;
    QSharedPointer<FieldReference> fieldReference(new FieldReference());
    QSharedPointer<FieldReference::IndexedReference> addressSpaceRef(new FieldReference::IndexedReference("testAddressSpace", QStringList()));
    QSharedPointer<FieldReference::IndexedReference> memoryMapRef(new FieldReference::IndexedReference("testMemoryMap", QStringList()));
    QSharedPointer<FieldReference::IndexedReference> fieldRef(new FieldReference::IndexedReference("testFieldRef", QStringList()));

    fieldReference->setReference(addressSpaceRef, FieldReference::ADDRESS_SPACE);
    fieldReference->setReference(memoryMapRef, FieldReference::MEMORY_MAP);

    fieldAccessPolicy_->getBroadcasts()->append(fieldReference);

    QString expectedErrorChoice("Field reference in test cannot contain both an address space reference"
        " and a memory map or memory remap reference."
    );

    QString expectedErrorField("Field reference in test must contain a reference to a field.");

    validator_.findErrorsIn(errors, fieldAccessPolicy_, "test");
    QCOMPARE(errors.size(), 2);
    QVERIFY(errorIsNotFoundInErrorList(expectedErrorChoice, errors) == false);
    QVERIFY(errorIsNotFoundInErrorList(expectedErrorField, errors) == false);
    QVERIFY(validator_.hasValidBroadcasts(fieldAccessPolicy_) == false);
    
    errors.clear();
    fieldReference->setReference(fieldRef, FieldReference::FIELD);
    fieldReference->clearReference(FieldReference::ADDRESS_SPACE);

    validator_.findErrorsIn(errors, fieldAccessPolicy_, "test");
    QCOMPARE(errors.size(), 0);
    QVERIFY(validator_.hasValidBroadcasts(fieldAccessPolicy_));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyValidator::testAccessRestrictionsAreValid()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyValidator::testAccessRestrictionsAreValid()
{
    QSharedPointer<FieldAccessPolicy::AccessRestriction> accessRestriction(new FieldAccessPolicy::AccessRestriction());
    QSharedPointer<FieldAccessPolicy::AccessRestriction> accessRestriction2(new FieldAccessPolicy::AccessRestriction());
    
    QSharedPointer<ModeReference> modeRef(new ModeReference());
    QSharedPointer<ModeReference> modeRef2(new ModeReference());
    
    accessRestriction->modeRefs_->append(modeRef);
    accessRestriction2->modeRefs_->append(modeRef2);

    fieldAccessPolicy_->getAccessRestrictions()->append(accessRestriction);
    fieldAccessPolicy_->getAccessRestrictions()->append(accessRestriction2);

    // No modes -> error
    QStringList errors;
    validator_.findErrorsIn(errors, fieldAccessPolicy_, "test");
    QCOMPARE(errors.size(), 1);
    
    errors.clear();

    // Duplicate modes in any of the access restrictions -> error
    modeRef->setReference("testMode");
    modeRef->setPriority("0");
    
    modeRef2->setReference("testMode");
    modeRef2->setPriority("1");

    validator_.findErrorsIn(errors, fieldAccessPolicy_, "test");
    QCOMPARE(errors.size(), 1);
    
    errors.clear();
    modeRef2->setReference("testMode2");

    // Invalid bit expressions in read/write access masks -> error
    accessRestriction->readAccessMask_ = QString("'b1100");
    accessRestriction2->readAccessMask_ = QString("etsdfasd"); // Not valid
    
    accessRestriction->writeAccessMask_ = QString("asdasd"); // Not valid
    accessRestriction2->writeAccessMask_ = QString("2'b11");

    validator_.findErrorsIn(errors, fieldAccessPolicy_, "test");
    QCOMPARE(errors.size(), 2);

    errors.clear();
    accessRestriction2->readAccessMask_ = QString("'h2");
    accessRestriction->writeAccessMask_ = QString("'b1111");

    validator_.findErrorsIn(errors, fieldAccessPolicy_, "test");
    QCOMPARE(errors.size(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyValidator::testReserved()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyValidator::testReserved()
{
    QFETCH(QString, reserved);
    QFETCH(bool, isValid);

    fieldAccessPolicy_->setReserved(reserved);

    QCOMPARE(validator_.hasValidReserved(fieldAccessPolicy_), isValid);

    if (!isValid)
    {
        QStringList foundErrors;
        validator_.findErrorsIn(foundErrors, fieldAccessPolicy_, "test");

        QString expectedError = QObject::tr("Invalid reserved set for field access policy within %1").arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyValidator::testReserved_data()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyValidator::testReserved_data()
{
    QTest::addColumn<QString>("reserved");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Empty reserved value is valid") << "" << true;
    QTest::newRow("White space is invalid reserved value") << "   " << false;
    QTest::newRow("10 is invalid reserved value") << "10" << false;
    QTest::newRow("4+5*3-20/2-8 is valid reserved value") << "4+5*3-20/2-8" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_FieldAccessPolicyValidator::errorIsNotFoundInErrorList(QString const& expectedError, QStringList errorList)
{
    if (!errorList.contains(expectedError))
    {
        qDebug() << "The following error:" << Qt::endl << expectedError << Qt::endl << "was not found in errorlist:";
        for (QString const& error : errorList)
        {
            qDebug() << error;
        }
        return true;
    }

    return false;
}


QTEST_APPLESS_MAIN(tst_FieldAccessPolicyValidator)

#include "tst_FieldAccessPolicyValidator.moc"
