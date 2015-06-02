//-----------------------------------------------------------------------------
// File: tst_QuartusGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.05.2015
//
// Description:
// Unit test for class QuartusGenerator.
//-----------------------------------------------------------------------------

#include <kactusGenerators/quartusGenerator/quartusgenerator.h>

#include <tests/MockObjects/LibraryMock.h>

#include <IPXACTmodels/librarycomponent.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/ComponentInstance.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/file.h>

#include <QtTest>
#include <QDateTime>

namespace
{
    const QString PROJECTFILE = ".qpf";
    const QString SETTINGSFILE = ".qsf";
};

class tst_QuartusGenerator : public QObject
{
    Q_OBJECT

public:
    tst_QuartusGenerator();

private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testGeneratorWithoutFilesets();

    void testGeneratorWithDesignContainingTopComponentVerilogFile();
    void testGeneratorWithDesignContainingInstances();

    void testGeneratorWithConfiguredViews();
    void testGeneratorInInstancesWithoutActiveViews();

private:

    void readOutputFile(QString const& fileType);

    void compareOutputToExpected(QString const& fileType, QString const& expectedOutput);

    QString getQuartusHeader(QString const& currentTime);

    QSharedPointer<Component> createTestComponent(QString const& componentName);

    QSharedPointer<Design> createTestDesign(QString const& designName,
        QList<QSharedPointer<Component> > containedComponents);

    QSharedPointer<DesignConfiguration> createTestDesignConfiguration(QString const& name,
        QSharedPointer<Design> referencedDesign);

    QSharedPointer<FileSet> createTestFileset (QString const& filesetName, QStringList filesToAdd);

    QSharedPointer<QFile> createEmptyFile(QString const& fileName);

    void setViewOverridesForDesignConfiguration(QSharedPointer<DesignConfiguration> testDesignConfiguration,
        QList<ComponentInstance> componentInstances);

    void setFileTypesForFileSet(QSharedPointer<FileSet> selectedFileset, QStringList fileTypes);

    QSharedPointer<Component> topComponent_;

    LibraryMock library_;

    QWidget* generatorParentWidget_;

    QuartusGenerator* quartusGenerator_;

    QString targetPath_;

    QString targetEntity_;

