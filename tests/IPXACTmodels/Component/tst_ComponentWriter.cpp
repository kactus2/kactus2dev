//-----------------------------------------------------------------------------
// File: tst_ComponentWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.10.2015
//
// Description:
// Unit test for class ComponentWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/ComponentWriter.h>
#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Component/businterface.h>
#include <IPXACTmodels/Component/channel.h>
#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/ComponentGenerator.h>
#include <IPXACTmodels/Component/choice.h>
#include <IPXACTmodels/common/Enumeration.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/cpu.h>
#include <IPXACTmodels/Component/OtherClockDriver.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/Assertion.h>

#include <IPXACTmodels/kactusExtensions/SWView.h>
#include <IPXACTmodels/kactusExtensions/ComProperty.h>
#include <IPXACTmodels/kactusExtensions/SystemView.h>
#include <IPXACTmodels/kactusExtensions/ComInterface.h>
#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/FileDependency.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QtTest>

class tst_ComponentWriter : public QObject
{
    Q_OBJECT

public:
    tst_ComponentWriter();

private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void writeSimpleComponent();

    void writeXMLProcessingInstructions();

    void writeBusInterfaces();
    void writeChannels();
    void writeRemapStates();
    void writeAddressSpace();
    void writeMemoryMap();

    void writeView();
    void writeInstantiations();
    void writePorts();

    void writeComponentGenerators();
    void writeChoices();
    void writeFileSets();
    void writeCPUs();
    void writeOtherClockDrivers();

    void writeParameters();
    void writeAssertions();
    void writeVendorExtensions();

    void writeKactusAttributes();
    void writeSwViews();
    void writeSwProperties();
    void writeSystemViews();
    void writeComInterfaces();
    void writeApiInterfaces();
    void writeFileDependencies();

private:

    QSharedPointer<Component> testComponent_;
};

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::tst_ComponentWriter()
//-----------------------------------------------------------------------------
tst_ComponentWriter::tst_ComponentWriter() :
testComponent_()
{

}


