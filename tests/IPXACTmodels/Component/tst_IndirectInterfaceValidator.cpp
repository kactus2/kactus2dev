//-----------------------------------------------------------------------------
// File: tst_IndirectInterfaceValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 09.08.2017
//
// Description:
// Unit test for class IndirectInterfaceValidator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/IndirectInterface.h>
#include <IPXACTmodels/Component/TransparentBridge.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>

#include <IPXACTmodels/Component/validators/IndirectInterfaceValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

#include <editors/ComponentEditor/common/ExpressionParser.h>
#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <QDomDocument>
#include <QDomNode>

class tst_IndirectInterfaceValidator : public QObject
{
    Q_OBJECT

public:
    tst_IndirectInterfaceValidator();


private slots:
  
    void testValidateName();
    void testValidateName_data();

    void testValidateIndirectFieldReferences();
    void testValidateIndirectFieldReferences_data();

    void testValidateMemoryMapReference();
    void testValidateMemoryMapReference_data();

    void testValidateTransparentBridges();
    void testValidateTransparentBridges_data();

    void testValidateOnlyMemoryMapOrTransparentBridgePresent();
    void testValidateOnlyMemoryMapOrTransparentBridgePresent_data();

    void testValidateBitsInLau();
    void testValidateBitsInLau_data();

    void testValidateEndianness();
    void testValidateEndianness_data();

private:
    QSharedPointer<Component> containingComponent_;

    QSharedPointer<ExpressionParser> expressionParser_;