    QString output_;
};

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::tst_QuartusGenerator()
//-----------------------------------------------------------------------------
tst_QuartusGenerator::tst_QuartusGenerator():
    topComponent_(), generatorParentWidget_(new QWidget), library_(this), quartusGenerator_(), targetPath_("./"),
    targetEntity_("generatorOutput"), output_()
{

}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::initTestCase()
//-----------------------------------------------------------------------------
void tst_QuartusGenerator::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_QuartusGenerator::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::init()
//-----------------------------------------------------------------------------
void tst_QuartusGenerator::init()
{
    library_.clear();

    quartusGenerator_ = new QuartusGenerator(&library_, generatorParentWidget_);

    topComponent_ = createTestComponent("topComponent");

    if (QFile::exists(targetEntity_ + PROJECTFILE))
    {
        QWARN("Quartus project file already exists.");
    }

    if (QFile::exists(targetEntity_ + SETTINGSFILE))
    {
        QWARN("Quartus settings file already exists.");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::cleanup()
//-----------------------------------------------------------------------------
void tst_QuartusGenerator::cleanup()
{
    topComponent_.clear();
    output_.clear();

    delete quartusGenerator_;
    quartusGenerator_ = 0;

    QFile::remove(targetEntity_ + PROJECTFILE);
    QFile::remove(targetEntity_ + SETTINGSFILE);
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::testGeneratorWithoutFilesets()
//-----------------------------------------------------------------------------
void tst_QuartusGenerator::testGeneratorWithoutFilesets()
{
    QString currentTime = QDateTime::currentDateTime().toString(QString("hh:mm:ss dd.MM.yyyy"));
    quartusGenerator_->replaceTime(currentTime);

    quartusGenerator_->generateProject(targetPath_, targetEntity_);

    QString expectedProjectOutput( getQuartusHeader(currentTime) + 
        "QUARTUS_VERSION = \"10.0\"\n"
        "\n"
        "# Revisions\n"
        "\n"
        "PROJECT_REVISION = \"generatorOutput\"");

    compareOutputToExpected(PROJECTFILE, expectedProjectOutput);

    QString expectedSettingsOutput ( getQuartusHeader(currentTime) +
        "set_global_assignment -name TOP_LEVEL_ENTITY generatorOutput\n"
        "set_global_assignment -name ORIGINAL_QUARTUS_VERSION \"10.0 SP1\"");

    compareOutputToExpected(SETTINGSFILE, expectedSettingsOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::testGeneratorWithDesignContainingTopComponentVerilogFile()
//-----------------------------------------------------------------------------
void tst_QuartusGenerator::testGeneratorWithDesignContainingTopComponentVerilogFile()
{
    QSharedPointer<Design> quartusDesign = createTestDesign("quartusDesign", QList<QSharedPointer<Component> > ());

    QSharedPointer<QFile> topVerilogFile = createEmptyFile("topComponentVerilog.v");

    QStringList filesForTopComponent;
    filesForTopComponent.append(topVerilogFile->fileName());

    QSharedPointer<FileSet> topComponentFileSet = createTestFileset("verilogFileSet", filesForTopComponent);
    topComponent_->addFileSet(topComponentFileSet);

    View* quartusTopView (new View("quartusTopView"));
    quartusTopView->addFileSetRef(topComponentFileSet->getName());
    topComponent_->addView(quartusTopView);

    View* quartusTestView (new View("quartusView"));
    quartusTestView->setHierarchyRef(*quartusDesign->getVlnv());
    quartusTestView->setTopLevelView(quartusTopView->getName());
    topComponent_->addView(quartusTestView);

    QString currentTime = QDateTime::currentDateTime().toString(QString("hh:mm:ss dd.MM.yyyy"));
    quartusGenerator_->replaceTime(currentTime);

    quartusGenerator_->parseFiles(topComponent_, quartusTestView->getName());
    quartusGenerator_->generateProject(targetPath_, targetEntity_);

    QString expectedProjectOutput( getQuartusHeader(currentTime) + 
        "QUARTUS_VERSION = \"10.0\"\n"
        "\n"
        "# Revisions\n"
        "\n"
        "PROJECT_REVISION = \"generatorOutput\"");

    compareOutputToExpected(PROJECTFILE, expectedProjectOutput);

    QString expectedSettingsOutput ( getQuartusHeader(currentTime) +
        "set_global_assignment -name TOP_LEVEL_ENTITY generatorOutput\n"
        "set_global_assignment -name ORIGINAL_QUARTUS_VERSION \"10.0 SP1\"\n"
        "set_global_assignment -name VERILOG_FILE " + QFileInfo(*topVerilogFile).absoluteFilePath() + "\n"
        );

    compareOutputToExpected(SETTINGSFILE, expectedSettingsOutput);

    QFile::remove(topVerilogFile->fileName());
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::testGeneratorWithDesignContainingInstances()
//-----------------------------------------------------------------------------
void tst_QuartusGenerator::testGeneratorWithDesignContainingInstances()
{
    QSharedPointer<Component> componentOne = createTestComponent("componentOne");
    QSharedPointer<QFile> componentOneVhdlFile = createEmptyFile("componentOneVHDL.vhdl");

    QStringList componentOneFiles;
    componentOneFiles.append(componentOneVhdlFile->fileName());
    QSharedPointer<FileSet> componentOneFileset = createTestFileset("vhdlFiles", componentOneFiles);
    componentOne->addFileSet(componentOneFileset);

    View* componentOneView (new View("vhdlView"));
    componentOneView->addFileSetRef(componentOneFileset->getName());
    componentOne->addView(componentOneView);

    QSharedPointer<Component> componentTwo = createTestComponent("componentTwo");
    QSharedPointer<QFile> componentTwoQipFile = createEmptyFile("componentTwoQip.qip");

    QStringList componentTwoFiles;
    componentTwoFiles.append(componentTwoQipFile->fileName());
    QSharedPointer<FileSet> componentTwoFileset = createTestFileset("qipFiles", componentTwoFiles);
    componentTwo->addFileSet(componentTwoFileset);

    View* componentTwoView (new View("qipView"));
    componentTwoView->addFileSetRef(componentTwoFileset->getName());
    componentTwo->addView(componentTwoView);

    QSharedPointer<Component> componentThree = createTestComponent("componentThree");
    QSharedPointer<QFile> componentThreeSdcFile = createEmptyFile("componentThreeSdc.sdc");

    QStringList componentThreeFiles;
    componentThreeFiles.append(componentThreeSdcFile->fileName());
    QSharedPointer<FileSet> componentThreeFileSet = createTestFileset("sdcFiles", componentThreeFiles);
    componentThree->addFileSet(componentThreeFileSet);

    View* componentThreeView (new View("sdcView"));
    componentThreeView->addFileSetRef(componentThreeFileSet->getName());
    componentThree->addView(componentThreeView);

    QList<QSharedPointer<Component> > componentsInDesign;
    componentsInDesign.append(componentOne);
    componentsInDesign.append(componentTwo);
    componentsInDesign.append(componentThree);

    QSharedPointer<Design> quartusDesign = createTestDesign("quartusDesign", componentsInDesign);

    QSharedPointer<DesignConfiguration> quartusDesignConf = createTestDesignConfiguration("quartusDesignConf",
        quartusDesign);

    QSharedPointer<QFile> topVerilogFile = createEmptyFile("topComponentVerilog.v");

    QStringList topComponentFiles;
    topComponentFiles.append(topVerilogFile->fileName());
    QSharedPointer<FileSet> topComponentFileSet = createTestFileset("topFiles", topComponentFiles);
    topComponent_->addFileSet(topComponentFileSet);

    View* quartusTopLevelView (new View("quartusTopView"));
    quartusTopLevelView->addFileSetRef(topComponentFileSet->getName());
    topComponent_->addView(quartusTopLevelView);

    View* quartusDesignView (new View("quartusView"));
    quartusDesignView->setHierarchyRef(*quartusDesignConf->getVlnv());
    quartusDesignView->setTopLevelView(quartusTopLevelView->getName());
    topComponent_->addView(quartusDesignView);

    QString currentTime = QDateTime::currentDateTime().toString(QString("hh:mm:ss dd.MM.yyyy"));
    quartusGenerator_->replaceTime(currentTime);

    quartusGenerator_->parseFiles(topComponent_, quartusDesignView->getName());
    quartusGenerator_->generateProject(targetPath_, targetEntity_);

    QString expectedProjectOutput( getQuartusHeader(currentTime) + 
        "QUARTUS_VERSION = \"10.0\"\n"
        "\n"
        "# Revisions\n"
        "\n"
        "PROJECT_REVISION = \"generatorOutput\"");

    compareOutputToExpected(PROJECTFILE, expectedProjectOutput);

    QString expectedSettingsOutput ( getQuartusHeader(currentTime) +
        "set_global_assignment -name TOP_LEVEL_ENTITY generatorOutput\n"
        "set_global_assignment -name ORIGINAL_QUARTUS_VERSION \"10.0 SP1\"\n"
        "set_global_assignment -name VHDL_FILE " + QFileInfo(*componentOneVhdlFile).absoluteFilePath() + "\n"
        "set_global_assignment -name QIP_FILE " + QFileInfo(*componentTwoQipFile).absoluteFilePath() + "\n"
        "set_global_assignment -name SDC_FILE " + QFileInfo(*componentThreeSdcFile).absoluteFilePath() + "\n"
        "set_global_assignment -name VERILOG_FILE " + QFileInfo(*topVerilogFile).absoluteFilePath() + "\n"
        );

    compareOutputToExpected(SETTINGSFILE, expectedSettingsOutput);

    QFile::remove(topVerilogFile->fileName());
    QFile::remove(componentOneVhdlFile->fileName());
    QFile::remove(componentTwoQipFile->fileName());
    QFile::remove(componentThreeSdcFile->fileName());
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::testGeneratorWithConfiguredViews()
//-----------------------------------------------------------------------------
void tst_QuartusGenerator::testGeneratorWithConfiguredViews()
{
    QSharedPointer<Component> componentOne = createTestComponent("componentOne");
    QSharedPointer<QFile> componentOneVhdlFile = createEmptyFile("componentOneVHDL.vhdl");

    QStringList componentOneFiles;
    componentOneFiles.append(componentOneVhdlFile->fileName());
    QSharedPointer<FileSet> componentOneFileset = createTestFileset("vhdlFiles", componentOneFiles);
    componentOne->addFileSet(componentOneFileset);

    View* componentOneView (new View("vhdlView"));
    componentOneView->addFileSetRef(componentOneFileset->getName());
    componentOne->addView(componentOneView);

    QSharedPointer<QFile> componentOneVerilogFile = createEmptyFile("changingWithView.v");

    componentOneFiles.clear();
    componentOneFiles.append(componentOneVerilogFile->fileName());
    componentOneFiles.append(componentOneVhdlFile->fileName());
    QSharedPointer<FileSet> componentOneOtherFileset = createTestFileset("otherFiles", componentOneFiles);
    componentOne->addFileSet(componentOneOtherFileset);

    View* componentOneOtherView (new View("restView"));
    componentOneOtherView->addFileSetRef(componentOneOtherFileset->getName());
    componentOne->addView(componentOneOtherView);

    QSharedPointer<Component> componentTwo = createTestComponent("componentTwo");
    QSharedPointer<QFile> componentTwoQipFile = createEmptyFile("componentTwoQip.qip");

    QStringList componentTwoFiles;
    componentTwoFiles.append(componentTwoQipFile->fileName());
    QSharedPointer<FileSet> componentTwoFileset = createTestFileset("qipFiles", componentTwoFiles);
    componentTwo->addFileSet(componentTwoFileset);

    View* componentTwoView (new View("qipView"));
    componentTwoView->addFileSetRef(componentTwoFileset->getName());
    componentTwo->addView(componentTwoView);

    View* componentTwoOtherView (new View("componentTwoEmptyView"));
    componentTwo->addView(componentTwoOtherView);

    QList<QSharedPointer<Component> > componentsInDesign;
    componentsInDesign.append(componentOne);
    componentsInDesign.append(componentTwo);

    QSharedPointer<Design> quartusDesign = createTestDesign("quartusDesign", componentsInDesign);
    QSharedPointer<DesignConfiguration> quartusDesignConf = createTestDesignConfiguration("quartusDesignConf",
        quartusDesign);

    setViewOverridesForDesignConfiguration(quartusDesignConf, quartusDesign->getComponentInstances());

    View* quartusDesignView (new View("quartusView"));
    quartusDesignView->setHierarchyRef(*quartusDesignConf->getVlnv());
    topComponent_->addView(quartusDesignView);

    QString currentTime = QDateTime::currentDateTime().toString(QString("hh:mm:ss dd.MM.yyyy"));
    quartusGenerator_->replaceTime(currentTime);

    quartusGenerator_->parseFiles(topComponent_, quartusDesignView->getName());
    quartusGenerator_->generateProject(targetPath_, targetEntity_);

    QString expectedProjectOutput( getQuartusHeader(currentTime) + 
        "QUARTUS_VERSION = \"10.0\"\n"
        "\n"
        "# Revisions\n"
        "\n"
        "PROJECT_REVISION = \"generatorOutput\"");

    compareOutputToExpected(PROJECTFILE, expectedProjectOutput);

    QString expectedSettingsOutput ( getQuartusHeader(currentTime) +
        "set_global_assignment -name TOP_LEVEL_ENTITY generatorOutput\n"
        "set_global_assignment -name ORIGINAL_QUARTUS_VERSION \"10.0 SP1\"\n"
        "set_global_assignment -name VERILOG_FILE " + QFileInfo(*componentOneVerilogFile).absoluteFilePath() + "\n"
        "set_global_assignment -name VHDL_FILE " + QFileInfo(*componentOneVhdlFile).absoluteFilePath() + "\n"
        );

    compareOutputToExpected(SETTINGSFILE, expectedSettingsOutput);

    QFile::remove(componentOneVhdlFile->fileName());
    QFile::remove(componentTwoQipFile->fileName());
    QFile::remove(componentOneVerilogFile->fileName());
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::testGeneratorInInstancesWithoutActiveViews()
//-----------------------------------------------------------------------------
void tst_QuartusGenerator::testGeneratorInInstancesWithoutActiveViews()
{
    QSharedPointer<Component> componentOne = createTestComponent("componentOne");
    QSharedPointer<QFile> componentOneVhdlFile = createEmptyFile("componentOneVHDL.vhdl");

    QStringList componentOneFiles;
    componentOneFiles.append(componentOneVhdlFile->fileName());
    QSharedPointer<FileSet> componentOneFileset = createTestFileset("vhdlFiles", componentOneFiles);
    componentOne->addFileSet(componentOneFileset);

    QSharedPointer<QFile> componentOneVerilogFile = createEmptyFile("componentOneVerilog.v");

    componentOneFiles.clear();
    componentOneFiles.append(componentOneVerilogFile->fileName());
    componentOneFiles.append(componentOneVhdlFile->fileName());
    QSharedPointer<FileSet> componentOneOtherFileset = createTestFileset("otherFiles", componentOneFiles);
    componentOne->addFileSet(componentOneOtherFileset);

    QSharedPointer<Component> componentTwo = createTestComponent("componentTwo");
    QSharedPointer<QFile> componentTwoQipFile = createEmptyFile("componentTwoQip.qip");

    QStringList componentTwoFiles;
    componentTwoFiles.append(componentTwoQipFile->fileName());
    QSharedPointer<FileSet> componentTwoFileset = createTestFileset("qipFiles", componentTwoFiles);
    componentTwo->addFileSet(componentTwoFileset);

    componentTwoFiles.append(componentOneVerilogFile->fileName());
    componentTwoFiles.append(componentOneVhdlFile->fileName());
    QSharedPointer<FileSet> componentTwoOtherFileset = createTestFileset("allFiles", componentTwoFiles);
    componentTwo->addFileSet(componentTwoOtherFileset);

    View* componentTwoView (new View("qipView"));
    componentTwoView->addFileSetRef(componentTwoFileset->getName());
    componentTwo->addView(componentTwoView);

    View* componentTwoOtherView (new View("componentTwoEmptyView"));
    componentTwo->addView(componentTwoOtherView);

    QList<QSharedPointer<Component> > componentsInDesign;
    componentsInDesign.append(componentOne);
    componentsInDesign.append(componentTwo);

    QStringList fileTypes;
    fileTypes.append("vhdlSource");
    fileTypes.append("verilogSource");
    setFileTypesForFileSet(componentOneFileset, fileTypes);
    setFileTypesForFileSet(componentOneOtherFileset, fileTypes);
    setFileTypesForFileSet(componentTwoFileset, fileTypes);
    setFileTypesForFileSet(componentTwoOtherFileset, fileTypes);

    QSharedPointer<Design> quartusDesign = createTestDesign("quartusDesign", componentsInDesign);
    QSharedPointer<DesignConfiguration> quartusDesignConf = createTestDesignConfiguration("quartusDesignConf",
        quartusDesign);

    QMap<QString, QString> viewConfigurations;
    foreach (ComponentInstance instance, quartusDesign->getComponentInstances())
    {
        viewConfigurations.insert(instance.getInstanceName(), "");
    }
    quartusDesignConf->setViewConfigurations(viewConfigurations);

    View* quartusDesignView (new View("quartusView"));
    quartusDesignView->setHierarchyRef(*quartusDesignConf->getVlnv());
    topComponent_->addView(quartusDesignView);

    QString currentTime = QDateTime::currentDateTime().toString(QString("hh:mm:ss dd.MM.yyyy"));
    quartusGenerator_->replaceTime(currentTime);

    quartusGenerator_->parseFiles(topComponent_, quartusDesignView->getName());
    quartusGenerator_->generateProject(targetPath_, targetEntity_);

    QString expectedProjectOutput( getQuartusHeader(currentTime) + 
        "QUARTUS_VERSION = \"10.0\"\n"
        "\n"
        "# Revisions\n"
        "\n"
        "PROJECT_REVISION = \"generatorOutput\"");

    compareOutputToExpected(PROJECTFILE, expectedProjectOutput);

    QString expectedSettingsOutput ( getQuartusHeader(currentTime) +
        "set_global_assignment -name TOP_LEVEL_ENTITY generatorOutput\n"
        "set_global_assignment -name ORIGINAL_QUARTUS_VERSION \"10.0 SP1\"\n"
        "set_global_assignment -name VHDL_FILE " + QFileInfo(*componentOneVhdlFile).absoluteFilePath() + "\n"
        "set_global_assignment -name VERILOG_FILE " + QFileInfo(*componentOneVerilogFile).absoluteFilePath() + "\n"
        "set_global_assignment -name QIP_FILE " + QFileInfo(*componentTwoQipFile).absoluteFilePath() + "\n"
        );

    compareOutputToExpected(SETTINGSFILE, expectedSettingsOutput);

    QFile::remove(componentOneVhdlFile->fileName());
    QFile::remove(componentTwoQipFile->fileName());
    QFile::remove(componentOneVerilogFile->fileName());
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::readOutputFile()
//-----------------------------------------------------------------------------
void tst_QuartusGenerator::readOutputFile(QString const& fileType)
{
    output_.clear();

    QFile outputFile (targetPath_ + targetEntity_ + fileType);

    QVERIFY(outputFile.open(QIODevice::ReadOnly));

    output_ = outputFile.readAll();
    outputFile.close();
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::compareOutputToExpected()
//-----------------------------------------------------------------------------
void tst_QuartusGenerator::compareOutputToExpected(QString const& fileType, QString const& expectedOutput)
{
    readOutputFile(fileType);

    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            readOutputFile(fileType);
            QCOMPARE(output_, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::getQuartusHeader()
//-----------------------------------------------------------------------------
QString tst_QuartusGenerator::getQuartusHeader(QString const& currentTime)
{
    QString header(
        "# ----------------------------------------------------------- #\n"
        "\n"
        "# Quartus project generated by Kactus2\n"
        "# Date created " + currentTime + "\n"
        "\n"
        "# ----------------------------------------------------------- #\n"
        "\n");

    return header;
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::createTestComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_QuartusGenerator::createTestComponent(QString const& componentName)
{
    VLNV newVlnv(VLNV::COMPONENT, "TUT", "TestLibrary", componentName, "1.0");
    QSharedPointer<Component> newComponent = QSharedPointer<Component>(new Component(newVlnv));

    library_.writeModelToFile(QFileInfo(".").absoluteFilePath() + "/", newComponent);
    library_.addComponent(newComponent);

    return newComponent;
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::createTestDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> tst_QuartusGenerator::createTestDesign(QString const& designName,
    QList<QSharedPointer<Component> > containedComponents)
{
    VLNV designVLNV(VLNV::DESIGN, "TUT", "TestLibrary", designName, "1.0");
    QSharedPointer<Design> newDesign = QSharedPointer<Design>(new Design(designVLNV));

    library_.writeModelToFile(QFileInfo(".").absoluteFilePath() + "/", newDesign);
    library_.addComponent(newDesign);

    QList<ComponentInstance> componentInstances;
    QMap<QString, int> usedInstances;
    foreach (QSharedPointer<Component> currentComponent, containedComponents)
    {
        int instanceNumber = usedInstances.value(currentComponent->getVlnv()->getName(), 0);
        QString instanceName = currentComponent->getVlnv()->getName() + "_" + QString::number(instanceNumber);
        
        ComponentInstance newInstance (instanceName, "", "", *currentComponent->getVlnv(), QPointF(),
            instanceName + "ID");

        componentInstances.append(newInstance);
        
        usedInstances.insert(currentComponent->getVlnv()->getName(), instanceNumber + 1);
    }
    newDesign->setComponentInstances(componentInstances);

    return newDesign;
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::createTestDesignConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfiguration> tst_QuartusGenerator::createTestDesignConfiguration(QString const& name,
    QSharedPointer<Design> referencedDesign)
{
    VLNV designConfigurationVLNV(VLNV::DESIGNCONFIGURATION, "TUT", "TestLibrary", name, "1.0");
    QSharedPointer<DesignConfiguration> newDesignConfiguration =
        QSharedPointer<DesignConfiguration>(new DesignConfiguration(designConfigurationVLNV));

    library_.writeModelToFile(QFileInfo(".").absoluteFilePath() + "/", newDesignConfiguration);
    library_.addComponent(newDesignConfiguration);

    newDesignConfiguration->setDesignRef(*referencedDesign->getVlnv());

    foreach (ComponentInstance currentInstance, referencedDesign->getComponentInstances())
    {
        VLNV componentReference = currentInstance.getComponentRef();
        QSharedPointer<Component> referencedComponent =
            library_.getModel(componentReference).dynamicCast<Component>();

        if (referencedComponent && referencedComponent->viewCount() > 0)
        {
            newDesignConfiguration->addViewConfiguration(currentInstance.getInstanceName(),
                referencedComponent->getViews().at(0)->getName());
        }
    }

    return newDesignConfiguration;
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::createTestFileset()
//-----------------------------------------------------------------------------
QSharedPointer<FileSet> tst_QuartusGenerator::createTestFileset(QString const& filesetName, QStringList filesToAdd)
{
    QSharedPointer<FileSet> newFileSet (new FileSet(filesetName));

    foreach (QString fileName, filesToAdd)
    {
        QSharedPointer<File> newFileForFileSet (new File(fileName, newFileSet.data()));

        newFileSet->addFile(newFileForFileSet);
    }

    return newFileSet;
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::createEmptyFile()
//-----------------------------------------------------------------------------
QSharedPointer<QFile> tst_QuartusGenerator::createEmptyFile(QString const& fileName)
{
    QSharedPointer<QFile> newFile(new QFile(fileName));
    newFile->open(QFile::WriteOnly);
    newFile->close();

    return newFile;
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::setViewOverridesForDesignConfiguration()
//-----------------------------------------------------------------------------
void tst_QuartusGenerator::setViewOverridesForDesignConfiguration(
    QSharedPointer<DesignConfiguration> testDesignConfiguration, QList<ComponentInstance> componentInstances)
{
    QMap<QString, QString> viewOverrides;
    foreach (ComponentInstance instance, componentInstances)
    {
        VLNV componentReferenceVLNV = instance.getComponentRef();
        QSharedPointer<Component> referencedComponent =
            library_.getModel(componentReferenceVLNV).dynamicCast<Component>();

        if (referencedComponent && referencedComponent->viewCount() > 1)
        {
            viewOverrides.insert(instance.getUuid(), referencedComponent->getViews().at(1)->getName());
        }
    }

    testDesignConfiguration->setKactus2ViewOverrides(viewOverrides);
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusGenerator::setFileTypesForFileSet()
//-----------------------------------------------------------------------------
void tst_QuartusGenerator::setFileTypesForFileSet(QSharedPointer<FileSet> selectedFileset, QStringList fileTypes)
{
    foreach (QSharedPointer<File> currentFile, selectedFileset->getFiles())
    {
        currentFile->setFileTypes(fileTypes);
    }
}

//QTEST_APPLESS_MAIN(tst_QuartusGenerator)
QTEST_MAIN(tst_QuartusGenerator)

#include "tst_QuartusGenerator.moc"
