//-----------------------------------------------------------------------------
// File: tst_BusInterfaceValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.12.2015
//
// Description:
// Unit test for class BusInterfaceValidator.
//-----------------------------------------------------------------------------

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WirePort.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>

#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>
#include <IPXACTmodels/Component/validators/PortMapValidator.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/AbstractionType.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/MasterInterface.h>
#include <IPXACTmodels/Component/SlaveInterface.h>
#include <IPXACTmodels/Component/MirroredSlaveInterface.h>

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/RemapState.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/Range.h>

#include <tests/MockObjects/LibraryMock.h>

#include <QSharedPointer>
#include <QtTest>

class tst_BusInterfaceValidator : public QObject
{
    Q_OBJECT

public:
    tst_BusInterfaceValidator();

private slots:

    void testHasValidName();
    void testHasValidName_data();

    void testHasValidIsPresent();
    void testHasValidIsPresent_data();

    void testHasValidBusType();

    void testHasValidAbstractionTypeAbstractionRef();
    void testHasValidAbstractionTypeViewRef();
    void testHasValidAbstractionTypeViewRef_data();

    void testPortMapLogicalPortIsValid();
    void testPortMapLogicalPortIsValid_data();
    void testMultiplePortMapLogicalPortsAreValid();
    void testMultiplePortMapLogicalPortsAreValid_data();

    void testPortMapPhysicalPortIsValid();
    void testPortMapPhysicalPortIsValid_data();

    void testPortMapConnectedPortDirectionsAreValid();
    void testPortMapConnectedPortDirectionsAreValid_data();
    void testPortMapConnectedPortsHaveValidType();
    void testPortMapConnectedPortsHaveValidType_data();
    void testPortMapConnectedPortsHaveSameRange();
    void testPortMapConnectedPortsHaveSameRange_data();

    void testHasValidMasterInterface();
    void testHasValidMasterInterface_data();
    void testHasValidSlaveInterface();
    void testHasValidSlaveInterface_data();
    void testHasValidSystemInterface();
    void testHasValidSystemInterface_data();
    void testHasValidMirroredSlaveInterface();
    void testHasValidMirroredSlaveInterface_data();
    void testHasValidMonitorInterface();
    void testHasValidMonitorInterface_data();

    void testHasValidBitsInLau();
    void testHasValidBitsInLau_data();

    void testHasValidBitSteeringForInterfaceMode();
    void testHasValidBitSteeringForInterfaceMode_data();

