//-----------------------------------------------------------------------------
// File: tst_ComponentValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.12.2015
//
// Description:
// Unit test for class ComponentValidator.
//-----------------------------------------------------------------------------

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <IPXACTmodels/Component/validators/ComponentValidator.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>
#include <IPXACTmodels/Component/validators/ChannelValidator.h>
#include <IPXACTmodels/Component/validators/RemapStateValidator.h>
#include <IPXACTmodels/Component/validators/AddressSpaceValidator.h>
#include <IPXACTmodels/Component/validators/MemoryMapBaseValidator.h>
#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>
#include <IPXACTmodels/Component/validators/RegisterValidator.h>
#include <IPXACTmodels/Component/validators/FieldValidator.h>
#include <IPXACTmodels/Component/validators/EnumeratedValueValidator.h>
#include <IPXACTmodels/Component/validators/ViewValidator.h>
#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>
#include <IPXACTmodels/Component/validators/PortValidator.h>
#include <IPXACTmodels/Component/validators/ComponentGeneratorValidator.h>
#include <IPXACTmodels/Component/validators/ChoiceValidator.h>
#include <IPXACTmodels/Component/validators/FileSetValidator.h>
#include <IPXACTmodels/Component/validators/FileValidator.h>
#include <IPXACTmodels/Component/validators/CPUValidator.h>
#include <IPXACTmodels/Component/validators/OtherClockDriverValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>
#include <IPXACTmodels/common/validators/AssertionValidator.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/MasterInterface.h>
#include <IPXACTmodels/Component/Channel.h>
#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/ComponentGenerator.h>
#include <IPXACTmodels/Component/Choice.h>
#include <IPXACTmodels/common/Enumeration.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/Cpu.h>
#include <IPXACTmodels/Component/OtherClockDriver.h>
#include <IPXACTmodels/common/ClockUnit.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/Assertion.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <tests/MockObjects/LibraryMock.h>

#include <QtTest>

class tst_ComponentValidator : public QObject
{
    Q_OBJECT

public:
    tst_ComponentValidator();

private slots:

    void testHasValidVLNV();
    void testHasValidVLNV_data();

    void testHasValidBusInterfaces();
    void testHasValidBusInterfaces_data();

    void testHasValidChannels();
    void testHasValidChannels_data();

    void testHasValidRemapStates();
    void testHasValidRemapStates_data();

    void testHasValidAddressSpaces();
    void testHasValidAddressSpaces_data();

    void testHasValidMemoryMaps();
    void testHasValidMemoryMaps_data();

    void testHasValidViews();
    void testHasValidViews_data();

    void testHasValidComponentInstantiations();
    void testHasValidComponentInstantiations_data();

    void testHasValidDesignInstantiations();
    void testHasValidDesignInstantiations_data();

    void testHasValidDesignConfigurationInstantiations();
    void testHasValidDesignConfigurationInstantiations_data();

    void testHasValidPorts();
    void testHasValidPorts_data();

    void testHasValidComponentGenerators();
    void testHasValidComponentGenerators_data();

    void testHasValidChoices();
    void testHasValidChoices_data();

    void testHasValidFileSets();
    void testHasValidFileSets_data();

    void testHasValidCPUs();
    void testHasValidCPUs_data();

    void testHasValidOtherClockDrivers();
    void testHasValidOtherClockDrivers_data();

    void testHasValidParameters();
    void testHasValidParameters_data();

