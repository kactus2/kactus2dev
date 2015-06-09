//-----------------------------------------------------------------------------
// File: tst_MemoryMapHeaderGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 05.05.2015
//
// Description:
// Unit test for class MemoryMapHeaderGenerator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <Plugins/MemoryMapHeaderGenerator/memorymapheadergenerator.h>
#include <Plugins/PluginSystem/PluginUtilityAdapter.h>

#include <tests/MockObjects/LibraryMock.h>

#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/register.h>
#include <IPXACTmodels/registermodel.h>
#include <IPXACTmodels/addressspace.h>
#include <IPXACTmodels/memorymap.h>
#include <IPXACTmodels/memorymapitem.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/ComponentInstance.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/masterinterface.h>
#include <IPXACTmodels/slaveinterface.h>
#include <IPXACTmodels/cpu.h>
#include <IPXACTmodels/Interconnection.h>
#include <IPXACTmodels/SystemView.h>
#include <IPXACTmodels/channel.h>
#include <IPXACTmodels/mirroredslaveinterface.h>
#include <IPXACTmodels/subspacemap.h>

#include <QFile>

class tst_MemoryMapHeaderGenerator : public QObject
{
    Q_OBJECT

public:
    tst_MemoryMapHeaderGenerator();

private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    void testLocalMemoryMapHeaderGeneration();
    void testGenerationWithHexadecimalRegisterOffset();
    void testGenerationWithReferencingRegisterOffset();
    void testLocalMemoryMapAddressBlockIsMemoryOrReserved();

    void testDesignMemoryMapHeaderGeneration();
    void testMemoryMapHeaderGenerationInDesignWithMultipleSlaves();
    void testDesignMemoryMapHeaderWithReferences();
    void testDesignMemoryMapHeaderWithConfigurableElements();
    
    void testDesignMemoryMapHeaderWithChannel();
    void testChannelDesignWithReferences();

    void testDesignMemoryMapHeaderWithBridge();
    void testDesignMemoryMapHeaderWithOpaqueBridge();

    //void testSystemMemoryMapHeaderGeneration();

private:

    MemoryMapHeaderGenerator* createTestGenerator();

    QSharedPointer<Register> createTestRegister(QString const& registerName, QString const& addressOffset);

    QSharedPointer<AddressBlock> createTestAddressBlock(QString const& addressName,
        QList<QSharedPointer<Register> > addressBlockRegisters);
    
    QSharedPointer<AddressSpace> createTestAddressSpace(QString const& spaceName,
        QSharedPointer<MemoryMap> localMemoryMap);

    QSharedPointer<MemoryMap> createTestMemoryMap(QString const& memoryMapName,
        QList<QSharedPointer<AddressBlock> > addressBlocks);

    QSharedPointer<Parameter> createTestParameter(QString const& paramName, QString const& paramID,
        QString const& value);

    QSharedPointer<Component> createTestMasterComponent(QString const& componentName,
        QString const& instanceBaseAddress);

    QSharedPointer<Component> createTestSlaveComponent(QString const& componentName, QString const& registerOffset);

    QSharedPointer<Design> createTestHWDesign(QString const& designName, QSharedPointer<Component> masterComponent,
        QSharedPointer<Component> slaveComponent);

    QSharedPointer<Component> createTestChannelComponent(QString const& componentName,
        QString const& mirroredMasterBaseAddress, QString const& mirroredSlaveRemap,
        QString const& mirroredSlaveRange);

    QSharedPointer<Design> createTestMiddleDesign(QString const& designName,
        QSharedPointer<Component> masterComponent, QSharedPointer<Component> slaveComponent,
        QSharedPointer<Component> middleComponent);

    QSharedPointer<Component> createTestBridgeComponent(QString const& componentName,
        QString const& masterBaseAddress, bool bridgeIsOpaque, QString const& bridgeReference);

    void readOutPutFile();

    MemoryMapHeaderGenerator* headerGenerator_;

    QSharedPointer<Component> topComponent_;

    LibraryMock library_;

    QString targetPath_;

