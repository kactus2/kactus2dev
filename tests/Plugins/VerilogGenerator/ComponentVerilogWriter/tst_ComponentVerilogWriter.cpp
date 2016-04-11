//-----------------------------------------------------------------------------
// File: tst_ComponentVerilogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 01.08.2014
//
// Description:
// Unit test for class ComponentVerilogWriter.
//-----------------------------------------------------------------------------

#include <QtTest>
#include <QSharedPointer>

#include <Plugins/VerilogGenerator/ComponentVerilogWriter/ComponentVerilogWriter.h>
#include <Plugins/VerilogGenerator/PortSorter/InterfaceDirectionNameSorter.h>

#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Model.h>

#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/common/ModuleParameter.h>

class tst_ComponentVerilogWriter : public QObject
{
    Q_OBJECT

public:
    tst_ComponentVerilogWriter();

private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test cases:
    void testNullPointerAsConstructorParameter();

    void testNamedModule();
    void testNamedModule_data();

    void testPortDescriptionIsWrittenAfterPort();

    void testInterfaceDescriptionIsPrinted();
    void testInterfaceDescriptionIsPrinted_data();

    void testPortsOrderedByName();
    void testPortsOrderedByDirectionThenName();
    void testPortsOrderedByInterfaceThenDirectionThenName();

    void testComponentWithModelParameters();

    void testParametersPrecedePorts();

    void testParametrizedPort();

    void testParameterizedModelParameter();

private:
    void writeComponent(QString const& activeView = QString()); 

    void addPort(QString const& portName, int portSize, DirectionTypes::Direction direction);

    void addModuleParameter(QString const& name, QString const& value, QString const& description = "");

    void addInterface( QString const& interfaceName );

    void mapPortToInterface( QString const& portName, QString const& interfaceName );
   

    //! The top component to write to Verilog.
    QSharedPointer<Component> component_;

    //! Sorter for top component's ports.
    QSharedPointer<PortSorter> sorter_;

    //! The writer output as a string.
    QString outputString_;

    //! The writer output stream.
    QTextStream outputStream_;

