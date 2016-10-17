//-----------------------------------------------------------------------------
// File: tst_InterfaceDirectionNameSorter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.08.2014
//
// Description:
// Unit test for class InterfaceDirectionNameSorter.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <Plugins/common/PortSorter/InterfaceDirectionNameSorter.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PortMap.h>

class tst_InterfaceDirectionNameSorter : public QObject
{
    Q_OBJECT

public:
    tst_InterfaceDirectionNameSorter();

private slots:
    void init();
    void cleanup();
    void testNullComponent();
    void testComponentWithNoPorts();
    void testPortsOrderedByName();
    void testPortsOrderedByDirectionThenName();
    void testPortsOrderedByInterfaceThenDirection();

private:    
    void addPort(QString portName, DirectionTypes::Direction direction = DirectionTypes::IN);
    void addInterface( QString const& interfaceName );
    void mapPortToInterface( QString const& portName, QString const& interfaceName );
   
    QSharedPointer<Component> component_;
};

//-----------------------------------------------------------------------------
// Function: tst_InterfaceDirectionNameSorter::tst_InterfaceDirectionNameSorter()
//-----------------------------------------------------------------------------
tst_InterfaceDirectionNameSorter::tst_InterfaceDirectionNameSorter()
{
}

//-----------------------------------------------------------------------------
// Function: tst_InterfaceDirectionNameSorter::init()
//-----------------------------------------------------------------------------
void tst_InterfaceDirectionNameSorter::init()
{
    component_ = QSharedPointer<Component>(new Component());
}

