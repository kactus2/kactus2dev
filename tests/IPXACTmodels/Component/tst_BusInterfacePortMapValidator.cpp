//-----------------------------------------------------------------------------
// File: tst_BusInterfacePortMapValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.05.2019
//
// Description:
// Unit test for class BusInterfacePortMapValidator.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>

#include <IPXACTmodels/AbstractionDefinition/TransactionalPort.h>
#include <IPXACTmodels/AbstractionDefinition/WirePort.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>

#include <IPXACTmodels/common/ConfigurableVLNVReference.h>

#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/AbstractionType.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/Component/validators/PortMapValidator.h>

#include <KactusAPI/include/SystemVerilogExpressionParser.h>

#include <tests/MockObjects/LibraryMock.h>

#include <QtTest>

class tst_BusInterfacePortMapValidator : public QObject
{
    Q_OBJECT

public:
    tst_BusInterfacePortMapValidator();

private slots:

    void testPortMapLogicalPortIsValid();
    void testPortMapLogicalPortIsValid_data();

    void testPortMapPhysicalPortIsValid();
    void testPortMapPhysicalPortIsValid_data();

    void testPortMapConnectedPortDirectionsAreValid();
    void testPortMapConnectedPortDirectionsAreValid_data();
    void testPortMapConnectedPortInitiativesAreValid();
    void testPortMapConnectedPortInitiativesAreValid_data();

    void testPortMapConnectedPortsHaveValidType();
    void testPortMapConnectedPortsHaveValidType_data();
    void testPortMapConnectedPortsHaveSameRange();
    void testPortMapConnectedPortsHaveSameRange_data();

private:

    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);

    QSharedPointer<Port> createWirePort(QString const& portName);

    QSharedPointer<Port> createTransactionalPort(QString const& portName);

    QSharedPointer<PortMapValidator> createPortMapValidator(QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<QList<QSharedPointer<Port> > > ports, LibraryInterface* libraryHandler);
};

