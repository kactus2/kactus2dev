//-----------------------------------------------------------------------------
// File: tst_VerilogGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Unit test for class VerilogGenerator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/ComponentInstance.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/vlnv.h>

#include <Plugins/VerilogGenerator/VerilogGenerator.h>

const QString INDENT = "    ";

class tst_VerilogGenerator : public QObject
{
    Q_OBJECT

public:
    tst_VerilogGenerator();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test cases:
    void testEmptyOutputIfNoComponentParsed();
    void testNamedModule();
    void testModuleWithMultiplePorts();
	void testModulePortsOrderedByInterface();

    void runGenerator(); 


    void testComponentWithModelParameters();
    void testComponentInstantiations();
    void testHierarchicalConnection();
    void testConsecutiveParseCalls();

private:    
    void addPort(QString const& portName, int portSize, General::Direction direction);

    void addModelParameter(QString const& name, QString const& value);

    void compareLineByLine(QString const& expectedOutput);

    void addInterface( QString const& interfaceName );

    void mapPortToInterface( QString const& portName, QString const& interfaceName );

    //! The top level component for which the generator is run.
    QSharedPointer<Component> component_;
    
    //! The generator output as a string.
    QString outputString_;

    //! The generator output stream.
    QTextStream outputStream_;
};

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::tst_VerilogGenerator()
//-----------------------------------------------------------------------------
tst_VerilogGenerator::tst_VerilogGenerator(): component_(), outputString_(), outputStream_()
{

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::initTestCase()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::init()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::init()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    component_ = QSharedPointer<Component>(new Component(vlnv));

    outputStream_.setString(&outputString_);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::cleanup()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::cleanup()
{
    component_.clear();

    outputStream_.reset();
    outputString_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testEmptyComponent()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testEmptyOutputIfNoComponentParsed()
{
    VerilogGenerator generator;

    generator.write(outputStream_);

    QCOMPARE(outputString_, QString());
}


//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testNamedComponent()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testNamedModule()
{
    runGenerator();

    compareLineByLine(QString("module TestComponent();\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::runGenerator()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::runGenerator()
{
    VerilogGenerator generator;
    generator.parse(component_);
    generator.write(outputStream_);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testComponentWithMultiplePorts()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testModuleWithMultiplePorts()
{
	addPort("clk", 1, General::IN);
    addPort("reset", 1, General::IN);
    addPort("data", 8, General::OUT);
    
    addPort("debug", 2, General::OUT);
    component_->getPort("debug")->setTypeName("integer");

    addPort("enable", 1, General::INOUT);
    component_->getPort("enable")->setTypeName("tri");
    
    runGenerator();

    compareLineByLine(QString("module TestComponent(clk, reset, data, debug, enable);\n" +
        INDENT + "input clk;\n" +
        INDENT + "input reset;\n" +
        INDENT + "output [7:0] data;\n" +
        INDENT + "output integer [1:0] debug;\n" +
        INDENT + "inout tri enable;\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addPort()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addPort( QString const& portName, int portSize, General::Direction direction )
{
    QSharedPointer<Port> port = QSharedPointer<Port>(new Port(portName, direction, portSize - 1, 0, "", true));
    component_->addPort(port);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testComponentPortsOrderedByInterface()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testModulePortsOrderedByInterface()
{
    addPort("portInNoInterface", 1, General::IN);

    addPort("portInLastInterface", 1, General::IN);
    addInterface("ZisLastAphabetically");
    mapPortToInterface("portInLastInterface", "ZisLastAphabetically");

    addPort("portInFirstInterface", 1, General::IN);
    addInterface("AisFirstAlphabetically");
    mapPortToInterface("portInFirstInterface", "AisFirstAlphabetically");

    addPort("portInSeveralInterfaces", 1, General::IN);
    addInterface("AisFirstAlphabetically");
    mapPortToInterface("portInSeveralInterfaces", "AisFirstAlphabetically");

    runGenerator();

    compareLineByLine(QString("module TestComponent(portInFirstInterface, portInLastInterface, portInNoInterface, portInSeveralInterfaces);\n" +
        INDENT + "input portInFirstInterface;\n" +
        INDENT + "input portInLastInterface;\n" +        
        INDENT + "input portInNoInterface;\n" +
        INDENT + "input portInSeveralInterfaces;\n" +
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addInterface()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addInterface( QString const& interfaceName )
{
    QSharedPointer<BusInterface> busIf = QSharedPointer<BusInterface>(new BusInterface());
    busIf->setName(interfaceName);
    component_->addBusInterface(busIf);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::mapPortToInterface()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::mapPortToInterface( QString const& portName, QString const& interfaceName )
{
    QSharedPointer<General::PortMap> portMap = QSharedPointer<General::PortMap>(new General::PortMap());
    portMap->logicalPort_ = portName.toUpper();
    portMap->physicalPort_ = portName;

    QList<QSharedPointer<General::PortMap> > portMapList = component_->getBusInterface(interfaceName)->getPortMaps();
    portMapList.append(portMap);    
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testComponentWithTypedPort()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testComponentWithModelParameters()
{
    addModelParameter("freq", "5000");
    addModelParameter("dataWidth", "8");

    runGenerator();

    compareLineByLine(QString("module TestComponent();\n" +
        INDENT + "parameter freq = 5000;\n" +
        INDENT + "parameter dataWidth = 8;\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addModelParameter( QString const& name, QString const& value )
{
    QSharedPointer<ModelParameter> parameter = QSharedPointer<ModelParameter>(new ModelParameter());
    parameter->setName(name);
    parameter->setValue(value);
    component_->getModel()->addModelParameter(parameter);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testComponentIstantiation()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testComponentInstantiations()
{
    QSharedPointer<Port> port = QSharedPointer<Port>(new Port("clk", General::IN, 0, 0, "", true));
    component_->addPort(port);

    VLNV subcomponentVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "SubComponent", "1.0");
    ComponentInstance subcomponentInstance("instance1", "", "", subcomponentVLNV, QPoint(), "");   
    ComponentInstance subcomponentInstance2("instance2", "", "", subcomponentVLNV, QPoint(), "");   

    VLNV designVLNV(VLNV::DESIGN, "Test", "TestLibrary", "TopComponent.des", "1.0");
    QSharedPointer<Design> topDesign = QSharedPointer<Design>(new Design(designVLNV));

    QList<ComponentInstance> instances;
    instances.append(subcomponentInstance);
    instances.append(subcomponentInstance2);

    topDesign->setComponentInstances(instances);

    VerilogGenerator generator;

    generator.parse(component_, topDesign);
    generator.write(outputStream_);

    compareLineByLine(QString("module TestComponent(clk);\n" +
        INDENT + "input clk;\n" +
        "\n" +
        INDENT + "instance1 SubComponent();\n" +
        "\n" +
        INDENT + "instance2 SubComponent();\n" +
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testComponentIstantiation()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testHierarchicalConnection()
{
    addPort("clk", 1, General::IN);

    VLNV subcomponentVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "SubComponent", "1.0");
    QSharedPointer<Component> subComponent = QSharedPointer<Component>(new Component(subcomponentVLNV));
    

    QSharedPointer<Port> instancePort = QSharedPointer<Port>(new Port("clk_in", General::IN, 0, 0, "", true));
    subComponent->addPort(instancePort);

    ComponentInstance subcomponentInstance("instance1", "", "", subcomponentVLNV, QPoint(), "");   

    VLNV designVLNV(VLNV::DESIGN, "Test", "TestLibrary", "TopComponent.des", "1.0");
    QSharedPointer<Design> topDesign = QSharedPointer<Design>(new Design(designVLNV));

    QList<ComponentInstance> instances;
    instances.append(subcomponentInstance);

    topDesign->setComponentInstances(instances);

    HierConnection connection("topClock", Interface(subcomponentVLNV.toString(), "clk_if"));    
    
    QList<HierConnection> hierConnections;
    hierConnections.append(connection);

    topDesign->setHierarchicalConnections(hierConnections);

    VerilogGenerator generator;

    generator.parse(component_, topDesign);
    generator.write(outputStream_);

    compareLineByLine(QString("module TestComponent(clk);\n" +
        INDENT + "input clk;\n" +
        "\n" +
        INDENT + "instance1 SubComponent();\n" +
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testConsecutiveParseCalls()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testConsecutiveParseCalls()
{
    VLNV secondVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "Override", "1.0");
    QSharedPointer<Component> secondComponent = QSharedPointer<Component>(new Component());
    secondComponent->setVlnv(secondVlnv);

    VerilogGenerator generator;

    generator.parse(component_);
    generator.parse(secondComponent);

    generator.write(outputStream_);

    compareLineByLine(QString("module Override();\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::compareLineByLine()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::compareLineByLine(QString const& expectedOutput)
{
    QStringList outputLines = outputString_.split("\n");
    QStringList expectedLines = expectedOutput.split("\n");

    int lineCount = qMin(outputLines.count(), expectedLines.count());
    for (int i = 0; i < lineCount; i++)
    {
        QCOMPARE(outputLines.at(i), expectedLines.at(i));
    }
}



QTEST_APPLESS_MAIN(tst_VerilogGenerator)

#include "tst_VerilogGenerator.moc"