    QSharedPointer<ParameterValidator> parameterValidator_;
};

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceValidator::tst_IndirectInterfaceValidator()
//-----------------------------------------------------------------------------
tst_IndirectInterfaceValidator::tst_IndirectInterfaceValidator():
containingComponent_ (new Component()),
    expressionParser_(new SystemVerilogExpressionParser()),
    parameterValidator_(new ParameterValidator(expressionParser_, 
    QSharedPointer<QList<QSharedPointer<Choice> > >(0)))
{    
    QSharedPointer<Field> accessField(new Field("access"));
    accessField->setId("accessField");

    QSharedPointer<Field> readOnlyField(new Field("readOnly"));
    readOnlyField->setAccess(AccessTypes::READ_ONLY);
    readOnlyField->setId("readOnlyField");

    QSharedPointer<Field> readWriteOnceField(new Field("readWriteOnce"));
    readWriteOnceField->setAccess(AccessTypes::READ_WRITEONCE);
    readWriteOnceField->setId("readWriteOnceField");
   
    QSharedPointer<Field> writeOnceField(new Field("writeOnce"));
    writeOnceField->setAccess(AccessTypes::WRITEONCE);
    writeOnceField->setId("writeOnceField");

    QSharedPointer<Register> accessRegister(new Register());
    accessRegister->getFields()->append(accessField);
    accessRegister->getFields()->append(readOnlyField);
    accessRegister->getFields()->append(readWriteOnceField);
    accessRegister->getFields()->append(writeOnceField);

    QSharedPointer<AddressBlock> accessBlock(new AddressBlock());
    accessBlock->getRegisterData()->append(accessRegister);

    QSharedPointer<MemoryMap> accessMemoryMap(new MemoryMap("accessMemoryMap"));
    accessMemoryMap->getMemoryBlocks()->append(accessBlock);

    QSharedPointer<Field> targetField(new Field("target"));
    targetField->setId("targetMapField");

    QSharedPointer<Register> targetReg(new Register());
    targetReg->getFields()->append(targetField);

    QSharedPointer<AddressBlock> targetBlock(new AddressBlock());
    targetBlock->getRegisterData()->append(targetReg);

    QSharedPointer<MemoryMap> targetMap(new MemoryMap("targetMemoryMap"));
    targetMap->getMemoryBlocks()->append(targetBlock);

    QSharedPointer<BusInterface> testMasterInterface(new BusInterface());
    testMasterInterface->setName("master1");
    testMasterInterface->setInterfaceMode(General::MASTER);

    QSharedPointer<BusInterface> testSlaveInterface(new BusInterface());
    testSlaveInterface->setName("slave1");
    testSlaveInterface->setInterfaceMode(General::SLAVE);

    containingComponent_->getBusInterfaces()->append(testMasterInterface);
    containingComponent_->getBusInterfaces()->append(testSlaveInterface);

    containingComponent_->getMemoryMaps()->append(accessMemoryMap);
    containingComponent_->getMemoryMaps()->append(targetMap);    
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceValidator::testValidateName()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceValidator::testValidateName()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<IndirectInterface> testInterface (new IndirectInterface());
    testInterface->setName(name);
    
    //QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    IndirectInterfaceValidator validator(containingComponent_, expressionParser_, parameterValidator_);

    QCOMPARE(validator.hasValidName(testInterface), isValid);
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceValidator::testValidateName_data()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceValidator::testValidateName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceValidator::testValidateIndirectFieldReferences()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceValidator::testValidateIndirectFieldReferences()
{
    QFETCH(QString, reference);
    QFETCH(bool, dataValid);
    QFETCH(bool, addressValid);

    QSharedPointer<IndirectInterface> testInterface (new IndirectInterface());
    testInterface->setIndirectAddressRef(reference);
    testInterface->setIndirectDataRef(reference);
    testInterface->setMemoryMapRef("targetMemoryMap");

    IndirectInterfaceValidator validator(containingComponent_, expressionParser_, parameterValidator_);

    QCOMPARE(validator.hasValidDataReference(testInterface), dataValid);
    QCOMPARE(validator.hasValidAddressReference(testInterface), addressValid);    
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceValidator::testValidateIndirectFieldReferences_data()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceValidator::testValidateIndirectFieldReferences_data()
{
    QTest::addColumn<QString>("reference");
    QTest::addColumn<bool>("dataValid");
    QTest::addColumn<bool>("addressValid");

    QTest::newRow("Empty reference is invalid") << "" << false << false;
    QTest::newRow("Nonexisting reference is invalid") << "none" << false << false;
    QTest::newRow("Existing field is valid") << "accessField" << true << true;
    QTest::newRow("Field in referenced memory map is invalid") << "targetMapField" << false << false;
    QTest::newRow("Field address of access read-only is invalid") << "readOnlyField" << true << false;
    QTest::newRow("Field address of access read-writeOnce is invalid") << "readWriteOnceField" << true << false;
    QTest::newRow("Field address of access writeOnce is invalid") << "writeOnceField" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceValidator::testValidateMemoryMapReference()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceValidator::testValidateMemoryMapReference()
{
    QFETCH(QString, memoryMap);
    QFETCH(bool, isValid);

    QSharedPointer<IndirectInterface> testInterface (new IndirectInterface());
    testInterface->setMemoryMapRef(memoryMap);
    
    IndirectInterfaceValidator validator(containingComponent_, expressionParser_, parameterValidator_);

    QCOMPARE(validator.hasValidMemoryMapReference(testInterface), isValid);
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceValidator::testValidateMemoryMapReference_data()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceValidator::testValidateMemoryMapReference_data()
{
    QTest::addColumn<QString>("memoryMap");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Nonexisting reference is invalid") << "none" << false;
    QTest::newRow("Existing memory map is valid") << "targetMemoryMap" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceValidator::testValidateTransparentBridges()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceValidator::testValidateTransparentBridges()
{
    QFETCH(QString, masterRef);
    QFETCH(bool, isValid);

    QSharedPointer<IndirectInterface> testInterface (new IndirectInterface());
    
    QSharedPointer<TransparentBridge> testBridge(new TransparentBridge(masterRef));
    testInterface->getTransparentBridges()->append(testBridge);

    IndirectInterfaceValidator validator(containingComponent_, expressionParser_, parameterValidator_);

    QCOMPARE(validator.hasValidTransparentBridges(testInterface), isValid);
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceValidator::testValidateTransparentBridges_data()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceValidator::testValidateTransparentBridges_data()
{
    QTest::addColumn<QString>("masterRef");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Nonexisting reference is invalid") << "none" << false;
    QTest::newRow("Existing master interface is valid") << "master1" << true;
    QTest::newRow("Existing slave interface is invalid") << "slave1" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceValidator::testValidateOnlyMemoryMapOrTransparentBridgePresent()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceValidator::testValidateOnlyMemoryMapOrTransparentBridgePresent()
{
    QFETCH(QString, memoryMapRef);
    QFETCH(bool, hasBridge);
    QFETCH(bool, isValid);
    
    QSharedPointer<IndirectInterface> testInterface (new IndirectInterface());
    testInterface->setName("testInterface");
    testInterface->setIndirectAddressRef("accessField");
    testInterface->setIndirectDataRef("accessField");
    
    testInterface->setMemoryMapRef(memoryMapRef);

    if (hasBridge)
    {
        QSharedPointer<TransparentBridge> testBridge(new TransparentBridge("master1"));
        testInterface->getTransparentBridges()->append(testBridge);
    }

    IndirectInterfaceValidator validator(containingComponent_, expressionParser_, parameterValidator_);

    QCOMPARE(validator.validate(testInterface), isValid);
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceValidator::testValidateOnlyMemoryMapOrTransparentBridgePresent_data()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceValidator::testValidateOnlyMemoryMapOrTransparentBridgePresent_data()
{
    QTest::addColumn<QString>("memoryMapRef");
    QTest::addColumn<bool>("hasBridge");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Memory map only is valid") << "targetMemoryMap" << false << true;
    QTest::newRow("Transparent bridge only is valid") << "" << true << true;
    QTest::newRow("No transparent bridge or memory map is invalid") << "" << false << false;
    QTest::newRow("Bot transparent bridge and memory map is invalid") << "targetMemoryMap" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceValidator::testValidateBitsInLau()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceValidator::testValidateBitsInLau()
{
    QFETCH(QString, bitsInLau);
    QFETCH(bool, isValid);

    QSharedPointer<IndirectInterface> testInterface (new IndirectInterface());
    testInterface->setName("testInterface");
    testInterface->setIndirectAddressRef("accessField");
    testInterface->setIndirectDataRef("accessField");

    testInterface->setBitsInLau(bitsInLau);

    IndirectInterfaceValidator validator(containingComponent_, expressionParser_, parameterValidator_);

    QCOMPARE(validator.hasValidBitsInLau(testInterface), isValid);
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceValidator::testValidateBitsInLau_data()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceValidator::testValidateBitsInLau_data()
{
    QTest::addColumn<QString>("bitsInLau");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Lau of 10 is valid") << "10" << true;
    QTest::newRow("Empty Lau is valid") << "" << true;
    QTest::newRow("Lau of -2 is invalid") << "-2" << false;
    QTest::newRow("Lau of 20*80 is valid") << "20*80" << true;
    QTest::newRow("Text is invalid for Lau") << "text" << false;
    QTest::newRow("String is invalid for Lau") << "\"text\"" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceValidator::testValidateEndianness()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceValidator::testValidateEndianness()
{
    QFETCH(QString, endianness);
    QFETCH(bool, isValid);

    QSharedPointer<IndirectInterface> testInterface (new IndirectInterface());
    testInterface->setName("testInterface");
    testInterface->setEndianness(endianness);
   
    IndirectInterfaceValidator validator(containingComponent_, expressionParser_, parameterValidator_);

    QCOMPARE(validator.hasValidEndianness(testInterface), isValid);
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceValidator::testValidateEndianness_data()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceValidator::testValidateEndianness_data()
{
    QTest::addColumn<QString>("endianness");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("big is valid endianness") << "big" << true;
    QTest::newRow("little is valid endianness") << "little" << true;
    QTest::newRow("unset endianness is valid") << "" << true;
    QTest::newRow("other words are invalid") << "other" << false;
    QTest::newRow("numbers are invalid") << "128" << false;
}

QTEST_APPLESS_MAIN(tst_IndirectInterfaceValidator)

#include "tst_IndirectInterfaceValidator.moc"