tst_BusInterfacePortMapValidator::tst_BusInterfacePortMapValidator()
{
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfacePortMapValidator::testPortMapLogicalPortIsValid()
//-----------------------------------------------------------------------------
void tst_BusInterfacePortMapValidator::testPortMapLogicalPortIsValid()
{
    QFETCH(QString, logicalPortName);
    QFETCH(QString, rangeLeft);
    QFETCH(QString, rangeRight);
    QFETCH(QString, abstractionWidth);
    QFETCH(bool, createPort);
    QFETCH(bool, isWire);
    QFETCH(bool, isValid);

    QSharedPointer<ConfigurableVLNVReference> abstractionReference (new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "testVendor", "testLibrary", "Everett", "5.1"));

    QSharedPointer<AbstractionDefinition> abstractionDefinition(new AbstractionDefinition(*abstractionReference,
        Document::Revision::Std14));

    if (createPort)
    {
        QSharedPointer<PortAbstraction> testLogicalAbstraction(new PortAbstraction());

        if (isWire)
        {
            QSharedPointer<WirePort> masterWire(new WirePort());
            masterWire->setDirection(DirectionTypes::IN);
            masterWire->setWidth(abstractionWidth);

            QSharedPointer<WireAbstraction> logicalWireAbstraction(new WireAbstraction());
            logicalWireAbstraction->setMasterPort(masterWire);

            testLogicalAbstraction->setWire(logicalWireAbstraction);
        }
        else
        {
            QSharedPointer<TransactionalPort> masterTransactional(new TransactionalPort());
            masterTransactional->setInitiative("requires");
            masterTransactional->setBusWidth(abstractionWidth);

            QSharedPointer<TransactionalAbstraction> logicalTransactionalAbstraction(
                new TransactionalAbstraction());
            logicalTransactionalAbstraction->setMasterPort(masterTransactional);

            testLogicalAbstraction->setTransactional(logicalTransactionalAbstraction);
        }

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

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

    QSharedPointer<Port> newPort;
    if (isWire)
    {
        newPort = createWirePort(testPhysical->name_);
        newPort->setLeftBound(rangeLeft);
        newPort->setRightBound(rangeRight);
    }
    else
    {
        newPort = createTransactionalPort(testPhysical->name_);
        
        // Set transactional port width to smallest needed for tests.
        quint64 logicalWidth = qMax(parser->parseExpression(rangeLeft).toULongLong(),
            parser->parseExpression(rangeRight).toULongLong()) + 1;

        newPort->getTransactional()->setBusWidth(QString::number(logicalWidth));
    }

    QSharedPointer<QList<QSharedPointer<Port> > > componentPorts (new QList<QSharedPointer<Port> > ());
    componentPorts->append(newPort);

    QSharedPointer<PortMapValidator> validator = createPortMapValidator(parser, componentPorts, mockLibrary);
    validator->busInterfaceChanged(abstractionReference, General::MASTER, QString(""));

    QCOMPARE(validator->hasValidLogicalPort(testPortMap->getLogicalPort()), isValid);

    if (!isValid)
    {
        quint64 logicalWidth =
            parser->parseExpression(rangeRight).toULongLong() - parser->parseExpression(rangeLeft).toULongLong();
        quint64 abstractionWidthValue = parser->parseExpression(abstractionWidth).toULongLong();

        QString context = QLatin1String("test");

        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testPortMap, context);

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
        else if (parser->parseExpression(rangeLeft).toInt() < 0)
        {
            expectedError = QObject::tr("Invalid value given for logical left in logical port %1 mapped within %2")
                    .arg(logicalPortName).arg(context);
        }
        else if (parser->parseExpression(rangeRight).toInt() < 0)
        {
            expectedError =
                QObject::tr("Invalid value given for logical right in logical port %1 mapped within %2")
                .arg(logicalPortName).arg(context);
        }
        else if (rangeLeft.isEmpty())
        {
            expectedError = QObject::tr("Empty left value in logical port %1 mapped within %2")
                .arg(logicalPortName).arg(context);
        }
        else if (rangeRight.isEmpty())
        {
            expectedError = QObject::tr("Empty right value in logical port %1 mapped within %2")
                .arg(logicalPortName).arg(context);
        }
        else if (logicalWidth > abstractionWidthValue)
        {
            expectedError = QObject::tr(
                "Range is not within the referenced port width in mapped logical port %1 in %2")
                .arg(logicalPortName).arg(context);
        }

        if (errorIsNotFoundInErrorList(expectedError, errorsFound))
        {
            QFAIL("Error was not found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfacePortMapValidator::testPortMapLogicalPortIsValid_data()
//-----------------------------------------------------------------------------
void tst_BusInterfacePortMapValidator::testPortMapLogicalPortIsValid_data()
{
    QTest::addColumn<QString>("logicalPortName");
    QTest::addColumn<QString>("rangeLeft");
    QTest::addColumn<QString>("rangeRight");
    QTest::addColumn<QString>("abstractionWidth");
    QTest::addColumn<bool>("createPort");
    QTest::addColumn<bool>("isWire");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Wire: Logical port with long range is valid") <<
        "logicalPort" << "4000000000" << "4000000001" << "5000000000" << true << true << true;

    QTest::newRow("Wire: Logical port without range is valid") <<
        "logicalPort" << "" << "" << "" << true << true << true;
    QTest::newRow("Wire: Logical port with range is valid") <<
        "logicalPort" << "4+4" << "12" << "" << true << true << true;
    QTest::newRow("Wire: Logical port with either range as 0 is valid") <<
        "logicalPort" << "0" << "0" << "" << true << true << true;
    QTest::newRow("Wire: Logical port with negative range left value is not valid") <<
        "logicalPort" << "-1" << "1" << "" << true << true << false;
    QTest::newRow("Wire: Logical port with negative range right value is not valid") <<
        "logicalPort" << "1" << "-1" << "" << true << true << false;
    QTest::newRow("Wire: Logical port without left range value is not valid") <<
        "logicalPort" << "" << "12" << "" << true << true << false;
    QTest::newRow("Wire: Logical port without right range value is not valid") <<
        "logicalPort" << "12" << "" << "" << true << true << false;
    QTest::newRow("Wire: Referencing non-existing logical port is not valid") <<
        "logicalPort" << "" << "" << "" << false << true << false;
    QTest::newRow("Wire: Empty logical port reference is not valid") << "" << "" << "" << "" << true << true << false;

    QTest::newRow("Wire: Range within abstraction width is valid") <<
        "illogical" << "0" << "5" << "10" << true << true << true;
    QTest::newRow("Wire: Right range not within abstraction width is not valid") <<
        "illogical" << "0" << "20" << "10" << true << true << false;
    QTest::newRow("Wire: Left range not within abstraction width is not valid") <<
        "illogical" << "15" << "8" << "10" << true << true << false;

    QTest::newRow("Transactional: Logical port with long range is valid") <<
        "logicalPort" << "4000000000" << "4000000001" << "5000000000" << true << false << true;

    QTest::newRow("Transactional: Logical port without range is valid") <<
        "logicalPort" << "" << "" << "" << true << false << true;
    QTest::newRow("Transactional: Logical port with range is valid") <<
        "logicalPort" << "4+4" << "12" << "" << true << false << true;
    QTest::newRow("Transactional: Logical port with either range as 0 is valid") <<
        "logicalPort" << "0" << "0" << "" << true << false << true;
    QTest::newRow("Transactional: Logical port with negative range left value is not valid") <<
        "logicalPort" << "-1" << "1" << "" << true << false << false;
    QTest::newRow("Transactional: Logical port with negative range right value is not valid") <<
        "logicalPort" << "1" << "-1" << "" << true << false << false;
    QTest::newRow("Transactional: Logical port without left range value is not valid") <<
        "logicalPort" << "" << "12" << "" << true << false << false;
    QTest::newRow("Transactional: Logical port without right range value is not valid") <<
        "logicalPort" << "12" << "" << "" << true << false << false;
    QTest::newRow("Transactional: Referencing non-existing logical port is not valid") <<
        "logicalPort" << "" << "" << "" << false << false << false;
    QTest::newRow("Transactional: Empty logical port reference is not valid")
        << "" << "" << "" << "" << true << false << false;

    QTest::newRow("Transactional: Range within abstraction width is valid") <<
        "illogical" << "0" << "5" << "10" << true << false << true;
    QTest::newRow("Transactional: Right range not within abstraction width is not valid") <<
        "illogical" << "0" << "20" << "10" << true << false << false;
    QTest::newRow("Transactional: Left range not within abstraction width is not valid") <<
        "illogical" << "15" << "8" << "10" << true << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfacePortMapValidator::testPortMapPhysicalPortIsValid()
//-----------------------------------------------------------------------------
void tst_BusInterfacePortMapValidator::testPortMapPhysicalPortIsValid()
{
    QFETCH(QString, physicalPortName);
    QFETCH(QString, physicalRangeLeft);
    QFETCH(QString, physicalRangeRight);
    QFETCH(QString, portLeftBound);
    QFETCH(QString, portRigthBound);
    QFETCH(bool, createPort);
    QFETCH(bool, isWire);
    QFETCH(bool, isValid);

    QSharedPointer<ConfigurableVLNVReference> abstractionReference (new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "testVendor", "testLibrary", "Everett", "5.1"));

    QSharedPointer<AbstractionDefinition> abstractionDefinition(new AbstractionDefinition(*abstractionReference,
        Document::Revision::Std14));

    QSharedPointer<PortAbstraction> testLogicalAbstraction (new PortAbstraction());
    testLogicalAbstraction->setLogicalName("testLogical");
    abstractionDefinition->getLogicalPorts()->append(testLogicalAbstraction);

    if (isWire)
    {
        QSharedPointer<WireAbstraction> logicalWireAbstraction(new WireAbstraction());
        testLogicalAbstraction->setWire(logicalWireAbstraction);
    }
    else
    {
        QSharedPointer<TransactionalAbstraction> logicalTransactionalAbstraction(new TransactionalAbstraction());
        testLogicalAbstraction->setTransactional(logicalTransactionalAbstraction);
    }

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

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

    QSharedPointer<QList<QSharedPointer<Port> > > componentPorts (new QList<QSharedPointer<Port> > ());
    if (createPort)
    {
        QSharedPointer<Port> newPort;
        if (isWire)
        {
            newPort = createWirePort(physicalPortName);
            newPort->setLeftBound(portLeftBound);
            newPort->setRightBound(portRigthBound);
        }
        else
        {
            newPort = createTransactionalPort(physicalPortName);

            // Set transactional port width to smallest needed for tests.
            quint64 logicalWidth = qMax(parser->parseExpression(portLeftBound).toULongLong(),
                parser->parseExpression(portRigthBound).toULongLong()) + 1;

            newPort->getTransactional()->setBusWidth(QString::number(logicalWidth));
        }

        if (physicalPortName.isEmpty())
        {
            physicalPortName = "Champloo";
            newPort->setName(physicalPortName);
            physicalPortName.clear();
        }
        componentPorts->append(newPort);
    }

    QSharedPointer<PortMapValidator> validator = createPortMapValidator(parser, componentPorts, mockLibrary);
    validator->busInterfaceChanged(abstractionReference, General::MASTER, QLatin1String(""));

    QCOMPARE(validator->hasValidPhysicalPort(testPhysical), isValid);

    if (!isValid)
    {
        quint64 leftValue = parser->parseExpression(physicalRangeLeft).toULongLong();
        quint64 rightValue = parser->parseExpression(physicalRangeRight).toULongLong();
        quint64 portMax = qMax(parser->parseExpression(portLeftBound).toULongLong(),
            parser->parseExpression(portRigthBound).toULongLong());
        quint64 portMin = qMin(parser->parseExpression(portLeftBound).toULongLong(),
            parser->parseExpression(portRigthBound).toULongLong());

        QString context = QLatin1String("test");
        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testPortMap, "test");

        QString expectedError;

        if (physicalPortName.isEmpty())
        {
            expectedError = QObject::tr("Port map does not contain a physical port or a logical tie off within %1")
                .arg(context);
        }
        else if (!createPort)
        {
            expectedError = QObject::tr("Could not locate physical port %1 mapped within %2")
                .arg(physicalPortName).arg(context);
        }
        else if (parser->parseExpression(physicalRangeLeft).toInt() < 0)
        {
            expectedError =
                QObject::tr("Invalid value given for physical left in physical port %1 mapped within %2")
                .arg(physicalPortName).arg(context);
        }
        else if (parser->parseExpression(physicalRangeRight).toInt() < 0)
        {
            expectedError =
                QObject::tr("Invalid value given for physical right in physical port %1 mapped within %2")
                .arg(physicalPortName).arg(context);
        }
        else if (physicalRangeLeft.isEmpty())
        {
            expectedError = QObject::tr("Empty left value in physical port %1 mapped within %2")
                .arg(physicalPortName).arg(context);
        }
        else if (physicalRangeRight.isEmpty())
        {
            expectedError = QObject::tr("Empty right value in physical port %1 mapped within %2")
                .arg(physicalPortName).arg(context);
        }
        else if (leftValue > portMax || rightValue > portMax || leftValue < portMin || rightValue < portMin)
        {
            expectedError = QObject::tr(
                "Range is not within the referenced port width in mapped physical port %1 in %2")
                .arg(physicalPortName).arg(context);
        }

        if (errorIsNotFoundInErrorList(expectedError, errorsFound))
        {
            QFAIL("Error was not found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfacePortMapValidator::testPortMapPhysicalPortIsValid_data()
//-----------------------------------------------------------------------------
void tst_BusInterfacePortMapValidator::testPortMapPhysicalPortIsValid_data()
{
    QTest::addColumn<QString>("physicalPortName");
    QTest::addColumn<QString>("physicalRangeLeft");
    QTest::addColumn<QString>("physicalRangeRight");
    QTest::addColumn<QString>("portLeftBound");
    QTest::addColumn<QString>("portRigthBound");
    QTest::addColumn<bool>("createPort");
    QTest::addColumn<bool>("isWire");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Wire: Physical port without range is valid")
        << "physicalPort" << "" << "" << "0" << "20" << true << true << true;
    QTest::newRow("Wire: Physical port with range is valid")
        << "physicalPort" << "4+4" << "12" << "0" << "20" <<  true << true << true;
    QTest::newRow("Wire: Physical port with either range as 0 is valid")
        << "physicalPort" << "0" << "0" << "0" << "20" << true << true << true;
    QTest::newRow("Wire: Physical port with negative range left value is not valid")
        << "physicalPort" << "-1" << "1" << "0" << "20" << true << true << false;
    QTest::newRow("Wire: Physical port with negative range right value is not valid")
        << "physicalPort" << "1" << "-1" << "0" << "20" << true << true << false;
    QTest::newRow("Wire: Physical port without left range value is not valid")
        << "leftRangeIsEmpty" << "" << "12" << "0" << "20" << true << true << false;
    QTest::newRow("Wire: Physical port without right range value is not valid")
        << "physicalPort" << "12" << "" << "0" << "20" << true << true << false;
    QTest::newRow("Wire: Referencing non-existing physical port is not valid")
        << "physicalPort" << "" << "" << "0" << "20" << false << true << false;
    QTest::newRow("Wire: Empty physical port reference is not valid")
        << "" << "" << "" << "0" << "20" << true << true << false;

    QTest::newRow("Wire: Physical port ranges outside referenced port range is not valid")
        << "testPort" << "0" << "4" << "1" << "10" << true << true << false;
    QTest::newRow("Wire: Physical port ranges outside referenced port range is not valid")
        << "testPort" << "12" << "4" << "1" << "10" << true << true << false;

    QTest::newRow("Wire: Physical port with long range is valid") <<
        "longWire" << "4000000000" << "4000000000" << "5000000000" << "3000000000" << true << true << true;


    QTest::newRow("Transactional: Physical port without range is valid")
        << "physicalPort" << "" << "" << "0" << "20" << true << false << true;
    QTest::newRow("Transactional: Physical port with range is valid")
        << "physicalPort" << "4+4" << "12" << "0" << "20" << true << false << true;
    QTest::newRow("Transactional: Physical port with either range as 0 is valid")
        << "physicalPort" << "0" << "0" << "0" << "20" << true << false << true;
    QTest::newRow("Transactional: Physical port with negative range left value is not valid")
        << "physicalPort" << "-1" << "1" << "0" << "20" << true << false << false;
    QTest::newRow("Transactional: Physical port with negative range right value is not valid")
        << "physicalPort" << "1" << "-1" << "0" << "20" << true << false << false;
    QTest::newRow("Transactional: Physical port without left range value is not valid")
        << "physicalPort" << "" << "12" << "0" << "20" << true << false << false;
    QTest::newRow("Transactional: Physical port without right range value is not valid")
        << "physicalPort" << "12" << "" << "0" << "20" << true << false << false;
    QTest::newRow("Transactional: Referencing non-existing physical port is not valid")
        << "physicalPort" << "" << "" << "0" << "20" << false << false << false;
    QTest::newRow("Transactional: Empty physical port reference is not valid")
        << "" << "" << "" << "0" << "20" << true << false << false;

    QTest::newRow("Transactional: Physical port ranges outside referenced port range is not valid")
        << "transactionalRangeTest" << "0" << "11" << "0" << "10" << true << false << false;
    QTest::newRow("Transactional: Physical port ranges outside referenced port range is not valid")
        << "testPort" << "12" << "4" << "1" << "10" << true << false << false;

    QTest::newRow("Transactional: Physical port with long range is valid") <<
        "testPort" << "4000000000" << "4000000000" << "5000000000" << "3000000000" << true << false << true;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfacePortMapValidator::testPortMapConnectedPortDirectionsAreValid()
//-----------------------------------------------------------------------------
void tst_BusInterfacePortMapValidator::testPortMapConnectedPortDirectionsAreValid()
{
    QFETCH(QString, logicalDirection);
    QFETCH(QString, physicalDirection);
    QFETCH(bool, physicalAllLogicalDirectionsAllowed);
    QFETCH(bool, isMirroredInterface);
    QFETCH(bool, isValid);

    QSharedPointer<ConfigurableVLNVReference> abstractionReference (new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "testVendor", "testLibrary", "Everett", "5.1"));

    QSharedPointer<AbstractionDefinition> abstractionDefinition (new AbstractionDefinition(*abstractionReference,
        Document::Revision::Std14));

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

    QSharedPointer<Port> newPort = createWirePort(testPhysical->name_);
    newPort->getWire()->setDirection(
        DirectionTypes::str2Direction(physicalDirection, DirectionTypes::DIRECTION_INVALID));
    newPort->getWire()->setAllLogicalDirectionsAllowed(physicalAllLogicalDirectionsAllowed);
    QSharedPointer<QList<QSharedPointer<Port> > > componentPorts (new QList<QSharedPointer<Port> > ());
    componentPorts->append(newPort);

    QSharedPointer<PortMap> testPortMap (new PortMap());
    testPortMap->setLogicalPort(testLogical);
    testPortMap->setPhysicalPort(testPhysical);

    General::InterfaceMode busMode = General::MASTER;

    if (isMirroredInterface)
    {
        busMode = General::MIRRORED_MASTER;
    }

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());

    QSharedPointer<PortMapValidator> validator = createPortMapValidator(parser, componentPorts, mockLibrary);
    validator->abstractionDefinitionChanged(abstractionDefinition, busMode);
    validator->busInterfaceChanged(abstractionReference, busMode, QString(""));

    QCOMPARE(validator->connectedPortsHaveValidDirections(testLogicalAbstraction, newPort), isValid);

    if (!isValid)
    {
        QString context = QLatin1String("test");

        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testPortMap, "test");

        QString expectedError = QObject::tr("Invalid connection made between logical port '%1' with direction "
            "'%2' and physical port '%3' with direction '%4' within %5")
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
// Function: tst_BusInterfacePortMapValidator::testPortMapConnectedPortDirectionsAreValid_data()
//-----------------------------------------------------------------------------
void tst_BusInterfacePortMapValidator::testPortMapConnectedPortDirectionsAreValid_data()
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
// Function: tst_BusInterfacePortMapValidator::testPortMapConnectedPortDirectionsAreValid()
//-----------------------------------------------------------------------------
void tst_BusInterfacePortMapValidator::testPortMapConnectedPortInitiativesAreValid()
{
    QFETCH(QString, logicalInitiative);
    QFETCH(QString, physicalInitiative);
    QFETCH(bool, isValid);

    QSharedPointer<ConfigurableVLNVReference> abstractionReference(new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "testVendor", "testLibrary", "Everett", "5.1"));

    QSharedPointer<AbstractionDefinition> abstractionDefinition(new AbstractionDefinition(*abstractionReference,
        Document::Revision::Std14));

    QSharedPointer<TransactionalPort> masterTransactional(new TransactionalPort());
    masterTransactional->setInitiative(logicalInitiative);

    QSharedPointer<TransactionalAbstraction> logicalTransactionalAbstraction(new TransactionalAbstraction());

    QSharedPointer<PortAbstraction> testLogicalAbstraction(new PortAbstraction());
    testLogicalAbstraction->setLogicalName("testLogical");
    testLogicalAbstraction->setTransactional(logicalTransactionalAbstraction);
    testLogicalAbstraction->getTransactional()->setMasterPort(masterTransactional);

    abstractionDefinition->getLogicalPorts()->append(testLogicalAbstraction);

    LibraryMock* mockLibrary(new LibraryMock(this));
    mockLibrary->addComponent(abstractionDefinition);

    QSharedPointer<PortMap::LogicalPort> testLogical(new PortMap::LogicalPort());
    testLogical->name_ = testLogicalAbstraction->name();

    QSharedPointer<PortMap::PhysicalPort> testPhysical(new PortMap::PhysicalPort());
    testPhysical->name_ = "testPhysicalPort";

    QSharedPointer<Port> newPort = createTransactionalPort(testPhysical->name_);
    newPort->getTransactional()->setInitiative(physicalInitiative);
    QSharedPointer<QList<QSharedPointer<Port> > > componentPorts(new QList<QSharedPointer<Port> >());
    componentPorts->append(newPort);

    QSharedPointer<PortMap> testPortMap(new PortMap());
    testPortMap->setLogicalPort(testLogical);
    testPortMap->setPhysicalPort(testPhysical);

    General::InterfaceMode busMode = General::MASTER;

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    QSharedPointer<PortMapValidator> validator = createPortMapValidator(parser, componentPorts, mockLibrary);
    validator->abstractionDefinitionChanged(abstractionDefinition, busMode);
    validator->busInterfaceChanged(abstractionReference, busMode, QString(""));

    QCOMPARE(validator->connectedPortsHaveValidInitiatives(testLogicalAbstraction, newPort), isValid);

    if (!isValid)
    {
        QString context = QLatin1String("test");

        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testPortMap, "test");

        QString expectedError = QObject::tr("Invalid connection made between logical port '%1' with initiative "
            "'%2' and physical port '%3' with initiative '%4' within %5")
            .arg(testPortMap->getLogicalPort()->name_).arg(logicalInitiative)
            .arg(testPortMap->getPhysicalPort()->name_).arg(physicalInitiative)
            .arg(context);

        if (errorIsNotFoundInErrorList(expectedError, errorsFound))
        {
            QFAIL("Error was not found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfacePortMapValidator::testPortMapConnectedPortInitiativesAreValid_data()
//-----------------------------------------------------------------------------
void tst_BusInterfacePortMapValidator::testPortMapConnectedPortInitiativesAreValid_data()
{
    QTest::addColumn<QString>("logicalInitiative");
    QTest::addColumn<QString>("physicalInitiative");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Logical port initiative=requires, physical port initiative=requires is valid")
        << "requires" << "requires" << true;
    QTest::newRow("Logical port initiative=requires, physical port initiative=both is valid")
        << "requires" << "both" << true;
    QTest::newRow("Logical port initiative=requires, physical port initiative=phantom is valid")
        << "requires" << "phantom" << true;
    QTest::newRow("Logical port initiative=requires, physical port initiative=provides is not valid")
        << "requires" << "provides" << false;

    QTest::newRow("Logical port initiative=provides, physical port initiative=requires is not valid")
        << "provides" << "requires" << false;
    QTest::newRow("Logical port initiative=provides, physical port initiative=both is valid")
        << "provides" << "both" << true;
    QTest::newRow("Logical port initiative=provides, physical port initiative=phantom is valid")
        << "provides" << "phantom" << true;
    QTest::newRow("Logical port initiative=provides, physical port initiative=provides is valid")
        << "provides" << "provides" << true;

    QTest::newRow("Logical port initiative=both, physical port initiative=requires is not valid")
        << "both" << "requires" << false;
    QTest::newRow("Logical port initiative=both, physical port initiative=both is valid")
        << "both" << "both" << true;
    QTest::newRow("Logical port initiative=both, physical port initiative=phantom is valid")
        << "both" << "phantom" << true;
    QTest::newRow("Logical port initiative=both, physical port initiative=provides is not valid")
        << "both" << "provides" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfacePortMapValidator::testPortMapConnectedPortsHaveValidType()
//-----------------------------------------------------------------------------
void tst_BusInterfacePortMapValidator::testPortMapConnectedPortsHaveValidType()
{
    QFETCH(bool, logicalHasWire);
    QFETCH(bool, physicalHasWire);
    QFETCH(bool, isValid);

    QSharedPointer<ConfigurableVLNVReference> abstractionReference (new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "testVendor", "testLibrary", "Everett", "5.1"));

    QSharedPointer<AbstractionDefinition> abstractionDefinition(new AbstractionDefinition(*abstractionReference,
        Document::Revision::Std14));

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

    QSharedPointer<Port> newPort = createWirePort(testPhysical->name_);
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

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());

    QSharedPointer<PortMapValidator> validator = createPortMapValidator(parser, componentPorts, mockLibrary);
    validator->busInterfaceChanged(abstractionReference, General::MASTER, QString(""));

    QCOMPARE(validator->connectedPortsHaveValidPortTypes(testLogicalAbstraction, newPort), isValid);

    if (!isValid)
    {
        QString context = QLatin1String("test");

        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testPortMap, context);

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
// Function: tst_BusInterfacePortMapValidator::testPortMapConnectedPortsHaveValidType_data()
//-----------------------------------------------------------------------------
void tst_BusInterfacePortMapValidator::testPortMapConnectedPortsHaveValidType_data()
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
// Function: tst_BusInterfacePortMapValidator::testPortMapConnectedPortsHaveSameRange()
//-----------------------------------------------------------------------------
void tst_BusInterfacePortMapValidator::testPortMapConnectedPortsHaveSameRange()
{
    QFETCH(QString, logicalRangeLeft);
    QFETCH(QString, logicalRangeRight);
    QFETCH(QString, physicalRangeLeft);
    QFETCH(QString, physicalRangeRight);
    QFETCH(bool, isWire);
    QFETCH(bool, isValid);

    QSharedPointer<ConfigurableVLNVReference> abstractionReference (new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "testVendor", "testLibrary", "Everett", "5.1"));

    QSharedPointer<AbstractionDefinition> abstractionDefinition(new AbstractionDefinition(*abstractionReference,
        Document::Revision::Std14));

    QSharedPointer<PortAbstraction> testLogicalAbstraction(new PortAbstraction());
    testLogicalAbstraction->setLogicalName("testLogical");

    if (isWire)
    {
        QSharedPointer<WireAbstraction> logicalWireAbstraction(new WireAbstraction());
        testLogicalAbstraction->setWire(logicalWireAbstraction);
    }
    else
    {
        QSharedPointer<TransactionalAbstraction> logicalTransactionalAbstraction(new TransactionalAbstraction());
        testLogicalAbstraction->setTransactional(logicalTransactionalAbstraction);
    }

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

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

    QSharedPointer<QList<QSharedPointer<Port> > > componentPorts (new QList<QSharedPointer<Port> > ());
    QSharedPointer<Port> newPort;
    if (isWire)
    {
        newPort = createWirePort(testPhysical->name_);
        newPort->setLeftBound(physicalRangeLeft);
        newPort->setRightBound(physicalRangeRight);
    }
    else
    {
        newPort = createTransactionalPort(testPhysical->name_);
        
        // Set transactional port width to smallest needed for tests.
        quint64 logicalWidth = qMax(parser->parseExpression(physicalRangeLeft).toULongLong(),
            parser->parseExpression(physicalRangeRight).toULongLong()) + 1;
        
        newPort->getTransactional()->setBusWidth(QString::number(logicalWidth));
    }

    componentPorts->append(newPort);

    QSharedPointer<PortMap> testPortMap (new PortMap());
    testPortMap->setLogicalPort(testLogical);
    testPortMap->setPhysicalPort(testPhysical);

    QSharedPointer<PortMapValidator> validator = createPortMapValidator(parser, componentPorts, mockLibrary);
    validator->busInterfaceChanged(abstractionReference, General::MASTER, QString(""));

    QCOMPARE(validator->connectedPortsHaveSameRange(logicalRange, physicalPart), isValid);

    if (!isValid)
    {
        QString context = QLatin1String("test");

        QVector<QString> errorsFound;
        validator->findErrorsIn(errorsFound, testPortMap, context);

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
// Function: tst_BusInterfacePortMapValidator::testPortMapConnectedPortsHaveSameRange()
//-----------------------------------------------------------------------------
void tst_BusInterfacePortMapValidator::testPortMapConnectedPortsHaveSameRange_data()
{
    QTest::addColumn<QString>("logicalRangeLeft");
    QTest::addColumn<QString>("logicalRangeRight");
    QTest::addColumn<QString>("physicalRangeLeft");
    QTest::addColumn<QString>("physicalRangeRight");
    QTest::addColumn<bool>("isWire");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Wire: Connected ports without range is valid") <<
        "" << "" << "" << "" << true << true;
    QTest::newRow("Wire: Connected ports with the same range values is valid") <<
        "0" << "10" << "0" << "10" << true << true;
    QTest::newRow("Wire: Connected ports with the same range size is valid") <<
        "0" << "5+1" << "18-8" << "10+3*2" << true << true;
    QTest::newRow("Wire: Connected ports without same range size is not valid") <<
        "" << "" << "4" << "3" << true << false;

    QTest::newRow("Transactional: Connected ports without range is valid") <<
        "" << "" << "" << "" << false << true;
    QTest::newRow("Transactional: Connected ports with the same range values is valid") <<
        "0" << "10" << "0" << "10" << false << true;
    QTest::newRow("Transactional: Connected ports with the same range size is valid") <<
        "0" << "5+1" << "18-8" << "10+3*2" << false << true;
    QTest::newRow("Transactional: Connected ports without same range size is not valid") <<
        "" << "" << "4" << "3" << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfacePortMapValidator::createPortMapValidator()
//-----------------------------------------------------------------------------
QSharedPointer<PortMapValidator> tst_BusInterfacePortMapValidator::createPortMapValidator(
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<QList<QSharedPointer<Port> > > ports,
    LibraryInterface* libraryHandler)
{
    QSharedPointer<PortMapValidator> newPortMapValidator(new PortMapValidator(
        expressionParser, ports, libraryHandler));

    return newPortMapValidator;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfacePortMapValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_BusInterfacePortMapValidator::errorIsNotFoundInErrorList(QString const& expectedError,
    QVector<QString> errorList)
{
    if (!errorList.contains(expectedError))
    {
        qDebug() << "The following error:" << Qt::endl << expectedError << Qt::endl << "was not found in error list:";
        for (QString const& error : errorList)
        {
            qDebug() << error;
        }
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfacePortMapValidator::createWirePort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> tst_BusInterfacePortMapValidator::createWirePort(QString const& portName)
{
    QSharedPointer<Port> newPort(new Port(portName));

    QSharedPointer<Wire> newWire(new Wire());
    newPort->setWire(newWire);

    return newPort;
}

//-----------------------------------------------------------------------------
// Function: tst_BusInterfacePortMapValidator::createTransactionalPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> tst_BusInterfacePortMapValidator::createTransactionalPort(QString const& portName)
{
    QSharedPointer<Port> newPort(new Port(portName));

    QSharedPointer<Transactional> newTransactional(new Transactional());
    newPort->setTransactional(newTransactional);

    return newPort;
}

QTEST_APPLESS_MAIN(tst_BusInterfacePortMapValidator)

#include "tst_BusInterfacePortMapValidator.moc"
