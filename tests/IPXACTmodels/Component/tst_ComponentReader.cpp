//-----------------------------------------------------------------------------
// File: tst_ComponentReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.10.2015
//
// Description:
// Unit test for class ComponentReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/ComponentReader.h>

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
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/cpu.h>
#include <IPXACTmodels/Component/OtherClockDriver.h>

#include <IPXACTmodels/kactusExtensions/SWView.h>
#include <IPXACTmodels/kactusExtensions/ComProperty.h>
#include <IPXACTmodels/kactusExtensions/SystemView.h>
#include <IPXACTmodels/kactusExtensions/ComInterface.h>
#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/FileDependency.h>

#include <QtTest>
#include <QDomNode>

class tst_ComponentReader : public QObject
{
    Q_OBJECT

public:
    tst_ComponentReader();

private slots:

    void readSimpleComponent();
    
    void readXMLProcessingInstructions();

    void readBusInterfaces();
    void readChannels();
    void readRemapStates();
    void readAddressSpaces();
    void readMemoryMaps();

    void readViews();
    void readInstantiations();
    void readPorts();

    void readComponentGenerators();
    void readChoices();
    void readFileSets();
    void readCPUs();
    void readOtherClockDrivers();

    void readParameters();
    void readAssertions();
    void readVendorExtensions();

    void readKactusAttributes();
    void readSwViews();
    void readSwComProperties();
    void readSystemViews();
    void readComInterfaces();
    void readApiInterfaces();
    void readFileDependencies();
    void readAuthor();
};

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::tst_ComponentReader()
//-----------------------------------------------------------------------------
tst_ComponentReader::tst_ComponentReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readSimpleComponent()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readSimpleComponent()
{
    QString documentContent(
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
            "<ipxact:version>8.14</ipxact:version>"
            "<ipxact:description>coreDrill</ipxact:description>"
        "</ipxact:component>\n"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getVendor(), QString("TUT"));
    QCOMPARE(testComponent->getVlnv().getLibrary(), QString("TestLibrary"));
    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getVlnv().getVersion(), QString("8.14"));
    QCOMPARE(testComponent->getVlnv().getType(), VLNV::COMPONENT);
    QCOMPARE(testComponent->getDescription(), QString("coreDrill"));
}


