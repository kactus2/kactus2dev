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

#include <Plugins/VerilogGenerator/PortSorter/InterfaceDirectionNameSorter.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/port.h>

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
    void addPort(QString portName, General::Direction direction = General::IN);
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
void tst_InterfaceDirectionNameSorter::addPort(QString portName, General::Direction direction)
{
    component_->addPort(QSharedPointer<Port>(new Port(portName, direction, 0, 0, "", true)));
}

//-----------------------------------------------------------------------------
// Function: tst_InterfaceDirectionNameSorter::testPortsOrderedByDirection()
//-----------------------------------------------------------------------------
void tst_InterfaceDirectionNameSorter::testPortsOrderedByDirectionThenName()
{
    addPort("a_inout", General::INOUT);
    addPort("k_out", General::OUT);    
    addPort("j_out", General::OUT);    
    addPort("i_out", General::OUT);    
    addPort("x_in", General::IN);    
    
    addPort("phantom", General::DIRECTION_PHANTOM);
    addPort("invalid", General::DIRECTION_INVALID);

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

    addPort("x_1st", General::INOUT);    
    mapPortToInterface("x_1st", "firstInterface");
    addPort("y_1st", General::OUT);
    mapPortToInterface("y_1st", "firstInterface");
    addPort("z_1st", General::IN);
    mapPortToInterface("z_1st", "firstInterface");

    addPort("a_2nd", General::OUT);
    mapPortToInterface("a_2nd", "secondInterface");
   
    addPort("i_3rd", General::OUT);
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
    component_->addBusInterface(busIf);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::mapPortToInterface()
//-----------------------------------------------------------------------------
void tst_InterfaceDirectionNameSorter::mapPortToInterface( QString const& portName, QString const& interfaceName )
{
    QSharedPointer<General::PortMap> portMap = QSharedPointer<General::PortMap>(new General::PortMap());
    portMap->logicalPort_ = portName.toUpper();
    portMap->physicalPort_ = portName;

    QList<QSharedPointer<General::PortMap> >& portMapList = component_->getBusInterface(interfaceName)->getPortMaps();
    portMapList.append(portMap);
}

QTEST_APPLESS_MAIN(tst_InterfaceDirectionNameSorter)

#include "tst_InterfaceDirectionNameSorter.moc"
