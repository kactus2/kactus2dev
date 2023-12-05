//-----------------------------------------------------------------------------
// File: tst_ModeValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 14.08.2023
//
// Description:
// Unit test for class ModeValidator.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Mode.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/validators/ModeValidator.h>
#include <IPXACTmodels/Component/validators/PortSliceValidator.h>
#include <IPXACTmodels/Component/validators/FieldSliceValidator.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <KactusAPI/include/SystemVerilogExpressionParser.h>
#include <KactusAPI/include/ModeConditionParser.h>

#include <QDebug>
#include <QtTest>

class tst_ModeValidator : public QObject
{
    Q_OBJECT

public:
    tst_ModeValidator();

private slots:

    void testNameIsNotEmpty();
	
    void testPortSliceName();
    void testPortSliceName_data();

    void testPortSliceNameIsUnique();

    void testPortRefs();
    void testPortRefs_data();

    void testFieldSliceName();
    void testFieldSliceName_data();

    void testFieldSliceNameIsUnique();

    void testFieldRefs();
    void testFieldRefs_data();

    void testFieldRefRange();
    void testFieldRefRange_data();

    void testCondition();
    void testCondition_data();
};

//-----------------------------------------------------------------------------
// Function: tst_ModeValidator::tst_ModeValidator()
//-----------------------------------------------------------------------------
tst_ModeValidator::tst_ModeValidator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ModeValidator::testNameIsNotEmpty()
//-----------------------------------------------------------------------------
void tst_ModeValidator::testNameIsNotEmpty()
{
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<Component> testComponent(new Component(VLNV(), Document::Revision::Std22));

    ModeValidator validator(testComponent, parser);

    QSharedPointer<Mode> mode( new Mode() );
    mode->setName(QString());

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, mode, "test");

    QVERIFY(!validator.validate(mode));
    QVERIFY(errorList.size() > 0);
    QCOMPARE(errorList.first(), QString("Invalid name '' set for mode within test."));
}

//-----------------------------------------------------------------------------
// Function: tst_ModeValidator::testPortSliceName()
//-----------------------------------------------------------------------------
void tst_ModeValidator::testPortSliceName()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);
    QFETCH(QString, expectedError);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<Component> testComponent(new Component(VLNV(), Document::Revision::Std22));

    PortSliceValidator sliceValidator(testComponent, parser);

    QSharedPointer<PortSlice> portSlice(new PortSlice(name));

    QVERIFY(sliceValidator.hasValidName(portSlice->name()) == isValid);

    if (isValid == false)
    {
        QVector<QString> errorList;
        sliceValidator.findErrorsIn(errorList, portSlice, "mode 'testMode'");
        QVERIFY(errorList.size() > 0);
        QCOMPARE(errorList.first(), expectedError);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ModeValidator::testPortSliceName_data()
//-----------------------------------------------------------------------------
void tst_ModeValidator::testPortSliceName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");
    QTest::addColumn<QString>("expectedError");

    QTest::newRow("Name test is valid") << "test" << true << "";
    QTest::newRow("Empty name is not valid") << "" << false << "Invalid name '' set for port condition within mode 'testMode'.";
}

//-----------------------------------------------------------------------------
// Function: tst_ModeValidator::testPortSliceNameIsUnique()
//-----------------------------------------------------------------------------
void tst_ModeValidator::testPortSliceNameIsUnique()
{
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<Component> testComponent(new Component(VLNV(), Document::Revision::Std22));

    ModeValidator validator(testComponent, parser);

    QSharedPointer<Mode> mode(new Mode());
    mode->setName("testMode");

    QSharedPointer<PortSlice> portSlice(new PortSlice("slice"));
    mode->getPortSlices()->append(portSlice);

    QSharedPointer<PortSlice> duplicateSlice(new PortSlice("slice"));
    mode->getPortSlices()->append(duplicateSlice);

    QVERIFY(validator.validate(mode) == false);

    QString expectedError("Port condition name 'slice' is not unique within mode 'testMode'.");

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, mode, "test");
    QVERIFY(errorList.size() > 0);
    QVERIFY(errorList.contains(expectedError));
}