//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::initTestCase()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::init()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::init()
{
    VLNV componentVLNV(VLNV::COMPONENT, "TUT", "TestLibrary", "TestComponent", "0.11");
    testComponent_ = QSharedPointer<Component>(new Component(componentVLNV));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::cleanup()
{
    testComponent_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeSimpleComponent()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeSimpleComponent()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    testComponent_->setDescription("coreDrill");

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:description>coreDrill</ipxact:description>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeXMLProcessingInstructions()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeXMLProcessingInstructions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    testComponent_->addXmlProcessingInstructions("xml-stylesheet", "href=\"style.css\"");

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<?xml-stylesheet href=\"style.css\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeBusInterfaces()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeBusInterfaces()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<BusInterface> busInterface (new BusInterface());
    busInterface->setName("testInterface");
    busInterface->setInterfaceMode(General::MASTER);
    busInterface->setBusType(VLNV(VLNV::BUSDEFINITION,"TUT","TestLibrary","busDefinition","0.2"));

    testComponent_->getBusInterfaces()->append(busInterface);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:busInterfaces>\n"
                "\t\t<ipxact:busInterface>\n"
                    "\t\t\t<ipxact:name>testInterface</ipxact:name>\n"
                    "\t\t\t<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"busDefinition\""
                        " version=\"0.2\"/>\n"
                    "\t\t\t<ipxact:master/>\n"
                "\t\t</ipxact:busInterface>\n"
            "\t</ipxact:busInterfaces>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeChannels()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeChannels()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QStringList busInterfaceRefs;
    busInterfaceRefs.append("interfaceOne");
    busInterfaceRefs.append("interfaceTwo");

    QSharedPointer<Channel> testChannel(new Channel());
    testChannel->setName("testChannel");
    testChannel->setInterfaces(busInterfaceRefs);

    testComponent_->getChannels()->append(testChannel);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
        "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
        "\t<ipxact:library>TestLibrary</ipxact:library>\n"
        "\t<ipxact:name>TestComponent</ipxact:name>\n"
        "\t<ipxact:version>0.11</ipxact:version>\n"
        "\t<ipxact:channels>\n"
            "\t\t<ipxact:channel>\n"
                "\t\t\t<ipxact:name>testChannel</ipxact:name>\n"
                "\t\t\t<ipxact:busInterfaceRef>\n"
                    "\t\t\t\t<ipxact:localName>interfaceOne</ipxact:localName>\n"
                "\t\t\t</ipxact:busInterfaceRef>\n"
                "\t\t\t<ipxact:busInterfaceRef>\n"
                    "\t\t\t\t<ipxact:localName>interfaceTwo</ipxact:localName>\n"
                "\t\t\t</ipxact:busInterfaceRef>\n"
            "\t\t</ipxact:channel>\n"
        "\t</ipxact:channels>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeRemapStates()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeRemapStates()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<RemapState> remapState (new RemapState("remap"));

    testComponent_->getRemapStates()->append(remapState);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
        "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
        "\t<ipxact:library>TestLibrary</ipxact:library>\n"
        "\t<ipxact:name>TestComponent</ipxact:name>\n"
        "\t<ipxact:version>0.11</ipxact:version>\n"
        "\t<ipxact:remapStates>\n"
            "\t\t<ipxact:remapState>\n"
                "\t\t\t<ipxact:name>remap</ipxact:name>\n"
            "\t\t</ipxact:remapState>\n"
        "\t</ipxact:remapStates>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeAddressSpace()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeAddressSpace()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<AddressSpace> testAddressSpace (new AddressSpace());
    testAddressSpace->setName("StarControl");
    testAddressSpace->setRange("coreward");
    testAddressSpace->setWidth("front");

    testComponent_->getAddressSpaces()->append(testAddressSpace);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:addressSpaces>\n"
                "\t\t<ipxact:addressSpace>\n"
                    "\t\t\t<ipxact:name>StarControl</ipxact:name>\n"
                    "\t\t\t<ipxact:range>coreward</ipxact:range>\n"
                    "\t\t\t<ipxact:width>front</ipxact:width>\n"
                "\t\t</ipxact:addressSpace>\n"
            "\t</ipxact:addressSpaces>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeMemoryMap()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeMemoryMap()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<MemoryMap> testMemoryMap (new MemoryMap("memoryMap"));

    testComponent_->getMemoryMaps()->append(testMemoryMap);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:memoryMaps>\n"
                "\t\t<ipxact:memoryMap>\n"
                    "\t\t\t<ipxact:name>memoryMap</ipxact:name>\n"
                "\t\t</ipxact:memoryMap>\n"
            "\t</ipxact:memoryMaps>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeView()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeView()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<View> testView (new View("testView"));

    testComponent_->getViews()->append(testView);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:model>\n"
                "\t\t<ipxact:views>\n"
                    "\t\t\t<ipxact:view>\n"
                        "\t\t\t\t<ipxact:name>testView</ipxact:name>\n"
                    "\t\t\t</ipxact:view>\n"
                "\t\t</ipxact:views>\n"
            "\t</ipxact:model>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeComponentInstantiations()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeInstantiations()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<ComponentInstantiation> componentInstantiation (new ComponentInstantiation("component"));

    QSharedPointer<ConfigurableVLNVReference> designReference (
        new ConfigurableVLNVReference(VLNV::DESIGN, "TUT", "TestLibrary", "designReference", "1.0"));

    QSharedPointer<DesignInstantiation> designInstantiation (new DesignInstantiation("design"));
    designInstantiation->setDesignReference(designReference);

    QSharedPointer<ConfigurableVLNVReference> configurationReference
        (new ConfigurableVLNVReference(VLNV::DESIGNCONFIGURATION, "TUT", "TestLibrary", "configuration", "8.1"));

    QSharedPointer<DesignConfigurationInstantiation> testDesignConfigurationInstantiation
        (new DesignConfigurationInstantiation("designConfiguration"));
    testDesignConfigurationInstantiation->setDesignConfigurationReference(configurationReference);

    testComponent_->getComponentInstantiations()->append(componentInstantiation);
    testComponent_->getDesignInstantiations()->append(designInstantiation);
    testComponent_->getDesignConfigurationInstantiations()->append(testDesignConfigurationInstantiation);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:model>\n"
                "\t\t<ipxact:instantiations>\n"
                    "\t\t\t<ipxact:componentInstantiation>\n"
                        "\t\t\t\t<ipxact:name>component</ipxact:name>\n"
                    "\t\t\t</ipxact:componentInstantiation>\n"
                    "\t\t\t<ipxact:designInstantiation>\n"
                        "\t\t\t\t<ipxact:name>design</ipxact:name>\n"
                        "\t\t\t\t<ipxact:designRef vendor=\"TUT\" library=\"TestLibrary\""
                        " name=\"designReference\" version=\"1.0\"/>\n"
                    "\t\t\t</ipxact:designInstantiation>\n"
                    "\t\t\t<ipxact:designConfigurationInstantiation>\n"
                        "\t\t\t\t<ipxact:name>designConfiguration</ipxact:name>\n"
                        "\t\t\t\t<ipxact:designConfigurationRef vendor=\"TUT\" library=\"TestLibrary\""
                        " name=\"configuration\" version=\"8.1\"/>\n"
                    "\t\t\t</ipxact:designConfigurationInstantiation>\n"
                "\t\t</ipxact:instantiations>\n"
            "\t</ipxact:model>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writePorts()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writePorts()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<Port> testPort (new Port("testPort"));

    testComponent_->getPorts()->append(testPort);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:model>\n"
                "\t\t<ipxact:ports>\n"
                    "\t\t\t<ipxact:port>\n"
                        "\t\t\t\t<ipxact:name>testPort</ipxact:name>\n"
                        "\t\t\t\t<ipxact:wire>\n"
                            "\t\t\t\t\t<ipxact:direction>in</ipxact:direction>\n"
                        "\t\t\t\t</ipxact:wire>\n"
                    "\t\t\t</ipxact:port>\n"
                "\t\t</ipxact:ports>\n"
            "\t</ipxact:model>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeComponentGenerators()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeComponentGenerators()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<ComponentGenerator> testGenerator(new ComponentGenerator());
    testGenerator->setName("generator");
    testGenerator->setGeneratorExe("saMatra");

    testComponent_->getComponentGenerators()->append(testGenerator);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:componentGenerators>\n"
                "\t\t<ipxact:componentGenerator>\n"
                    "\t\t\t<ipxact:name>generator</ipxact:name>\n"
                    "\t\t\t<ipxact:generatorExe>saMatra</ipxact:generatorExe>\n"
                "\t\t</ipxact:componentGenerator>\n"
            "\t</ipxact:componentGenerators>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeChoices()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeChoices()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<Enumeration> choiceEnumeration(new Enumeration());
    choiceEnumeration->setValue("14");

    QSharedPointer<Choice> testChoice (new Choice());
    testChoice->setName("chosenOne");
    testChoice->enumerations()->append(choiceEnumeration);

    testComponent_->getChoices()->append(testChoice);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:choices>\n"
                "\t\t<ipxact:choice>\n"
                    "\t\t\t<ipxact:name>chosenOne</ipxact:name>\n"
                    "\t\t\t<ipxact:enumeration>14</ipxact:enumeration>\n"
                "\t\t</ipxact:choice>\n"
            "\t</ipxact:choices>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeFileSets()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeFileSets()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<FileSet> fileSet (new FileSet("urquanMasters"));

    testComponent_->getFileSets()->append(fileSet);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:fileSets>\n"
                "\t\t<ipxact:fileSet>\n"
                    "\t\t\t<ipxact:name>urquanMasters</ipxact:name>\n"
                "\t\t</ipxact:fileSet>\n"
            "\t</ipxact:fileSets>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeCPUs()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeCPUs()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<Cpu::AddressSpaceRef> reference (new Cpu::AddressSpaceRef("Avatar"));

    QSharedPointer<Cpu> testCPU (new Cpu());
    testCPU->setName("Chmmr");
    testCPU->getAddressSpaceReferences()->append(reference);

    testComponent_->getCpus()->append(testCPU);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:cpus>\n"
                "\t\t<ipxact:cpu>\n"
                    "\t\t\t<ipxact:name>Chmmr</ipxact:name>\n"
                    "\t\t\t<ipxact:addressSpaceRef addressSpaceRef=\"Avatar\"/>\n"
                "\t\t</ipxact:cpu>\n"
            "\t</ipxact:cpus>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeOtherClockDrivers()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeOtherClockDrivers()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<ClockUnit> clockPeriod(new ClockUnit("watch"));
    QSharedPointer<ClockUnit> clockPulseOffset(new ClockUnit("Laser"));
    QSharedPointer<ClockUnit> clockPulseDuration(new ClockUnit("eternity"));

    QSharedPointer<OtherClockDriver> clockDriver (new OtherClockDriver());
    clockDriver->setClockName("clockName");
    clockDriver->setClockPeriod(clockPeriod);
    clockDriver->setClockPulseOffset(clockPulseOffset);
    clockDriver->setClockPulseValue("KingOsrik");
    clockDriver->setClockPulseDuration(clockPulseDuration);

    testComponent_->getOtherClockDrivers()->append(clockDriver);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:otherClockDrivers>\n"
                "\t\t<ipxact:otherClockDriver clockName=\"clockName\">\n"
                    "\t\t\t<ipxact:clockPeriod>watch</ipxact:clockPeriod>\n"
                    "\t\t\t<ipxact:clockPulseOffset>Laser</ipxact:clockPulseOffset>\n"
                    "\t\t\t<ipxact:clockPulseValue>KingOsrik</ipxact:clockPulseValue>\n"
                    "\t\t\t<ipxact:clockPulseDuration>eternity</ipxact:clockPulseDuration>\n"
                "\t\t</ipxact:otherClockDriver>\n"
            "\t</ipxact:otherClockDrivers>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeParameters()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeParameters()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setValueId("testID");
    testParameter->setName("testParameter");
    testParameter->setValue("1");

    testComponent_->getParameters()->append(testParameter);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:parameters>\n"
                "\t\t<ipxact:parameter parameterId=\"testID\">\n"
                    "\t\t\t<ipxact:name>testParameter</ipxact:name>\n"
                    "\t\t\t<ipxact:value>1</ipxact:value>\n"
                "\t\t</ipxact:parameter>\n"
            "\t</ipxact:parameters>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeAssertions()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeAssertions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<Assertion> testAssertion(new Assertion());
    testAssertion->setName("testAssertion");
    testAssertion->setDisplayName("assertionDisplay");
    testAssertion->setDescription("assertionDescription");
    testAssertion->setAssert("13");

    testComponent_->getAssertions()->append(testAssertion);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:assertions>\n"
                "\t\t<ipxact:assertion>\n"
                    "\t\t\t<ipxact:name>testAssertion</ipxact:name>\n"
                    "\t\t\t<ipxact:displayName>assertionDisplay</ipxact:displayName>\n"
                    "\t\t\t<ipxact:description>assertionDescription</ipxact:description>\n"
                    "\t\t\t<ipxact:assert>13</ipxact:assert>\n"
                "\t\t</ipxact:assertion>\n"
            "\t</ipxact:assertions>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeVendorExtensions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    testComponent_->getVendorExtensions()->append(testExtension);
    testComponent_->setVersion("3.0.0");

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:vendorExtensions>\n"
                "\t\t<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>\n"
                "\t\t<kactus2:version>3.0.0</kactus2:version>\n"
            "\t</ipxact:vendorExtensions>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeKactusAttributes()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeKactusAttributes()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testComponent_->setHierarchy(KactusAttribute::IP);
    testComponent_->setImplementation(KactusAttribute::SYSTEM);
    testComponent_->setFirmness(KactusAttribute::FIXED);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>"
            "<ipxact:component "
            "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
            "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
            "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
            "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
            "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
                "<ipxact:vendor>TUT</ipxact:vendor>"
                "<ipxact:library>TestLibrary</ipxact:library>"
                "<ipxact:name>TestComponent</ipxact:name>"
                "<ipxact:version>0.11</ipxact:version>"
                "<ipxact:vendorExtensions>"   
                    "<kactus2:kts_attributes>"
                        "<kactus2:kts_productHier>IP</kactus2:kts_productHier>"
                        "<kactus2:kts_implementation>SYS</kactus2:kts_implementation>"
                        "<kactus2:kts_firmness>Fixed</kactus2:kts_firmness>"
                    "</kactus2:kts_attributes>"
                "</ipxact:vendorExtensions>"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeSwViews()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeSwViews()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    VLNV hierarchyVLNV(VLNV::COMPONENT, "TUT", "TestLibrary", "hierarchy", "0.3");

    QSharedPointer<SWView> testSW (new SWView("swView"));
    testSW->setDisplayName("displayed");
    testSW->setDescription("described");
    testSW->addFileSetRef("fileSet");
    testSW->setHierarchyRef(hierarchyVLNV);

    QList<QSharedPointer<SWView> > swViewList;
    swViewList.append(testSW);

    testComponent_->setVersion("3.0.0");
    testComponent_->setSWViews(swViewList);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:vendorExtensions>\n"
                "\t\t<kactus2:version>3.0.0</kactus2:version>\n"
                "\t\t<kactus2:swViews>\n"
                    "\t\t\t<kactus2:swView>\n"
                        "\t\t\t\t<ipxact:name>swView</ipxact:name>\n"
                        "\t\t\t\t<ipxact:displayName>displayed</ipxact:displayName>\n"
                        "\t\t\t\t<ipxact:description>described</ipxact:description>\n"
                        "\t\t\t\t<kactus2:hierarchyRef vendor=\"TUT\" library=\"TestLibrary\" name=\"hierarchy\" "
                            "version=\"0.3\"/>\n"
                        "\t\t\t\t<kactus2:fileSetRef>fileSet</kactus2:fileSetRef>\n"
                    "\t\t\t</kactus2:swView>\n"
                "\t\t</kactus2:swViews>\n"
            "\t</ipxact:vendorExtensions>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeSwProperties()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeSwProperties()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<ComProperty> swProperty (new ComProperty());
    swProperty->setName("Priority");
    swProperty->setRequired(true);
    swProperty->setType("override.");
    swProperty->setDefaultValue("New behaviour");
    swProperty->setDescription("dictated");

    QList<QSharedPointer<ComProperty> > properties;
    properties.append(swProperty);

    testComponent_->setVersion("3.0.0");
    testComponent_->setSWProperties(properties);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:vendorExtensions>\n"
                "\t\t<kactus2:version>3.0.0</kactus2:version>\n"
                "\t\t<kactus2:properties>\n"
                    "\t\t\t<kactus2:property name=\"Priority\" required=\"true\" propertyType=\"override.\""
                        " defaultValue=\"New behaviour\" description=\"dictated\"/>\n"
                "\t\t</kactus2:properties>\n"
            "\t</ipxact:vendorExtensions>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeSystemViews()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeSystemViews()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    VLNV hierarchyVLNV(VLNV::COMPONENT, "TUT", "TestLibrary", "hierarchy", "0.3");

    QStringList fileSetReferences;
    fileSetReferences.append("burn");

    QSharedPointer<SystemView> testSystem (new SystemView("system"));
    testSystem->setDisplayName("crash");
    testSystem->setDescription("BSOD");
    testSystem->setHierarchyRef(hierarchyVLNV);
    testSystem->setHWViewRef("malfunction");
    testSystem->setFileSetRefs(fileSetReferences);

    QList<QSharedPointer<SystemView> > systemViews;
    systemViews.append(testSystem);

    testComponent_->setVersion("3.0.0");
    testComponent_->setSystemViews(systemViews);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:vendorExtensions>\n"
                "\t\t<kactus2:version>3.0.0</kactus2:version>\n"
                "\t\t<kactus2:systemViews>\n"
                    "\t\t\t<kactus2:systemView>\n"
                        "\t\t\t\t<ipxact:name>system</ipxact:name>\n"
                        "\t\t\t\t<ipxact:displayName>crash</ipxact:displayName>\n"
                        "\t\t\t\t<ipxact:description>BSOD</ipxact:description>\n"
                        "\t\t\t\t<kactus2:hierarchyRef vendor=\"TUT\" library=\"TestLibrary\" name=\"hierarchy\" "
                            "version=\"0.3\"/>\n"
                        "\t\t\t\t<kactus2:hwViewRef>malfunction</kactus2:hwViewRef>\n"
                        "\t\t\t\t<kactus2:fileSetRef>burn</kactus2:fileSetRef>\n"
                    "\t\t\t</kactus2:systemView>\n"
                "\t\t</kactus2:systemViews>\n"
            "\t</ipxact:vendorExtensions>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeComInterfaces()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeComInterfaces()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    VLNV hierarchyVLNV(VLNV::COMPONENT, "TUT", "TestLibrary", "hierarchy", "0.3");

    QMap<QString, QString> propertyValues;
    propertyValues.insert("spathi", "eluder");

    QSharedPointer<ComInterface> testComInterface (new ComInterface());
    testComInterface->setName("coms");
    testComInterface->setDisplayName("spec");
    testComInterface->setDescription("ops");
    testComInterface->setComType(hierarchyVLNV);
    testComInterface->setTransferType("Transfer");
    testComInterface->setDirection(DirectionTypes::IN);
    testComInterface->setPropertyValues(propertyValues);
    testComInterface->setComImplementation(hierarchyVLNV);

    QList<QSharedPointer<ComInterface> > comInterfaces;
    comInterfaces.append(testComInterface);

    testComponent_->setVersion("3.0.0");
    testComponent_->setComInterfaces(comInterfaces);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:vendorExtensions>\n"
                "\t\t<kactus2:version>3.0.0</kactus2:version>\n"
                "\t\t<kactus2:comInterfaces>\n"
                    "\t\t\t<kactus2:comInterface>\n"
                        "\t\t\t\t<ipxact:name>coms</ipxact:name>\n"
                        "\t\t\t\t<ipxact:displayName>spec</ipxact:displayName>\n"
                        "\t\t\t\t<ipxact:description>ops</ipxact:description>\n"
                        "\t\t\t\t<kactus2:comType vendor=\"TUT\" library=\"TestLibrary\" name=\"hierarchy\" "
                            "version=\"0.3\"/>\n"
                        "\t\t\t\t<kactus2:transferType>Transfer</kactus2:transferType>\n"
                        "\t\t\t\t<kactus2:comDirection>in</kactus2:comDirection>\n"
                        "\t\t\t\t<kactus2:propertyValues>\n"
                            "\t\t\t\t\t<kactus2:propertyValue name=\"spathi\" value=\"eluder\"/>\n"
                        "\t\t\t\t</kactus2:propertyValues>\n"
                        "\t\t\t\t<kactus2:comImplementationRef vendor=\"TUT\" library=\"TestLibrary\" "
                            "name=\"hierarchy\" version=\"0.3\"/>\n"
                    "\t\t\t</kactus2:comInterface>\n"
                "\t\t</kactus2:comInterfaces>\n"
            "\t</ipxact:vendorExtensions>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeApiInterfaces()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeApiInterfaces()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    VLNV hierarchyVLNV(VLNV::COMPONENT, "TUT", "TestLibrary", "hierarchy", "0.3");

    QSharedPointer<ApiInterface> testApiInterface (new ApiInterface());
    testApiInterface->setName("api");
    testApiInterface->setApiType(hierarchyVLNV);


    QList<QSharedPointer<ApiInterface> > apiInterfaces;
    apiInterfaces.append(testApiInterface);

    testComponent_->setVersion("3.0.0");
    testComponent_->setApiInterfaces(apiInterfaces);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:vendorExtensions>\n"
                "\t\t<kactus2:version>3.0.0</kactus2:version>\n"
                "\t\t<kactus2:apiInterfaces>\n"
                    "\t\t\t<kactus2:apiInterface>\n"
                        "\t\t\t\t<ipxact:name>api</ipxact:name>\n"
                        "\t\t\t\t<kactus2:apiType vendor=\"TUT\" library=\"TestLibrary\" name=\"hierarchy\" "
                            "version=\"0.3\"/>\n"
                        "\t\t\t\t<kactus2:dependencyDirection>provider</kactus2:dependencyDirection>\n"
                    "\t\t\t</kactus2:apiInterface>\n"
               "\t\t</kactus2:apiInterfaces>\n"
            "\t</ipxact:vendorExtensions>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentWriter::writeFileDependencies()
//-----------------------------------------------------------------------------
void tst_ComponentWriter::writeFileDependencies()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<FileDependency> pendingDependency (new FileDependency());
    pendingDependency->setManual(true);
    pendingDependency->setBidirectional(true);
    pendingDependency->setLocked(false);
    pendingDependency->setFile1("super");
    pendingDependency->setFile2("melee");
    pendingDependency->setDescription("Immediately obvious to the most casual observer");

    QList<QSharedPointer<FileDependency> > fileDependencies;
    fileDependencies.append(pendingDependency);

    testComponent_->setVersion("3.0.0");
    testComponent_->setPendingFileDependencies(fileDependencies);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>TUT</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestComponent</ipxact:name>\n"
            "\t<ipxact:version>0.11</ipxact:version>\n"
            "\t<ipxact:vendorExtensions>\n"
                "\t\t<kactus2:version>3.0.0</kactus2:version>\n"
                "\t\t<kactus2:fileDependencies>\n"
                    "\t\t\t<kactus2:fileDependency manual=\"true\" bidirectional=\"true\" locked=\"false\">\n"
                        "\t\t\t\t<kactus2:fileRef1>super</kactus2:fileRef1>\n"
                        "\t\t\t\t<kactus2:fileRef2>melee</kactus2:fileRef2>\n"
                        "\t\t\t\t<ipxact:description>Immediately obvious to the most casual observer"
                            "</ipxact:description>\n"
                    "\t\t\t</kactus2:fileDependency>\n"
                "\t\t</kactus2:fileDependencies>\n"
            "\t</ipxact:vendorExtensions>\n"
        "</ipxact:component>\n"
        );

    ComponentWriter componentWriter;
    componentWriter.writeComponent(xmlStreamWriter, testComponent_);

    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_ComponentWriter)

#include "tst_ComponentWriter.moc"