    QString outPut_;
};

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::tst_MemoryMapHeaderGenerator()
//-----------------------------------------------------------------------------
tst_MemoryMapHeaderGenerator::tst_MemoryMapHeaderGenerator():
headerGenerator_(), topComponent_(), library_(this), targetPath_("./generatorOutput.h"), outPut_()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::initTestCase()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::init()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::init()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    topComponent_ = QSharedPointer<Component>(new Component(vlnv));

    library_.clear();

    headerGenerator_ = createTestGenerator();

    if (QFile::exists(targetPath_))
    {
        QWARN("Output file already exists.");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::cleanup()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::cleanup()
{
    topComponent_.clear();
    outPut_.clear();

    delete headerGenerator_;
    headerGenerator_ = 0;
    
    QFile::remove(targetPath_);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::testLocalMemoryMapHeaderGeneration()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testLocalMemoryMapHeaderGeneration()
{
    QSharedPointer<Register> headerRegister1 = createTestRegister("headerReg", "4");
    QSharedPointer<Register> headerRegister2 = createTestRegister("headerReg2", "10");
    QSharedPointer<Register> headerRegister3 = createTestRegister("headerReg3", "24");

    QList<QSharedPointer<Register> > registerList;
    registerList.append(headerRegister1);
    registerList.append(headerRegister2);

    QList<QSharedPointer<Register> > otherRegisterList;
    otherRegisterList.append(headerRegister3);

    QList<QSharedPointer<AddressBlock> > addressBlocks;
    QSharedPointer<AddressBlock> addressBlock = createTestAddressBlock("localAddressBlock", registerList);
    QSharedPointer<AddressBlock> otherAddressBlock = createTestAddressBlock("otherAddressBlock", otherRegisterList);
    QSharedPointer<AddressBlock> thirdAddressBlock = createTestAddressBlock("thirdBlock",
        QList<QSharedPointer<Register> > ());
    addressBlocks.append(addressBlock);
    addressBlocks.append(otherAddressBlock);
    addressBlocks.append(thirdAddressBlock);

    QSharedPointer<MemoryMap> localMemoryMap = createTestMemoryMap("localMemoryMap", addressBlocks);

    QList<QSharedPointer<AddressSpace> > addressSpaceList;
    QSharedPointer<AddressSpace> localAddressSpace = createTestAddressSpace("addressSpace1", localMemoryMap);
    addressSpaceList.append(localAddressSpace);

    topComponent_->setAddressSpaces(addressSpaceList);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setLocalSaveFileOptions(localMemoryMap, saveFileInfo, "swView");

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, this);
    QSharedPointer<LibraryComponent> libComponent = library_.getModel(*topComponent_->getVlnv());

    QSignalSpy generatorSpy(&adapter, SIGNAL(infoMessage(QString const&)));

    headerGenerator_->runGenerator(&adapter, libComponent);

    QCOMPARE(generatorSpy.count(), 4);

    QString initialMessage = generatorSpy.first().at(0).toString();
    QCOMPARE(initialMessage, QString("Running %1 %2.").arg(headerGenerator_->getName(),
        headerGenerator_->getVersion()));
    QString generatorStartMessage = generatorSpy.at(1).first().toString();
    QCOMPARE(generatorStartMessage, QString("Generation started %1").arg(QDateTime::currentDateTime().
        toString(Qt::LocalDate)));
    QString writingFinishedMessage = generatorSpy.at(2).first().toString();
    QCOMPARE(writingFinishedMessage, QString("Finished writing file generatorOutput.h."));
    QString generatorFinishedMessage = generatorSpy.at(3).first().toString();
    QCOMPARE(generatorFinishedMessage, QString("Generation complete."));

    readOutPutFile();

    QString headerDefine = "__" + topComponent_->getVlnv()->toString("_") + "_" + localMemoryMap->getName() + "_h";
    headerDefine = headerDefine.toUpper();

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 from local memory map \"" + localMemoryMap->getName() + "\".\n"
        " * This file contains addresses of the memories and registers defined in the local memory map.\n"
        " * Source component: " + topComponent_->getVlnv()->toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Address block: " + addressBlock->getName() + "\n"
        "*/\n"
        "/*\n"
        " * Register name: " + headerRegister1->getName() + "\n"
        " * Offset: 4\n"
        "*/\n"
        "#define " + headerRegister1->getName().toUpper() + " 0x4\n"
        "/*\n"
        " * Register name: " + headerRegister2->getName() + "\n"
        " * Offset: 10\n"
        "*/\n"
        "#define " + headerRegister2->getName().toUpper() + " 0xa\n"
        "\n"
        "/*\n"
        " * Address block: " + otherAddressBlock->getName() + "\n"
        "*/\n"
        "/*\n"
        " * Register name: " + headerRegister3->getName() + "\n"
        " * Offset: 24\n"
        "*/\n"
        "#define " + headerRegister3->getName().toUpper() + " 0x18\n"
        "\n"
        "/*\n"
        " * Address block: " + thirdAddressBlock->getName() + "\n"
        "*/\n"
        "\n"
        "#endif /* " + headerDefine + " */\n"
        );

    if (!outPut_.contains(expectedOutput))
    {
        QStringList outputLines = outPut_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(outPut_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (outPut_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(outPut_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::testGenerationWithHexadecimalNumber()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testGenerationWithHexadecimalRegisterOffset()
{
    QSharedPointer<Register> headerRegister1 = createTestRegister("headerReg", "'ha");

    QList<QSharedPointer<Register> > registerList;
    registerList.append(headerRegister1);

    QList<QSharedPointer<AddressBlock> > addressBlocks;
    QSharedPointer<AddressBlock> addressBlock = createTestAddressBlock("localAddressBlock", registerList);
    addressBlocks.append(addressBlock);

    QSharedPointer<MemoryMap> localMemoryMap = createTestMemoryMap("localMemoryMap", addressBlocks);

    QList<QSharedPointer<AddressSpace> > addressSpaceList;
    QSharedPointer<AddressSpace> localAddressSpace = createTestAddressSpace("addressSpace1", localMemoryMap);
    addressSpaceList.append(localAddressSpace);

    topComponent_->setAddressSpaces(addressSpaceList);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setLocalSaveFileOptions(localMemoryMap, saveFileInfo, "swView");

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, this);
    QSharedPointer<LibraryComponent> libComponent = library_.getModel(*topComponent_->getVlnv());

    headerGenerator_->runGenerator(&adapter, libComponent);

    readOutPutFile();

    QString headerDefine = "__" + topComponent_->getVlnv()->toString("_") + "_" + localMemoryMap->getName() + "_h";
    headerDefine = headerDefine.toUpper();

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 from local memory map \"" + localMemoryMap->getName() + "\".\n"
        " * This file contains addresses of the memories and registers defined in the local memory map.\n"
        " * Source component: " + topComponent_->getVlnv()->toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Address block: " + addressBlock->getName() + "\n"
        "*/\n"
        "/*\n"
        " * Register name: " + headerRegister1->getName() + "\n"
        " * Offset: 'ha\n"
        "*/\n"
        "#define " + headerRegister1->getName().toUpper() + " 0xa\n"
        "\n"
        "#endif /* " + headerDefine + " */\n"
    );

    if (!outPut_.contains(expectedOutput))
    {
        QStringList outputLines = outPut_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(outPut_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (outPut_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(outPut_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::testGenerationWithReferencingRegisterOffset()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testGenerationWithReferencingRegisterOffset()
{
    QSharedPointer<Parameter> referencedParameter = createTestParameter("testParameter", "Param-ID", "40");
    topComponent_->getParameters()->append(referencedParameter);

    QSharedPointer<Register> headerRegister1 = createTestRegister("headerReg", "Param-ID");
    QSharedPointer<Register> headerRegister2 = createTestRegister("headerReg2", "Param-ID * 2");
    QSharedPointer<Register> headerRegister3 = createTestRegister("headerReg3", "'h4 + Param-ID");

    QList<QSharedPointer<Register> > registerList;
    registerList.append(headerRegister1);
    registerList.append(headerRegister2);
    registerList.append(headerRegister3);

    QList<QSharedPointer<AddressBlock> > addressBlocks;
    QSharedPointer<AddressBlock> addressBlock = createTestAddressBlock("localAddressBlock", registerList);
    addressBlocks.append(addressBlock);

    QSharedPointer<MemoryMap> localMemoryMap = createTestMemoryMap("localMemoryMap", addressBlocks);

    QList<QSharedPointer<AddressSpace> > addressSpaceList;
    QSharedPointer<AddressSpace> localAddressSpace = createTestAddressSpace("addressSpace1", localMemoryMap);
    addressSpaceList.append(localAddressSpace);

    topComponent_->setAddressSpaces(addressSpaceList);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setLocalSaveFileOptions(localMemoryMap, saveFileInfo, "swView");

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, this);
    QSharedPointer<LibraryComponent> libComponent = library_.getModel(*topComponent_->getVlnv());

    headerGenerator_->runGenerator(&adapter, libComponent);

    readOutPutFile();

    QString headerDefine = "__" + topComponent_->getVlnv()->toString("_") + "_" + localMemoryMap->getName() + "_h";
    headerDefine = headerDefine.toUpper();

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 from local memory map \"" + localMemoryMap->getName() + "\".\n"
        " * This file contains addresses of the memories and registers defined in the local memory map.\n"
        " * Source component: " + topComponent_->getVlnv()->toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Address block: " + addressBlock->getName() + "\n"
        "*/\n"
        "/*\n"
        " * Register name: " + headerRegister1->getName() + "\n"
        " * Offset: testParameter = 0x28\n"
        "*/\n"
        "#define " + headerRegister1->getName().toUpper() + " 0x28\n"
        "/*\n"
        " * Register name: " + headerRegister2->getName() + "\n"
        " * Offset: testParameter * 2 = 0x50\n"
        "*/\n"
        "#define " + headerRegister2->getName().toUpper() + " 0x50\n"
         "/*\n"
        " * Register name: " + headerRegister3->getName() + "\n"
        " * Offset: 'h4 + testParameter = 0x2c\n"
        "*/\n"
        "#define " + headerRegister3->getName().toUpper() + " 0x2c\n"
        "\n"
        "#endif /* " + headerDefine + " */\n"
    );

    if (!outPut_.contains(expectedOutput))
    {
        QStringList outputLines = outPut_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(outPut_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (outPut_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(outPut_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::testLocalMemoryMapAddressBlockIsMemory()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testLocalMemoryMapAddressBlockIsMemoryOrReserved()
{
    QSharedPointer<Parameter> referencedParameter = createTestParameter("refParam", "ID", "14");
    topComponent_->getParameters()->append(referencedParameter);

    QSharedPointer<AddressBlock> memoryAddressBlock = createTestAddressBlock("headerOne",
        QList<QSharedPointer<Register> > ());
    memoryAddressBlock->setUsage(General::MEMORY);
    memoryAddressBlock->setWidthExpression("4 * 8");
    memoryAddressBlock->setRange("8");
    memoryAddressBlock->setBaseAddress("ID + 2");

    QSharedPointer<AddressBlock> reservedAddressBlock = createTestAddressBlock("headerTwo",
        QList<QSharedPointer<Register> > ());
    reservedAddressBlock->setUsage(General::RESERVED);
    reservedAddressBlock->setWidth(16);
    reservedAddressBlock->setRange("ID * ID");

    QList<QSharedPointer<AddressBlock> > addressList;
    addressList.append(memoryAddressBlock);
    addressList.append(reservedAddressBlock);

    QSharedPointer<MemoryMap> localMemoryMap = createTestMemoryMap("localMap", addressList);

    QSharedPointer<AddressSpace> localAddressSpace = createTestAddressSpace("localSpace", localMemoryMap);
    QList<QSharedPointer<AddressSpace> > addressSpaceList;
    addressSpaceList.append(localAddressSpace);

    topComponent_->setAddressSpaces(addressSpaceList);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setLocalSaveFileOptions(localMemoryMap, saveFileInfo, "swView");

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, this);
    QSharedPointer<LibraryComponent> libComponent = library_.getModel(*topComponent_->getVlnv());

    headerGenerator_->runGenerator(&adapter, libComponent);

    readOutPutFile();

    QString headerDefine = "__" + topComponent_->getVlnv()->toString("_") + "_" + localMemoryMap->getName() + "_h";
    headerDefine = headerDefine.toUpper();

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 from local memory map \"" + localMemoryMap->getName() + "\".\n"
        " * This file contains addresses of the memories and registers defined in the local memory map.\n"
        " * Source component: " + topComponent_->getVlnv()->toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Memory block name: " + memoryAddressBlock->getName() + "\n"
        " * Width: 32 = " + memoryAddressBlock->getWidthExpression() + "\n"
        " * Range: " + memoryAddressBlock->getRange() + "\n"
        "*/\n"
        "#define " + memoryAddressBlock->getName().toUpper() + "_START 0x10 // refParam + 2" +
            "\n"
        "#define " + memoryAddressBlock->getName().toUpper() + "_END 0x17\n"
        "\n"
         "/*\n"
         " * Reserved block name: " + reservedAddressBlock->getName() + "\n"
         " * Width: 16\n"
         " * Range: 196 = refParam * refParam\n"
         "*/\n"
         "#define " + reservedAddressBlock->getName().toUpper() + "_START 0x" +
             reservedAddressBlock->getBaseAddress() + "\n"
         "#define " + reservedAddressBlock->getName().toUpper() + "_END 0xc3\n"
         "\n"
         "#endif /* " + headerDefine + " */\n"
        );

    if (!outPut_.contains(expectedOutput))
    {
        QStringList outputLines = outPut_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(outPut_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (outPut_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(outPut_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderGeneration()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderGeneration()
{
    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "8");
    QSharedPointer<Component> slaveComponent = createTestSlaveComponent("slaveComponent", "4");
    QSharedPointer<Design> headerDesign = createTestHWDesign("testDesign", masterComponent, slaveComponent);

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(headerDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/testDesign.1.0.xml", topComponent_);
    
    View* headerTestView = new View("headerView");
    headerTestView->setHierarchyRef(*headerDesign->getVlnv());
    topComponent_->addView(headerTestView);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, this);

    QSignalSpy generatorSpy(&adapter, SIGNAL(infoMessage(QString const&)));

    headerGenerator_->runGenerator(&adapter, topComponent_, QSharedPointer<LibraryComponent>(), headerDesign);

    QCOMPARE(generatorSpy.count(), 4);

    QString initialMessage = generatorSpy.first().at(0).toString();
    QCOMPARE(initialMessage, QString("Running %1 %2.").arg(headerGenerator_->getName(),
        headerGenerator_->getVersion()));
    QString generatorStartMessage = generatorSpy.at(1).first().toString();
    QCOMPARE(generatorStartMessage, QString("Generation started %1").arg(QDateTime::currentDateTime().
        toString(Qt::LocalDate)));
    QString writingFinishedMessage = generatorSpy.at(2).first().toString();
    QCOMPARE(writingFinishedMessage, QString("Finished writing file generatorOutput.h."));
    QString generatorFinishedMessage = generatorSpy.at(3).first().toString();
    QCOMPARE(generatorFinishedMessage, QString("Generation complete."));


    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 for instance \"masterInstance_0\" interface \"masterBusInterface\".\n"
        " * This file contains addresses of the memories and registers defined in the memory maps of connected "
            "components.\n"
        " * Source component: " + topComponent_->getVlnv()->toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Instance: slaveInstance_0 Interface: slaveBusInterface\n"
        " * Instance base address: 0x8\n"
        " * Source component: " + slaveComponent->getVlnv()->toString(":") + "\n"
        " * The defines for the memory map \"slaveMemoryMap\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddressBlock\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: 4\n"
        "*/\n"
        "#define SLAVEINSTANCE_0_SLAVEREGISTER 0xc\n"
        "\n"
        "#endif /* " + headerDefine + " */"
        );

    if (!outPut_.contains(expectedOutput))
    {
        QStringList outputLines = outPut_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(outPut_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (outPut_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(outPut_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderGenerationWithMultipleSlaves()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testMemoryMapHeaderGenerationInDesignWithMultipleSlaves()
{
    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "0");
    QSharedPointer<Component> slaveComponent = createTestSlaveComponent("slaveComponent", "4");
    QSharedPointer<Component> slaveComponentTwo = createTestSlaveComponent("slaveComponentTwo", "8");

    QSharedPointer<Design> headerDesign (new Design(VLNV(VLNV::DESIGN, "TUT", "TestLibrary", "headerDesign",
        "1.0")));

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponentTwo);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponentTwo.1.0.xml", topComponent_);
    library_.addComponent(headerDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/testDesign.1.0.xml", topComponent_);

    ComponentInstance masterInstance ("masterInstance_0", "", "", *masterComponent->getVlnv(), QPointF(),
        "masterID");
    ComponentInstance slaveInstanceOne ("slaveInstance_0", "", "", *slaveComponent->getVlnv(), QPointF(),
        "slaveID_0");
    ComponentInstance slaveInstanceTwo ("slaveInstance_1", "", "", *slaveComponent->getVlnv(), QPointF(),
        "slaveID_1");
    ComponentInstance slaveInstanceThree ("slaveInstanceTwo_0", "", "", *slaveComponentTwo->getVlnv(), QPointF(),
        "slaveIDTwo_0");
    QList<ComponentInstance> componentInstances;
    componentInstances.append(masterInstance);
    componentInstances.append(slaveInstanceOne);
    componentInstances.append(slaveInstanceTwo);
    componentInstances.append(slaveInstanceThree);
    headerDesign->setComponentInstances(componentInstances);

    Interface interfaceMaster (masterInstance.getInstanceName(), masterComponent->getBusInterfaceNames().at(0));
    Interface interfaceSlaveOne (slaveInstanceOne.getInstanceName(), slaveComponent->getBusInterfaceNames().at(0));
    Interface interfaceSlaveTwo (slaveInstanceTwo.getInstanceName(), slaveComponent->getBusInterfaceNames().at(0));
    Interface interfaceSlaveThree (slaveInstanceThree.getInstanceName(), slaveComponent->getBusInterfaceNames().at(0));

    Interconnection masterSlaveOneConnection("masterSlaveOne", interfaceMaster, interfaceSlaveOne);
    Interconnection masterSlaveTwoConnection("masterSlaveTwo", interfaceMaster, interfaceSlaveTwo);
    Interconnection masterSlaveThreeConnection("msaterSlaveThree", interfaceMaster, interfaceSlaveThree);
    QList <Interconnection> interconnectionList;
    interconnectionList.append(masterSlaveOneConnection);
    interconnectionList.append(masterSlaveTwoConnection);
    interconnectionList.append(masterSlaveThreeConnection);
    headerDesign->setInterconnections(interconnectionList);
    
    View* headerTestView = new View("headerView");
    headerTestView->setHierarchyRef(*headerDesign->getVlnv());
    topComponent_->addView(headerTestView);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, this);

    headerGenerator_->runGenerator(&adapter, topComponent_, QSharedPointer<LibraryComponent>(), headerDesign);

    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 for instance \"masterInstance_0\" interface \"masterBusInterface\".\n"
        " * This file contains addresses of the memories and registers defined in the memory maps of connected "
            "components.\n"
        " * Source component: " + topComponent_->getVlnv()->toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Instance: " + slaveInstanceOne.getInstanceName() + " Interface: " +
            slaveComponent->getBusInterfaceNames().at(0) + "\n"
        " * Instance base address: 0x0\n"
        " * Source component: " + slaveComponent->getVlnv()->toString(":") + "\n"
        " * The defines for the memory map \"slaveMemoryMap\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddressBlock\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: 4\n"
        "*/\n"
        "#define " + slaveInstanceOne.getInstanceName().toUpper() + "_SLAVEREGISTER 0x4\n"
        "\n"
        "/*\n"
        " * Instance: " + slaveInstanceTwo.getInstanceName() + " Interface: " +
            slaveComponent->getBusInterfaceNames().at(0) + "\n"
        " * Instance base address: 0x0\n"
        " * Source component: " + slaveComponent->getVlnv()->toString(":") + "\n"
        " * The defines for the memory map \"slaveMemoryMap\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddressBlock\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: 4\n"
        "*/\n"
        "#define " + slaveInstanceTwo.getInstanceName().toUpper() + "_SLAVEREGISTER 0x4\n"
        "\n"
        "/*\n"
        " * Instance: " + slaveInstanceThree.getInstanceName() + " Interface: " +
            slaveComponentTwo->getBusInterfaceNames().at(0) + "\n"
        " * Instance base address: 0x0\n"
        " * Source component: " + slaveComponentTwo->getVlnv()->toString(":") + "\n"
        " * The defines for the memory map \"slaveMemoryMap\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddressBlock\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: 8\n"
        "*/\n"
        "#define " + slaveInstanceThree.getInstanceName().toUpper() + "_SLAVEREGISTER 0x8\n"
        "\n"
        );

    if (!outPut_.contains(expectedOutput))
    {
        QStringList outputLines = outPut_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(outPut_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (outPut_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(outPut_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithReferences()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithReferences()
{
    QSharedPointer<Parameter> masterParameter = createTestParameter("masterParameter", "param_ID", "14");
    QSharedPointer<Parameter> slaveParameter = createTestParameter("slaveParameter", "param_ID", "10");

    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "8");
    QSharedPointer<Component> slaveComponent = createTestSlaveComponent("slaveComponent", "4 * param_ID");
    QSharedPointer<Design> headerDesign = createTestHWDesign("testDesign", masterComponent, slaveComponent);

    masterComponent->getParameters()->append(masterParameter);
    slaveComponent->getParameters()->append(slaveParameter);

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(headerDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/testDesign.1.0.xml", topComponent_);
    
    View* headerTestView = new View("headerView");
    headerTestView->setHierarchyRef(*headerDesign->getVlnv());
    topComponent_->addView(headerTestView);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, this);

    headerGenerator_->runGenerator(&adapter, topComponent_, QSharedPointer<LibraryComponent>(), headerDesign);

    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 for instance \"masterInstance_0\" interface \"masterBusInterface\".\n"
        " * This file contains addresses of the memories and registers defined in the memory maps of connected "
            "components.\n"
        " * Source component: " + topComponent_->getVlnv()->toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Instance: slaveInstance_0 Interface: slaveBusInterface\n"
        " * Instance base address: 0x8\n"
        " * Source component: " + slaveComponent->getVlnv()->toString(":") + "\n"
        " * The defines for the memory map \"slaveMemoryMap\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddressBlock\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: 4 * slaveParameter = 0x28\n"
        "*/\n"
        "#define SLAVEINSTANCE_0_SLAVEREGISTER 0x30\n"
        "\n"
        "#endif /* " + headerDefine + " */"
        );

    if (!outPut_.contains(expectedOutput))
    {
        QStringList outputLines = outPut_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(outPut_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (outPut_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(outPut_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithConfigurableElements()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithConfigurableElements()
{
    QSharedPointer<Parameter> slaveParameter = createTestParameter("slaveParameter", "param_ID", "10");
    QSharedPointer<Parameter> otherSlaveParameter = createTestParameter("otherParameter", "other_ID", "14");

    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "0");

    QSharedPointer<Component> slaveComponent (new Component(VLNV(VLNV::COMPONENT, "TUT", "TestLibrary",
        "slaveComponent", "1.0")));

    QSharedPointer<Register> slaveRegister = createTestRegister("slaveRegister", "param_ID * 6");
    QSharedPointer<Register> otherSlaveRegister = createTestRegister("otherRegister", "other_ID + 2");
    QList<QSharedPointer<Register> > slaveRegisterList;
    slaveRegisterList.append(slaveRegister);
    slaveRegisterList.append(otherSlaveRegister);

    QSharedPointer<AddressBlock> slaveAddress = createTestAddressBlock("slaveAddress", slaveRegisterList);
    QList<QSharedPointer<AddressBlock> > slaveAddressList;
    slaveAddressList.append(slaveAddress);

    QSharedPointer<MemoryMap> slaveMemory = createTestMemoryMap("slaveMemory", slaveAddressList);
    QList<QSharedPointer<MemoryMap> > slaveMemoryList;
    slaveMemoryList.append(slaveMemory);

    QSharedPointer<BusInterface> slaveBus (new BusInterface());
    slaveBus->setName("slaveBusInterface");
    QSharedPointer<SlaveInterface> enslavedInterface(new SlaveInterface);
    enslavedInterface->setMemoryMapRef(slaveMemory->getName());
    slaveBus->setSlave(enslavedInterface);

    slaveComponent->setMemoryMaps(slaveMemoryList);

    QList<QSharedPointer<BusInterface> > slaveBusInterfaceList;
    slaveBusInterfaceList.append(slaveBus);
    slaveComponent->setBusInterfaces(slaveBusInterfaceList);

    slaveComponent->getParameters()->append(slaveParameter);
    slaveComponent->getParameters()->append(otherSlaveParameter);

    QSharedPointer<Design> headerDesign (new Design(VLNV(VLNV::DESIGN, "TUT", "TestLibrary", "testDesign", "1.0")));

    QSharedPointer<DesignConfiguration> headerDesignConfiguration (new DesignConfiguration(VLNV(
        VLNV::DESIGNCONFIGURATION, "TUT", "TestLibrary", "testDesignConfiguration", "1.0")));

    ComponentInstance masterInstance ("masterInstance_0", "", "", *masterComponent->getVlnv(), QPointF(),
        "masterID");
    ComponentInstance slaveInstance ("slaveInstance_0", "", "", *slaveComponent->getVlnv(), QPointF(), "slaveID");

    QMap<QString, QString> slaveConfigurableElements;
    slaveConfigurableElements.insert("param_ID", "4");
    slaveConfigurableElements.insert("other_ID", "param_ID + 20");

    QList<ComponentInstance> componentInstances;
    componentInstances.append(masterInstance);
    componentInstances.append(slaveInstance);
    headerDesign->setComponentInstances(componentInstances);

    headerDesignConfiguration->setConfigurableElementValues(slaveInstance.getUuid(), slaveConfigurableElements);

    Interface interfaceMaster (masterInstance.getInstanceName(), masterComponent->getBusInterfaceNames().at(0));
    Interface interfaceSlave (slaveInstance.getInstanceName(), slaveComponent->getBusInterfaceNames().at(0));

    Interconnection masterSlaveConnection("masterSlaveConnection", interfaceMaster, interfaceSlave);
    QList<Interconnection> interconnectionList;
    interconnectionList.append(masterSlaveConnection);
    headerDesign->setInterconnections(interconnectionList);

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(headerDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/testDesign.1.0.xml", topComponent_);
    library_.addComponent(headerDesignConfiguration);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/testDesignConfiguration.1.0.xml",
        topComponent_);

    View* headerTestView = new View("headerView");
    headerTestView->setHierarchyRef(*headerDesignConfiguration->getVlnv());
    topComponent_->addView(headerTestView);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, this);

    headerGenerator_->runGenerator(&adapter, topComponent_, headerDesignConfiguration, headerDesign);

    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 for instance \"masterInstance_0\" interface \"masterBusInterface\".\n"
        " * This file contains addresses of the memories and registers defined in the memory maps of connected "
            "components.\n"
        " * Source component: " + topComponent_->getVlnv()->toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Instance: slaveInstance_0 Interface: slaveBusInterface\n"
        " * Instance base address: 0x0\n"
        " * Source component: " + slaveComponent->getVlnv()->toString(":") + "\n"
        " * The defines for the memory map \"slaveMemory\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddress\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: slaveParameter * 6 = 0x18\n"
        "*/\n"
        "#define SLAVEINSTANCE_0_SLAVEREGISTER 0x18\n"
        "/*\n"
        " * Register name: otherRegister\n"
        " * Offset: otherParameter + 2 = 0x1a\n"
        "*/\n"
        "#define SLAVEINSTANCE_0_OTHERREGISTER 0x1a\n"
        "\n"
        "#endif /* " + headerDefine + " */"
        );

    if (!outPut_.contains(expectedOutput))
    {
        QStringList outputLines = outPut_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(outPut_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (outPut_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(outPut_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }

    // Check that the parameters were not changed.
    QCOMPARE(slaveParameter->getValue(), QString("10"));
    QCOMPARE(otherSlaveParameter->getValue(), QString("14"));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithChannel()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithChannel()
{
    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "4");
    QSharedPointer<Component> slaveComponent = createTestSlaveComponent("slaveComponent", "8");
    QSharedPointer<Component> channelComponent = createTestChannelComponent("channelComponent", "0", "4", "32");
    QSharedPointer<Design> headerDesign = createTestMiddleDesign("channelDesign", masterComponent, slaveComponent,
        channelComponent);

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(channelComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/channelComponent.1.0.xml", topComponent_);
    library_.addComponent(headerDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/channelDesign.1.0.xml", topComponent_);

    View* headerTestView = new View("headerView");
    headerTestView->setHierarchyRef(*headerDesign->getVlnv());
    topComponent_->addView(headerTestView);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, this);

    headerGenerator_->runGenerator(&adapter, topComponent_, QSharedPointer<LibraryComponent>(), headerDesign);

    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 for instance \"masterInstance_0\" interface \"masterBusInterface\".\n"
        " * This file contains addresses of the memories and registers defined in the memory maps of connected "
            "components.\n"
        " * Source component: " + topComponent_->getVlnv()->toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Instance: slaveInstance_0 Interface: slaveBusInterface\n"
        " * Instance base address: 0x8\n"
        " * Source component: " + slaveComponent->getVlnv()->toString(":") + "\n"
        " * The defines for the memory map \"slaveMemoryMap\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddressBlock\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: 8\n"
        "*/\n"
        "#define SLAVEINSTANCE_0_SLAVEREGISTER 0x10\n"
        "\n"
        "#endif /* " + headerDefine + " */\n"
        );

    if (!outPut_.contains(expectedOutput))
    {
        QStringList outputLines = outPut_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(outPut_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (outPut_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(outPut_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::testChannelDesignWithReferences()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testChannelDesignWithReferences()
{
    QSharedPointer<Parameter> slaveParameter = createTestParameter("slaveParameter", "param_ID", "8");
    QSharedPointer<Parameter> otherSlaveParameter = createTestParameter("otherParameter", "other_ID", "16");

    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "4");
    QSharedPointer<Component> slaveComponent = createTestSlaveComponent("slaveComponent", "8");
    QSharedPointer<Component> channelComponent = createTestChannelComponent("channelComponent", "0", "param_ID",
        "other_ID");
    QSharedPointer<Design> headerDesign = createTestMiddleDesign("channelDesign", masterComponent, slaveComponent,
        channelComponent);

    channelComponent->getParameters()->append(slaveParameter);
    channelComponent->getParameters()->append(otherSlaveParameter);

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(channelComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/channelComponent.1.0.xml", topComponent_);
    library_.addComponent(headerDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/channelDesign.1.0.xml", topComponent_);

    View* headerTestView = new View("headerView");
    headerTestView->setHierarchyRef(*headerDesign->getVlnv());
    topComponent_->addView(headerTestView);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, this);

    headerGenerator_->runGenerator(&adapter, topComponent_, QSharedPointer<LibraryComponent>(), headerDesign);

    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 for instance \"masterInstance_0\" interface \"masterBusInterface\".\n"
        " * This file contains addresses of the memories and registers defined in the memory maps of connected "
            "components.\n"
        " * Source component: " + topComponent_->getVlnv()->toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Instance: slaveInstance_0 Interface: slaveBusInterface\n"
        " * Instance base address: 0xc\n"
        " * Source component: " + slaveComponent->getVlnv()->toString(":") + "\n"
        " * The defines for the memory map \"slaveMemoryMap\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddressBlock\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: 8\n"
        "*/\n"
        "#define SLAVEINSTANCE_0_SLAVEREGISTER 0x14\n"
        "\n"
        "#endif /* " + headerDefine + " */\n"
        );

    if (!outPut_.contains(expectedOutput))
    {
        QStringList outputLines = outPut_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(outPut_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (outPut_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(outPut_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithBridge()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithBridge()
{
    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "4");
    QSharedPointer<Component> slaveComponent = createTestSlaveComponent("slaveComponent", "4");
    QSharedPointer<Component> bridgeComponent = createTestBridgeComponent("bridgeComponent", "16", false, "");

    QSharedPointer<Design> headerDesign = createTestMiddleDesign("bridgeDesign", masterComponent, slaveComponent,
        bridgeComponent);

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(bridgeComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/bridgeComponent.1.0.xml", topComponent_);
    library_.addComponent(headerDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/bridgeDesign.1.0.xml", topComponent_);

        View* headerTestView = new View("headerView");
    headerTestView->setHierarchyRef(*headerDesign->getVlnv());
    topComponent_->addView(headerTestView);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, this);

    headerGenerator_->runGenerator(&adapter, topComponent_, QSharedPointer<LibraryComponent>(), headerDesign);

    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        " * Created on: " + QDate::currentDate().toString("dd.MM.yyyy") + "\n"
        " * Generated by: \n"
        " *\n"
        " * Description:\n"
        " * Header file generated by Kactus2 for instance \"masterInstance_0\" interface \"masterBusInterface\".\n"
        " * This file contains addresses of the memories and registers defined in the memory maps of connected "
            "components.\n"
        " * Source component: " + topComponent_->getVlnv()->toString(":") + ".\n"
        "*/\n"
        "\n"
        "#ifndef " + headerDefine + "\n"
        "#define " + headerDefine + "\n"
        "\n"
        "/*\n"
        " * Instance: slaveInstance_0 Interface: slaveBusInterface\n"
        " * Instance base address: 0x14\n"
        " * Source component: " + slaveComponent->getVlnv()->toString(":") + "\n"
        " * The defines for the memory map \"slaveMemoryMap\":\n"
        "*/\n"
        "\n"
        "/*\n"
        " * Address block: slaveAddressBlock\n"
        "*/\n"
        "/*\n"
        " * Register name: slaveRegister\n"
        " * Offset: 4\n"
        "*/\n"
        "#define SLAVEINSTANCE_0_SLAVEREGISTER 0x18\n"
        "\n"
        "#endif /* " + headerDefine + " */\n"
        );

    if (!outPut_.contains(expectedOutput))
    {
        QStringList outputLines = outPut_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(outPut_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (outPut_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(outPut_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithOpaqueBridge()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::testDesignMemoryMapHeaderWithOpaqueBridge()
{
    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "0");
    QSharedPointer<Component> slaveComponent = createTestSlaveComponent("slaveComponent", "2");

    QSharedPointer<MemoryMap> opaqueMemoryMap = createTestMemoryMap("opaqueMemoryMap",
        QList<QSharedPointer<AddressBlock> > ());
    
    QSharedPointer<SubspaceMap> opaqueSubspace (new SubspaceMap());
    opaqueSubspace->setBaseAddress("16");

    QList<QSharedPointer<MemoryMapItem> > opaqueItems;
    opaqueItems.append(opaqueSubspace);
    opaqueMemoryMap->setItems(opaqueItems);

    QSharedPointer<Component> bridgeComponent = createTestBridgeComponent("opqueBridge", "4", true,
        opaqueMemoryMap->getName());

    QList<QSharedPointer<MemoryMap> > bridgeMemoryMapList;
    bridgeMemoryMapList.append(opaqueMemoryMap);
    bridgeComponent->setMemoryMaps(bridgeMemoryMapList);

    QSharedPointer<Design> headerDesign = createTestMiddleDesign("bridgeDesign", masterComponent, slaveComponent,
        bridgeComponent);

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(bridgeComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/bridgeComponent.1.0.xml", topComponent_);
    library_.addComponent(headerDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/bridgeDesign.1.0.xml", topComponent_);

        View* headerTestView = new View("headerView");
    headerTestView->setHierarchyRef(*headerDesign->getVlnv());
    topComponent_->addView(headerTestView);

    QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, this);

    QSignalSpy errorMessageSignals(&adapter, SIGNAL(errorMessage(QString const&)));

    headerGenerator_->runGenerator(&adapter, topComponent_, QSharedPointer<LibraryComponent>(), headerDesign);

    QCOMPARE(errorMessageSignals.count(), 1);
    QString errorMessage = errorMessageSignals.first().first().toString();
    QString expectedErrorMessage("An opaque bridge was found in interface bridgeSlaveBusInterface in instance "
        "channelInstance_0. Currently, opaque bridges are handled as transparent bridges.");
    QCOMPARE(errorMessage, expectedErrorMessage);

    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "/*\n"
        " * File: generatorOutput.h\n"
        );

    if (!outPut_.contains(expectedOutput))
    {
        QStringList outputLines = outPut_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(outPut_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (outPut_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(outPut_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::testSystemMemoryMapHeaderGeneration()
//-----------------------------------------------------------------------------
/*
void tst_MemoryMapHeaderGenerator::testSystemMemoryMapHeaderGeneration()
{
    QSharedPointer<Component> masterComponent = createTestMasterComponent("masterComponent", "0");
    QSharedPointer<Component> slaveComponent = createTestSlaveComponent("slaveComponent", "8");
    QSharedPointer<Design> hwDesign = createTestHWDesign("hwDesign", masterComponent, slaveComponent);
    QSharedPointer<DesignConfiguration> hwDesignConfiguration (new DesignConfiguration(VLNV(
        VLNV::DESIGNCONFIGURATION, "TUT", "TestLibrary", "hwDesignConfiguration", "1.0")));

    QSharedPointer<Design> systemDesign (new Design(VLNV(VLNV::DESIGN, "TUT", "TestLibrary", "systemDesign",
        "1.0")));
    QSharedPointer<DesignConfiguration> systemDesignConfiguration (new DesignConfiguration(VLNV(
        VLNV::DESIGNCONFIGURATION, "TUT", "TestLibrary", "systemDesignConfiguration", "1.0")));

    library_.addComponent(masterComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/masterComponent.1.0.xml", topComponent_);
    library_.addComponent(slaveComponent);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/slaveComponent.1.0.xml", topComponent_);
    library_.addComponent(hwDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/hwDesign.1.0.xml", topComponent_);
    library_.addComponent(hwDesignConfiguration);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/hwDesignConfiguration.1.0.xml", topComponent_);
    library_.addComponent(systemDesign);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/systemDesign.1.0.xml", topComponent_);
    library_.addComponent(systemDesignConfiguration);
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/systemDesignConfiguration.1.0.xml",
        topComponent_);

    View* hwDesignView = new View("headerView");
    hwDesignView->setHierarchyRef(*hwDesignConfiguration->getVlnv());
    topComponent_->addView(hwDesignView);

    SystemView* systemDesignView = new SystemView("systemView");
    systemDesignView->setHierarchyRef(*systemDesignConfiguration->getVlnv());
    systemDesignView->setHWViewRef(hwDesignView->getName());
    topComponent_->addSystemView(systemDesignView);

       QFileInfo saveFileInfo (targetPath_);

    headerGenerator_->setGlobalSaveFileOptions(topComponent_, "masterInstance_0", "masterID",
        masterComponent->getBusInterfaceNames().at(0), saveFileInfo);

    QWidget parentWidget;
    PluginUtilityAdapter adapter(&library_, &parentWidget, this);

    headerGenerator_->runGenerator(&adapter, topComponent_, systemDesignConfiguration, systemDesign);

    readOutPutFile();

    QString headerDefine = "__MASTERINSTANCE_0_" + masterComponent->getBusInterfaceNames().at(0).toUpper() + "_H";

    QString expectedOutput(
        "abc"
        );

    if (!outPut_.contains(expectedOutput))
    {
        QStringList outputLines = outPut_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutPutFile();
            QCOMPARE(outPut_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (outPut_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(outPut_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}*/

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestGenerator()
//-----------------------------------------------------------------------------
MemoryMapHeaderGenerator* tst_MemoryMapHeaderGenerator::createTestGenerator()
{
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml", topComponent_);
    library_.addComponent(topComponent_);

    MemoryMapHeaderGenerator* generator = new MemoryMapHeaderGenerator();

    return generator;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestRegister()
//-----------------------------------------------------------------------------
QSharedPointer<Register> tst_MemoryMapHeaderGenerator::createTestRegister(QString const& registerName,
    QString const& addressOffset)
{
    QSharedPointer<Register> newRegister (new Register());
    newRegister->setName(registerName);
    newRegister->setAddressOffset(addressOffset);
    newRegister->setSize(16);

    return newRegister;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestAddressBlock()
//-----------------------------------------------------------------------------
QSharedPointer<AddressBlock> tst_MemoryMapHeaderGenerator::createTestAddressBlock(QString const& addressName,
    QList<QSharedPointer<Register> > addressBlockRegisters)
{
    QSharedPointer<AddressBlock> newAddressBlock (new AddressBlock());
    newAddressBlock->setName(addressName);
    newAddressBlock->setBaseAddress("0");
    newAddressBlock->setRange("16");
    newAddressBlock->setWidth(32);
    newAddressBlock->setUsage(General::REGISTER);
    QList<QSharedPointer<RegisterModel> >& registerModelRef(newAddressBlock->getRegisterData());

    foreach(QSharedPointer<Register> targetRegister, addressBlockRegisters)
    {
        registerModelRef.append(targetRegister);
    }

    return newAddressBlock;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestMemoryMap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMap> tst_MemoryMapHeaderGenerator::createTestMemoryMap(QString const& memoryMapName,
    QList<QSharedPointer<AddressBlock> > addressBlocks)
{
    QSharedPointer<MemoryMap> newMemoryMap (new MemoryMap());
    newMemoryMap->setName(memoryMapName);
    newMemoryMap->setAddressUnitBits(8);

    QList<QSharedPointer<MemoryMapItem> > newMemoryMapItems = newMemoryMap->getItems();

    foreach (QSharedPointer<AddressBlock> newAddressBlock, addressBlocks)
    {
        newMemoryMapItems.append(newAddressBlock);
    }

    newMemoryMap->setItems(newMemoryMapItems);
    return newMemoryMap;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestAddressSpace()
//-----------------------------------------------------------------------------
QSharedPointer<AddressSpace> tst_MemoryMapHeaderGenerator::createTestAddressSpace(QString const& spaceName,
    QSharedPointer<MemoryMap> localMemoryMap)
{
    QSharedPointer<AddressSpace> newAddressSpace(new AddressSpace());
    newAddressSpace->setName(spaceName);
    newAddressSpace->setLocalMemoryMap(localMemoryMap);

    return newAddressSpace;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> tst_MemoryMapHeaderGenerator::createTestParameter(QString const& paramName,
    QString const& paramID, QString const& value)
{
    QSharedPointer<Parameter> newParameter(new Parameter);
    newParameter->setName(paramName);
    newParameter->setValueId(paramID);
    newParameter->setValue(value);

    return newParameter;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestMasterComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_MemoryMapHeaderGenerator::createTestMasterComponent(QString const& componentName,
    QString const& instanceBaseAddress)
{
    QSharedPointer<Component> newMaster (new Component(VLNV(VLNV::COMPONENT, "TUT", "TestLibrary", componentName,
        "1.0")));

    QSharedPointer<AddressSpace> localAddressSpace = createTestAddressSpace("localSpace", QSharedPointer<MemoryMap> ());
    QList<QSharedPointer<AddressSpace> > addressSpaceList;
    addressSpaceList.append(localAddressSpace);

    newMaster->setAddressSpaces(addressSpaceList);

    QStringList addressSpaceRefs;
    addressSpaceRefs.append(localAddressSpace->getName());

    QSharedPointer<Cpu> masterCpu (new Cpu());
    masterCpu->setAddressSpaceRefs(addressSpaceRefs);
    QList<QSharedPointer<Cpu> > cpuList;
    cpuList.append(masterCpu);

    newMaster->setCpus(cpuList);

    QSharedPointer<BusInterface> masterBus (new BusInterface());
    masterBus->setName("masterBusInterface");
    QSharedPointer<MasterInterface> interfaceMaster(new MasterInterface);
    interfaceMaster->setBaseAddress(instanceBaseAddress);
    interfaceMaster->setAddressSpaceRef(localAddressSpace->getName());
    masterBus->setMaster(interfaceMaster);
    masterBus->setInterfaceMode(General::MASTER);

    QList<QSharedPointer<BusInterface> > busInterfaceList;
    busInterfaceList.append(masterBus);
    newMaster->setBusInterfaces(busInterfaceList);

    return newMaster;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createSlaveComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_MemoryMapHeaderGenerator::createTestSlaveComponent(QString const& componentName,
    QString const& registerOffset)
{
    QSharedPointer<Component> newSlave (new Component(VLNV(VLNV::COMPONENT, "TUT", "TestLibrary", componentName,
        "1.0")));

    QSharedPointer<Register> slaveRegister = createTestRegister("slaveRegister", registerOffset);
    QList<QSharedPointer<Register> > componentRegisters;
    componentRegisters.append(slaveRegister);

    QSharedPointer<AddressBlock> slaveAddressBlock = createTestAddressBlock("slaveAddressBlock",
        componentRegisters);
    QList<QSharedPointer<AddressBlock> > addressBlocks;
    addressBlocks.append(slaveAddressBlock);

    QSharedPointer<MemoryMap> slaveMemoryMap = createTestMemoryMap("slaveMemoryMap", addressBlocks);
    QList<QSharedPointer<MemoryMap> > memoryMaps;
    memoryMaps.append(slaveMemoryMap);
    newSlave->setMemoryMaps(memoryMaps);

    QSharedPointer<BusInterface> slaveBus (new BusInterface());
    slaveBus->setName("slaveBusInterface");
    QSharedPointer<SlaveInterface> interfaceSlave(new SlaveInterface);
    interfaceSlave->setMemoryMapRef(slaveMemoryMap->getName());
    slaveBus->setSlave(interfaceSlave);

    QList<QSharedPointer<BusInterface> > busInterfaceList;
    busInterfaceList.append(slaveBus);
    newSlave->setBusInterfaces(busInterfaceList);

    return newSlave;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> tst_MemoryMapHeaderGenerator::createTestHWDesign(QString const& designName,
    QSharedPointer<Component> masterComponent, QSharedPointer<Component> slaveComponent)
{
    QSharedPointer<Design> newDesign(new Design(VLNV(VLNV::DESIGN, "TUT", "TestLibrary", designName, "1.0")));

    ComponentInstance masterInstance ("masterInstance_0", "", "", *masterComponent->getVlnv(), QPointF(),
        "masterID");
    ComponentInstance slaveInstance ("slaveInstance_0", "", "", *slaveComponent->getVlnv(), QPointF(), "slaveID");
    QList<ComponentInstance> componentInstances;
    componentInstances.append(masterInstance);
    componentInstances.append(slaveInstance);
    newDesign->setComponentInstances(componentInstances);

    Interface interfaceMaster (masterInstance.getInstanceName(), masterComponent->getBusInterfaceNames().at(0));
    Interface interfaceSlave (slaveInstance.getInstanceName(), slaveComponent->getBusInterfaceNames().at(0));

    Interconnection masterSlaveConnection("masterSlaveConnection", interfaceMaster, interfaceSlave);
    QList<Interconnection> interconnectionList;
    interconnectionList.append(masterSlaveConnection);
    newDesign->setInterconnections(interconnectionList);

    return newDesign;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestChannelComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_MemoryMapHeaderGenerator::createTestChannelComponent(QString const& componentName,
    QString const& mirroredMasterBaseAddress, QString const& mirroredSlaveRemap, QString const& mirroredSlaveRange)
{
    QSharedPointer<Component> newChannelComponent (new Component(VLNV(VLNV::COMPONENT, "TUT", "TestLibrary",
        componentName, "1.0")));

    QSharedPointer<Register> localRegister = createTestRegister("mirroredRegister", "4");
    QList<QSharedPointer<Register> > localRegisterList;
    localRegisterList.append(localRegister);

    QSharedPointer<AddressBlock> mirroredAddressBlock = createTestAddressBlock("mirroredAddressBlock",
        localRegisterList);
    QList<QSharedPointer<AddressBlock> > localAddressBlockList;
    localAddressBlockList.append(mirroredAddressBlock);

    QSharedPointer<MemoryMap> localMemoryMap = createTestMemoryMap("localMasterMemory", localAddressBlockList);

    QSharedPointer<AddressSpace> localAddressSpace = createTestAddressSpace("localAddressSpace", localMemoryMap);
    QList<QSharedPointer<AddressSpace> > addressSpaceList;
    addressSpaceList.append(localAddressSpace);

    newChannelComponent->setAddressSpaces(addressSpaceList);

    // Create mirrored master interface.
    QSharedPointer<BusInterface> mirroredMasterBus(new BusInterface);
    mirroredMasterBus->setName("mirroredMasterInterface");
    QSharedPointer<MasterInterface> mirroredMasterInterface(new MasterInterface);
    mirroredMasterInterface->setBaseAddress(mirroredMasterBaseAddress);
    mirroredMasterInterface->setAddressSpaceRef(localAddressSpace->getName());
    mirroredMasterBus->setMaster(mirroredMasterInterface);
    mirroredMasterBus->setInterfaceMode(General::MIRROREDMASTER);

    // Create mirrored slave interface.
    QSharedPointer<MirroredSlaveInterface> interfaceMirroredSlave (new MirroredSlaveInterface);
    interfaceMirroredSlave->setRemapAddress(mirroredSlaveRemap);
    interfaceMirroredSlave->setRange(mirroredSlaveRange);

    QSharedPointer<BusInterface> mirroredSlaveBus(new BusInterface);
    mirroredSlaveBus->setName("mirroredSlaveInterface");
    mirroredSlaveBus->setMirroredSlave(interfaceMirroredSlave);
    mirroredSlaveBus->setInterfaceMode(General::MIRROREDSLAVE);

    QList<QSharedPointer<BusInterface> > busInterfaceList;
    busInterfaceList.append(mirroredMasterBus);
    busInterfaceList.append(mirroredSlaveBus);
    newChannelComponent->setBusInterfaces(busInterfaceList);

    QStringList interfaceNames;
    interfaceNames.append(mirroredMasterBus->getName());
    interfaceNames.append(mirroredSlaveBus->getName());

    QSharedPointer<Channel> masterSlaveChannel (new Channel());
    masterSlaveChannel->setName("masterSlaveChannel");
    masterSlaveChannel->setInterfaces(interfaceNames);

    QList<QSharedPointer<Channel> > channelList;
    channelList.append(masterSlaveChannel);
    newChannelComponent->setChannels(channelList);

    return newChannelComponent;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestChannelDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> tst_MemoryMapHeaderGenerator::createTestMiddleDesign(QString const& designName,
    QSharedPointer<Component> masterComponent, QSharedPointer<Component> slaveComponent,
    QSharedPointer<Component> middleComponent)
{
    QSharedPointer<Design> newDesign(new Design(VLNV(VLNV::DESIGN, "TUT", "TestLibrary", designName, "1.0")));

    ComponentInstance masterInstance ("masterInstance_0", "", "", *masterComponent->getVlnv(), QPointF(),
        "masterID");
    ComponentInstance slaveInstance ("slaveInstance_0", "", "", *slaveComponent->getVlnv(), QPointF(), "slaveID");
    ComponentInstance channelInstance ("channelInstance_0", "", "", *middleComponent->getVlnv(), QPointF(),
        "middleID");
    QList<ComponentInstance> componentInstances;
    componentInstances.append(masterInstance);
    componentInstances.append(slaveInstance);
    componentInstances.append(channelInstance);
    newDesign->setComponentInstances(componentInstances);

    Interface interfaceMaster (masterInstance.getInstanceName(), masterComponent->getBusInterfaceNames().at(0));
    Interface interfaceMiddleFirst (channelInstance.getInstanceName(),
        middleComponent->getBusInterfaceNames().at(0));
    Interface interfaceMiddleSecond (channelInstance.getInstanceName(),
        middleComponent->getBusInterfaceNames().at(1));
    Interface interfaceSlave (slaveInstance.getInstanceName(), slaveComponent->getBusInterfaceNames().at(0));

    Interconnection masterMiddleConnect("masterMirrored", interfaceMaster, interfaceMiddleFirst);
    Interconnection middleSlaveConnect("mirroredSlave", interfaceMiddleSecond, interfaceSlave);

    QList<Interconnection> interconnectionList;
    interconnectionList.append(masterMiddleConnect);
    interconnectionList.append(middleSlaveConnect);

    newDesign->setInterconnections(interconnectionList);

    return newDesign;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::createTestBridgeComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_MemoryMapHeaderGenerator::createTestBridgeComponent(QString const& componentName,
    QString const& masterBaseAddress, bool bridgeIsOpaque, QString const& bridgeReference)
{
    QSharedPointer<Component> newBridgeComponent (new Component(VLNV(VLNV::COMPONENT, "TUT", "TestLibrary",
        componentName, "1.0")));

    QSharedPointer<Register> masterRegister = createTestRegister("bridgeMasterRegister", "4");
    QList<QSharedPointer<Register> > masterRegisterList;
    masterRegisterList.append(masterRegister);

    QSharedPointer<AddressBlock> masterAddressBlock = createTestAddressBlock("bridgeMasterAddressBlock",
        masterRegisterList);
    QList<QSharedPointer<AddressBlock> > masterAddressBlockList;
    masterAddressBlockList.append(masterAddressBlock);

    QSharedPointer<MemoryMap> masterMemory = createTestMemoryMap("bridgeMasterMemory", masterAddressBlockList);

    QSharedPointer<AddressSpace> masterAddressSpace = createTestAddressSpace("bridgetMasterSpace", masterMemory);
    QList<QSharedPointer<AddressSpace> > masterSpaceList;
    masterSpaceList.append(masterAddressSpace);

    newBridgeComponent->setAddressSpaces(masterSpaceList);

    // Create master bus interface.
    QSharedPointer<MasterInterface> bridgeMasterInterface(new MasterInterface);
    bridgeMasterInterface->setBaseAddress(masterBaseAddress);
    bridgeMasterInterface->setAddressSpaceRef(masterAddressSpace->getName());

    QSharedPointer<BusInterface> bridgeMasterBus(new BusInterface);
    bridgeMasterBus->setName("bridgeMasterBusInterface");
    bridgeMasterBus->setMaster(bridgeMasterInterface);
    bridgeMasterBus->setInterfaceMode(General::MASTER);

    // Create slave bus interface.
    QSharedPointer<SlaveInterface> bridgeSlaveInterface (new SlaveInterface);
    if (!bridgeReference.isEmpty())
    {
        bridgeSlaveInterface->setMemoryMapRef(bridgeReference);
    }

    QSharedPointer<SlaveInterface::Bridge> bridge(new SlaveInterface::Bridge());
    bridge->masterRef_ = bridgeMasterBus->getName();
    bridge->opaque_ = bridgeIsOpaque;

    QList<QSharedPointer<SlaveInterface::Bridge> > slaveInterfaceBridges;
    slaveInterfaceBridges.append(bridge);
    bridgeSlaveInterface->setBridges(slaveInterfaceBridges);

    QSharedPointer<BusInterface> bridgeSlaveBus (new BusInterface);
    bridgeSlaveBus->setName("bridgeSlaveBusInterface");
    bridgeSlaveBus->setSlave(bridgeSlaveInterface);
    bridgeSlaveBus->setInterfaceMode(General::SLAVE);

    QList<QSharedPointer<BusInterface> > busInterfaceList;
    busInterfaceList.append(bridgeSlaveBus);
    busInterfaceList.append(bridgeMasterBus);
    newBridgeComponent->setBusInterfaces(busInterfaceList);

    return newBridgeComponent;
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapHeaderGenerator::readOutPutFile()
//-----------------------------------------------------------------------------
void tst_MemoryMapHeaderGenerator::readOutPutFile()
{
    QFile outputFile(targetPath_);
    QVERIFY(outputFile.open(QIODevice::ReadOnly));

    outPut_ = outputFile.readAll();
    outputFile.close();
}

//QTEST_APPLESS_MAIN(tst_MemoryMapHeaderGenerator)
QTEST_MAIN(tst_MemoryMapHeaderGenerator)

#include "tst_MemoryMapHeaderGenerator.moc"