//-----------------------------------------------------------------------------
// Function: tst_ModeValidator::testPortRefs()
//-----------------------------------------------------------------------------
void tst_ModeValidator::testPortRefs()
{
    QFETCH(QString, portRef);
    QFETCH(QString, leftBound);
    QFETCH(QString, rightBound);
    QFETCH(bool, isValid);
    QFETCH(QString, expectedError);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<Component> testComponent(new Component(VLNV(), Document::Revision::Std22));

    QSharedPointer<Port> targetPort(new Port("rst_n"));
    targetPort->setLeftBound("0");
    targetPort->setRightBound("0");
    testComponent->getPorts()->append(targetPort);


    ModeValidator validator(testComponent, parser);

    QSharedPointer<Mode> mode(new Mode());
    mode->setName("testMode");

    QSharedPointer<PortSlice> portSlice(new PortSlice("slice"));
    portSlice->setPortRef(portRef);
    portSlice->setLeftRange(leftBound);
    portSlice->setRightRange(rightBound);
    mode->getPortSlices()->append(portSlice);

    QVERIFY(validator.validate(mode) == isValid);

    if (isValid == false)
    {
        QVector<QString> errorList;
        validator.findErrorsIn(errorList, mode, "test");
        QVERIFY(errorList.size() > 0);
        QCOMPARE(errorList.first(), expectedError);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ModeValidator::testPortRefs_data()
//-----------------------------------------------------------------------------
void tst_ModeValidator::testPortRefs_data()
{
    QTest::addColumn<QString>("portRef");
    QTest::addColumn<QString>("leftBound");
    QTest::addColumn<QString>("rightBound");
    QTest::addColumn<bool>("isValid");
    QTest::addColumn<QString>("expectedError");

    QTest::newRow("Empty port reference is not valid") << "" << "" << "" << false <<
        "No port reference set for 'slice' within mode 'testMode'.";
    QTest::newRow("Reference to existing port is valid") << "rst_n" << "" << "" << true <<
        "";
    QTest::newRow("Reference to unknown port is not valid") << "void" << "" << "" << false <<
        "Port 'void' in port condition 'slice' in mode 'testMode' could not be found in the component.";
    QTest::newRow("Reference within port bounds is valid") << "rst_n" << "0" << "0" << true <<
        "";
    QTest::newRow("Reference left range outside port bounds is not valid") << "rst_n" << "99" << "0" << false <<
        "Range in port condition 'slice' is outside the bounds of port 'rst_n' in mode 'testMode'.";
    QTest::newRow("Reference right range outside port bounds is not valid") << "rst_n" << "0" << "99" << false <<
        "Range in port condition 'slice' is outside the bounds of port 'rst_n' in mode 'testMode'.";
    QTest::newRow("Reference left and right range outside port bounds is not valid") << "rst_n" << "99" << "99" << false <<
        "Range in port condition 'slice' is outside the bounds of port 'rst_n' in mode 'testMode'.";
    QTest::newRow("Reference left range incorrect expression is not valid") << "rst_n" << "abc" << "0" << false <<
        "Left range in port condition 'slice' is not a valid expression in mode 'testMode'.";
    QTest::newRow("Reference right range incorrect expression is not valid") << "rst_n" << "0" << "x" << false <<
        "Right range in port condition 'slice' is not a valid expression in mode 'testMode'.";
}

//-----------------------------------------------------------------------------
// Function: tst_ModeValidator::testFieldSliceName()
//-----------------------------------------------------------------------------
void tst_ModeValidator::testFieldSliceName()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);
    QFETCH(QString, expectedError);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<Component> testComponent(new Component(VLNV(), Document::Revision::Std22));

    QSharedPointer<FieldSliceValidator> sliceValidator(new FieldSliceValidator(testComponent, parser));


    QSharedPointer<FieldSlice> fieldSlice(new FieldSlice("slice"));
    fieldSlice->setName(name);

    QVERIFY(sliceValidator->hasValidName(fieldSlice->name()) == isValid);

    if (isValid == false)
    {
        QVector<QString> errorList;
        sliceValidator->findErrorsIn(errorList, fieldSlice, "mode 'testMode'");
        QVERIFY(errorList.size() > 0);
        QCOMPARE(errorList.first(), expectedError);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ModeValidator::testFieldSliceName_data()
//-----------------------------------------------------------------------------
void tst_ModeValidator::testFieldSliceName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");
    QTest::addColumn<QString>("expectedError");

    QTest::newRow("Empty field slice name is not valid") << "" << false <<
        "Invalid name '' set for field condition within mode 'testMode'.";
    QTest::newRow("Field slice name is valid") << "slice" << true <<
        "";
}

//-----------------------------------------------------------------------------
// Function: tst_ModeValidator::testFieldSliceNameIsUnique()
//-----------------------------------------------------------------------------
void tst_ModeValidator::testFieldSliceNameIsUnique()
{
    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<Component> testComponent(new Component(VLNV(), Document::Revision::Std22));

    ModeValidator validator(testComponent, parser);

    QSharedPointer<Mode> mode(new Mode());
    mode->setName("testMode");

    QSharedPointer<FieldSlice> fieldSlice(new FieldSlice("slice"));
    mode->getFieldSlices()->append(fieldSlice);

    QSharedPointer<FieldSlice> duplicateSlice(new FieldSlice("slice"));
    mode->getFieldSlices()->append(duplicateSlice);

    QVERIFY(validator.validate(mode) == false);

    QString expectedError("Field condition name 'slice' is not unique within mode 'testMode'.");

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, mode, "test");
    QVERIFY(errorList.size() > 0);
    QVERIFY(errorList.contains(expectedError));
}