//-----------------------------------------------------------------------------
// Function: tst_InterfaceDirectionNameSorter::cleanup()
//-----------------------------------------------------------------------------
void tst_InterfaceDirectionNameSorter::cleanup()
{
    component_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_InterfaceDirectionNameSorter::testNullComponent()
//-----------------------------------------------------------------------------
void tst_InterfaceDirectionNameSorter::testNullComponent()
{
   InterfaceDirectionNameSorter sorter;

   QStringList sorted = sorter.sortedPortNames(QSharedPointer<Component>(0));

   QVERIFY(sorted.isEmpty());
}

//-----------------------------------------------------------------------------
// Function: tst_InterfaceDirectionNameSorter::testComponentWithNoPorts()
//-----------------------------------------------------------------------------
void tst_InterfaceDirectionNameSorter::testComponentWithNoPorts()
{
    InterfaceDirectionNameSorter sorter;

    QVERIFY(sorter.sortedPortNames(component_).isEmpty());
}

//-----------------------------------------------------------------------------
// Function: tst_InterfaceDirectionNameSorter::testPortsOrderedByName()
//-----------------------------------------------------------------------------
void tst_InterfaceDirectionNameSorter::testPortsOrderedByName()
{
    addPort("c");
    addPort("C");
    addPort("B");
    addPort("b");
    addPort("a");
    addPort("x");
    addPort("z");
    addPort("");

    InterfaceDirectionNameSorter sorter;

    QStringList expectedNames;
    expectedNames << "" << "a" << "B" << "b" << "c" << "C" << "x" << "z";
    QCOMPARE(sorter.sortedPortNames(component_), expectedNames);
}

//-----------------------------------------------------------------------------
// Function: tst_InterfaceDirectionNameSorter::addPort()
//-----------------------------------------------------------------------------
void tst_InterfaceDirectionNameSorter::addPort(QString portName, DirectionTypes::Direction direction)
{
    QSharedPointer<Port> newPort (new Port(portName, direction));
    newPort->setAllLogicalDirectionsAllowed(true);
    component_->getPorts()->append(newPort);
}

//-----------------------------------------------------------------------------
// Function: tst_InterfaceDirectionNameSorter::testPortsOrderedByDirection()
//-----------------------------------------------------------------------------
void tst_InterfaceDirectionNameSorter::testPortsOrderedByDirectionThenName()
{
    addPort("a_inout", DirectionTypes::INOUT);
    addPort("k_out", DirectionTypes::OUT);
    addPort("j_out", DirectionTypes::OUT);
    addPort("i_out", DirectionTypes::OUT);
    addPort("x_in", DirectionTypes::IN);
    
    addPort("phantom", DirectionTypes::DIRECTION_PHANTOM);
    addPort("invalid", DirectionTypes::DIRECTION_INVALID);

    InterfaceDirectionNameSorter sorter;

    QStringList expectedNames;
    expectedNames  << "x_in" << "i_out" << "j_out" << "k_out" << "a_inout" << "phantom" << "invalid";
    QCOMPARE( sorter.sortedPortNames(component_), expectedNames);
}

//-----------------------------------------------------------------------------
// Function: tst_InterfaceDirectionNameSorter::testPortsOrderedByInterfaceThenDirection()
//-----------------------------------------------------------------------------
void tst_InterfaceDirectionNameSorter::testPortsOrderedByInterfaceThenDirection()
{
    addInterface("firstInterface");
    addInterface("secondInterface");
    addInterface("thirdInterface");

    addPort("k_none");

    addPort("z_several");
    mapPortToInterface("z_several", "firstInterface");
    mapPortToInterface("z_several", "thirdInterface");    

    addPort("x_1st", DirectionTypes::INOUT);
    mapPortToInterface("x_1st", "firstInterface");
    addPort("y_1st", DirectionTypes::OUT);
    mapPortToInterface("y_1st", "firstInterface");
    addPort("z_1st", DirectionTypes::IN);
    mapPortToInterface("z_1st", "firstInterface");

    addPort("a_2nd", DirectionTypes::OUT);
    mapPortToInterface("a_2nd", "secondInterface");
   
    addPort("i_3rd", DirectionTypes::OUT);
    mapPortToInterface("i_3rd", "thirdInterface");

    InterfaceDirectionNameSorter sorter;

    QStringList expectedNames;
    expectedNames << "z_1st" << "y_1st" << "x_1st" << "a_2nd" << "i_3rd" << "z_several" << "k_none";
    QCOMPARE(sorter.sortedPortNames(component_), expectedNames);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::addInterface()
//-----------------------------------------------------------------------------
void tst_InterfaceDirectionNameSorter::addInterface( QString const& interfaceName )
{
    QSharedPointer<BusInterface> busIf = QSharedPointer<BusInterface>(new BusInterface());
    busIf->setName(interfaceName);

    component_->getBusInterfaces()->append(busIf);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::mapPortToInterface()
//-----------------------------------------------------------------------------
void tst_InterfaceDirectionNameSorter::mapPortToInterface( QString const& portName, QString const& interfaceName )
{
    QSharedPointer<PortMap> portMap = QSharedPointer<PortMap>(new PortMap());

    QSharedPointer<PortMap::LogicalPort> theLogical (new PortMap::LogicalPort());
    theLogical->name_ = portName.toUpper();
    portMap->setLogicalPort(theLogical);

    QSharedPointer<PortMap::PhysicalPort> thePhysical (new PortMap::PhysicalPort());
    thePhysical->name_ = portName;
    portMap->setPhysicalPort(thePhysical);

    QSharedPointer<QList<QSharedPointer<PortMap> > > portMapList =
        component_->getBusInterface(interfaceName)->getPortMaps();

    if (!portMapList)
    {
        QSharedPointer<QList<QSharedPointer<PortMap> > > newPortMapList (new QList<QSharedPointer<PortMap> > ());

        if (component_->getBusInterface(interfaceName)->getAbstractionTypes()->isEmpty())
        {
            QSharedPointer<AbstractionType> testAbstraction (new AbstractionType());
            component_->getBusInterface(interfaceName)->getAbstractionTypes()->append(testAbstraction);
        }

        component_->getBusInterface(interfaceName)->setPortMaps(newPortMapList);
        portMapList = component_->getBusInterface(interfaceName)->getPortMaps();
    }

    portMapList->append(portMap);
}

QTEST_APPLESS_MAIN(tst_InterfaceDirectionNameSorter)

#include "tst_InterfaceDirectionNameSorter.moc"
