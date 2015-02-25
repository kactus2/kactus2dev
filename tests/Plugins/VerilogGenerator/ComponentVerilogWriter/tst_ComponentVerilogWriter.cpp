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

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/PortMap.h>
#include <IPXACTmodels/vlnv.h>
#include <IPXACTmodels/kactusExtensions/ModuleParameter.h>

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

    void testModuleParameterIsWrittenAfterModelParameter();

private:
    void writeComponent(QString const& activeView = QString()); 

    void addPort(QString const& portName, int portSize, General::Direction direction);

    void addModelParameter(QString const& name, QString const& value, QString description = "");

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
    addPort("a", 1,  General::IN);
    component_->getPort("a")->setDescription("Description for a.");
    addPort("b", 1, General::IN);
    addPort("c", 1, General::IN);
    component_->getPort("c")->setDescription("Description for c.");
        
    writeComponent();

    QCOMPARE(outputString_, 
        QString("module TestComponent(\n"
        "    // These ports are not in any interface\n" 
        "    input                 a,    // Description for a.\n"
        "    input                 b,\n"
        "    input                 c    // Description for c.\n"
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
    
    addPort("port", 1 , General::IN);
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
        "    input                 port\n"
        ");\n"
        "\n"
        "endmodule\n";

    QTest::newRow("basic description") << "Description of A" <<
        "module TestComponent(\n"
        "    // Interface: A\n"
        "    // Description of A\n"
        "    input                 port\n"
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
        "    input                 port\n"
        ");\n"
        "\n"
        "endmodule\n";
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testPortsOrderedByName()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testPortsOrderedByName()
{
    addPort("c", 1, General::IN);
    addPort("b", 1, General::IN);
    addPort("a", 1, General::IN);    

    writeComponent();

    QCOMPARE(outputString_, QString(
        "module TestComponent(\n"
        "    // These ports are not in any interface\n" 
        "    input                 a,\n"
        "    input                 b,\n"
        "    input                 c\n"
        ");\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::addPort()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::addPort( QString const& portName, int portSize, General::Direction direction )
{
    QSharedPointer<Port> port = QSharedPointer<Port>(new Port(portName, direction, portSize - 1, 0, "", true));
    component_->addPort(port);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testPortsOrderedByDirectionThenName()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testPortsOrderedByDirectionThenName()
{
    addPort("a_in", 1, General::IN);
    addPort("x_in", 1, General::IN);
    addPort("a_out", 1, General::OUT);    
    addPort("y_out", 1, General::OUT);
    addPort("a_inout", 1, General::INOUT);
    addPort("z_inout", 1, General::INOUT);    

    writeComponent();

    QCOMPARE(outputString_, QString(
        "module TestComponent(\n"
        "    // These ports are not in any interface\n" 
        "    input                 a_in,\n" 
        "    input                 x_in,\n" 
        "    output                a_out,\n" 
        "    output                y_out,\n" 
        "    inout                 a_inout,\n" 
        "    inout                 z_inout\n"
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
    addPort("d_in", 1, General::IN);
    mapPortToInterface("d_in", "B");
    
    addInterface("A");
    addPort("a_in", 1, General::IN);
    addPort("b_out", 1, General::OUT);
    addPort("c_in", 1, General::IN);
    mapPortToInterface("a_in", "A");
    mapPortToInterface("b_out", "A");
    mapPortToInterface("c_in", "A");

    addPort("portInSeveralInterfaces", 1, General::IN);
    mapPortToInterface("portInSeveralInterfaces", "A");
    mapPortToInterface("portInSeveralInterfaces", "B");

    addPort("portInNoInterface", 1, General::IN);

    writeComponent();

    QCOMPARE(outputString_, QString(
        "module TestComponent(\n"
        "    // Interface: A\n" 
        "    input                 a_in,\n" 
        "    input                 c_in,\n" 
        "    output                b_out,\n"
        "\n"
        "    // Interface: B\n" 
        "    input                 d_in,\n" 
        "\n"
        "    // There ports are contained in many interfaces\n" 
        "    input                 portInSeveralInterfaces,\n"

        "\n"
        "    // These ports are not in any interface\n" 
        "    input                 portInNoInterface\n"
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
    component_->addBusInterface(busIf);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::mapPortToInterface()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::mapPortToInterface( QString const& portName, QString const& interfaceName )
{
    QSharedPointer<PortMap> portMap = QSharedPointer<PortMap>(new PortMap());
    portMap->setLogicalPort(portName.toUpper());
    portMap->setPhysicalPort(portName);

    QList<QSharedPointer<PortMap> >& portMapList = component_->getBusInterface(interfaceName)->getPortMaps();
    portMapList.append(portMap);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testComponentWithTypedPort()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testComponentWithModelParameters()
{
    addModelParameter("freq", "5000",  "Description for freq.");
    addModelParameter("dataWidth", "8");    

    writeComponent();

    QCOMPARE(outputString_, QString(
        "module TestComponent #(\n"
        "    parameter         freq             = 5000,    // Description for freq.\n"
        "    parameter         dataWidth        = 8\n"
        ") ();\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::addModelParameter()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::addModelParameter(QString const& name, QString const& value, QString description)
{
    QSharedPointer<ModelParameter> parameter = QSharedPointer<ModelParameter>(new ModelParameter());
    parameter->setName(name);
    parameter->setValue(value);
    parameter->setDescription(description);
    component_->getModel()->addModelParameter(parameter);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testComponentWithTypedPort()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testParametersPrecedePorts()
{
    addPort("data", 8, General::OUT);
    addModelParameter("dataWidth", "8");    

    writeComponent();

    QCOMPARE(outputString_, QString(
        "module TestComponent #(\n"
        "    parameter         dataWidth        = 8\n"
        ") (\n"
        "    // These ports are not in any interface\n" 
        "    output         [7:0]  data\n"
        ");\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testParametrizedPort()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testParametrizedPort()
{
    addModelParameter("dataWidth", "8");    
    QString parameterId = component_->getModelParameters()->first()->getValueId();

    QSharedPointer<Port> port = QSharedPointer<Port>(new Port("data", General::OUT, 0, 0, "", true));
    port->setLeftBoundExpression(parameterId + "-1");
    port->setRightBoundExpression("0");
    component_->addPort(port);

    writeComponent();

    QCOMPARE(outputString_, QString(
        "module TestComponent #(\n"
        "    parameter         dataWidth        = 8\n"
        ") (\n"
        "    // These ports are not in any interface\n" 
        "    output         [dataWidth-1:0] data\n"
        ");\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testParameterizedModelParameter()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testParameterizedModelParameter()
{
    QSharedPointer<ModelParameter> parameter(new ModelParameter());
    parameter->setName("target");
    parameter->setValue("40");
    parameter->setValueId("TARGET-ID");
    component_->getModel()->addModelParameter(parameter);

    addModelParameter("referer", "TARGET-ID");

    writeComponent();

    QCOMPARE(outputString_, QString(
        "module TestComponent #(\n"
        "    parameter         target           = 40,\n"
        "    parameter         referer          = target\n"
        ") ();\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentVerilogWriter::testModuleParameterIsWrittenAfterModelParameter()
//-----------------------------------------------------------------------------
void tst_ComponentVerilogWriter::testModuleParameterIsWrittenAfterModelParameter()
{
    addModelParameter("modelParameter", "0");

    QSharedPointer<ModuleParameter> moduleParameter(new ModuleParameter());
    moduleParameter->setName("moduleParameter1");
    moduleParameter->setValue("1");
    
    View* activeView = new View();
    activeView->setName("rtl");
    activeView->getModuleParameters()->append(moduleParameter);
    component_->addView(activeView);

    writeComponent("rtl");

    QCOMPARE(outputString_, QString(
        "module TestComponent #(\n"
        "    parameter         modelParameter   = 0,\n"
        "    parameter         moduleParameter1 = 1\n"
        ") ();\n"
        "\n"
        "endmodule\n"));
}

QTEST_APPLESS_MAIN(tst_ComponentVerilogWriter)

#include "tst_ComponentVerilogWriter.moc"