//-----------------------------------------------------------------------------
// Function: tst_ModeValidator::testFieldRefs()
//-----------------------------------------------------------------------------
void tst_ModeValidator::testFieldRefs()
{
    QFETCH(QString, spaceRef);
    QFETCH(QString, memoryRef);
    QFETCH(QString, blockRef);
    QFETCH(QString, registerRef);
    QFETCH(QString, fieldRef);
    QFETCH(bool, isValid);
    QFETCH(QString, expectedError);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<Component> testComponent(new Component(VLNV(), Document::Revision::Std22));

    QSharedPointer<AddressSpace> addressSpace(new AddressSpace("space"));
    QSharedPointer<MemoryMap> localMemoryMap(new MemoryMap("local"));
    addressSpace->setLocalMemoryMap(localMemoryMap);
    testComponent->getAddressSpaces()->append(addressSpace);


    QSharedPointer<MemoryMap> memoryMap(new MemoryMap("memory"));
    testComponent->getMemoryMaps()->append(memoryMap);

    QSharedPointer<AddressBlock> addressBlock(new AddressBlock("block"));
    memoryMap->getMemoryBlocks()->append(addressBlock);
    addressSpace->getLocalMemoryMap()->getMemoryBlocks()->append(addressBlock);

    QSharedPointer<Register> register_(new Register("register"));
    addressBlock->getRegisterData()->append(register_);

    QSharedPointer<Field> field(new Field("field"));
    register_->getFields()->append(field);

    ModeValidator validator(testComponent, parser);

    QSharedPointer<Mode> mode(new Mode());
    mode->setName("testMode");

    QSharedPointer<FieldSlice> fieldSlice(new FieldSlice("slice"));

    auto asRef = QSharedPointer<FieldReference::IndexedReference>(new FieldReference::IndexedReference(spaceRef));
    fieldSlice->setReference(asRef, FieldReference::ADDRESS_SPACE);

    auto mmRef = QSharedPointer<FieldReference::IndexedReference>(new FieldReference::IndexedReference(memoryRef));
    fieldSlice->setReference(mmRef, FieldReference::MEMORY_MAP);

    auto addressRef = QSharedPointer<FieldReference::IndexedReference>(new FieldReference::IndexedReference(blockRef));
    fieldSlice->setReference(addressRef, FieldReference::ADDRESS_BLOCK);

    auto regRef = QSharedPointer<FieldReference::IndexedReference>(new FieldReference::IndexedReference(registerRef));
    fieldSlice->setReference(regRef, FieldReference::REGISTER);

    auto bitRef = QSharedPointer<FieldReference::IndexedReference>(new FieldReference::IndexedReference(fieldRef));
    fieldSlice->setReference(bitRef, FieldReference::FIELD);

    mode->getFieldSlices()->append(fieldSlice);

    QVERIFY(validator.validate(mode) == isValid);

    if (isValid == false)
    {
        QVector<QString> errorList;
        validator.findErrorsIn(errorList, mode, "test");
        QVERIFY(errorList.size() > 0);
        QCOMPARE(errorList.first(), expectedError);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ModeValidator::testFieldRefs_data()
//-----------------------------------------------------------------------------
void tst_ModeValidator::testFieldRefs_data()
{
    QTest::addColumn<QString>("spaceRef");
    QTest::addColumn<QString>("memoryRef");
    QTest::addColumn<QString>("blockRef");
    QTest::addColumn<QString>("registerRef");
    QTest::addColumn<QString>("fieldRef");
    QTest::addColumn<bool>("isValid");
    QTest::addColumn<QString>("expectedError");

    QTest::newRow("Empty memory map reference is not valid") << "" << "" << "block" << "register" << "field" << false <<
        "Field reference in condition 'slice' is not valid in mode 'testMode'.";
    QTest::newRow("Empty address block reference is not valid") << "" << "memory" << "" << "register" << "field" << false <<
        "Field reference in condition 'slice' is not valid in mode 'testMode'.";
    QTest::newRow("Empty register reference is not valid") << "" << "memory" << "block" << "" << "field" << false <<
        "Field reference in condition 'slice' is not valid in mode 'testMode'.";
    QTest::newRow("Empty field reference is not valid") << "" << "memory" << "block" << "register" << "" << false <<
        "Field reference in condition 'slice' is not valid in mode 'testMode'.";

    QTest::newRow("Non-existent memory map reference is not valid") << "" << "none" << "block" << "register" << "field" << false <<
        "Field reference in condition 'slice' is not valid in mode 'testMode'.";
    QTest::newRow("Non-existent address space reference is not valid") << "none" << "" << "block" << "register" << "field" << false <<
        "Field reference in condition 'slice' is not valid in mode 'testMode'.";
    QTest::newRow("Non-existent address block reference is not valid") << "" << "memory" << "none" << "register" << "field" << false <<
        "Field reference in condition 'slice' is not valid in mode 'testMode'.";
    QTest::newRow("Non-existent register reference is not valid") << "" << "memory" << "block" << "none" << "field" << false <<
        "Field reference in condition 'slice' is not valid in mode 'testMode'.";
    QTest::newRow("Non-existent field reference is not valid") << "" << "memory" << "block" << "register" << "none" << false <<
        "Field reference in condition 'slice' is not valid in mode 'testMode'.";

    QTest::newRow("Reference to address space is valid") << "space" << "" << "block" << "register" << "field" << true <<
        "";

    QTest::newRow("All references are valid") << "" << "memory" << "block" << "register" << "field" << true <<
        "";
}

//-----------------------------------------------------------------------------
// Function: tst_ModeValidator::testFieldRefRange()
//-----------------------------------------------------------------------------
void tst_ModeValidator::testFieldRefRange()
{
    QFETCH(QString, left);
    QFETCH(QString, right);
    QFETCH(bool, isValid);
    QFETCH(QString, expectedError);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<Component> testComponent(new Component(VLNV(), Document::Revision::Std22));


    QSharedPointer<MemoryMap> memoryMap(new MemoryMap("memory"));
    testComponent->getMemoryMaps()->append(memoryMap);

    QSharedPointer<AddressBlock> addressBlock(new AddressBlock("block"));
    memoryMap->getMemoryBlocks()->append(addressBlock);

    QSharedPointer<Register> register_(new Register("register"));
    addressBlock->getRegisterData()->append(register_);

    QSharedPointer<Field> field(new Field("field"));
    field->setBitOffset("0");
    field->setBitWidth("8");

    register_->getFields()->append(field);

    ModeValidator validator(testComponent, parser);

    QSharedPointer<Mode> mode(new Mode());
    mode->setName("testMode");

    QSharedPointer<FieldSlice> fieldSlice(new FieldSlice("slice"));

    auto mmRef = QSharedPointer<FieldReference::IndexedReference>(new FieldReference::IndexedReference("memory"));
    fieldSlice->setReference(mmRef, FieldReference::MEMORY_MAP);

    auto addressRef = QSharedPointer<FieldReference::IndexedReference>(new FieldReference::IndexedReference("block"));
    fieldSlice->setReference(addressRef, FieldReference::ADDRESS_BLOCK);

    auto regRef = QSharedPointer<FieldReference::IndexedReference>(new FieldReference::IndexedReference("register"));
    fieldSlice->setReference(regRef, FieldReference::REGISTER);

    auto bitRef = QSharedPointer<FieldReference::IndexedReference>(new FieldReference::IndexedReference("field"));
    fieldSlice->setReference(bitRef, FieldReference::FIELD);

    mode->getFieldSlices()->append(fieldSlice);

    fieldSlice->setLeft(left);
    fieldSlice->setRight(right);

    QVERIFY(validator.validate(mode) == isValid);

    if (isValid == false)
    {
        QVector<QString> errorList;
        validator.findErrorsIn(errorList, mode, "test");
        QVERIFY(errorList.size() > 0);
        QCOMPARE(errorList.first(), expectedError);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ModeValidator::testFieldRefRange_data()
//-----------------------------------------------------------------------------
void tst_ModeValidator::testFieldRefRange_data()
{
    QTest::addColumn<QString>("left");
    QTest::addColumn<QString>("right");
    QTest::addColumn<bool>("isValid");
    QTest::addColumn<QString>("expectedError");

    QTest::newRow("Empty ranges is valid") << "" << "" << true << "";
    QTest::newRow("Both ranges are valid") << "0" << "0" << true << "";

    QTest::newRow("Left range without right is not valid") << "0" << "" << false <<
        "Left range in field condition 'slice' is not valid in mode 'testMode'.";
    QTest::newRow("Right range without left is not valid") << "" << "0" << false <<
        "Right range in field condition 'slice' is not valid in mode 'testMode'.";

}

//-----------------------------------------------------------------------------
// Function: tst_ModeValidator::testCondition()
//-----------------------------------------------------------------------------
void tst_ModeValidator::testCondition()
{
    QFETCH(QString, condition);
    QFETCH(bool, isValid);
    QFETCH(QString, expectedError);

    QSharedPointer<Component> testComponent(new Component(VLNV(), Document::Revision::Std22));

    QSharedPointer<Mode> mode(new Mode());
    mode->setName("testMode");
    mode->setCondition(condition);
    QSharedPointer<ExpressionParser> parser(new ModeConditionParser(nullptr, mode->getPortSlices(),
        mode->getFieldSlices(), testComponent->getModes()));

    ModeValidator validator(testComponent, parser);
    
    QVERIFY(validator.hasValidCondition(mode) == isValid);
    if (isValid == false)
    {
        QVector<QString> errorList;
        validator.findErrorsIn(errorList, mode, "test");
        QVERIFY(errorList.size() > 0);
        QCOMPARE(errorList.first(), expectedError);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ModeValidator::testCondition_data()
//-----------------------------------------------------------------------------
void tst_ModeValidator::testCondition_data()
{
    QTest::addColumn<QString>("condition");
    QTest::addColumn<bool>("isValid");
    QTest::addColumn<QString>("expectedError");

    QTest::newRow("Empty condition is valid") << "" << true << "";

    QTest::newRow("Empty reference is invalid") << "$ipxact_mode_condition()" << false <<
        "Condition is not a valid expression within mode 'testMode'.";

    //! Reference validation is checked in tst_ModeConditionParser.
}


QTEST_APPLESS_MAIN(tst_ModeValidator)

#include "tst_ModeValidator.moc"