    void testHasValidParameters();

private:
    
    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);

    QSharedPointer<BusInterfaceValidator> createBusInterfaceValidator(
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<QList<QSharedPointer<Choice> > > choices,
        QSharedPointer<QList<QSharedPointer<View> > > views, QSharedPointer<QList<QSharedPointer<Port> > > ports,
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces,
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps,
        QSharedPointer<QList<QSharedPointer<BusInterface> > > busInterfaces,
        QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets,
        QSharedPointer<QList<QSharedPointer<RemapState> > > remapStates,
        LibraryInterface* libraryHandler);
};

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::tst_BusInterfaceValidator()
//-----------------------------------------------------------------------------
tst_BusInterfaceValidator::tst_BusInterfaceValidator()
{
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testNameIsvalid()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidName()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName(name);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (),
        QSharedPointer<QList<QSharedPointer<View> > > (), QSharedPointer<QList<QSharedPointer<Port> > > (),
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (), 0);

    QCOMPARE(validator->hasValidName(testBus), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testBus, "test");

        QString expectedError = QObject::tr("Invalid name specified for bus interface %1 within %2")
            .arg(testBus->name()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidName_data()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidIsPresent()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidIsPresent()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName("testBus");
    testBus->setIsPresent(isPresent);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (),
        QSharedPointer<QList<QSharedPointer<View> > > (), QSharedPointer<QList<QSharedPointer<Port> > > (),
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (), 0);

    QCOMPARE(validator->hasValidIsPresent(testBus->getIsPresent()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testBus, "test");

        QString expectedError = QObject::tr("Invalid isPresent set for bus interface %1 within %2")
            .arg(testBus->name()).arg("test");
        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidIsPresent_data()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidIsPresent_data()
{
    QTest::addColumn<QString>("isPresent");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("IsPresent 1 is valid") << "1" << true;
    QTest::newRow("IsPresent 1*3-3 is valid") << "1*3-3" << true;
    QTest::newRow("IsPresent 2*100 is invalid") << "2*100" << false;
    QTest::newRow("IsPresent -14 is invalid") << "-14" << false;
    QTest::newRow("Real number isPresent  0.12 is invalid") << "0.12" << false;
    QTest::newRow("Text as isPresent is invalid") << "test" << false;
    QTest::newRow("String as isPresent is invalid") << "\"test\"" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidBusType()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidBusType()
{
    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName("testBus");

    LibraryMock* mockLibrary (new LibraryMock(this));
    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (),
        QSharedPointer<QList<QSharedPointer<View> > > (), QSharedPointer<QList<QSharedPointer<Port> > > (),
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (),
        mockLibrary);

    QCOMPARE(validator->hasValidBusType(testBus), false);

    QVector<QString> foundErrors;
    validator->findErrorsIn(foundErrors, testBus, "test");

    QString expectedError = QObject::tr("Bus definition must be given for bus interface %1 within %2")
        .arg(testBus->name()).arg("test");
    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }

    ConfigurableVLNVReference testType(VLNV::BUSDEFINITION, "testVendor", "testLibrary", "busDefinition", "1.1");
    testBus->setBusType(testType);

    QCOMPARE(validator->hasValidBusType(testBus), false);

    foundErrors.clear();
    validator->findErrorsIn(foundErrors, testBus, "test");

    expectedError = QObject::tr("Bus definition %1 set for bus interface %2 within %3 could not be found "
        "in the library").arg(testType.toString()).arg(testBus->name()).arg("test");
    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }

    QSharedPointer<BusDefinition> testDefinition (new BusDefinition());
    testDefinition->setVlnv(testType);

    mockLibrary->addComponent(testDefinition);

    QCOMPARE(validator->hasValidBusType(testBus), true);

    ConfigurableVLNVReference componentType (VLNV::COMPONENT, "testVendor", "testLbirary", "component", "0.8");
    testBus->setBusType(componentType);

    QCOMPARE(validator->hasValidBusType(testBus), false);

    foundErrors.clear();
    validator->findErrorsIn(foundErrors, testBus, "test");

    expectedError = QObject::tr("Invalid bus definition set for %1 within %2").arg(testBus->name()).arg("test");
    if (errorIsNotFoundInErrorList(expectedError, foundErrors))
    {
        QFAIL("No error message found");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidAbstractionTypeViewRef()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidAbstractionTypeAbstractionRef()
{
    QSharedPointer<AbstractionType> testAbstraction(new AbstractionType());

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName("testBus");

    LibraryMock* mockLibrary (new LibraryMock(this));
    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (),
        QSharedPointer<QList<QSharedPointer<View> > > (), QSharedPointer<QList<QSharedPointer<Port> > > (),
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (),
        mockLibrary);

    QCOMPARE(validator->hasValidAbstractionTypes(testBus), true);

    testBus->getAbstractionTypes()->append(testAbstraction);

    QCOMPARE(validator->hasValidAbstractionTypes(testBus), false);

    QVector<QString> errorsFound;
    validator->findErrorsIn(errorsFound, testBus, "test");
    QString expectedError = QObject::tr("Abstraction reference must be set for each abstraction type in "
        "bus interface %1").arg(testBus->name());
    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }

    QSharedPointer<ConfigurableVLNVReference> abstractionReference (new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "testVendor", "testLibrary", "Everett", "5.1"));
    testAbstraction->setAbstractionRef(abstractionReference);

    QCOMPARE(validator->hasValidAbstractionTypes(testBus), false);

    errorsFound.clear();
    validator->findErrorsIn(errorsFound, testBus, "test");
    expectedError = QObject::tr("Abstraction reference %1 set for bus interface %2 could not be found "
        "in the library").arg(abstractionReference->toString()).arg(testBus->name());
    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }

    QSharedPointer<AbstractionDefinition> abstractionDefinition (new AbstractionDefinition());
    abstractionDefinition->setVlnv(*abstractionReference.data());

    mockLibrary->addComponent(abstractionDefinition);

    QCOMPARE(validator->hasValidAbstractionTypes(testBus), true);
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidAbstractionTypeViewRef()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidAbstractionTypeViewRef()
{
    QFETCH(QString, viewName);
    QFETCH(bool, createView);
    QFETCH(bool, isValid);

    QSharedPointer<QList<QSharedPointer<View> > > testViews (new QList<QSharedPointer<View> > ());
    if (createView)
    {
        if (viewName.isEmpty())
        {
            viewName = "Champloo";
        }

        QSharedPointer<View> newview (new View(viewName));
        testViews->append(newview);
    }

    QSharedPointer<ConfigurableVLNVReference> abstractionReference (new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "testVendor", "testLibrary", "Everett", "5.1"));

    QSharedPointer<AbstractionDefinition> abstractionDefinition (new AbstractionDefinition());
    abstractionDefinition->setVlnv(*abstractionReference.data());

    LibraryMock* mockLibrary (new LibraryMock(this));
    mockLibrary->addComponent(abstractionDefinition);

    QSharedPointer<AbstractionType> testAbstraction(new AbstractionType());
    testAbstraction->setAbstractionRef(abstractionReference);
    testAbstraction->setViewRef(viewName);

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName("testBus");
    testBus->getAbstractionTypes()->append(testAbstraction);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator  = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (), testViews,
        QSharedPointer<QList<QSharedPointer<Port> > > (), QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (),
        mockLibrary);

    QCOMPARE (validator->hasValidAbstractionTypes(testBus), isValid);

    if (!isValid)
    {
        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testBus, "test");
        QString expectedError = QObject::tr("Invalid view reference %1 set for abstraction type in bus interface "
            "%2").arg(viewName).arg(testBus->name());
        if (errorIsNotFoundInErrorList(expectedError, errorsFound))
        {
            QFAIL("Error was not found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidAbstractionTypeViewRef_data()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidAbstractionTypeViewRef_data()
{
    QTest::addColumn<QString>("viewName");
    QTest::addColumn<bool>("createView");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("View reference view is valid when view exists") << "view" << true << true;
    QTest::newRow("Empty view reference is valid when views do not exist") << "" << false << true;
    QTest::newRow("Empty view reference is valid when views exist") << "" << true << true;
    QTest::newRow("View reference is not valid when view does not exist") << "view" << false << false;
}


//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testPortMapLogicalPortIsValid()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testPortMapLogicalPortIsValid()
{
    QFETCH(QString, logicalPortName);
    QFETCH(QString, rangeLeft);
    QFETCH(QString, rangeRight);
    QFETCH(QString, abstractionWidth);
    QFETCH(bool, createPort);
    QFETCH(bool, isValid);

    QSharedPointer<ConfigurableVLNVReference> abstractionReference (new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "testVendor", "testLibrary", "Everett", "5.1"));

    QSharedPointer<AbstractionDefinition> abstractionDefinition (new AbstractionDefinition());
    abstractionDefinition->setVlnv(*abstractionReference.data());

    if (createPort)
    {
        QSharedPointer<WirePort> masterWire (new WirePort());
        masterWire->setDirection(DirectionTypes::IN);
        masterWire->setWidth(abstractionWidth);

        QSharedPointer<WireAbstraction> logicalWireAbstraction (new WireAbstraction());
        logicalWireAbstraction->setMasterPort(masterWire);

        QSharedPointer<PortAbstraction> testLogicalAbstraction (new PortAbstraction());
        testLogicalAbstraction->setWire(logicalWireAbstraction);

        testLogicalAbstraction->setLogicalName(logicalPortName);
        abstractionDefinition->getLogicalPorts()->append(testLogicalAbstraction);
    }

    LibraryMock* mockLibrary (new LibraryMock(this));
    mockLibrary->addComponent(abstractionDefinition);

    QSharedPointer<Range> logicalRange (new Range(rangeLeft, rangeRight));

    QSharedPointer<PortMap::LogicalPort> testLogical (new PortMap::LogicalPort());
    testLogical->name_ = logicalPortName;
    testLogical->range_ = logicalRange;

    QSharedPointer<PartSelect> physicalPart (new PartSelect(rangeLeft, rangeRight));

    QSharedPointer<PortMap::PhysicalPort> testPhysical (new PortMap::PhysicalPort());
    testPhysical->name_ = QLatin1String("Physical");
    testPhysical->partSelect_ = physicalPart;

    QSharedPointer<PortMap> testPortMap (new PortMap());
    testPortMap->setLogicalPort(testLogical);
    testPortMap->setPhysicalPort(testPhysical);

    QSharedPointer<AbstractionType> testAbstraction(new AbstractionType());
    testAbstraction->setAbstractionRef(abstractionReference);
    testAbstraction->getPortMaps()->append(testPortMap);

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName("testBus");
    testBus->setInterfaceMode(General::MASTER);
    testBus->getAbstractionTypes()->append(testAbstraction);

    QSharedPointer<Port> newPort (new Port(testPhysical->name_));
    newPort->setLeftBound(rangeLeft);
    newPort->setRightBound(rangeRight);

    QSharedPointer<QList<QSharedPointer<Port> > > componentPorts (new QList<QSharedPointer<Port> > ());
    componentPorts->append(newPort);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (),
        QSharedPointer<QList<QSharedPointer<View> > > (), componentPorts,
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (),
        mockLibrary);

    QCOMPARE (validator->hasValidAbstractionTypes(testBus), isValid);

    if (!isValid)
    {
        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testBus, "test");

        QString context = QObject::tr("%1 bus interface %2")
            .arg(General::interfaceMode2Str(testBus->getInterfaceMode())).arg(testBus->name());

        QString expectedError;
        if (logicalPortName.isEmpty())
        {
            expectedError = QObject::tr("Port map does not contain a logical port within %1").arg(context);
        }
        else if (!createPort)
        {
            expectedError = QObject::tr("Could not locate logical port %1 mapped within %2")
                .arg(logicalPortName).arg(context);
        }

        else if (parser->parseExpression(rangeLeft).toInt() < 0 || parser->parseExpression(rangeRight) < 0 ||
            rangeLeft.isEmpty() || rangeRight.isEmpty())
        {
            expectedError = QObject::tr("Invalid range given for logical port %1 mapped within %2")
                .arg(logicalPortName).arg(context);
        }
        else if (!abstractionWidth.isEmpty())
        {
            expectedError = QObject::tr("Logical port %1 range is not within referenced abstraction definition "
                "port width within %2").arg(logicalPortName).arg(context);
        }

        if (errorIsNotFoundInErrorList(expectedError, errorsFound))
        {
            QFAIL("Error was not found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testPortMapLogicalPortIsValid_data()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testPortMapLogicalPortIsValid_data()
{
    QTest::addColumn<QString>("logicalPortName");
    QTest::addColumn<QString>("rangeLeft");
    QTest::addColumn<QString>("rangeRight");
    QTest::addColumn<QString>("abstractionWidth");
    QTest::addColumn<bool>("createPort");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Logical port with long range is valid") <<
        "logicalPort" << "4000000000" << "4000000001" << "5000000000" << true << true;

    QTest::newRow("Logical port without range is valid") << "logicalPort" << "" << "" << "" << true << true;
    QTest::newRow("Logical port with range is valid") << "logicalPort" << "4+4" << "12" << "" << true << true;
    QTest::newRow("Logical port with either range as 0 is valid") << "logicalPort" << "0" << "0" << "" << true <<
        true;
    QTest::newRow("Logical port with negative range value is not valid") << "logicalPort" << "-1" << "1" << "" <<
        true << false;
    QTest::newRow("Logical port without other range value is not valid") << "logicalPort" << "12" << "" << "" <<
        true << false;
    QTest::newRow("Referencing non-existing logical port is not valid") << "logicalPort" << "" << "" << "" <<
        false << false;
    QTest::newRow("Empty logical port reference is not valid") << "" << "" << "" << "" << true << false;

    QTest::newRow("Range within abstraction width is valid") << "illogical" << "0" << "5" << "10" << true << true;
    QTest::newRow("Right range not within abstraction width is not valid") << "illogical" << "0" << "20" <<
        "10" << true << false;
    QTest::newRow("Left range not within abstraction width is not valid") << "illogical" << "15" << "8" << "10" <<
        true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testMultiplePortMapLogicalPortsAreValid()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testMultiplePortMapLogicalPortsAreValid()
{
    QFETCH(QString, logicalNameOne);
    QFETCH(QString, rangeLeftOne);
    QFETCH(QString, rangeRightOne);
    QFETCH(QString, logicalNameTwo);
    QFETCH(QString, rangeLeftTwo);
    QFETCH(QString, rangeRightTwo);
    QFETCH(bool, isValid);

    QSharedPointer<ConfigurableVLNVReference> abstractionReference (new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "testVendor", "testLibrary", "Everett", "5.1"));

    QSharedPointer<AbstractionDefinition> abstractionDefinition (new AbstractionDefinition());
    abstractionDefinition->setVlnv(*abstractionReference);

    QSharedPointer<WirePort> masterWireOne(new WirePort());
    masterWireOne->setDirection(DirectionTypes::IN);

    QSharedPointer<WireAbstraction> logicalWireAbstractionOne (new WireAbstraction());
    logicalWireAbstractionOne->setMasterPort(masterWireOne);

    QSharedPointer<PortAbstraction> testLogicalAbstractionOne (new PortAbstraction());
    testLogicalAbstractionOne->setWire(logicalWireAbstractionOne);

    testLogicalAbstractionOne->setLogicalName(logicalNameOne);
    abstractionDefinition->getLogicalPorts()->append(testLogicalAbstractionOne);

    if (logicalNameTwo != logicalNameOne)
    {
        QSharedPointer<WirePort> masterWireTwo (new WirePort());
        masterWireTwo->setDirection(DirectionTypes::IN);

        QSharedPointer<WireAbstraction> logicalWireAbstractionTwo (new WireAbstraction());
        logicalWireAbstractionTwo->setMasterPort(masterWireTwo);

        QSharedPointer<PortAbstraction> testLogicalAbstractionTwo (new PortAbstraction());
        testLogicalAbstractionTwo->setWire(logicalWireAbstractionTwo);
        testLogicalAbstractionTwo->setLogicalName(logicalNameTwo);
        
        abstractionDefinition->getLogicalPorts()->append(testLogicalAbstractionTwo);
    }

    LibraryMock* mockLibrary (new LibraryMock(this));
    mockLibrary->addComponent(abstractionDefinition);

    QSharedPointer<Range> logicalRangeOne (new Range(rangeLeftOne, rangeRightOne));

    QSharedPointer<PortMap::LogicalPort> testLogicalOne (new PortMap::LogicalPort());
    testLogicalOne->name_ = logicalNameOne;
    testLogicalOne->range_ = logicalRangeOne;

    QSharedPointer<PartSelect> physicalPartOne (new PartSelect(rangeLeftOne, rangeRightOne));

    QSharedPointer<PortMap::PhysicalPort> testPhysicalOne (new PortMap::PhysicalPort());
    testPhysicalOne->name_ = QLatin1String("Physical");
    testPhysicalOne->partSelect_ = physicalPartOne;

    QSharedPointer<PortMap> testPortMapOne (new PortMap());
    testPortMapOne->setLogicalPort(testLogicalOne);
    testPortMapOne->setPhysicalPort(testPhysicalOne);

    QSharedPointer<Range> logicalRangeTwo (new Range(rangeLeftTwo, rangeRightTwo));
    
    QSharedPointer<PortMap::LogicalPort> testLogicalTwo (new PortMap::LogicalPort());
    testLogicalTwo->name_ = logicalNameTwo;
    testLogicalTwo->range_ = logicalRangeTwo;

    QSharedPointer<PartSelect> physicalPartTwo (new PartSelect(rangeLeftTwo, rangeRightTwo));

    QSharedPointer<PortMap::PhysicalPort> testPhysicalTwo (new PortMap::PhysicalPort());
    testPhysicalTwo->name_ = QLatin1String("PhysicalTwo");
    testPhysicalTwo->partSelect_ = physicalPartTwo;

    QSharedPointer<PortMap> testPortMapTwo (new PortMap());
    testPortMapTwo->setLogicalPort(testLogicalTwo);
    testPortMapTwo->setPhysicalPort(testPhysicalTwo);

    QSharedPointer<AbstractionType> testAbstraction(new AbstractionType());
    testAbstraction->setAbstractionRef(abstractionReference);
    testAbstraction->getPortMaps()->append(testPortMapOne);
    testAbstraction->getPortMaps()->append(testPortMapTwo);

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName("testBus");
    testBus->setInterfaceMode(General::MASTER);
    testBus->getAbstractionTypes()->append(testAbstraction);

    QSharedPointer<Port> newPortOne (new Port(testPhysicalOne->name_));
    newPortOne->setLeftBound(rangeLeftOne);
    newPortOne->setRightBound(rangeRightOne);

    QSharedPointer<Port> newPortTwo (new Port(testPhysicalTwo->name_));
    newPortTwo->setLeftBound(rangeLeftTwo);
    newPortTwo->setRightBound(rangeRightTwo);

    QSharedPointer<QList<QSharedPointer<Port> > > componentPorts (new QList<QSharedPointer<Port> > ());
    componentPorts->append(newPortOne);
    componentPorts->append(newPortTwo);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (),
        QSharedPointer<QList<QSharedPointer<View> > > (), componentPorts,
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (),
        mockLibrary);

    QCOMPARE (validator->hasValidAbstractionTypes(testBus), isValid);

    if (!isValid)
    {
        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testBus, "test");

        QString context = tr("%1 bus interface %2").arg(General::interfaceMode2Str(testBus->getInterfaceMode()),
            testBus->name());

        QString expectedError = QObject::tr("Multiple definitions of logical port %1 overlap within %2").arg(
            logicalNameTwo, context);

        if (errorIsNotFoundInErrorList(expectedError, errorsFound))
        {
            QFAIL("Error was not found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testMultiplePortMapLogicalPortsAreValid_data()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testMultiplePortMapLogicalPortsAreValid_data()
{
    QTest::addColumn<QString>("logicalNameOne");
    QTest::addColumn<QString>("rangeLeftOne");
    QTest::addColumn<QString>("rangeRightOne");
    QTest::addColumn<QString>("logicalNameTwo");
    QTest::addColumn<QString>("rangeLeftTwo");
    QTest::addColumn<QString>("rangeRightTwo");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Ports referencing different abstraction ports is valid") <<
        "Gurren" << "0" << "4" << "Lagann" << "0" << "4" << true;
    QTest::newRow("Ports referencing the same abstraction port with different ranges is valid") <<
        "GurrenLagann" << "0" << "4" << "GurrenLagann" << "8" << "10" << true;
    QTest::newRow("Logical1: range = [0,10]; Logical2: range = [5,15] is not valid") <<
        "GurrenLagann" << "0" << "10" << "GurrenLagann" << "5" << "15" << false;
    QTest::newRow("Logical1: range = [5,15]; Logical2: range = [0,10] is not valid") <<
        "GurrenLagann" << "5" << "15" << "GurrenLagann" << "0" << "10" << false;
    QTest::newRow("Logical1: range = [5,8]; Logical2: range = [0,10] is not valid") <<
        "GurrenLagann" << "5" << "8" << "GurrenLagann" << "0" << "10" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testPortMapPhysicalPortIsValid()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testPortMapPhysicalPortIsValid()
{
    QFETCH(QString, physicalPortName);
    QFETCH(QString, physicalRangeLeft);
    QFETCH(QString, physicalRangeRight);
    QFETCH(QString, portLeftBound);
    QFETCH(QString, portRigthBound);
    QFETCH(bool, createPort);
    QFETCH(bool, isValid);

    QSharedPointer<ConfigurableVLNVReference> abstractionReference (new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "testVendor", "testLibrary", "Everett", "5.1"));

    QSharedPointer<AbstractionDefinition> abstractionDefinition (new AbstractionDefinition());
    abstractionDefinition->setVlnv(*abstractionReference.data());

    QSharedPointer<WireAbstraction> logicalWireAbstraction (new WireAbstraction());

    QSharedPointer<PortAbstraction> testLogicalAbstraction (new PortAbstraction());
    testLogicalAbstraction->setLogicalName("testLogical");
    testLogicalAbstraction->setWire(logicalWireAbstraction);
    abstractionDefinition->getLogicalPorts()->append(testLogicalAbstraction);

    LibraryMock* mockLibrary (new LibraryMock(this));
    mockLibrary->addComponent(abstractionDefinition);

    QSharedPointer<Range> logicalRange (new Range(physicalRangeLeft, physicalRangeRight));

    QSharedPointer<PortMap::LogicalPort> testLogical (new PortMap::LogicalPort());
    testLogical->name_ = testLogicalAbstraction->name();
    testLogical->range_ = logicalRange;

    QSharedPointer<PartSelect> physicalPart (new PartSelect(physicalRangeLeft, physicalRangeRight));

    QSharedPointer<PortMap::PhysicalPort> testPhysical (new PortMap::PhysicalPort());
    testPhysical->name_ = physicalPortName;
    testPhysical->partSelect_ = physicalPart;

    QSharedPointer<PortMap> testPortMap (new PortMap());
    testPortMap->setLogicalPort(testLogical);
    testPortMap->setPhysicalPort(testPhysical);

    QSharedPointer<AbstractionType> testAbstraction(new AbstractionType());
    testAbstraction->setAbstractionRef(abstractionReference);
    testAbstraction->getPortMaps()->append(testPortMap);

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName("testBus");
    testBus->getAbstractionTypes()->append(testAbstraction);

    QSharedPointer<QList<QSharedPointer<Port> > > componentPorts (new QList<QSharedPointer<Port> > ());
    if (createPort)
    {
        QSharedPointer<Port> newPort (new Port(physicalPortName));
        newPort->setLeftBound(portLeftBound);
        newPort->setRightBound(portRigthBound);

        if (physicalPortName.isEmpty())
        {
            physicalPortName = "Champloo";
            newPort->setName(physicalPortName);
            physicalPortName.clear();
        }
        componentPorts->append(newPort);
    }

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (),
        QSharedPointer<QList<QSharedPointer<View> > > (), componentPorts,
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (),
        mockLibrary);

    QCOMPARE (validator->hasValidAbstractionTypes(testBus), isValid);

    if (!isValid)
    {
        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testBus, "test");

        QString expectedError;

        if (physicalPortName.isEmpty())
        {
            expectedError = QObject::tr("Port map does not contain a physical port or a logical tie off within "
                "%1 bus interface %2")
                .arg(General::interfaceMode2Str(testBus->getInterfaceMode())).arg(testBus->name());
        }
        else if (!createPort)
        {
            expectedError = QObject::tr("Could not locate physical port %1 mapped within %2 bus interface %3")
                .arg(physicalPortName).arg(General::interfaceMode2Str(testBus->getInterfaceMode()))
                .arg(testBus->name());
        }

        else if (parser->parseExpression(physicalRangeLeft).toInt() < 0 || parser->parseExpression(physicalRangeRight) < 0 ||
            physicalRangeLeft.isEmpty() || physicalRangeRight.isEmpty())
        {
            expectedError = QObject::tr("Invalid part select given for physical port %1 mapped within %2 bus "
                "interface %3").arg(physicalPortName).arg(General::interfaceMode2Str(testBus->getInterfaceMode()))
                .arg(testBus->name());
        }
        else
        {
            expectedError = QObject::tr("Physical port ranges are not within the referenced port %1 within %2 bus "
                "interface %3")
                .arg(physicalPortName).arg(General::interfaceMode2Str(testBus->getInterfaceMode()))
                .arg(testBus->name());
        }

        if (errorIsNotFoundInErrorList(expectedError, errorsFound))
        {
            QFAIL("Error was not found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testPortMapPhysicalPortIsValid_data()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testPortMapPhysicalPortIsValid_data()
{
    QTest::addColumn<QString>("physicalPortName");
    QTest::addColumn<QString>("physicalRangeLeft");
    QTest::addColumn<QString>("physicalRangeRight");
    QTest::addColumn<QString>("portLeftBound");
    QTest::addColumn<QString>("portRigthBound");
    QTest::addColumn<bool>("createPort");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Physical port without range is valid")
        << "physicalPort" << "" << "" << "0" << "20" << true << true;
    QTest::newRow("Physical port with range is valid")
        << "physicalPort" << "4+4" << "12" << "0" << "20" <<  true << true;
    QTest::newRow("Physical port with either range as 0 is valid")
        << "physicalPort" << "0" << "0" << "0" << "20" << true << true;
    QTest::newRow("Physical port with negative range value is not valid")
        << "physicalPort" << "-1" << "1" << "0" << "20" << true << false;
    QTest::newRow("Physical port without other range value is not valid")
        << "physicalPort" << "12" << "" << "0" << "20" << true << false;
    QTest::newRow("Referencing non-existing physical port is not valid")
        << "physicalPort" << "" << "" << "0" << "20" << false << false;
    QTest::newRow("Empty physical port reference is not valid")
        << "" << "" << "" << "0" << "20" << true << false;

    QTest::newRow("Physical port ranges outside referenced port range is not valid")
        << "testPort" << "0" << "4" << "1" << "10" << true << false;
    QTest::newRow("Physical port ranges outside referenced port range is not valid")
        << "testPort" << "12" << "4" << "1" << "10" << true << false;

    QTest::newRow("Physical port with long range is valid") <<
        "testPort" << "4000000000" << "4000000000" << "5000000000" << "3000000000" << true << true;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testPortMapConnectedPortDirectionsAreValid()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testPortMapConnectedPortDirectionsAreValid()
{
    QFETCH(QString, logicalDirection);
    QFETCH(QString, physicalDirection);
    QFETCH(bool, physicalAllLogicalDirectionsAllowed);
    QFETCH(bool, isMirroredInterface);
    QFETCH(bool, isValid);

    QSharedPointer<ConfigurableVLNVReference> abstractionReference (new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "testVendor", "testLibrary", "Everett", "5.1"));

    QSharedPointer<AbstractionDefinition> abstractionDefinition (new AbstractionDefinition());
    abstractionDefinition->setVlnv(*abstractionReference.data());

    QSharedPointer<WirePort> masterWire (new WirePort());
    masterWire->setDirection(DirectionTypes::str2Direction(logicalDirection, DirectionTypes::DIRECTION_INVALID));

    QSharedPointer<WireAbstraction> logicalWireAbstraction (new WireAbstraction());

    QSharedPointer<PortAbstraction> testLogicalAbstraction (new PortAbstraction());
    testLogicalAbstraction->setLogicalName("testLogical");
    testLogicalAbstraction->setWire(logicalWireAbstraction);
    testLogicalAbstraction->getWire()->setMasterPort(masterWire);

    abstractionDefinition->getLogicalPorts()->append(testLogicalAbstraction);

    LibraryMock* mockLibrary (new LibraryMock(this));
    mockLibrary->addComponent(abstractionDefinition);

    QSharedPointer<PortMap::LogicalPort> testLogical (new PortMap::LogicalPort());
    testLogical->name_ = testLogicalAbstraction->name();

    QSharedPointer<PortMap::PhysicalPort> testPhysical (new PortMap::PhysicalPort());
    testPhysical->name_ = "testPhysicalPort";

    QSharedPointer<Port> newPort (new Port(testPhysical->name_));
    newPort->getWire()->setDirection(
        DirectionTypes::str2Direction(physicalDirection, DirectionTypes::DIRECTION_INVALID));
    newPort->getWire()->setAllLogicalDirectionsAllowed(physicalAllLogicalDirectionsAllowed);
    QSharedPointer<QList<QSharedPointer<Port> > > componentPorts (new QList<QSharedPointer<Port> > ());
    componentPorts->append(newPort);

    QSharedPointer<PortMap> testPortMap (new PortMap());
    testPortMap->setLogicalPort(testLogical);
    testPortMap->setPhysicalPort(testPhysical);

    QSharedPointer<AbstractionType> testAbstraction(new AbstractionType());
    testAbstraction->setAbstractionRef(abstractionReference);
    testAbstraction->getPortMaps()->append(testPortMap);

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName("testBus");
    testBus->getAbstractionTypes()->append(testAbstraction);

    if (isMirroredInterface)
    {
        testBus->setInterfaceMode(General::MIRROREDMASTER);
    }
    else
    {
        testBus->setInterfaceMode(General::MASTER);
    }

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (),
        QSharedPointer<QList<QSharedPointer<View> > > (), componentPorts,
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (),
        mockLibrary);

    QCOMPARE (validator->hasValidAbstractionTypes(testBus), isValid);

    if (!isValid)
    {
        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testBus, "test");

        QString context = QObject::tr("%1 bus interface %2")
            .arg(General::interfaceMode2Str(testBus->getInterfaceMode())).arg(testBus->name());

        QString expectedError = QObject::tr("Invalid connection made between logical port %1 with direction %2 and "
            "physical port %3 with direction %4 within %5")
            .arg(testPortMap->getLogicalPort()->name_).arg(logicalDirection)
            .arg(testPortMap->getPhysicalPort()->name_).arg(physicalDirection)
            .arg(context);

        if (errorIsNotFoundInErrorList(expectedError, errorsFound))
        {
            QFAIL("Error was not found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testPortMapConnectedPortDirectionsAreValid_data()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testPortMapConnectedPortDirectionsAreValid_data()
{
    QTest::addColumn<QString>("logicalDirection");
    QTest::addColumn<QString>("physicalDirection");
    QTest::addColumn<bool>("physicalAllLogicalDirectionsAllowed");
    QTest::addColumn<bool>("isMirroredInterface");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Logical port direction=in, physical port direction=in is valid")
        << "in" << "in" << false << false << true;
    QTest::newRow("Logical port direction=in, physical port direction=out is not valid")
        << "in" << "out" << false << false << false;
    QTest::newRow("Logical port direction=in, physical port direction=inout is valid")
        << "in" << "inout" << false << false << true;
    QTest::newRow("Logical port direction=in, physical port direction=phantom is valid")
        << "in" << "phantom" << false << false << true;
    QTest::newRow("Logical port direction=in, physical port has allLogicalDirectionsAllowed is valid")
        << "in" << "out" << true << false << true;

    QTest::newRow("Logical port direction=out, physical port direction=in is not valid")
        << "out" << "in" << false << false << false;
    QTest::newRow("Logical port direction=out, physical port direction=out is valid")
        << "out" << "out" << false << false << true;
    QTest::newRow("Logical port direction=out, physical port direction=inout is valid")
        << "out" << "inout" << false << false << true;
    QTest::newRow("Logical port direction=out, physical port direction=phantom is valid")
        << "out" << "phantom" << false << false << true;
    QTest::newRow("Logical port direction=out, physical port has allLogicalDirectionsAllowed is valid")
        << "out" << "in" << true << false << true;

    QTest::newRow("Logical port direction=inout, physical port direction=in is not valid")
        << "inout" << "in" << false << false << false;
    QTest::newRow("Logical port direction=out, physical port direction=out is not valid")
        << "inout" << "out" << false << false << false;
    QTest::newRow("Logical port direction=out, physical port direction=inout is valid")
        << "inout" << "inout" << false << false << true;
    QTest::newRow("Logical port direction=out, physical port direction=phantom is valid")
        << "inout" << "phantom" << false << false << true;
    QTest::newRow("Logical port direction=out, physical port has allLogicalDirectionsAllowed is valid")
        << "inout" << "in" << true << false << true;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testPortMapConnectedPortsHaveValidType()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testPortMapConnectedPortsHaveValidType()
{
    QFETCH(bool, logicalHasWire);
    QFETCH(bool, physicalHasWire);
    QFETCH(bool, isValid);

    QSharedPointer<ConfigurableVLNVReference> abstractionReference (new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "testVendor", "testLibrary", "Everett", "5.1"));

    QSharedPointer<AbstractionDefinition> abstractionDefinition (new AbstractionDefinition());
    abstractionDefinition->setVlnv(*abstractionReference.data());

    QSharedPointer<PortAbstraction> testLogicalAbstraction (new PortAbstraction());
    testLogicalAbstraction->setLogicalName("testLogical");

    if (logicalHasWire)
    {
        QSharedPointer<WireAbstraction> logicalWireAbstraction (new WireAbstraction());
        testLogicalAbstraction->setWire(logicalWireAbstraction);
    }
    else
    {
        QSharedPointer<TransactionalAbstraction> logicalTransactional (new TransactionalAbstraction());
        testLogicalAbstraction->setTransactional(logicalTransactional);
    }

    abstractionDefinition->getLogicalPorts()->append(testLogicalAbstraction);

    LibraryMock* mockLibrary (new LibraryMock(this));
    mockLibrary->addComponent(abstractionDefinition);

    QSharedPointer<PortMap::LogicalPort> testLogical (new PortMap::LogicalPort());
    testLogical->name_ = testLogicalAbstraction->name();

    QSharedPointer<PortMap::PhysicalPort> testPhysical (new PortMap::PhysicalPort());
    testPhysical->name_ = "testPhysicalPort";

    QSharedPointer<Port> newPort (new Port(testPhysical->name_));
    if (!physicalHasWire)
    {
        QSharedPointer<Transactional> physicalTransactional (new Transactional());
        newPort->setTransactional(physicalTransactional);
    }

    QSharedPointer<QList<QSharedPointer<Port> > > componentPorts (new QList<QSharedPointer<Port> > ());
    componentPorts->append(newPort);

    QSharedPointer<PortMap> testPortMap (new PortMap());
    testPortMap->setLogicalPort(testLogical);
    testPortMap->setPhysicalPort(testPhysical);

    QSharedPointer<AbstractionType> testAbstraction(new AbstractionType());
    testAbstraction->setAbstractionRef(abstractionReference);
    testAbstraction->getPortMaps()->append(testPortMap);

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName("testBus");
    testBus->getAbstractionTypes()->append(testAbstraction);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (),
        QSharedPointer<QList<QSharedPointer<View> > > (), componentPorts,
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (),
        mockLibrary);

    QCOMPARE (validator->hasValidAbstractionTypes(testBus), isValid);

    if (!isValid)
    {
        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testBus, "test");

        QString context = QObject::tr("%1 bus interface %2")
            .arg(General::interfaceMode2Str(testBus->getInterfaceMode())).arg(testBus->name());

        QString expectedError = QObject::tr("Connected logical port %1 and physical port %3 do not have the same "
            "port type within %5")
            .arg(testPortMap->getLogicalPort()->name_).arg(testPortMap->getPhysicalPort()->name_).arg(context);

        if (errorIsNotFoundInErrorList(expectedError, errorsFound))
        {
            QFAIL("Error was not found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testPortMapConnectedPortsHaveValidType_data()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testPortMapConnectedPortsHaveValidType_data()
{
    QTest::addColumn<bool>("logicalHasWire");
    QTest::addColumn<bool>("physicalHasWire");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Logical wire port connected to physical wire port is valid") << true << true << true;
    QTest::newRow("Logical wire port connected to physical transactional port is not valid") << true << false <<
        false;
    QTest::newRow("Logical transactional port connected to physical wire port is not valid") << false << true <<
        false;
    QTest::newRow("Logical transactional port connected to physical transactional port is valid") << false <<
        false << true;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testPortMapConnectedPortsHaveSameRange()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testPortMapConnectedPortsHaveSameRange()
{
    QFETCH(QString, logicalRangeLeft);
    QFETCH(QString, logicalRangeRight);
    QFETCH(QString, physicalRangeLeft);
    QFETCH(QString, physicalRangeRight);
    QFETCH(bool, isValid);

    QSharedPointer<ConfigurableVLNVReference> abstractionReference (new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "testVendor", "testLibrary", "Everett", "5.1"));

    QSharedPointer<AbstractionDefinition> abstractionDefinition (new AbstractionDefinition());
    abstractionDefinition->setVlnv(*abstractionReference.data());

    QSharedPointer<WireAbstraction> logicalWireAbstraction (new WireAbstraction());

    QSharedPointer<PortAbstraction> testLogicalAbstraction (new PortAbstraction());
    testLogicalAbstraction->setLogicalName("testLogical");
    testLogicalAbstraction->setWire(logicalWireAbstraction);

    abstractionDefinition->getLogicalPorts()->append(testLogicalAbstraction);

    LibraryMock* mockLibrary (new LibraryMock(this));
    mockLibrary->addComponent(abstractionDefinition);

    QSharedPointer<Range> logicalRange (new Range(logicalRangeLeft, logicalRangeRight));

    QSharedPointer<PortMap::LogicalPort> testLogical (new PortMap::LogicalPort());
    testLogical->name_ = testLogicalAbstraction->name();
    testLogical->range_ = logicalRange;

    QSharedPointer<PartSelect> physicalPart (new PartSelect(physicalRangeLeft, physicalRangeRight));

    QSharedPointer<PortMap::PhysicalPort> testPhysical (new PortMap::PhysicalPort());
    testPhysical->name_ = "testPhysicalPort";
    testPhysical->partSelect_ = physicalPart;

    QSharedPointer<QList<QSharedPointer<Port> > > componentPorts (new QList<QSharedPointer<Port> > ());
    QSharedPointer<Port> newPort (new Port(testPhysical->name_));
    newPort->setLeftBound(physicalRangeLeft);
    newPort->setRightBound(physicalRangeRight);
    componentPorts->append(newPort);

    QSharedPointer<PortMap> testPortMap (new PortMap());
    testPortMap->setLogicalPort(testLogical);
    testPortMap->setPhysicalPort(testPhysical);

    QSharedPointer<AbstractionType> testAbstraction(new AbstractionType());
    testAbstraction->setAbstractionRef(abstractionReference);
    testAbstraction->getPortMaps()->append(testPortMap);

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName("testBus");
    testBus->getAbstractionTypes()->append(testAbstraction);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (),
        QSharedPointer<QList<QSharedPointer<View> > > (), componentPorts,
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (),
        mockLibrary);

    QCOMPARE (validator->hasValidAbstractionTypes(testBus), isValid);

    if (!isValid)
    {
        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testBus, "test");

        QString context = QObject::tr("%1 bus interface %2")
            .arg(General::interfaceMode2Str(testBus->getInterfaceMode())).arg(testBus->name());

        QString expectedError = QObject::tr("Connected logical port %1 and physical port %2 do not have the same "
            "range size within %3")
            .arg(testPortMap->getLogicalPort()->name_).arg(testPortMap->getPhysicalPort()->name_).arg(context);

        if (errorIsNotFoundInErrorList(expectedError, errorsFound))
        {
            QFAIL("Error was not found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testPortMapConnectedPortsHaveSameRange()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testPortMapConnectedPortsHaveSameRange_data()
{
    QTest::addColumn<QString>("logicalRangeLeft");
    QTest::addColumn<QString>("logicalRangeRight");
    QTest::addColumn<QString>("physicalRangeLeft");
    QTest::addColumn<QString>("physicalRangeRight");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Connected ports without range is valid") << "" << "" << "" << "" << true;
    QTest::newRow("Connected ports with the same range values is valid") << "0" << "10" << "0" << "10" << true;
    QTest::newRow("Connected ports with the same range size is valid") << "0" << "5+1" << "18-8" << "10+3*2" << true;
    QTest::newRow("Connected ports without same range size is not valid") << "" << "" << "4" << "3" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidMasterInterface()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidMasterInterface()
{
    QFETCH(QString, addressSpaceRef);
    QFETCH(QString, baseAddress);
    QFETCH(QString, masterPresence);
    QFETCH(QString, spacePresence);
    QFETCH(bool, createSpace);
    QFETCH(bool, isValid);

    QSharedPointer<MasterInterface> testMaster (new MasterInterface());
    testMaster->setAddressSpaceRef(addressSpaceRef);
    testMaster->setBaseAddress(baseAddress);
    testMaster->setIsPresent(masterPresence);

    QSharedPointer<QList<QSharedPointer<AddressSpace> > > componentSpaces
        (new QList<QSharedPointer<AddressSpace> > ());
    if (createSpace)
    {
        QSharedPointer<AddressSpace> space (new AddressSpace(addressSpaceRef));
        space->setIsPresent(spacePresence);
        if (addressSpaceRef.isEmpty())
        {
            space->setName("Champloo");
        }
        componentSpaces->append(space);
    }

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setMaster(testMaster);
    testBus->setInterfaceMode(General::MASTER);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > >(),
        QSharedPointer<QList<QSharedPointer<View> > > (), QSharedPointer<QList<QSharedPointer<Port> > > (),
        componentSpaces, QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (),
        0);

    QCOMPARE(validator->hasValidInterfaceMode(testBus), isValid);

    if (!isValid)
    {
        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testBus, "test");

        QString expectedError = QObject::tr("Invalid address space reference set for master bus interface %1")
            .arg(testBus->name());

        if (addressSpaceRef.isEmpty())
        {
            expectedError = QObject::tr("Invalid address space reference set for master bus interface %1")
                .arg(testBus->name());
        }
        else if (parser->parseExpression(baseAddress).toInt() < 0)
        {
            expectedError = QObject::tr("Invalid base address set for master bus interface %1")
                .arg(testBus->name());
        }
        else if (!createSpace)
        {
            expectedError = QObject::tr("Could not find address space %1 referenced by the master bus interface %2")
                .arg(testMaster->getAddressSpaceRef()).arg(testBus->name());
        }
        else if (parser->parseExpression(masterPresence).toInt() == 1 &&
            parser->parseExpression(spacePresence).toInt() == 0)
        {
            expectedError = QObject::tr("Cannot refer to non-present address space %1 in master bus interface %2")
                .arg(addressSpaceRef).arg(testBus->name());
        }

        if (errorIsNotFoundInErrorList(expectedError, errorsFound))
        {
            QFAIL("Error was not found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidMasterInterface_data()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidMasterInterface_data()
{
    QTest::addColumn<QString>("addressSpaceRef");
    QTest::addColumn<QString>("baseAddress");
    QTest::addColumn<QString>("masterPresence");
    QTest::addColumn<QString>("spacePresence");
    QTest::addColumn<bool>("createSpace");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Master referencing address space is valid") << "space" << "" << "" << "" << true << true;
    QTest::newRow("Empty master interface is valid") << "" << "" << ""  << "" << false << true;
    QTest::newRow("Master referencing non-existing space is not valid") << "space" << "" << "" << "" << false <<
        false;
    QTest::newRow("Master not containing space reference is valid") << "" << "" << "" << "" << true << true;
    QTest::newRow("Master containing base address is valid") << "space" << "4+4" << "" << "" << true << true;
    QTest::newRow("Master containing negative base address is not valid") << "space" << "-2" << "" << "" << true <<
        false;

    QTest::newRow("Present master referring present address space is valid") << "space" << "" << "1" << "1" <<
        true << true;
    QTest::newRow("Non-present master referring present address space is valid") << "space" << "" << "0" << "1" <<
        true << true;
    QTest::newRow("Non-present master referring non-present address space is valid") << "space" << "" << "0" <<
        "0" << true << true;
    QTest::newRow("Present master referring non-present address space is not valid") << "space" << "" << "1" <<
        "0" << true << false;

    QTest::newRow("Master containing long base address is valid") <<
        "space" << "4000000000" << "" << "" << true << true;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidSlaveInterface()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidSlaveInterface()
{
    QFETCH(QString, memoryMapRef);
    QFETCH(QString, bridgeMasterRef);
    QFETCH(QString, bridgeIsPresent);
    QFETCH(QString, fileSetRef);
    QFETCH(QString, interfacePresence);
    QFETCH(QString, memoryMapPresence);
    QFETCH(bool, createMemoryMap);
    QFETCH(bool, createMaster);
    QFETCH(bool, createFileSet);
    QFETCH(bool, isValid);

    QSharedPointer<SlaveInterface> testSlave (new SlaveInterface());
    testSlave->setMemoryMapRef(memoryMapRef);

    QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps (new QList<QSharedPointer<MemoryMap> > ());
    if (createMemoryMap)
    {
        QSharedPointer<MemoryMap> testMap (new MemoryMap(memoryMapRef));
        testMap->setIsPresent(memoryMapPresence);
        if (memoryMapRef.isEmpty())
        {
            testMap->setName(QLatin1String("Champloo"));
        }
        memoryMaps->append(testMap);
    }

    if (!bridgeMasterRef.isEmpty())
    {
        QSharedPointer<SlaveInterface::Bridge> testBridge (new SlaveInterface::Bridge());
        testBridge->masterRef_ = bridgeMasterRef;
        testBridge->isPresent_ = bridgeIsPresent;

        testSlave->getBridges()->append(testBridge);
    }

    QSharedPointer<QList<QSharedPointer<BusInterface> > > busInterfaces(new QList<QSharedPointer<BusInterface> > ());
    if (createMaster)
    {
        QSharedPointer<BusInterface> masterBus (new BusInterface());
        masterBus->setName(bridgeMasterRef);
        masterBus->setInterfaceMode(General::MASTER);
        if (bridgeMasterRef.isEmpty())
        {
            masterBus->setName("Samurai");
        }
        busInterfaces->append(masterBus);
    }

    if (!fileSetRef.isEmpty())
    {
        QSharedPointer<SlaveInterface::FileSetRefGroup> testGroup (new SlaveInterface::FileSetRefGroup());
        testGroup->group_ = fileSetRef + " group";
        testGroup->fileSetRefs_.append(fileSetRef);
        testSlave->getFileSetRefGroup()->append(testGroup);
    }

    QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets (new QList<QSharedPointer<FileSet> > ());
    if (createFileSet)
    {
        QSharedPointer<FileSet> testSet (new FileSet(fileSetRef));
        if (fileSetRef.isEmpty())
        {
            testSet->setName("fileSetReplacement");
        }
        fileSets->append(testSet);
    }

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName("testBus");
    testBus->setSlave(testSlave);
    testBus->setIsPresent(interfacePresence);
    busInterfaces->append(testBus);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (),
        QSharedPointer<QList<QSharedPointer<View> > > (), QSharedPointer<QList<QSharedPointer<Port> > > (),
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > (), memoryMaps, busInterfaces, fileSets,
        QSharedPointer<QList<QSharedPointer<RemapState> > > (),
        0);

    QCOMPARE(validator->hasValidInterfaceMode(testBus), isValid);

    if (!isValid)
    {
        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testBus, "test");

        QString expectedError = QObject::tr("Invalid slave interface set for bus interface %1")
            .arg(testBus->name());

        if (!testSlave->getMemoryMapRef().isEmpty() && !testSlave->getBridges()->isEmpty())
        {
            expectedError = QObject::tr("Both a memory map reference and transparent bridges are contained "
                "within slave bus interface %1").arg(testBus->name());
        }
        else if ((!memoryMapRef.isEmpty() && !createMemoryMap) || 
            (parser->parseExpression(interfacePresence).toInt() == 1 &&
            parser->parseExpression(memoryMapPresence).toInt() == 0))
        {
            expectedError = QObject::tr("Memory map %1 referenced by the slave bus interface %2 was not found")
                .arg(testSlave->getMemoryMapRef()).arg(testBus->name());
        }
        else if (!bridgeMasterRef.isEmpty() && !createMaster)
        {
            expectedError = QObject::tr("Master bus interface %1 referenced by the slave bus interface %2 was "
                "not found").arg(bridgeMasterRef).arg(testBus->name());
        }
        else if (parser->parseExpression(bridgeIsPresent).toInt() < 0 ||
            parser->parseExpression(bridgeIsPresent).toInt() > 1)
        {
            expectedError = QObject::tr("Transparent bridge within the slave bus interface %1 has invalid "
                "isPresent").arg(testBus->name());
        }
        else if (!fileSetRef.isEmpty() && !createFileSet)
        {
            expectedError = QObject::tr("Invalid file set %1 referenced within group %2 of slave bus interface %3")
                .arg(fileSetRef).arg(testSlave->getFileSetRefGroup()->first()->group_).arg(testBus->name());
        }

        if (errorIsNotFoundInErrorList(expectedError, errorsFound))
        {
            QFAIL("Error was not found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidSlaveInterface_data()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidSlaveInterface_data()
{
    QTest::addColumn<QString>("memoryMapRef");
    QTest::addColumn<QString>("bridgeMasterRef");
    QTest::addColumn<QString>("bridgeIsPresent");
    QTest::addColumn<QString>("fileSetRef");
    QTest::addColumn<QString>("interfacePresence");
    QTest::addColumn<QString>("memoryMapPresence");
    QTest::addColumn<bool>("createMemoryMap");
    QTest::addColumn<bool>("createMaster");
    QTest::addColumn<bool>("createFileSet");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Empty slave is valid") << "" << "" << "" << "" << "" << "" << true << true << true << true;
    QTest::newRow("Slave referencing memory map is valid")
        << "testMap" << "" << "" << "" << "" << "" << true << false << false << true;
    QTest::newRow("Slave referencing non-existing memory map is not valid")
        << "testMap" << "" << "" << "" << "" << "" << false << false << false << false;
    QTest::newRow("Non-present slave referencing present memory map is valid")
        << "testMap" << "" << "" << "" << "0" << "1" << true << false << false << true;
    QTest::newRow("Non-present slave referencing non-present memory map is valid")
        << "testMap" << "" << "" << "" << "0" << "0" << true << false << false << true;
    QTest::newRow("Present slave referencing present memory map is valid")
        << "testMap" << "" << "" << "" << "1" << "1" << true << false << false << true;
    QTest::newRow("Present slave referencing non-present memory map is not valid")
        << "testMap" << "" << "" << "" << "1" << "0" << true << false << false << false;

    QTest::newRow("Slave bridge referencing master is valid")
        << "" << "testMaster" << "" << "" << "" << "" << false << true << false << true;
    QTest::newRow("Slave bridge referencing non-existing master is not valid")
        << "" << "testMaster" << "" << "" << "" << "" << false << false << false << false;
    QTest::newRow("Slave bridge with bit isPresent is valid")
        << "" << "testMaster" << "1" << "" << "" << "" << false << true << false << true;
    QTest::newRow("Slave bridge with non-bit isPresent is not valid")
        << "" << "testMaster" << "18" << "" << "" << "" << false << true << false << false;

    QTest::newRow("Slave with both memory map ref and bridge is not valid")
        << "testMap" << "testMaster" << "" << "" << "" << "" << true << true << false << false;
    QTest::newRow("Slave with file set ref group is valid")
        << "" << "" << "" << "testFileGroup" << "" << "" << false << false << true << true;
    QTest::newRow("Slave with file set ref group referencing non-existing file set is not valid")
        << "" << "" << "" << "testFileGroup" << "" << "" << false << false << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidSystemInterface()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidSystemInterface()
{
    QFETCH(QString, systemGroup);
    QFETCH(bool, hasGroupName);
    QFETCH(bool, isValid);

    ConfigurableVLNVReference testType(VLNV::BUSDEFINITION, "testVendor", "testLibrary", "busDefinition", "1.1");

    QSharedPointer<BusDefinition> testDefinition (new BusDefinition());
    testDefinition->setVlnv(testType);

    if (hasGroupName)
    {
        QStringList systemGroupNames;
        systemGroupNames.append(systemGroup);
        testDefinition->setSystemGroupNames(systemGroupNames);
    }

    LibraryMock* mockLibrary (new LibraryMock(this));
    mockLibrary->addComponent(testDefinition);

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName("testBus");
    testBus->setInterfaceMode(General::SYSTEM);
    testBus->setSystem(systemGroup);
    testBus->setBusType(testType);
    
    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (),
        QSharedPointer<QList<QSharedPointer<View> > > (),
        QSharedPointer<QList<QSharedPointer<Port> > > (),
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (),
        mockLibrary);

    QCOMPARE(validator->hasValidInterfaceMode(testBus), isValid);

    if (!isValid)
    {
        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testBus, "test");

        QString expectedError = QObject::tr("No system group given for system bus interface %1")
            .arg(testBus->name());

        if (!hasGroupName)
        {
            expectedError = QObject::tr("Could not find system group %1 set for system bus interface %2 with bus "
                "definition %3.").arg(systemGroup).arg(testBus->name()).arg(testType.toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, errorsFound))
        {
            QFAIL("Error was not found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidSystemInterface_data()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidSystemInterface_data()
{
    QTest::addColumn<QString>("systemGroup");
    QTest::addColumn<bool>("hasGroupName");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("System with group referencing existing group name is valid") << "systemGroup" << true << true;
    QTest::newRow("System without group is not valid") << "" << true << false;
    QTest::newRow("System with group referencing non-existing group is not valid") << "systemGroup" << false <<
        false;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidMirroredSlaveInterface()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidMirroredSlaveInterface()
{
    QFETCH(QString, remapAddress);
    QFETCH(QString, remapStateName);
    QFETCH(QString, range);
    QFETCH(bool, createState);
    QFETCH(bool, isValid);

    QSharedPointer<MirroredSlaveInterface> testMirrorSlave (new MirroredSlaveInterface());
    testMirrorSlave->setRange(range);

    if (!remapAddress.isEmpty())
    {
        QSharedPointer<MirroredSlaveInterface::RemapAddress> slaveRemapAddress
            (new MirroredSlaveInterface::RemapAddress(remapAddress));
        slaveRemapAddress->state_ = remapStateName;
        testMirrorSlave->getRemapAddresses()->append(slaveRemapAddress);
    }

    QSharedPointer<QList<QSharedPointer<RemapState> > > remapStates (new QList<QSharedPointer<RemapState> > ());
    if (createState)
    {
        QSharedPointer<RemapState> newState (new RemapState(remapStateName));
        if (remapStateName.isEmpty())
        {
            newState->setName("replacedState");
        }
        remapStates->append(newState);
    }

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName("testBus");
    testBus->setMirroredSlave(testMirrorSlave);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (),
        QSharedPointer<QList<QSharedPointer<View> > > (), QSharedPointer<QList<QSharedPointer<Port> > > (),
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        remapStates,
        0);

    QCOMPARE(validator->hasValidInterfaceMode(testBus), isValid);

    if (!isValid)
    {
        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testBus, "test");

        QString expectedError = QObject::tr("Invalid mirroredSlave interface set for bus interface %1")
            .arg(testBus->name());

        if (parser->parseExpression(remapAddress).toInt() <= 0)
        {
            expectedError = QObject::tr("Invalid remap address set for mirroredSlave bus interface %1")
                .arg(testBus->name());
        }
        else if (parser->parseExpression(range).toInt() < 1)
        {
            expectedError = QObject::tr("Invalid range set for mirroredSlave bus interface %1")
                .arg(testBus->name());
        }
        else if (!remapStateName.isEmpty() && !createState)
        {
            expectedError = QObject::tr("Invalid remap state set for mirroredSlave bus interface %1")
                .arg(testBus->name());
        }

        if (errorIsNotFoundInErrorList(expectedError, errorsFound))
        {
            QFAIL("Error was not found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidMirroredSlaveInterface_data()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidMirroredSlaveInterface_data()
{
    QTest::addColumn<QString>("remapAddress");
    QTest::addColumn<QString>("range");
    QTest::addColumn<QString>("remapStateName");
    QTest::addColumn<bool>("createState");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Mirrored slave without elements is valid") << "" << "" << "" << false << true;
    QTest::newRow("Mirrored slave with remap address and range is valid") << "1+2" << "12" << "" << false << true;

    QTest::newRow("Mirrored slave with long remap address and long range is valid") <<
        "4000000000" << "4000000000" << "" << false << true;

    QTest::newRow("Mirrored slave with negative remap address is not valid") << "10-20" << "12" << "" << false <<
        false;
    QTest::newRow("Mirrored slave with non-positive range is not valid") << "1+2" << "0" << "" << false << false;
    QTest::newRow("Mirrored slave without remap address is not valid") << "" << "12" << "" << false << false;
    QTest::newRow("Mirrored slave without range is not valid") << "1+2" << "" << "" << false << false;

    QTest::newRow("Remap state referencing existing state is valid") << "1+2" << "12" << "testState" << true <<
        true;
    QTest::newRow("Remap state referencing non-existing state is not valid") << "1+2" << "12" << "testState" <<
        false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidMonitorInterface()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidMonitorInterface()
{
    QFETCH(QString, interfaceMode);
    QFETCH(QString, systemGroup);
    QFETCH(bool, createGroup);
    QFETCH(bool, isValid);

    ConfigurableVLNVReference testType(VLNV::BUSDEFINITION, "testVendor", "testLibrary", "busDefinition", "1.1");

    QSharedPointer<BusDefinition> testDefinition (new BusDefinition());
    testDefinition->setVlnv(testType);

    if (createGroup)
    {
        QStringList systemGroupNames;
        if (systemGroup.isEmpty())
        {
            systemGroupNames.append("Champloo");
        }
        else
        {
            systemGroupNames.append(systemGroup);
        }
        testDefinition->setSystemGroupNames(systemGroupNames);
    }

    LibraryMock* mockLibrary (new LibraryMock(this));
    mockLibrary->addComponent(testDefinition);

    QSharedPointer<BusInterface::MonitorInterface> testMonitor (new BusInterface::MonitorInterface());
    testMonitor->interfaceMode_ = General::str2Interfacemode(interfaceMode, General::INTERFACE_MODE_COUNT);
    testMonitor->group_ = systemGroup;

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName("testBus");
    testBus->setMonitor(testMonitor);
    testBus->setBusType(testType);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (),
        QSharedPointer<QList<QSharedPointer<View> > > (),
        QSharedPointer<QList<QSharedPointer<Port> > > (),
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (),
        mockLibrary);

    QCOMPARE(validator->hasValidInterfaceMode(testBus), isValid);

    if (!isValid)
    {
        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testBus, "test");

        QString expectedError = QObject::tr("Invalid interface mode set for monitor bus interface %1")
            .arg(testBus->name());

        if (testMonitor->interfaceMode_ != General::SYSTEM &&
            testMonitor->interfaceMode_ != General::MIRROREDSYSTEM && !systemGroup.isEmpty())
        {
            expectedError = QObject::tr("Group cannot be set for monitor bus interface %1 with interface mode "
                "master, slave, mirrored master or mirrored slave").arg(testBus->name());
        }
        else if (!systemGroup.isEmpty() && !createGroup)
        {
            expectedError = QObject::tr("Could not find system group %1 set for monitor bus interface %2 with bus "
                "definition %3.").arg(systemGroup).arg(testBus->name()).arg(testType.toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, errorsFound))
        {
            QFAIL("Error was not found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidMonitorInterface_data()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidMonitorInterface_data()
{
    QTest::addColumn<QString>("interfaceMode");
    QTest::addColumn<QString>("systemGroup");
    QTest::addColumn<bool>("createGroup");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Monitor with interface master is valid") << "master" << "" << false << true;
    QTest::newRow("Monitor with interface master containing a group is not valid") << "master" << "group" <<
        false <<false;

    QTest::newRow("Monitor with interface system containing group referencing existing group is valid") <<
        "system" << "group" << true << true;
    QTest::newRow("Monitor with interface system containing group referencing non-existing group is not valid") <<
        "system" << "group" << false << false;
    
    QTest::newRow("Monitor with interface undefined is not valid") << "undefined" << "" << false << false;
    QTest::newRow("Monitor with interface monitor is not valid") << "monitor" << "" << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidBitsInLau()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidBitsInLau()
{
    QFETCH(QString, bitsInLAU);
    QFETCH(bool, isValid);

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName("testBus");
    testBus->setBitsInLau(bitsInLAU);

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (),
        QSharedPointer<QList<QSharedPointer<View> > > (),
        QSharedPointer<QList<QSharedPointer<Port> > > (),
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (),
        0);

    QCOMPARE(validator->hasValidBitsInLAU(testBus), isValid);

    if (!isValid)
    {
        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testBus, "test");

        QString expectedError = QObject::tr("Invalid bits in LAU set for bus interface %1 within %2")
            .arg(testBus->name()).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, errorsFound))
        {
            QFAIL("Error was not found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidBitsInLau_data()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidBitsInLau_data()
{
    QTest::addColumn<QString>("bitsInLAU");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Empty bits in LAU is valid") << "" << true;
    QTest::newRow("Bits in LAU 1 is valid") << "1" << true;
    QTest::newRow("Bits in LAU 14*12 is valid") << "14*12" << true;
    QTest::newRow("Bits in LAU -14 is invalid") << "-14" << false;
    QTest::newRow("Real number bits in LAU 0.12 is invalid") << "0.12" << false;
    QTest::newRow("Text as bits in LAU is invalid") << "test" << false;
    QTest::newRow("String as bits in LAU is invalid") << "\"test\"" << false;

    QTest::newRow("Long bits in LAU value is valid") << "40000000000" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidBitSteeringForInterfaceMode()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidBitSteeringForInterfaceMode()
{
    QFETCH(QString, interfaceMode);
    QFETCH(bool, bitSteeringOk);
    QFETCH(bool, isValid);

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName("testBus");
    testBus->setInterfaceMode(General::str2Interfacemode(interfaceMode, General::INTERFACE_MODE_COUNT));

    if (bitSteeringOk)
    {
        testBus->setBitSteering(BusInterface::BITSTEERING_ON);
    }

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (),
        QSharedPointer<QList<QSharedPointer<View> > > (),
        QSharedPointer<QList<QSharedPointer<Port> > > (),
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (),
        0);

    QCOMPARE(validator->hasValidBitSteering(testBus), isValid);

    if (!isValid)
    {
        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testBus, "test");

        QString expectedError = QObject::tr("Bit steering value is not allowed in %1 bus interface %2 within %3")
            .arg(interfaceMode).arg(testBus->name()).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, errorsFound))
        {
            QFAIL("Error was not found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidBitSteeringForInterfaceMode_data()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidBitSteeringForInterfaceMode_data()
{
    QTest::addColumn<QString>("interfaceMode");
    QTest::addColumn<bool>("bitSteeringOk");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Master interface allows bit steering value") << "master" << true << true;
    QTest::newRow("Master interface allows unspecified bit steering value") << "master" << false << true;
    QTest::newRow("Slave interface allows bit steering value") << "slave" << true << true;
    QTest::newRow("Slave interface allows unspecified bit steering value") << "slave" << false << true;
    QTest::newRow("System interface does not allow bit steering value") << "system" << true << false;
    QTest::newRow("System interface allows unspecified bit steering value") << "system" << false << true;
    QTest::newRow("Mirrored master interface does not allow bit steering value") << "mirroredMaster" << true <<
        false;
    QTest::newRow("Mirrored master interface allows unspecified bit steering value") << "mirroredMaster" <<
        false << true;
    QTest::newRow("Mirrored slave interface allows bit steering value") << "mirroredSlave" << true << true;
    QTest::newRow("Mirrored slave interface allows unspecified bit steering value") << "mirroredSlave" << false <<
        true;
    QTest::newRow("Mirrored system interface does not allow bit steering value") << "mirroredSystem" << true <<
        false;
    QTest::newRow("Mirrored system interface allows unspecified bit steering value") << "mirroredSystem" <<
        false << true;
    QTest::newRow("Monitor interface allows bit steering value") << "monitor" << true << true;
    QTest::newRow("Monitor interface allows unspecified bit steering value") << "monitor" << false << true;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::testHasValidParameters()
//-----------------------------------------------------------------------------
void tst_BusInterfaceValidator::testHasValidParameters()
{
    QSharedPointer<Parameter> testParameter (new Parameter());
    testParameter->setValueId("Sanger");
    testParameter->setName("Daizengar");
    testParameter->setValue("");
    testParameter->setType("int");

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName("testBus");
    testBus->getParameters()->append(testParameter);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<BusInterfaceValidator> validator = createBusInterfaceValidator(parser,
        QSharedPointer<QList<QSharedPointer<Choice> > > (),
        QSharedPointer<QList<QSharedPointer<View> > > (), QSharedPointer<QList<QSharedPointer<Port> > > (),
        QSharedPointer<QList<QSharedPointer<AddressSpace> > > (),
        QSharedPointer<QList<QSharedPointer<MemoryMap> > > (),
        QSharedPointer<QList<QSharedPointer<BusInterface> > > (),
        QSharedPointer<QList<QSharedPointer<FileSet> > > (),
        QSharedPointer<QList<QSharedPointer<RemapState> > > (),
        0);

    QCOMPARE(validator->hasValidParameters(testBus), false);

    QVector<QString> errorsFound;
    validator->findErrorsIn(errorsFound, testBus, "test");

    QString expectedError = QObject::tr("No value specified for %1 %2 within bus interface %3").
        arg(testParameter->elementName()).arg(testParameter->name()).arg(testBus->name());

    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }

    testParameter->setValue("20");
    QCOMPARE(validator->hasValidParameters(testBus), true);

    QSharedPointer<Parameter> otherParameter (new Parameter(*testParameter.data()));
    otherParameter->setValue("2");
    testBus->getParameters()->append(otherParameter);

    QCOMPARE(validator->hasValidParameters(testBus), false);

    errorsFound.clear();
    validator->findErrorsIn(errorsFound, testBus, "test");
    expectedError = QObject::tr("Parameter name %1 is not unique within bus interface %2.")
        .arg(otherParameter->name()).arg(testBus->name());
    if (errorIsNotFoundInErrorList(expectedError, errorsFound))
    {
        QFAIL("Error was not found");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfaceValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_BusInterfaceValidator::errorIsNotFoundInErrorList(QString const& expectedError,
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
// Function: tst_BusInterfaceValidator::createBusInterfaceValidator()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterfaceValidator> tst_BusInterfaceValidator::createBusInterfaceValidator(
    QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<QList<QSharedPointer<Choice> > > choices,
    QSharedPointer<QList<QSharedPointer<View> > > views,
    QSharedPointer<QList<QSharedPointer<Port> > > ports,
    QSharedPointer<QList<QSharedPointer<AddressSpace> > > addressSpaces,
    QSharedPointer<QList<QSharedPointer<MemoryMap> > > memoryMaps,
    QSharedPointer<QList<QSharedPointer<BusInterface> > > busInterfaces,
    QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets,
    QSharedPointer<QList<QSharedPointer<RemapState> > > remapStates,
    LibraryInterface* libraryHandler)
{
    QSharedPointer<ParameterValidator2014> parameterValidator(
        new ParameterValidator2014(expressionParser, choices));

    QSharedPointer<PortMapValidator> newPortMapValidator (new PortMapValidator(
        expressionParser, ports, libraryHandler));

    QSharedPointer<BusInterfaceValidator> newValidator(new BusInterfaceValidator(expressionParser, choices, views,
        ports, addressSpaces, memoryMaps, busInterfaces, fileSets, remapStates, newPortMapValidator,
        parameterValidator, libraryHandler));

    return newValidator;
}

QTEST_APPLESS_MAIN(tst_BusInterfaceValidator)

#include "tst_BusInterfaceValidator.moc"