    void testHasValidAssertions();
    void testHasValidAssertions_data();

private:

    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);

    QSharedPointer<ComponentValidator> createComponentValidator(LibraryMock* mockLibrary);
};

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::tst_ComponentValidator()
//-----------------------------------------------------------------------------
tst_ComponentValidator::tst_ComponentValidator()
{
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidName()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidVLNV()
{
    QFETCH(QString, vendor);
    QFETCH(QString, library);
    QFETCH(QString, name);
    QFETCH(QString, version);
    QFETCH(bool, isValid);

    VLNV componentVLNV(VLNV::COMPONENT, vendor, library, name, version);
    QSharedPointer<Component> testComponent (new Component(componentVLNV));

    QSharedPointer<ComponentValidator> validator = createComponentValidator(0);
    QCOMPARE(validator->hasValidVLNV(testComponent), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("The type of the vlnv is invalid within component");

        if (vendor.isEmpty())
        {
            expectedError = QObject::tr("No vendor specified for vlnv within component");
        }
        else if (library.isEmpty())
        {
            expectedError = QObject::tr("No library specified for vlnv within component");
        }
        else if (name.isEmpty())
        {
            expectedError = QObject::tr("No name specified for vlnv within component");
        }
        else if (version.isEmpty())
        {
            expectedError = QObject::tr("No version specified for vlnv within component");
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidName_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidVLNV_data()
{
    QTest::addColumn<QString>("vendor");
    QTest::addColumn<QString>("library");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("version");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Component with all VLNVs is valid") << "Tengen" << "Toppa" << "Gurren" << "Lagann" << true;
    QTest::newRow("Component without vendor is not valid") << "" << "Toppa" << "Gurren" << "Lagann" << false;
    QTest::newRow("Component without library is not valid") << "Tengen" << "" << "Gurren" << "Lagann" << false;
    QTest::newRow("Component without name is not valid") << "Tengen" << "Toppa" << "" << "Lagann" << false;
    QTest::newRow("Component without version is not valid") << "Tengen" << "Toppa" << "Gurren" << "" << false;
    QTest::newRow("Empty VLNV is not valid") << "" << "" << "" << "" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidBusInterfaces()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidBusInterfaces()
{
    QFETCH(QString, busName);
    QFETCH(bool, hasBusType);
    QFETCH(bool, hasInterfaceMode);
    QFETCH(bool, copyInterface);
    QFETCH(bool, isValid);

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName(busName);

    LibraryMock* mockLibrary (new LibraryMock(this));

    if (hasBusType)
    {
        ConfigurableVLNVReference testType(VLNV::BUSDEFINITION, "testVendor", "testLibrary", "busDefinition", "1.1");
        testBus->setBusType(testType);

        QSharedPointer<BusDefinition> testDefinition (new BusDefinition());
        testDefinition->setVlnv(testType);

        mockLibrary->addComponent(testDefinition);
    }

    if (hasInterfaceMode)
    {
        QSharedPointer<MasterInterface> testMaster (new MasterInterface());
        testBus->setMaster(testMaster);
        testBus->setInterfaceMode(General::MASTER);
    }

    QSharedPointer<Component> testComponent (new Component(
        VLNV(VLNV::COMPONENT, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testComponent->getBusInterfaces()->append(testBus);

    if (copyInterface)
    {
        QSharedPointer<BusInterface> otherBus (new BusInterface(*testBus.data()));
        testComponent->getBusInterfaces()->append(otherBus);
    }

    QSharedPointer<ComponentValidator> validator = createComponentValidator(mockLibrary);
    QCOMPARE(validator->hasValidBusInterfaces(testComponent), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("Invalid name specified for bus interface %1 within component %2")
            .arg(testBus->name()).arg(testComponent->getVlnv().toString());

        if (!hasBusType)
        {
            expectedError = QObject::tr("Bus definition must be given for bus interface %1 within component %2")
                .arg(testBus->name()).arg(testComponent->getVlnv().toString());
        }
        else if (!hasInterfaceMode)
        {
            expectedError = QObject::tr("Unknown interface mode set for bus interface %1 within component %2")
                .arg(testBus->name()).arg(testComponent->getVlnv().toString()); 
        }
        else if (copyInterface)
        {
            expectedError = QObject::tr("Bus interface name %1 within component %2 is not unique.")
                .arg(testBus->name()).arg(testComponent->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidBusInterfaces_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidBusInterfaces_data()
{
    QTest::addColumn<QString>("busName");
    QTest::addColumn<bool>("hasBusType");
    QTest::addColumn<bool>("hasInterfaceMode");
    QTest::addColumn<bool>("copyInterface");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Bus interface with name, bus type and interface mode is valid") <<
        "Wanpan" << true << true << false << true;
    QTest::newRow("Bus interface without name is not valid")
        << "" << true << true << false << false;
    QTest::newRow("Bus interface without bus type is not valid") <<
        "Wanpan" << false << true <<  false << false;
    QTest::newRow("Bus interface without interface mode is not valid") <<
        "Wanpan" << true << false <<  false << false;
    QTest::newRow("Empty bus interface is not valid") << "" << false << false << false << false;
    QTest::newRow("Bus interfaces with same name is not valid") << "Wanpan" << true << true << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidChannels()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidChannels()
{
    QFETCH(QString, channelName);
    QFETCH(QString, firstBus);
    QFETCH(QString, secondBus);
    QFETCH(bool, copyChannel);
    QFETCH(bool, isValid);

    QStringList busReferences;
    busReferences.append(firstBus);
    busReferences.append(secondBus);

    QSharedPointer<Channel> testChannel (new Channel());
    testChannel->setName(channelName);
    testChannel->setInterfaces(busReferences);

    QSharedPointer<Component> testComponent (new Component(
        VLNV(VLNV::COMPONENT, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testComponent->getChannels()->append(testChannel);

    if (copyChannel)
    {
        QSharedPointer<Channel> otherChannel (new Channel(*testChannel.data()));
        testComponent->getChannels()->append(otherChannel);
    }

    if (!firstBus.isEmpty())
    {
        QSharedPointer<BusInterface> testBus (new BusInterface());
        testBus->setName(firstBus);
        testBus->setInterfaceMode(General::MIRROREDMASTER);
        testComponent->getBusInterfaces()->append(testBus);
    }
    if (!secondBus.isEmpty())
    {
        QSharedPointer<BusInterface> testBus (new BusInterface());
        testBus->setName(secondBus);
        testBus->setInterfaceMode(General::MIRROREDSLAVE);
        testComponent->getBusInterfaces()->append(testBus);
    }

    QSharedPointer<ComponentValidator> validator = createComponentValidator(0);
    QCOMPARE(validator->hasValidChannels(testComponent), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("Invalid name %1 specified for channel within component %2.")
            .arg(testChannel->name()).arg(testComponent->getVlnv().toString());

        if (firstBus.isEmpty())
        {
            expectedError = QObject::tr("Bus interface \'%1\' referenced within channel %2 not found.")
                .arg(firstBus).arg(channelName);
        }
        else if (copyChannel)
        {
            expectedError = QObject::tr("Channel name %1 within component %2 is not unique.")
                .arg(testChannel->name()).arg(testComponent->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidChannels_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidChannels_data()
{
    QTest::addColumn<QString>("channelName");
    QTest::addColumn<QString>("firstBus");
    QTest::addColumn<QString>("secondBus");
    QTest::addColumn<bool>("copyChannel");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Channel with name and two bus interface references is valid") <<
        "Gurren" << "Kamina" << "Yoko" << false << true;
    QTest::newRow("Channel without name is not valid") <<
        "" << "Kamina" << "Yoko" << false << false;
    QTest::newRow("Channel without at least two bus references is not valid") <<
        "Gurren" << "" << "Yoko" << false << false;
    QTest::newRow("Channels with same name is not valid") << "Gurren" << "Kamina" << "Yoko" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidRemapStates()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidRemapStates()
{
    QFETCH(QString, stateName);
    QFETCH(bool, copyState);
    QFETCH(bool, isValid);

    QSharedPointer<RemapState> testState (new RemapState(stateName));

    QSharedPointer<Component> testComponent (new Component(
        VLNV(VLNV::COMPONENT, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testComponent->getRemapStates()->append(testState);

    if (copyState)
    {
        QSharedPointer<RemapState> otherState (new RemapState(*testState.data()));
        testComponent->getRemapStates()->append(otherState);
    }

    QSharedPointer<ComponentValidator> validator = createComponentValidator(0);
    QCOMPARE(validator->hasValidRemapStates(testComponent), isValid);


    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("Invalid name specified for remap state %1 within component %2")
            .arg(stateName).arg(testComponent->getVlnv().toString());

        if (copyState)
        {
            expectedError = QObject::tr("Remap state name %1 within component %2 is not unique.")
                .arg(stateName).arg(testComponent->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidRemapStates_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidRemapStates_data()
{
    QTest::addColumn<QString>("stateName");
    QTest::addColumn<bool>("copyState");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Remap state with name is valid") << "gear" << false << true;
    QTest::newRow("Remap state without name is not valid") << "" << false << false;
    QTest::newRow("Remap states with same name is not valid") << "gear" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidAddressSpaces()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidAddressSpaces()
{
    QFETCH(QString, spaceName);
    QFETCH(QString, range);
    QFETCH(QString, width);
    QFETCH(bool, copySpace);
    QFETCH(bool, isValid);

    QSharedPointer<AddressSpace> testSpace (new AddressSpace(spaceName, range, width));

    QSharedPointer<Component> testComponent (new Component(
        VLNV(VLNV::COMPONENT, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testComponent->getAddressSpaces()->append(testSpace);

    if (copySpace)
    {
        QSharedPointer<AddressSpace> otherSpace (new AddressSpace(*testSpace.data()));
        testComponent->getAddressSpaces()->append(otherSpace);
    }

    QSharedPointer<ComponentValidator> validator = createComponentValidator(0);
    QCOMPARE(validator->hasValidAddressSpaces(testComponent), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("Invalid name specified for address space %1 within component %2")
            .arg(spaceName).arg(testComponent->getVlnv().toString());

        if (range.isEmpty())
        {
            expectedError = QObject::tr("Invalid range set for address space %1 within component %2")
                .arg(spaceName).arg(testComponent->getVlnv().toString());
        }
        else if (width.isEmpty())
        {
            expectedError = QObject::tr("Invalid width set for address space %1 within component %2")
                .arg(spaceName).arg(testComponent->getVlnv().toString());
        }
        else if (copySpace)
        {
            expectedError = QObject::tr("Address space name %1 within component %2 is not unique.")
                .arg(spaceName).arg(testComponent->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidAddressSpaces_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidAddressSpaces_data()
{
    QTest::addColumn<QString>("spaceName");
    QTest::addColumn<QString>("range");
    QTest::addColumn<QString>("width");
    QTest::addColumn<bool>("copySpace");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Address space with name, range and width is valid") << "Dandy" << "10" << "27" << false << true;
    QTest::newRow("Address space without name is not valid") << "" << "10" << "27" << false << false;
    QTest::newRow("Address space without range is not valid") << "Dandy" << "" << "27" << false << false;
    QTest::newRow("Address space without width is not valid") << "Dandy" << "10" << "" << false << false;
    QTest::newRow("Address spaces with the same name is not valid") << "Dandy" << "10" << "27" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidMemoryMaps()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidMemoryMaps()
{
    QFETCH(QString, mapName);
    QFETCH(bool, copyMap);
    QFETCH(bool, isValid);

    QSharedPointer<MemoryMap> testMap (new MemoryMap(mapName));

    QSharedPointer<Component> testComponent (new Component(
        VLNV(VLNV::COMPONENT, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testComponent->getMemoryMaps()->append(testMap);

    if (copyMap)
    {
        QSharedPointer<MemoryMap> otherMap (new MemoryMap(*testMap.data()));
        testComponent->getMemoryMaps()->append(otherMap);
    }

    QSharedPointer<ComponentValidator> validator = createComponentValidator(0);
    QCOMPARE(validator->hasValidMemoryMaps(testComponent), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("Invalid name specified for memory map %1 within component %2")
            .arg(mapName).arg(testComponent->getVlnv().toString());
        
        if (copyMap)
        {
            expectedError = QObject::tr("Memory map name %1 within component %2 is not unique.")
                .arg(mapName).arg(testComponent->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidMemoryMap_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidMemoryMaps_data()
{
    QTest::addColumn<QString>("mapName");
    QTest::addColumn<bool>("copyMap");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Memory map with name is valid") << "Soulafein" << false << true;
    QTest::newRow("Memory map without name is not valid") << "" << false << false;
    QTest::newRow("Memory maps with the same name is not valid") << "Soulafein" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidViews()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidViews()
{
    QFETCH(QString, viewName);
    QFETCH(bool, copyView);
    QFETCH(bool, isValid);

    QSharedPointer<View> testView (new View(viewName));

    QSharedPointer<Component> testComponent (new Component(
        VLNV(VLNV::COMPONENT, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testComponent->getViews()->append(testView);

    if (copyView)
    {
        QSharedPointer<View> otherView (new View(*testView.data()));
        testComponent->getViews()->append(otherView);
    }

    QSharedPointer<ComponentValidator> validator = createComponentValidator(0);
    QCOMPARE(validator->hasValidViews(testComponent), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("Invalid name specified for view %1 within component %2")
            .arg(viewName).arg(testComponent->getVlnv().toString());

        if (copyView)
        {
            expectedError = QObject::tr("View name %1 within component %2 is not unique.")
                .arg(viewName).arg(testComponent->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidViews_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidViews_data()
{
    QTest::addColumn<QString>("viewName");
    QTest::addColumn<bool>("copyView");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("View with name is valid") << "Soulafein" << false << true;
    QTest::newRow("View without name is not valid") << "" << false << false;
    QTest::newRow("Views with the same name is not valid") << "Soulafein" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidComponentInstantiations()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidComponentInstantiations()
{
    QFETCH(QString, instantiationName);
    QFETCH(bool, copyInstantiation);
    QFETCH(bool, isValid);

    QSharedPointer<ComponentInstantiation> testInstantiation (new ComponentInstantiation(instantiationName));

    QSharedPointer<Component> testComponent (new Component(
        VLNV(VLNV::COMPONENT, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testComponent->getComponentInstantiations()->append(testInstantiation);

    if (copyInstantiation)
    {
        QSharedPointer<ComponentInstantiation> otherInstantiation (
            new ComponentInstantiation(*testInstantiation.data()));
        testComponent->getComponentInstantiations()->append(otherInstantiation);
    }

    QSharedPointer<ComponentValidator> validator = createComponentValidator(0);
    QCOMPARE(validator->hasValidComponentInstantiations(testComponent), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("Invalid name set for component instantiation %1 within "
            "component %2").arg(instantiationName).arg(testComponent->getVlnv().toString());

        if (copyInstantiation)
        {
            expectedError = QObject::tr("Component instantiation name %1 within component %2 is not unique.")
                .arg(instantiationName).arg(testComponent->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidComponentInstantiations_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidComponentInstantiations_data()
{
    QTest::addColumn<QString>("instantiationName");
    QTest::addColumn<bool>("copyInstantiation");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Component instantiation with name is valid") << "Soulafein" << false << true;
    QTest::newRow("Component instantiation without name is not valid") << "" << false << false;
    QTest::newRow("Component instantiations with the same name is not valid") << "Soulafein" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidDesignInstantiations()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidDesignInstantiations()
{
    QFETCH(QString, instantiationName);
    QFETCH(bool, hasReference);
    QFETCH(bool, copyInstantiation);
    QFETCH(bool, isValid);

    LibraryMock* mockLibrary (new LibraryMock(this));

    QSharedPointer<DesignInstantiation> testInstantiation (new DesignInstantiation(instantiationName));

    if (hasReference)
    {
        QSharedPointer<ConfigurableVLNVReference> designReference( new
            ConfigurableVLNVReference( VLNV::DESIGN, "Shiki", "No", "Uta", "0.3") );

        QSharedPointer<Design> testDesign (new Design(*designReference.data()));
        mockLibrary->addComponent(testDesign);

        testInstantiation->setDesignReference(designReference);
    }


    QSharedPointer<Component> testComponent (new Component(
        VLNV(VLNV::COMPONENT, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testComponent->getDesignInstantiations()->append(testInstantiation);

    if (copyInstantiation)
    {
        QSharedPointer<DesignInstantiation> otherInstantiation (
            new DesignInstantiation(*testInstantiation.data()));
        testComponent->getDesignInstantiations()->append(otherInstantiation);
    }

    QSharedPointer<ComponentValidator> validator = createComponentValidator(mockLibrary);
    QCOMPARE(validator->hasValidDesignInstantiations(testComponent), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("Invalid name set for design instantiation %1 within component %2")
            .arg(instantiationName).arg(testComponent->getVlnv().toString());

        if (copyInstantiation)
        {
            expectedError = QObject::tr("Design instantiation name %1 within component %2 is not unique.")
                .arg(instantiationName).arg(testComponent->getVlnv().toString());
        }
        else if (!hasReference)
        {
            expectedError = QObject::tr("Invalid design reference %1 set for design instantiation %2")
                .arg(testInstantiation->getDesignReference()->toString()).arg(testInstantiation->name());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidDesignInstantiations_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidDesignInstantiations_data()
{
    QTest::addColumn<QString>("instantiationName");
    QTest::addColumn<bool>("hasReference");
    QTest::addColumn<bool>("copyInstantiation");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Design instantiation with name and design reference is valid") <<
        "Balduran" << true << false << true;
    QTest::newRow("Design instantiation without name is not valid") << "" << true << false << false;
    QTest::newRow("Design instantiation without design reference is not valid") <<
        "Balduran" << false << false << false;
    QTest::newRow("Design instantiations with the same name is not valid") << "Balduran" << true << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidDesignConfigurationInstantiations()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidDesignConfigurationInstantiations()
{
    QFETCH(QString, instantiationName);
    QFETCH(bool, hasReference);
    QFETCH(bool, copyInstantiation);
    QFETCH(bool, isValid);

    LibraryMock* mockLibrary (new LibraryMock(this));

    QSharedPointer<DesignConfigurationInstantiation> testInstantiation (
        new DesignConfigurationInstantiation(instantiationName));

    if (hasReference)
    {
        QSharedPointer<ConfigurableVLNVReference> designConfigurationReference( new
            ConfigurableVLNVReference( VLNV::DESIGNCONFIGURATION, "Shiki", "No", "Uta", "0.3") );

        QSharedPointer<DesignConfiguration> testDesignConfiguration (
            new DesignConfiguration(*designConfigurationReference.data()));
        mockLibrary->addComponent(testDesignConfiguration);

        testInstantiation->setDesignConfigurationReference(designConfigurationReference);
    }


    QSharedPointer<Component> testComponent (new Component(
        VLNV(VLNV::COMPONENT, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testComponent->getDesignConfigurationInstantiations()->append(testInstantiation);

    if (copyInstantiation)
    {
        QSharedPointer<DesignConfigurationInstantiation> otherInstantiation (
            new DesignConfigurationInstantiation(*testInstantiation.data()));
        testComponent->getDesignConfigurationInstantiations()->append(otherInstantiation);
    }

    QSharedPointer<ComponentValidator> validator = createComponentValidator(mockLibrary);
    QCOMPARE(validator->hasValidDesignConfigurationInstantiations(testComponent), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("Invalid name set for design configuration instantiation %1 within "
            "component %2").arg(instantiationName).arg(testComponent->getVlnv().toString());

        if (copyInstantiation)
        {
            expectedError = QObject::tr("Design configuration instantiation name %1 within component %2 is not "
                "unique.").arg(instantiationName).arg(testComponent->getVlnv().toString());
        }
        else if (!hasReference)
        {
            expectedError = QObject::tr("Invalid design configuration reference %1 set for design configuration "
                "instantiation %2").arg(testInstantiation->getDesignConfigurationReference()->toString())
                .arg(testInstantiation->name());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidDesignConfigurationInstantiations_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidDesignConfigurationInstantiations_data()
{
    QTest::addColumn<QString>("instantiationName");
    QTest::addColumn<bool>("hasReference");
    QTest::addColumn<bool>("copyInstantiation");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Design configuration instantiation with name and design configuration reference is valid") <<
        "Balduran" << true << false << true;
    QTest::newRow("Design configuration instantiation without name is not valid") << "" << true << false << false;
    QTest::newRow("Design configuration instantiation without design configuration reference is not valid") <<
        "Balduran" << false << false << false;
    QTest::newRow("Design configuration instantiations with the same name is not valid") <<
        "Balduran" << true << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidPorts()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidPorts()
{
    QFETCH(QString, portName);
    QFETCH(QString, wireDirection);
    QFETCH(bool, copyPort);
    QFETCH(bool, isValid);

    QSharedPointer<Port> testPort (
        new Port(portName, DirectionTypes::str2Direction(wireDirection, DirectionTypes::DIRECTION_INVALID)));

    QSharedPointer<Component> testComponent (new Component(
        VLNV(VLNV::COMPONENT, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testComponent->getPorts()->append(testPort);

    if (copyPort)
    {
        QSharedPointer<Port> otherPort (new Port(*testPort.data()));
        testComponent->getPorts()->append(otherPort);
    }

    QSharedPointer<ComponentValidator> validator = createComponentValidator(0);
    QCOMPARE(validator->hasValidPorts(testComponent), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("Invalid name set for port %1 within component %2")
            .arg(portName).arg(testComponent->getVlnv().toString());

        if (wireDirection.isEmpty())
        {
            expectedError = QObject::tr("Invalid direction set for port %1 within component %2")
                .arg(portName).arg(testComponent->getVlnv().toString());
        }
        else if (copyPort)
        {
            expectedError = QObject::tr("Port name %1 within component %2 is not unique.")
                .arg(portName).arg(testComponent->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidPorts_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidPorts_data()
{
    QTest::addColumn<QString>("portName");
    QTest::addColumn<QString>("wireDirection");
    QTest::addColumn<bool>("copyPort");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Port with name and direction is valid") << "Screed" << "in" << false << true;
    QTest::newRow("Port without name is not valid") << "" << "in" << false << false;
    QTest::newRow("Port without direction is not valid") << "Screed" << "" << false << false;
    QTest::newRow("Ports with the same name is not valid") << "Screed" << "in" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidComponentGenerators()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidComponentGenerators()
{
    QFETCH(QString, generatorName);
    QFETCH(QString, generatorExe);
    QFETCH(bool, copyGenerator);
    QFETCH(bool, isValid);

    QSharedPointer<ComponentGenerator> testGenerator (new ComponentGenerator());
    testGenerator->setName(generatorName);
    testGenerator->setGeneratorExe(generatorExe);

    QSharedPointer<Component> testComponent (new Component(
        VLNV(VLNV::COMPONENT, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testComponent->getComponentGenerators()->append(testGenerator);

    if (copyGenerator)
    {
        QSharedPointer<ComponentGenerator> otherGenerator (new ComponentGenerator(*testGenerator.data()));
        testComponent->getComponentGenerators()->append(otherGenerator);
    }

    QSharedPointer<ComponentValidator> validator = createComponentValidator(0);
    QCOMPARE(validator->hasValidComponentGenerators(testComponent), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("Invalid name specified for component generator %1 within component %2").
            arg(generatorName).arg(testComponent->getVlnv().toString());

        if (generatorExe.isEmpty())
        {
            expectedError = QObject::tr("Invalid generator exe set for component generator '%1' within component %2")
                .arg(generatorName).arg(testComponent->getVlnv().toString());
        }
        else if (copyGenerator)
        {
            expectedError = QObject::tr("Component generator name '%1' within component %2 is not unique.")
                .arg(generatorName).arg(testComponent->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidComponentGenerators_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidComponentGenerators_data()
{
    QTest::addColumn<QString>("generatorName");
    QTest::addColumn<QString>("generatorExe");
    QTest::addColumn<bool>("copyGenerator");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Generator with name and generator exe is valid")
        << "Nuclear" << "../bin/run.sh" << false << true;
    QTest::newRow("Generator without a name is not valid") << "" << "../bin/run.sh" << false << false;
    QTest::newRow("Generator without generator exe is not valid") << "Nuclear" << "" << false << false;
    QTest::newRow("Generators with the same name is not valid") << "Nuclear" << "../bin/run.sh" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidChoices()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidChoices()
{
    QFETCH(QString, choiceName);
    QFETCH(QString, enumeration);
    QFETCH(bool, copyChoice);
    QFETCH(bool, createEnumeration);
    QFETCH(bool, isValid);

    QSharedPointer<Choice> testChoice (new Choice());
    testChoice->setName(choiceName);
    
    if (createEnumeration)
    {
        QSharedPointer<Enumeration> testEnumeration (new Enumeration());
        testEnumeration->setValue(enumeration);
        testChoice->enumerations()->append(testEnumeration);
    }

    QSharedPointer<Component> testComponent (new Component(
        VLNV(VLNV::COMPONENT, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testComponent->getChoices()->append(testChoice);

    if (copyChoice)
    {
        QSharedPointer<Choice> otherChoice (new Choice(*testChoice.data()));
        testComponent->getChoices()->append(otherChoice);
    }

    QSharedPointer<ComponentValidator> validator = createComponentValidator(0);
    QCOMPARE(validator->hasValidChoices(testComponent), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("Invalid name set for choice %1 within component %2")
            .arg(choiceName).arg(testComponent->getVlnv().toString());

        if (!createEnumeration)
        {
            expectedError = QObject::tr("No enumerations found in choice '%1' within component %2")
                .arg(choiceName).arg(testComponent->getVlnv().toString());
        }

        else if (enumeration.isEmpty())
        {
            expectedError = QObject::tr("Invalid value '%1' set for enumeration in choice '%2' within component %3")
                .arg(enumeration).arg(choiceName).arg(testComponent->getVlnv().toString());
        }

        else if (copyChoice)
        {
            expectedError = QObject::tr("Choice name %1 within component %2 is not unique.")
                .arg(choiceName).arg(testComponent->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidChoices_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidChoices_data()
{
    QTest::addColumn<QString>("choiceName");
    QTest::addColumn<QString>("enumeration");
    QTest::addColumn<bool>("createEnumeration");
    QTest::addColumn<bool>("copyChoice");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Choice with name and enumeration is valid") << "Ackbar" << "10" << true << false << true;
    QTest::newRow("Choice without name is not valid") << "" << "10" << true << false << false;
    QTest::newRow("Choice without enumeration is not valid") << "Ackbar" << "" << false << false << false;
    QTest::newRow("Choice with empty enumeration value is not valid") << "Ackbar" << "" << true << false << false;
    QTest::newRow("Choices with the same name is not valid") << "Ackbar" << "10" << true << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidFileSets()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidFileSets()
{
    QFETCH(QString, fileSetName);
    QFETCH(bool, copyFileSet);
    QFETCH(bool, isValid);

    QSharedPointer<FileSet> testFileSet (new FileSet(fileSetName));

    QSharedPointer<Component> testComponent (new Component(
        VLNV(VLNV::COMPONENT, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testComponent->getFileSets()->append(testFileSet);

    if (copyFileSet)
    {
        QSharedPointer<FileSet> otherFileSet (new FileSet(*testFileSet.data()));
        testComponent->getFileSets()->append(otherFileSet);
    }

    QSharedPointer<ComponentValidator> validator = createComponentValidator(0);
    QCOMPARE(validator->hasValidFileSets(testComponent), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("Invalid name '%1' set for file set within component %2")
            .arg(fileSetName).arg(testComponent->getVlnv().toString());

        if (copyFileSet)
        {
            expectedError = QObject::tr("File set name %1 within component %2 is not unique.")
                .arg(fileSetName).arg(testComponent->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidFileSets_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidFileSets_data()
{
    QTest::addColumn<QString>("fileSetName");
    QTest::addColumn<bool>("copyFileSet");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("File set with name is valid") << "Soulafein" << false << true;
    QTest::newRow("File set without name is not valid") << "" << false << false;
    QTest::newRow("File sets with the same name is not valid") << "Soulafein" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidCPUs()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidCPUs()
{
    QFETCH(QString, name);
    QFETCH(QString, referencedAddressSpace);
    QFETCH(bool, copyCPU);
    QFETCH(bool, isValid);

    QSharedPointer<Cpu> testCPU (new Cpu(name));

    QSharedPointer<Component> testComponent (new Component(
        VLNV(VLNV::COMPONENT, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testComponent->getCpus()->append(testCPU);

    if (!referencedAddressSpace.isEmpty())
    {
        QSharedPointer<AddressSpace> testSpace (new AddressSpace(referencedAddressSpace, "0", "8"));
        testComponent->getAddressSpaces()->append(testSpace);

        QStringList addressSpaceReferences;
        addressSpaceReferences.append(referencedAddressSpace);
        testCPU->setAddressSpaceRefs(addressSpaceReferences);
    }

    if (copyCPU)
    {
        QSharedPointer<Cpu> otherCPU (new Cpu(*testCPU.data()));
        testComponent->getCpus()->append(otherCPU);
    }

    QSharedPointer<ComponentValidator> validator = createComponentValidator(0);
    QCOMPARE(validator->hasValidCPUs(testComponent), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("Invalid name '%1' set for CPU within component %2.")
            .arg(name).arg(testComponent->getVlnv().toString());

        if (referencedAddressSpace.isEmpty())
        {
            expectedError = QObject::tr("No address space reference set for CPU %1 within component %2.")
                .arg(name).arg(testComponent->getVlnv().toString());
        }
        else if (copyCPU)
        {
            expectedError = QObject::tr("CPU name %1 within component %2 is not unique.")
                .arg(name).arg(testComponent->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidCPUs_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidCPUs_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("referencedAddressSpace");
    QTest::addColumn<bool>("copyCPU");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("CPU with name and address space reference is valid") << "QT" << "Dandy" << false << true;
    QTest::newRow("CPU without name is not valid") << "" << "Dandy" << false << false;
    QTest::newRow("CPU without address space reference is not valid") << "QT" << "" << false << false;
    QTest::newRow("CPUs with the same name is not valid") << "QT" << "Dandy" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidOtherClockDrivers()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidOtherClockDrivers()
{
    QFETCH(QString, clockName);
    QFETCH(bool, createTimeUnits);
    QFETCH(bool, copyClock);
    QFETCH(bool, isValid);

    QSharedPointer<OtherClockDriver> testDriver (new OtherClockDriver(clockName));

    if (createTimeUnits)
    {
        QSharedPointer<ClockUnit> clockPeriod (new ClockUnit("0.2"));
        testDriver->setClockPeriod(clockPeriod);
        QSharedPointer<ClockUnit> clockPulseOffset (new ClockUnit("0.02"));
        testDriver->setClockPulseOffset(clockPulseOffset);
        QSharedPointer<ClockUnit> clockPulseDuration (new ClockUnit("0.002"));
        testDriver->setClockPulseDuration(clockPulseDuration);
        testDriver->setClockPulseValue("1");
    }

    QSharedPointer<Component> testComponent (new Component(
        VLNV(VLNV::COMPONENT, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testComponent->getOtherClockDrivers()->append(testDriver);

    if (copyClock)
    {
        QSharedPointer<OtherClockDriver> otherDriver (new OtherClockDriver(*testDriver.data()));
        testComponent->getOtherClockDrivers()->append(otherDriver);
    }

    QSharedPointer<ComponentValidator> validator = createComponentValidator(0);
    QCOMPARE(validator->hasValidOtherClockDrivers(testComponent), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("Invalid name '%1' set for other clock driver within component %2")
            .arg(clockName).arg(testComponent->getVlnv().toString());

        if (copyClock)
        {
            expectedError = QObject::tr("Other clock driver name %1 within component %2 is not unique.")
                .arg(clockName).arg(testComponent->getVlnv().toString());
        }

        else if (!createTimeUnits)
        {
            expectedError = QObject::tr("Invalid clock period %1 set for other clock driver %2 within component %3")
                .arg(testDriver->getClockPeriod()->getValue()).arg(testDriver->getClockName())
                .arg(testComponent->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidOtherClockDrivers_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidOtherClockDrivers_data()
{
    QTest::addColumn<QString>("clockName");
    QTest::addColumn<bool>("createTimeUnits");
    QTest::addColumn<bool>("copyClock");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Other clock driver with name and clock units is valid") << "Time" << true << false << true;
    QTest::newRow("Other clock driver without name is not valid") << "" << true << false << false;
    QTest::newRow("Other clock driver without clock units is not valid") << "Time" << false << false << false;
    QTest::newRow("Other clock drivers with the same name is not valid") << "Time" << true << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidParameter()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidParameters()
{
    QFETCH(QString, name);
    QFETCH(QString, value);
    QFETCH(bool, copyParameter);
    QFETCH(bool, isValid);

    QSharedPointer<Parameter> testParameter (new Parameter());
    testParameter->setName(name);
    testParameter->setValue(value);

    QSharedPointer<Component> testComponent (new Component(
        VLNV(VLNV::COMPONENT, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testComponent->getParameters()->append(testParameter);

    if (copyParameter)
    {
        QSharedPointer<Parameter> otherParameter (new Parameter(*testParameter.data()));
        testComponent->getParameters()->append(otherParameter);
    }

    QSharedPointer<ComponentValidator> validator = createComponentValidator(0);
    QCOMPARE(validator->hasValidParameters(testComponent), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("No valid name specified for parameter %1 within component %2")
            .arg(name).arg(testComponent->getVlnv().toString());

        if (value.isEmpty())
        {
            expectedError = QObject::tr("No value specified for parameter %1 within component %2")
                .arg(name).arg(testComponent->getVlnv().toString());
        }
        else if (copyParameter)
        {
            expectedError = QObject::tr("Parameter name %1 within component %2 is not unique.")
                .arg(name).arg(testComponent->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidParameters_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidParameters_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("value");
    QTest::addColumn<bool>("copyParameter");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Parameter with name and value is valid") << "Gunmen" << "8" << false << true;
    QTest::newRow("Parameter without name is not valid") << "" << "8" << false << false;
    QTest::newRow("Parameter without value is not valid") << "Gunmen" << "" << false << false;
    QTest::newRow("Parameters with the same name is not valid") << "Gunmen" << "8" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidAssertions()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidAssertions()
{
    QFETCH(QString, name);
    QFETCH(QString, assertValue);
    QFETCH(bool, copyAssertion);
    QFETCH(bool, isValid);

    QSharedPointer<Assertion> testAssertion (new Assertion());
    testAssertion->setName(name);
    testAssertion->setAssert(assertValue);

    QSharedPointer<Component> testComponent (new Component(
        VLNV(VLNV::COMPONENT, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testComponent->getAssertions()->append(testAssertion);

    if (copyAssertion)
    {
        QSharedPointer<Assertion> otherAssertion (new Assertion(*testAssertion.data()));
        testComponent->getAssertions()->append(otherAssertion);
    }

    QSharedPointer<ComponentValidator> validator = createComponentValidator(0);
    QCOMPARE(validator->hasValidAssertions(testComponent), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testComponent);

        QString expectedError = QObject::tr("Invalid name set for assertion %1 within component %2")
            .arg(name).arg(testComponent->getVlnv().toString());

        QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
        bool assertOk = true;
        int parsedAssert = parser->parseExpression(assertValue).toInt(&assertOk);

        if (!assertOk || parsedAssert != 1)
        {
            expectedError = QObject::tr("Invalid assert set for assertion %1 within component %2")
                .arg(name).arg(testComponent->getVlnv().toString());
        }
        else if (copyAssertion)
        {
            expectedError = QObject::tr("Assertion name %1 within component %2 is not unique.")
                .arg(name).arg(testComponent->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::testHasValidAssertions_data()
//-----------------------------------------------------------------------------
void tst_ComponentValidator::testHasValidAssertions_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("assertValue");
    QTest::addColumn<bool>("copyAssertion");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Assertion with name and assert value of 1 is valid") << "Onibocho" << "1" << false << true;
    QTest::newRow("Assertion without name is not valid") << "" << "4-3" << false << false;
    QTest::newRow("Assertion with incorrect assert value is not valid") << "Onibocho" << "4*2" << false << false;
    QTest::newRow("Assertion with assert value 0 is not valid") << "Onibocho" << "1*2-2" << false << false;
    QTest::newRow("Assertion without assert value is not valid") << "Onibocho" << "" << false << false;
    QTest::newRow("Assertions with the same name is not valid") << "Onibocho" << "1" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_ComponentValidator::errorIsNotFoundInErrorList(QString const& expectedError,
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
// Function: tst_ComponentValidator::createComponentValidator()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentValidator> tst_ComponentValidator::createComponentValidator(LibraryMock* mockLibrary)
{
    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
    QSharedPointer<ComponentValidator> componentValidator (new ComponentValidator(parser, mockLibrary));

    return componentValidator;
}

QTEST_APPLESS_MAIN(tst_ComponentValidator)

#include "tst_ComponentValidator.moc"