    //! The formatter for expressions.
    QSharedPointer<ExpressionFormatter> formatter_;
};

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::tst_ComponentVerilogWriter()
//-----------------------------------------------------------------------------
tst_ComponentVerilogWriter::tst_ComponentVerilogWriter(): component_(), 
    sorter_(new InterfaceDirectionNameSorter()),
    outputString_(), 
    outputStream_(),
    formatter_()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::initTestCase()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::init()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::init()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    component_ = QSharedPointer<Component>(new Component(vlnv));

    outputStream_.setString(&outputString_);

    QSharedPointer<ComponentParameterFinder> finder(new ComponentParameterFinder(component_));
    formatter_ = QSharedPointer<ExpressionFormatter>(new ExpressionFormatter(finder));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::cleanup()
{
    component_.clear();

    outputStream_.reset();
    outputString_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testEmptyComponent()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testNullPointerAsConstructorParameter()
{
    ComponentVerilogWriter writer(QSharedPointer<Component>(0), "",
        QSharedPointer<PortSorter>(0), QSharedPointer<ExpressionFormatter>(0));

    writer.write(outputStream_);

    QCOMPARE(outputString_, QString());
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testNamedComponent()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testNamedModule()
{
    QFETCH(QString, name);
    QFETCH(QString, expectedOutput);

    VLNV vlnv(VLNV::COMPONENT, "", "", name, "");
    component_ = QSharedPointer<Component>(new Component(vlnv));

    writeComponent();

    QCOMPARE(outputString_, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testNamedModule_data()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testNamedModule_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("expectedOutput");

    QTest::newRow("common") << "myComponent" 
        << "module myComponent();\n"
        "\n"
        "endmodule\n";
    QTest::newRow("underscore") << "_a_module" 
        << "module _a_module();\n"
        "\n"
        "endmodule\n";
    QTest::newRow("numbersInName") << "i386" 
        << "module i386();\n"
        "\n"
        "endmodule\n";
    QTest::newRow("dollarSign") << "bank$" 
        << "module bank$();\n"
        "\n"
        "endmodule\n";
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::writeComponent()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::writeComponent(QString const& activeView /*= QString()*/)
{
    ComponentVerilogWriter writer(component_, activeView, sorter_, formatter_);    
    writer.write(outputStream_);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testPortDescriptionIsWrittenAfterPort()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testPortDescriptionIsWrittenAfterPort()
{
    addPort("a", 1,  DirectionTypes::IN);
    component_->getPort("a")->setDescription("Description for a.");
    addPort("b", 1, DirectionTypes::IN);
    addPort("c", 1, DirectionTypes::IN);
    component_->getPort("c")->setDescription("Description for c.");
        
    writeComponent();

    QCOMPARE(outputString_, 
        QString("module TestComponent(\n"
        "    // These ports are not in any interface\n" 
        "    input                               a,    // Description for a.\n"
        "    input                               b,\n"
        "    input                               c    // Description for c.\n"
        ");\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testInterfaceDescriptionIsPrinted()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testInterfaceDescriptionIsPrinted()
{
    QFETCH(QString, description);
    QFETCH(QString, expectedOutput);

    addInterface("A");
    component_->getBusInterface("A")->setDescription(description);
    
    addPort("port", 1 , DirectionTypes::IN);
    mapPortToInterface("port", "A");

    writeComponent();

    QCOMPARE(outputString_, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testInterfaceDescriptionIsPrinted_data()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testInterfaceDescriptionIsPrinted_data()
{
    QTest::addColumn<QString>("description");
    QTest::addColumn<QString>("expectedOutput");

    QTest::newRow("empty description") << "" <<
        "module TestComponent(\n"
        "    // Interface: A\n"
        "    input                               port\n"
        ");\n"
        "\n"
        "endmodule\n";

    QTest::newRow("basic description") << "Description of A" <<
        "module TestComponent(\n"
        "    // Interface: A\n"
        "    // Description of A\n"
        "    input                               port\n"
        ");\n"
        "\n"
        "endmodule\n";

    QTest::newRow("multiline description") << "Multiline\ndescription\nfor\nA" <<
        "module TestComponent(\n"
        "    // Interface: A\n"
        "    // Multiline\n"
        "    // description\n"
        "    // for\n"
        "    // A\n"
        "    input                               port\n"
        ");\n"
        "\n"
        "endmodule\n";
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testPortsOrderedByName()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testPortsOrderedByName()
{
    addPort("c", 1, DirectionTypes::IN);
    addPort("b", 1, DirectionTypes::IN);
    addPort("a", 1, DirectionTypes::IN);    

    writeComponent();

    QCOMPARE(outputString_, QString(
        "module TestComponent(\n"
        "    // These ports are not in any interface\n" 
        "    input                               a,\n"
        "    input                               b,\n"
        "    input                               c\n"
        ");\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::addPort()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::addPort(QString const& portName, int portSize,
    DirectionTypes::Direction direction)
{
    QSharedPointer<Port> port (new Port(portName, direction));
    port->setLeftBound(QString::number(portSize - 1));
    component_->getPorts()->append(port);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testPortsOrderedByDirectionThenName()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testPortsOrderedByDirectionThenName()
{
    addPort("a_in", 1, DirectionTypes::IN);
    addPort("x_in", 1, DirectionTypes::IN);
    addPort("a_out", 1, DirectionTypes::OUT);    
    addPort("y_out", 1, DirectionTypes::OUT);
    addPort("a_inout", 1, DirectionTypes::INOUT);
    addPort("z_inout", 1, DirectionTypes::INOUT);    

    writeComponent();

    QCOMPARE(outputString_, QString(
        "module TestComponent(\n"
        "    // These ports are not in any interface\n" 
        "    input                               a_in,\n" 
        "    input                               x_in,\n" 
        "    output                              a_out,\n" 
        "    output                              y_out,\n" 
        "    inout                               a_inout,\n" 
        "    inout                               z_inout\n"
        ");\n" 
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testPortsOrderedByInterfaceThenDirectionThenName()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testPortsOrderedByInterfaceThenDirectionThenName()
{
    addInterface("B");
    addPort("d_in", 1, DirectionTypes::IN);
    mapPortToInterface("d_in", "B");
    
    addInterface("A");
    addPort("a_in", 1, DirectionTypes::IN);
    addPort("b_out", 1, DirectionTypes::OUT);
    addPort("c_in", 1, DirectionTypes::IN);
    mapPortToInterface("a_in", "A");
    mapPortToInterface("b_out", "A");
    mapPortToInterface("c_in", "A");

    addPort("portInSeveralInterfaces", 1, DirectionTypes::IN);
    mapPortToInterface("portInSeveralInterfaces", "A");
    mapPortToInterface("portInSeveralInterfaces", "B");

    addPort("portInNoInterface", 1, DirectionTypes::IN);

    writeComponent();

    QCOMPARE(outputString_, QString(
        "module TestComponent(\n"
        "    // Interface: A\n" 
        "    input                               a_in,\n" 
        "    input                               c_in,\n" 
        "    output                              b_out,\n"
        "\n"
        "    // Interface: B\n" 
        "    input                               d_in,\n" 
        "\n"
        "    // There ports are contained in many interfaces\n" 
        "    input                               portInSeveralInterfaces,\n"

        "\n"
        "    // These ports are not in any interface\n" 
        "    input                               portInNoInterface\n"
        ");\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::addInterface()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::addInterface( QString const& interfaceName )
{
    QSharedPointer<BusInterface> busIf = QSharedPointer<BusInterface>(new BusInterface());
    busIf->setName(interfaceName);
    component_->getBusInterfaces()->append(busIf);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::mapPortToInterface()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::mapPortToInterface( QString const& portName, QString const& interfaceName )
{
    QSharedPointer<PortMap> portMap = QSharedPointer<PortMap>(new PortMap());

    QSharedPointer<PortMap::LogicalPort> theLogical (new PortMap::LogicalPort());
    theLogical->name_ = portName;
    QSharedPointer<PortMap::PhysicalPort> thePhysical (new PortMap::PhysicalPort());
    thePhysical->name_ = portName;
    portMap->setLogicalPort(theLogical);
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

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testComponentWithTypedPort()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testComponentWithModelParameters()
{
    addModuleParameter("freq", "5000",  "Description for freq.");
    addModuleParameter("dataWidth", "8");    

    writeComponent(component_->getViews()->first()->name());

    QCOMPARE(outputString_, QString(
        "module TestComponent #(\n"
        "    parameter                              freq             = 5000,    // Description for freq.\n"
        "    parameter                              dataWidth        = 8\n"
        ") ();\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::addModuleParameter()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::addModuleParameter(QString const& name, QString const& value, 
    QString const& description)
{
    if (component_->getComponentInstantiations()->isEmpty())
    {
        QSharedPointer<ComponentInstantiation> instantiation (new ComponentInstantiation("testInstantiation"));
        component_->getComponentInstantiations()->append(instantiation);
    }
    if (component_->getViews()->isEmpty())
    {
        QSharedPointer<View> testView (new View("testView"));
        testView->setComponentInstantiationRef(component_->getComponentInstantiations()->first()->name());
        component_->getViews()->append(testView);
    }

    QSharedPointer<ModuleParameter> parameter (new ModuleParameter());
    parameter->setName(name);
    parameter->setValue(value);
    parameter->setDescription(description);
    component_->getComponentInstantiations()->first()->getModuleParameters()->append(parameter);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testComponentWithTypedPort()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testParametersPrecedePorts()
{
    addPort("data", 8, DirectionTypes::OUT);
    addModuleParameter("dataWidth", "8");    

    writeComponent(component_->getViews()->first()->name());

    QCOMPARE(outputString_, QString(
        "module TestComponent #(\n"
        "    parameter                              dataWidth        = 8\n"
        ") (\n"
        "    // These ports are not in any interface\n" 
        "    output         [7:0]                data\n"
        ");\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testParametrizedPort()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testParametrizedPort()
{
    addModuleParameter("dataWidth", "8");    
    QString parameterId =
        component_->getComponentInstantiations()->first()->getModuleParameters()->first()->getValueId();

    QSharedPointer<Port> port = QSharedPointer<Port>(new Port("data", DirectionTypes::OUT));
    port->setLeftBound(parameterId + "-1");
    port->setRightBound("0");
    component_->getPorts()->append(port);

    writeComponent(component_->getViews()->first()->name());

    QCOMPARE(outputString_, QString(
        "module TestComponent #(\n"
        "    parameter                              dataWidth        = 8\n"
        ") (\n"
        "    // These ports are not in any interface\n" 
        "    output         [dataWidth-1:0]      data\n"
        ");\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testParameterizedModelParameter()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testParameterizedModelParameter()
{
    QSharedPointer<ModuleParameter> parameter (new ModuleParameter());
    parameter->setName("target");
    parameter->setValue("40");
    parameter->setValueId("TARGET-ID");

    QSharedPointer<ComponentInstantiation> instantiation (new ComponentInstantiation("parameterizedInstantiation"));
    instantiation->getModuleParameters()->append(parameter);

    QSharedPointer<View> testView (new View("parameterizedView"));
    testView->setComponentInstantiationRef(instantiation->name());

    component_->getViews()->append(testView);
    component_->getComponentInstantiations()->append(instantiation);

    addModuleParameter("referer", "TARGET-ID");

    writeComponent(component_->getViews()->first()->name());

    QCOMPARE(outputString_, QString(
        "module TestComponent #(\n"
        "    parameter                              target           = 40,\n"
        "    parameter                              referer          = target\n"
        ") ();\n"
        "\n"
        "endmodule\n"));
}


QTEST_APPLESS_MAIN(tst_ComponentVerilogWriter)

#include "tst_ComponentVerilogWriter.moc"