//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readXMLProcessingInstructions()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readXMLProcessingInstructions()
{
    QString documentContent(
        "<?xml version=\"1.0\"?>"
        "<!-- Header comments -->"
        "<?xml-stylesheet href=\"style.css\"?>"
        "<?xml-stylesheet href=\"other.css\" attribute=\"value\" title=\"Other\"?>"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestComponent</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
        "</ipxact:component>\n"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getXmlProcessingInstructions().size(), 2);

    QPair<QString, QString> firstInstructions = testComponent->getXmlProcessingInstructions().first();
    QCOMPARE(firstInstructions.first, QString("xml-stylesheet"));
    QCOMPARE(firstInstructions.second, QString("href=\"style.css\""));

    QPair<QString, QString> secondInstructions = testComponent->getXmlProcessingInstructions().last();
    QCOMPARE(secondInstructions.first, QString("xml-stylesheet"));
    QCOMPARE(secondInstructions.second, QString("href=\"other.css\" attribute=\"value\" title=\"Other\""));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readBusInterfaces()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readBusInterfaces()
{
    QString documentContent(
        "<?xml version=\"1.0\"?>"
        "<ipxact:component "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestComponent</ipxact:name>"
            "<ipxact:version>0.11</ipxact:version>"
            "<ipxact:busInterfaces>"
                "<ipxact:busInterface>"
                    "<ipxact:name>testInterface</ipxact:name>"
                    "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"busDefinition\""
                        " version=\"0.2\"/>"
                    "<ipxact:master/>"
                "</ipxact:busInterface>"
            "</ipxact:busInterfaces>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getBusInterfaces()->size(), 1);

    QSharedPointer<BusInterface> busInterface = testComponent->getBusInterfaces()->first();
    QCOMPARE(busInterface->name(), QString("testInterface"));
    QCOMPARE(busInterface->getBusType().getVendor(), QString("TUT"));
    QCOMPARE(busInterface->getBusType().getLibrary(), QString("TestLibrary"));
    QCOMPARE(busInterface->getBusType().getName(), QString("busDefinition"));
    QCOMPARE(busInterface->getBusType().getVersion(), QString("0.2"));
    QCOMPARE(busInterface->getInterfaceMode(), General::MASTER);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readChannels()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readChannels()
{
    QString documentContent(
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
            "<ipxact:channels>"
                "<ipxact:channel>"
                    "<ipxact:name>testChannel</ipxact:name>"
                    "<ipxact:busInterfaceRef>"
                        "<ipxact:localName>interfaceOne</ipxact:localName>"
                    "</ipxact:busInterfaceRef>"
                    "<ipxact:busInterfaceRef>"
                        "<ipxact:localName>interfaceTwo</ipxact:localName>"
                    "</ipxact:busInterfaceRef>"
                "</ipxact:channel>"
            "</ipxact:channels>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getBusInterfaces()->size(), 0);
    QCOMPARE(testComponent->getChannels()->size(), 1);

    QSharedPointer<Channel> channel = testComponent->getChannels()->first();
    QCOMPARE(channel->name(), QString("testChannel"));
    QCOMPARE(channel->getInterfaces().size(), 2);
    QCOMPARE(channel->getInterfaces().first(), QString("interfaceOne"));
    QCOMPARE(channel->getInterfaces().last(), QString("interfaceTwo"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readRemapStates()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readRemapStates()
{
    QString documentContent(
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
            "<ipxact:remapStates>"
                "<ipxact:remapState>"
                    "<ipxact:name>remap</ipxact:name>"
                "</ipxact:remapState>"
            "</ipxact:remapStates>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getChannels()->size(), 0);
    QCOMPARE(testComponent->getRemapStates()->size(), 1);

    QSharedPointer<RemapState> remapState = testComponent->getRemapStates()->first();
    QCOMPARE(remapState->name(), QString("remap"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readAddressSpaces()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readAddressSpaces()
{
    QString documentContent(
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
            "<ipxact:addressSpaces>"
                "<ipxact:addressSpace>"
                    "<ipxact:name>StarControl</ipxact:name>"
                    "<ipxact:range>coreward</ipxact:range>"
                    "<ipxact:width>front</ipxact:width>"
                "</ipxact:addressSpace>"
            "</ipxact:addressSpaces>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getRemapStates()->size(), 0);
    QCOMPARE(testComponent->getAddressSpaces()->size(), 1);

    QSharedPointer<AddressSpace> addressSpace = testComponent->getAddressSpaces()->first();
    QCOMPARE(addressSpace->name(), QString("StarControl"));
    QCOMPARE(addressSpace->getRange(), QString("coreward"));
    QCOMPARE(addressSpace->getWidth(), QString("front"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readMemoryMaps()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readMemoryMaps()
{
    QString documentContent(
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
            "<ipxact:memoryMaps>"
                "<ipxact:memoryMap>"
                    "<ipxact:name>memoryMap</ipxact:name>"
                "</ipxact:memoryMap>"
            "</ipxact:memoryMaps>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getAddressSpaces()->size(), 0);
    QCOMPARE(testComponent->getMemoryMaps()->size(), 1);

    QSharedPointer<MemoryMap> memoryMap = testComponent->getMemoryMaps()->first();
    QCOMPARE(memoryMap->name(), QString("memoryMap"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readViews()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readViews()
{
    QString documentContent(
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
            "<ipxact:model>"
                "<ipxact:views>"
                    "<ipxact:view>"
                        "<ipxact:name>testView</ipxact:name>"
                    "</ipxact:view>"
                "</ipxact:views>"
            "</ipxact:model>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getMemoryMaps()->size(), 0);
    QCOMPARE(testComponent->getViews()->size(), 1);

    QSharedPointer<View> view = testComponent->getViews()->first();
    QCOMPARE(view->name(), QString("testView"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readInstantiations()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readInstantiations()
{
    QString documentContent(
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
            "<ipxact:model>"
                "<ipxact:instantiations>"
                    "<ipxact:componentInstantiation>"
                        "<ipxact:name>component</ipxact:name>"
                    "</ipxact:componentInstantiation>"
                    "<ipxact:designInstantiation>"
                        "<ipxact:name>design</ipxact:name>"
                        "<ipxact:designRef vendor=\"TUT\" library=\"TestLibrary\" name=\"designReference\" "
                            "version=\"1.0\"/>"
                    "</ipxact:designInstantiation>"
                    "<ipxact:designConfigurationInstantiation>"
                        "<ipxact:name>designConfiguration</ipxact:name>"
                        "<ipxact:designConfigurationRef vendor=\"TUT\" library=\"TestLibrary\""
                            " name=\"configuration\" version=\"8.1\"/>"
                    "</ipxact:designConfigurationInstantiation>"
                "</ipxact:instantiations>"
            "</ipxact:model>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getViews()->size(), 0);
    QCOMPARE(testComponent->getComponentInstantiations()->size(), 1);
    QCOMPARE(testComponent->getDesignInstantiations()->size(), 1);
    QCOMPARE(testComponent->getDesignConfigurationInstantiations()->size(), 1);

    QSharedPointer<ComponentInstantiation> componentInstantiation =
        testComponent->getComponentInstantiations()->first();
    QCOMPARE(componentInstantiation->name(), QString("component"));

    QSharedPointer<DesignInstantiation> designInstantiation = testComponent->getDesignInstantiations()->first();
    QCOMPARE(designInstantiation->name(), QString("design"));
    QCOMPARE(designInstantiation->getDesignReference()->getVendor(), QString("TUT"));
    QCOMPARE(designInstantiation->getDesignReference()->getLibrary(), QString("TestLibrary"));
    QCOMPARE(designInstantiation->getDesignReference()->getName(), QString("designReference"));
    QCOMPARE(designInstantiation->getDesignReference()->getVersion(), QString("1.0"));

    QSharedPointer<DesignConfigurationInstantiation> configurationInstantiation =
        testComponent->getDesignConfigurationInstantiations()->first();
    QCOMPARE(configurationInstantiation->name(), QString("designConfiguration"));
    QCOMPARE(configurationInstantiation->getDesignConfigurationReference()->getVendor(), QString("TUT"));
    QCOMPARE(configurationInstantiation->getDesignConfigurationReference()->getLibrary(), QString("TestLibrary"));
    QCOMPARE(configurationInstantiation->getDesignConfigurationReference()->getName(), QString("configuration"));
    QCOMPARE(configurationInstantiation->getDesignConfigurationReference()->getVersion(), QString("8.1"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readPorts()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readPorts()
{
    QString documentContent(
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
            "<ipxact:model>"
                "<ipxact:ports>"
                    "<ipxact:port>"
                        "<ipxact:name>testPort</ipxact:name>"
                        "<ipxact:wire>"
                            "<ipxact:direction>in</ipxact:direction>"
                        "</ipxact:wire>"
                    "</ipxact:port>"
                "</ipxact:ports>"
            "</ipxact:model>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getComponentInstantiations()->size(), 0);
    QCOMPARE(testComponent->getDesignInstantiations()->size(), 0);
    QCOMPARE(testComponent->getDesignConfigurationInstantiations()->size(), 0);
    QCOMPARE(testComponent->getPorts()->size(), 1);

    QSharedPointer<Port> port = testComponent->getPorts()->first();
    QCOMPARE(port->name(), QString("testPort"));
    QCOMPARE(port->getWire().isNull(), false);
    QCOMPARE(port->getWire()->getDirection(), DirectionTypes::IN);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readComponentGenerators()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readComponentGenerators()
{
    QString documentContent(
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
            "<ipxact:componentGenerators>"
                "<ipxact:componentGenerator>"
                    "<ipxact:name>generator</ipxact:name>"
                    "<ipxact:generatorExe>saMatra</ipxact:generatorExe>"
                "</ipxact:componentGenerator>"
            "</ipxact:componentGenerators>"
        "</ipxact:component>\n"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getPorts()->size(), 0);
    QCOMPARE(testComponent->getComponentGenerators()->size(), 1);

    QSharedPointer<ComponentGenerator> generator = testComponent->getComponentGenerators()->first();
    QCOMPARE(generator->name(), QString("generator"));
    QCOMPARE(generator->getGeneratorExe(), QString("saMatra"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readChoices()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readChoices()
{
    QString documentContent(
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
            "<ipxact:choices>"
                "<ipxact:choice>"
                    "<ipxact:name>chosenOne</ipxact:name>"
                    "<ipxact:enumeration>14</ipxact:enumeration>"
                "</ipxact:choice>"
            "</ipxact:choices>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getComponentGenerators()->size(), 0);
    QCOMPARE(testComponent->getChoices()->size(), 1);

    QSharedPointer<Choice> testChoice = testComponent->getChoices()->first();
    QCOMPARE(testChoice->name(), QString("chosenOne"));
    QCOMPARE(testChoice->getEnumerationValues().size(), 1);
    QCOMPARE(testChoice->getEnumerationValues().first(), QString("14"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readFileSets()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readFileSets()
{
    QString documentContent(
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
            "<ipxact:fileSets>"
                "<ipxact:fileSet>"
                    "<ipxact:name>urquanMasters</ipxact:name>"
                "</ipxact:fileSet>"
            "</ipxact:fileSets>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getChoices()->size(), 0);
    QCOMPARE(testComponent->getFileSets()->size(), 1);

    QSharedPointer<FileSet> fileSet = testComponent->getFileSets()->first();
    QCOMPARE(fileSet->name(), QString("urquanMasters"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readCPUs()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readCPUs()
{
    QString documentContent(
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
            "<ipxact:cpus>"
                "<ipxact:cpu>"
                    "<ipxact:name>Chmmr</ipxact:name>"
                    "<ipxact:addressSpaceRef addressSpaceRef=\"Avatar\"/>"
                "</ipxact:cpu>"
            "</ipxact:cpus>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getFileSets()->size(), 0);
    QCOMPARE(testComponent->getCpus()->size(), 1);

    QSharedPointer<Cpu> testCPU = testComponent->getCpus()->first();
    QCOMPARE(testCPU->name(), QString("Chmmr"));
    QCOMPARE(testCPU->getAddressSpaceReferences()->size(), 1);
    QCOMPARE(testCPU->getAddressSpaceReferences()->first()->addressSpaceRef_, QString("Avatar"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readOtherClockDrivers()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readOtherClockDrivers()
{
    QString documentContent(
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
            "<ipxact:otherClockDrivers>"
                "<ipxact:otherClockDriver clockName=\"clockName\">"
                    "<ipxact:clockPeriod>watch</ipxact:clockPeriod>"
                    "<ipxact:clockPulseOffset>Laser</ipxact:clockPulseOffset>"
                    "<ipxact:clockPulseValue>KingOsrik</ipxact:clockPulseValue>"
                    "<ipxact:clockPulseDuration>eternity</ipxact:clockPulseDuration>"
                "</ipxact:otherClockDriver>"
            "</ipxact:otherClockDrivers>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getCpus()->size(), 0);
    QCOMPARE(testComponent->getOtherClockDrivers()->size(), 1);

    QSharedPointer<OtherClockDriver> clockDriver = testComponent->getOtherClockDrivers()->first();
    QCOMPARE(clockDriver->getClockName(), QString("clockName"));
    QCOMPARE(clockDriver->getClockPeriod()->getValue(), QString("watch"));
    QCOMPARE(clockDriver->getClockPulseOffset()->getValue(), QString("Laser"));
    QCOMPARE(clockDriver->getClockPulseValue(), QString("KingOsrik"));
    QCOMPARE(clockDriver->getClockPulseDuration()->getValue(), QString("eternity"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readParameters()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readParameters()
{
    QString documentContent(
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
            "<ipxact:parameters>"
                "<ipxact:parameter parameterId=\"testID\">"
                    "<ipxact:name>testParameter</ipxact:name>"
                    "<ipxact:value>1</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getOtherClockDrivers()->size(), 0);
    QCOMPARE(testComponent->getParameters()->size(), 1);

    QSharedPointer<Parameter> testParameter = testComponent->getParameters()->first();
    QCOMPARE(testParameter->getValueId(), QString("testID"));
    QCOMPARE(testParameter->name(), QString("testParameter"));
    QCOMPARE(testParameter->getValue(), QString("1"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readAssertions()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readAssertions()
{
    QString documentContent(
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
            "<ipxact:assertions>"
                "<ipxact:assertion>"
                    "<ipxact:name>testAssertion</ipxact:name>"
                    "<ipxact:displayName>assertionDisplay</ipxact:displayName>"
                    "<ipxact:description>assertionDescription</ipxact:description>"
                    "<ipxact:assert>13</ipxact:assert>"
                "</ipxact:assertion>"
            "</ipxact:assertions>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getParameters()->size(), 0);
    QCOMPARE(testComponent->getAssertions()->size(), 1);

    QSharedPointer<Assertion> testAssertion = testComponent->getAssertions()->first();
    QCOMPARE(testAssertion->name(), QString("testAssertion"));
    QCOMPARE(testAssertion->displayName(), QString("assertionDisplay"));
    QCOMPARE(testAssertion->description(), QString("assertionDescription"));
    QCOMPARE(testAssertion->getAssert(), QString("13"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readVendorExtensions()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readVendorExtensions()
{
    QString documentContent(
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
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                "<kactus2:version>3.0.0</kactus2:version>"
            "</ipxact:vendorExtensions>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getAssertions()->size(), 0);
    QCOMPARE(testComponent->getVendorExtensions()->size(), 2);
    QCOMPARE(testComponent->getVendorExtensions()->first()->type(), QString("kactus2:version"));
    QCOMPARE(testComponent->getVendorExtensions()->last()->type(), QString("testExtension"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readKactusAttributes()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readKactusAttributes()
{
    QString documentContent(
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
                        "<kactus2:kts_implementation>SW</kactus2:kts_implementation>"
                        "<kactus2:kts_firmness>Fixed</kactus2:kts_firmness>"
                    "</kactus2:kts_attributes>"
                "</ipxact:vendorExtensions>"
            "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getHierarchy(), KactusAttribute::IP);
    QCOMPARE(testComponent->getImplementation(), KactusAttribute::SW);
    QCOMPARE(testComponent->getFirmness(), KactusAttribute::FIXED);    
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readSwViews()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readSwViews()
{
    QString documentContent(
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
                "<kactus2:version>3.0.0</kactus2:version>"
                "<kactus2:swViews>"
                    "<kactus2:swView>"
                        "<ipxact:name>swView</ipxact:name>"
                        "<ipxact:displayName>displayed</ipxact:displayName>"
                        "<ipxact:description>described</ipxact:description>"
                        "<kactus2:hierarchyRef vendor=\"TUT\" library=\"TestLibrary\" name=\"hierarchy\" "
                            "version=\"0.3\"/>"
                        "<kactus2:fileSetRef>fileSet</kactus2:fileSetRef>"
                    "</kactus2:swView>"
                "</kactus2:swViews>"
            "</ipxact:vendorExtensions>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getSWViews().size(), 1);

    QSharedPointer<SWView> swView = testComponent->getSWViews().first();
    QCOMPARE(swView->name(), QString("swView"));
    QCOMPARE(swView->displayName(), QString("displayed"));
    QCOMPARE(swView->description(), QString("described"));
    QCOMPARE(swView->getHierarchyRef().getVendor(), QString("TUT"));
    QCOMPARE(swView->getHierarchyRef().getLibrary(), QString("TestLibrary"));
    QCOMPARE(swView->getHierarchyRef().getName(), QString("hierarchy"));
    QCOMPARE(swView->getHierarchyRef().getVersion(), QString("0.3"));
    QCOMPARE(swView->getFileSetRefs().size(), 1);
    QCOMPARE(swView->getFileSetRefs().first(), QString("fileSet"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readSwComProperties()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readSwComProperties()
{
    QString documentContent(
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
                "<kactus2:version>3.0.0</kactus2:version>"
                "<kactus2:properties>"
                    "<kactus2:property name=\"Priority\" required=\"true\" propertyType=\"override.\""
                        " defaultValue=\"New behaviour\" description=\"dictated\"/>"
                "</kactus2:properties>"
            "</ipxact:vendorExtensions>"
        "</ipxact:component>\n"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getSWViews().size(), 0);
    QCOMPARE(testComponent->getSWProperties()->size(), 1);

    QSharedPointer<ComProperty> swProperty = testComponent->getSWProperties()->first();
    QCOMPARE(swProperty->name(), QString("Priority"));
    QCOMPARE(swProperty->isRequired(), true);
    QCOMPARE(swProperty->getType(), QString("override."));
    QCOMPARE(swProperty->getDefaultValue(), QString("New behaviour"));
    QCOMPARE(swProperty->getDescription(), QString("dictated"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readSystemViews()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readSystemViews()
{
    QString documentContent(
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
                "<kactus2:version>3.0.0</kactus2:version>"
                "<kactus2:systemViews>"
                    "<kactus2:systemView>"
                        "<ipxact:name>system</ipxact:name>"
                        "<ipxact:displayName>crash</ipxact:displayName>"
                        "<ipxact:description>BSOD</ipxact:description>"
                        "<kactus2:hierarchyRef vendor=\"TUT\" library=\"TestLibrary\" name=\"hierarchy\" "
                            "version=\"0.3\"/>"
                        "<kactus2:hwViewRef>malfunction</kactus2:hwViewRef>"
                        "<kactus2:fileSetRef>burn</kactus2:fileSetRef>"
                    "</kactus2:systemView>"
                "</kactus2:systemViews>"
            "</ipxact:vendorExtensions>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getSWProperties()->size(), 0);
    QCOMPARE(testComponent->getSystemViews().size(), 1);

    QSharedPointer<SystemView> systemView = testComponent->getSystemViews().first();
    QCOMPARE(systemView->name(), QString("system"));
    QCOMPARE(systemView->displayName(), QString("crash"));
    QCOMPARE(systemView->description(), QString("BSOD"));
    QCOMPARE(systemView->getHierarchyRef().getVendor(), QString("TUT"));
    QCOMPARE(systemView->getHierarchyRef().getLibrary(), QString("TestLibrary"));
    QCOMPARE(systemView->getHierarchyRef().getName(), QString("hierarchy"));
    QCOMPARE(systemView->getHierarchyRef().getVersion(), QString("0.3"));
    QCOMPARE(systemView->getHWViewRef(), QString("malfunction"));
    QCOMPARE(systemView->getFileSetRefs().size(), 1);
    QCOMPARE(systemView->getFileSetRefs().first(), QString("burn"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readComInterfaces()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readComInterfaces()
{
    QString documentContent(
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
                "<kactus2:version>3.0.0</kactus2:version>"
                "<kactus2:comInterfaces>"
                    "<kactus2:comInterface>"
                        "<ipxact:name>coms</ipxact:name>"
                        "<ipxact:displayName>spec</ipxact:displayName>"
                        "<ipxact:description>ops</ipxact:description>"
                        "<kactus2:comType vendor=\"TUT\" library=\"TestLibrary\" name=\"hierarchy\" "
                            "version=\"0.3\"/>"
                        "<kactus2:transferType>Transfer</kactus2:transferType>"
                        "<kactus2:comDirection>in</kactus2:comDirection>"
                        "<kactus2:propertyValues>"
                            "<kactus2:propertyValue name=\"spathi\" value=\"eluder\"/>"
                        "</kactus2:propertyValues>"
                        "<kactus2:comImplementationRef vendor=\"TUT\" library=\"TestLibrary\" name=\"hierarchy\" "
                            "version=\"0.3\"/>"
                    "</kactus2:comInterface>"
                "</kactus2:comInterfaces>"
            "</ipxact:vendorExtensions>"
        "</ipxact:component>\n"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getSystemViews().size(), 0);
    QCOMPARE(testComponent->getComInterfaces().size(), 1);

    QSharedPointer<ComInterface> comInterface = testComponent->getComInterfaces().first();
    QCOMPARE(comInterface->name(), QString("coms"));
    QCOMPARE(comInterface->displayName(), QString("spec"));
    QCOMPARE(comInterface->description(), QString("ops"));
    QCOMPARE(comInterface->getComType().getVendor(), QString("TUT"));
    QCOMPARE(comInterface->getComType().getLibrary(), QString("TestLibrary"));
    QCOMPARE(comInterface->getComType().getName(), QString("hierarchy"));
    QCOMPARE(comInterface->getComType().getVersion(), QString("0.3"));
    QCOMPARE(comInterface->getTransferType(), QString("Transfer"));
    QCOMPARE(comInterface->getDirection(), DirectionTypes::IN);
    QCOMPARE(comInterface->getPropertyValues().size(), 1);
    QCOMPARE(comInterface->getPropertyValues().firstKey(), QString("spathi"));
    QCOMPARE(comInterface->getPropertyValues().first(), QString("eluder"));
    QCOMPARE(comInterface->getComImplementation().getVendor(), QString("TUT"));
    QCOMPARE(comInterface->getComImplementation().getLibrary(), QString("TestLibrary"));
    QCOMPARE(comInterface->getComImplementation().getName(), QString("hierarchy"));
    QCOMPARE(comInterface->getComImplementation().getVersion(), QString("0.3"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readApiInterfaces()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readApiInterfaces()
{
    QString documentContent(
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
                "<kactus2:version>3.0.0</kactus2:version>"
                "<kactus2:apiInterfaces>"
                    "<kactus2:apiInterface>"
                        "<ipxact:name>api</ipxact:name>"
                        "<kactus2:apiType vendor=\"TUT\" library=\"TestLibrary\" name=\"hierarchy\" "
                            "version=\"0.3\"/>"
                        "<kactus2:dependencyDirection>provider</kactus2:dependencyDirection>"
                    "</kactus2:apiInterface>"
                "</kactus2:apiInterfaces>"
            "</ipxact:vendorExtensions>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getComInterfaces().size(), 0);
    QCOMPARE(testComponent->getApiInterfaces().size(), 1);

    QSharedPointer<ApiInterface> apiInterface = testComponent->getApiInterfaces().first();
    QCOMPARE(apiInterface->name(), QString("api"));
    QCOMPARE(apiInterface->getApiType().getVendor(), QString("TUT"));
    QCOMPARE(apiInterface->getApiType().getLibrary(), QString("TestLibrary"));
    QCOMPARE(apiInterface->getApiType().getName(), QString("hierarchy"));
    QCOMPARE(apiInterface->getApiType().getVersion(), QString("0.3"));
    QCOMPARE(apiInterface->getDependencyDirection(), DEPENDENCY_PROVIDER);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readFileDependencies()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readFileDependencies()
{
    QString documentContent(
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
                "<kactus2:version>3.0.0</kactus2:version>"
                "<kactus2:fileDependencies>"
                    "<kactus2:fileDependency manual=\"true\" bidirectional=\"true\" locked=\"false\">"
                        "<kactus2:fileRef1>super</kactus2:fileRef1>"
                        "<kactus2:fileRef2>melee</kactus2:fileRef2>"
                        "<ipxact:description>Immediately obvious to the most casual observer</ipxact:description>"
                    "</kactus2:fileDependency>"
                "</kactus2:fileDependencies>"
            "</ipxact:vendorExtensions>"
        "</ipxact:component>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getVlnv().getName(), QString("TestComponent"));
    QCOMPARE(testComponent->getApiInterfaces().size(), 0);
    QCOMPARE(testComponent->getFileDependencies().size(), 1);

    QSharedPointer<FileDependency> dependency = testComponent->getFileDependencies().first();
    QCOMPARE(dependency->isManual(), true);
    QCOMPARE(dependency->isBidirectional(), true);
    QCOMPARE(dependency->isLocked(), false);
    QCOMPARE(dependency->getFile1(), QString("super"));
    QCOMPARE(dependency->getFile2(), QString("melee"));
    QCOMPARE(dependency->getDescription(), QString("Immediately obvious to the most casual observer"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentReader::readAuthor()
//-----------------------------------------------------------------------------
void tst_ComponentReader::readAuthor()
{
    QString documentContent(
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
                "<kactus2:author>tester</kactus2:author>"              
            "</ipxact:vendorExtensions>"
        "</ipxact:component>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    ComponentReader componentReader;

    QSharedPointer<Component> testComponent = componentReader.createComponentFrom(document);

    QCOMPARE(testComponent->getAuthor(), QString("tester"));
}


QTEST_APPLESS_MAIN(tst_ComponentReader)

#include "tst_ComponentReader.moc"
