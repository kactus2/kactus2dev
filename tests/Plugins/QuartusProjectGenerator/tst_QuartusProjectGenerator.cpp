//-----------------------------------------------------------------------------
// File: tst_QuartusProjectGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.05.2015
//
// Description:
// Unit test for class QuartusGenerator.
//-----------------------------------------------------------------------------

#include <Plugins/QuartusProjectGenerator/QuartusGenerator.h>

#include <tests/MockObjects/LibraryMock.h>

#include <KactusAPI/include/IPluginUtility.h>

#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/ComponentInstance.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/Model.h>

#include <QtTest>
#include <QDateTime>

class tst_QuartusProjectGenerator : public QObject, public IPluginUtility
{
    Q_OBJECT

public:
    tst_QuartusProjectGenerator();

    virtual void printError(QString const& message);

    virtual void printInfo(QString const& message);

    virtual LibraryInterface* getLibraryInterface();

    virtual QWidget* getParentWidget();

    virtual QString getKactusVersion() const;

private slots:

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
        QSharedPointer<QList<QSharedPointer<ComponentInstance> > > componentInstances);

    void setFileTypesForFileSet(QSharedPointer<FileSet> selectedFileset, QSharedPointer<QStringList> fileTypes);
 
    QString projectFile() const;

    QString settingsFile() const;

    QSharedPointer<Component> topComponent_;

    LibraryMock library_;

    QWidget* generatorParentWidget_;

    QuartusGenerator* quartusGenerator_;

    QString targetPath_;

    QString targetEntity_;

    QString output_;

    QString generatorInformation_;
};

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::tst_QuartusProjectGenerator()
//-----------------------------------------------------------------------------
tst_QuartusProjectGenerator::tst_QuartusProjectGenerator():
    topComponent_(), generatorParentWidget_(new QWidget), library_(this), quartusGenerator_(), targetPath_("./"),
    targetEntity_("generatorOutput"), output_(), generatorInformation_("test generator")
{

}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::printError()
//-----------------------------------------------------------------------------
void tst_QuartusProjectGenerator::printError(QString const& message)
{
        // TODO.
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::printInfo()
//-----------------------------------------------------------------------------
void tst_QuartusProjectGenerator::printInfo(QString const& message)
{
    // TODO.
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::getLibraryInterface()
//-----------------------------------------------------------------------------
LibraryInterface* tst_QuartusProjectGenerator::getLibraryInterface()
{
    return &library_;
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::getParentWidget()
//-----------------------------------------------------------------------------
QWidget* tst_QuartusProjectGenerator::getParentWidget()
{
    return generatorParentWidget_;
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::getKactusVersion()
//-----------------------------------------------------------------------------
QString tst_QuartusProjectGenerator::getKactusVersion() const
{
    return QString();
}


//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::init()
//-----------------------------------------------------------------------------
void tst_QuartusProjectGenerator::init()
{
    library_.clear();

    quartusGenerator_ = new QuartusGenerator(this);

    topComponent_ = createTestComponent("topComponent");

    if (QFile::exists(targetEntity_ + projectFile()))
    {
        QWARN("Quartus project file already exists.");
    }

    if (QFile::exists(targetEntity_ + settingsFile()))
    {
        QWARN("Quartus settings file already exists.");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::cleanup()
//-----------------------------------------------------------------------------
void tst_QuartusProjectGenerator::cleanup()
{
    topComponent_.clear();
    output_.clear();

    delete quartusGenerator_;
    quartusGenerator_ = 0;

    QFile::remove(targetEntity_ + projectFile());
    QFile::remove(targetEntity_ + settingsFile());
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::testGeneratorWithoutFilesets()
//-----------------------------------------------------------------------------
void tst_QuartusProjectGenerator::testGeneratorWithoutFilesets()
{
    QString currentTime = QDateTime::currentDateTime().toString(QString("hh:mm:ss dd.MM.yyyy"));
    quartusGenerator_->replaceTime(currentTime);

    quartusGenerator_->generateProject(targetPath_, targetEntity_, generatorInformation_);

    QString expectedProjectOutput( getQuartusHeader(currentTime) + 
        "QUARTUS_VERSION = \"10.0\"\n"
        "\n"
        "# Revisions\n"
        "\n"
        "PROJECT_REVISION = \"generatorOutput\"");

    compareOutputToExpected(projectFile(), expectedProjectOutput);

    QString expectedSettingsOutput ( getQuartusHeader(currentTime) +
        "set_global_assignment -name TOP_LEVEL_ENTITY generatorOutput\n"
        "set_global_assignment -name ORIGINAL_QUARTUS_VERSION \"10.0 SP1\"");

    compareOutputToExpected(settingsFile(), expectedSettingsOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::testGeneratorWithDesignContainingTopComponentVerilogFile()
//-----------------------------------------------------------------------------
void tst_QuartusProjectGenerator::testGeneratorWithDesignContainingTopComponentVerilogFile()
{
    QSharedPointer<Design> quartusDesign = createTestDesign("quartusDesign", QList<QSharedPointer<Component> > ());

    QSharedPointer<QFile> topVerilogFile = createEmptyFile("topComponentVerilog.v");

    QStringList filesForTopComponent;
    filesForTopComponent.append(topVerilogFile->fileName());

    QSharedPointer<FileSet> topComponentFileSet = createTestFileset("verilogFileSet", filesForTopComponent);
    topComponent_->getFileSets()->append(topComponentFileSet);

	QSharedPointer<ComponentInstantiation> cimpTop( new ComponentInstantiation("TOP-INSTANTIATION") );
	cimpTop->getFileSetReferences()->append(QSharedPointer<FileSetRef>(new FileSetRef(topComponentFileSet->name())));
	topComponent_->getComponentInstantiations()->append(cimpTop);

	QSharedPointer<DesignInstantiation> dist( new DesignInstantiation("joku" ));
	topComponent_->getDesignInstantiations()->append(dist);
	QSharedPointer<ConfigurableVLNVReference> desgref( new ConfigurableVLNVReference(quartusDesign->getVlnv()) );
	dist->setDesignReference( desgref );

	QSharedPointer<View> quartusTestView (new View("quartusView"));
	quartusTestView->setDesignInstantiationRef(dist->name());
	quartusTestView->setComponentInstantiationRef(cimpTop->name());
	topComponent_->getViews()->append(quartusTestView);

    QString currentTime = QDateTime::currentDateTime().toString(QString("hh:mm:ss dd.MM.yyyy"));
    quartusGenerator_->replaceTime(currentTime);

    quartusGenerator_->parseFiles(topComponent_, quartusTestView);
    quartusGenerator_->generateProject(targetPath_, targetEntity_, generatorInformation_);

    QString expectedProjectOutput( getQuartusHeader(currentTime) + 
        "QUARTUS_VERSION = \"10.0\"\n"
        "\n"
        "# Revisions\n"
        "\n"
        "PROJECT_REVISION = \"generatorOutput\"");

    compareOutputToExpected(projectFile(), expectedProjectOutput);

    QString expectedSettingsOutput ( getQuartusHeader(currentTime) +
        "set_global_assignment -name TOP_LEVEL_ENTITY generatorOutput\n"
        "set_global_assignment -name ORIGINAL_QUARTUS_VERSION \"10.0 SP1\"\n"
        "set_global_assignment -name VERILOG_FILE " + QFileInfo(*topVerilogFile).absoluteFilePath() + "\n"
        );

    compareOutputToExpected(settingsFile(), expectedSettingsOutput);

    QFile::remove(topVerilogFile->fileName());
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::testGeneratorWithDesignContainingInstances()
//-----------------------------------------------------------------------------
void tst_QuartusProjectGenerator::testGeneratorWithDesignContainingInstances()
{
    QSharedPointer<Component> componentOne = createTestComponent("componentOne");
    QSharedPointer<QFile> componentOneVhdlFile = createEmptyFile("componentOneVHDL.vhdl");

    QStringList componentOneFiles;
    componentOneFiles.append(componentOneVhdlFile->fileName());
    QSharedPointer<FileSet> componentOneFileset = createTestFileset("vhdlFiles", componentOneFiles);
    componentOne->getFileSets()->append(componentOneFileset);

	QSharedPointer<ComponentInstantiation> cimpOne( new ComponentInstantiation("ONE-INSTANTIATION") );
	cimpOne->getFileSetReferences()->append(QSharedPointer<FileSetRef>(new FileSetRef(componentOneFileset->name())));
	componentOne->getComponentInstantiations()->append(cimpOne);

	QSharedPointer<View> componentOneView (new View("vhdlView"));
	componentOneView->setComponentInstantiationRef(cimpOne->name());
	componentOne->getViews()->append(componentOneView);

    QSharedPointer<Component> componentTwo = createTestComponent("componentTwo");
    QSharedPointer<QFile> componentTwoQipFile = createEmptyFile("componentTwoQip.qip");

    QStringList componentTwoFiles;
    componentTwoFiles.append(componentTwoQipFile->fileName());
    QSharedPointer<FileSet> componentTwoFileset = createTestFileset("qipFiles", componentTwoFiles);
    componentTwo->getFileSets()->append(componentTwoFileset);

	QSharedPointer<ComponentInstantiation> cimpTwo( new ComponentInstantiation("TWO-INSTANTIATION") );
	cimpTwo->getFileSetReferences()->append(QSharedPointer<FileSetRef>(new FileSetRef(componentTwoFileset->name())));
	componentTwo->getComponentInstantiations()->append(cimpTwo);

	QSharedPointer<View> componentTwoView (new View("qipView"));
    componentTwoView->setComponentInstantiationRef(cimpTwo->name());
    componentTwo->getViews()->append(componentTwoView);

    QSharedPointer<Component> componentThree = createTestComponent("componentThree");
    QSharedPointer<QFile> componentThreeSdcFile = createEmptyFile("componentThreeSdc.sdc");

    QStringList componentThreeFiles;
    componentThreeFiles.append(componentThreeSdcFile->fileName());
    QSharedPointer<FileSet> componentThreeFileSet = createTestFileset("sdcFiles", componentThreeFiles);
    componentThree->getFileSets()->append(componentThreeFileSet);

	QSharedPointer<View> componentThreeView (new View("sdcView"));
	QSharedPointer<ComponentInstantiation> cimpThree( new ComponentInstantiation("THREE-INSTANTIATION") );
	cimpThree->getFileSetReferences()->append(QSharedPointer<FileSetRef>(new FileSetRef(componentThreeFileSet->name())));
	componentThreeView->setComponentInstantiationRef(cimpThree->name());
	componentThree->getViews()->append(componentThreeView);
	componentThree->getComponentInstantiations()->append(cimpThree);

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
    topComponent_->getFileSets()->append(topComponentFileSet);

	QSharedPointer<DesignInstantiation> des( new DesignInstantiation("design" ) );
	QSharedPointer<ConfigurableVLNVReference> desRef( new ConfigurableVLNVReference( quartusDesign->getVlnv() ) );
	des->setDesignReference( desRef );
	topComponent_->getDesignInstantiations()->append(des);

	QSharedPointer<DesignConfigurationInstantiation> disg( new DesignConfigurationInstantiation("desgonf" ) );
	QSharedPointer<ConfigurableVLNVReference> desgConfRef( new ConfigurableVLNVReference( quartusDesignConf->getVlnv() ) );
	disg->setDesignConfigurationReference( desgConfRef );
	topComponent_->getDesignConfigurationInstantiations()->append(disg);

	QSharedPointer<ComponentInstantiation> cimpTop( new ComponentInstantiation("TOP-INSTANTIATION") );
	cimpTop->getFileSetReferences()->append(QSharedPointer<FileSetRef>(new FileSetRef(topComponentFileSet->name())));
	topComponent_->getComponentInstantiations()->append(cimpTop);

	QSharedPointer<View> quartusDesignView (new View("quartusView"));
	quartusDesignView->setDesignInstantiationRef(des->name());
	quartusDesignView->setDesignConfigurationInstantiationRef(disg->name());
	quartusDesignView->setComponentInstantiationRef(cimpTop->name());
	topComponent_->getViews()->append(quartusDesignView);

    QString currentTime = QDateTime::currentDateTime().toString(QString("hh:mm:ss dd.MM.yyyy"));
    quartusGenerator_->replaceTime(currentTime);

    quartusGenerator_->parseFiles(topComponent_, quartusDesignView);
    quartusGenerator_->generateProject(targetPath_, targetEntity_, generatorInformation_);

    QString expectedProjectOutput( getQuartusHeader(currentTime) + 
        "QUARTUS_VERSION = \"10.0\"\n"
        "\n"
        "# Revisions\n"
        "\n"
        "PROJECT_REVISION = \"generatorOutput\"");

    compareOutputToExpected(projectFile(), expectedProjectOutput);

    QString expectedSettingsOutput ( getQuartusHeader(currentTime) +
        "set_global_assignment -name TOP_LEVEL_ENTITY generatorOutput\n"
        "set_global_assignment -name ORIGINAL_QUARTUS_VERSION \"10.0 SP1\"\n"
        "set_global_assignment -name VHDL_FILE " + QFileInfo(*componentOneVhdlFile).absoluteFilePath() + "\n"
        "set_global_assignment -name QIP_FILE " + QFileInfo(*componentTwoQipFile).absoluteFilePath() + "\n"
        "set_global_assignment -name SDC_FILE " + QFileInfo(*componentThreeSdcFile).absoluteFilePath() + "\n"
        "set_global_assignment -name VERILOG_FILE " + QFileInfo(*topVerilogFile).absoluteFilePath() + "\n"
        );

    compareOutputToExpected(settingsFile(), expectedSettingsOutput);

    QFile::remove(topVerilogFile->fileName());
    QFile::remove(componentOneVhdlFile->fileName());
    QFile::remove(componentTwoQipFile->fileName());
    QFile::remove(componentThreeSdcFile->fileName());
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::testGeneratorWithConfiguredViews()
//-----------------------------------------------------------------------------
void tst_QuartusProjectGenerator::testGeneratorWithConfiguredViews()
{
    QSharedPointer<Component> componentOne = createTestComponent("componentOne");
    QSharedPointer<QFile> componentOneVhdlFile = createEmptyFile("componentOneVHDL.vhdl");

    QStringList componentOneFiles;
    componentOneFiles.append(componentOneVhdlFile->fileName());
    QSharedPointer<FileSet> componentOneFileset = createTestFileset("vhdlFiles", componentOneFiles);
    componentOne->getFileSets()->append(componentOneFileset);

    QSharedPointer<View> componentOneView (new View("vhdlView"));
	QSharedPointer<ComponentInstantiation> cimpOne( new ComponentInstantiation("ONE-INSTANTIATION") );
	cimpOne->getFileSetReferences()->append(QSharedPointer<FileSetRef>(new FileSetRef(componentOneFileset->name())));
    componentOneView->setComponentInstantiationRef(cimpOne->name());
	componentOne->getViews()->append(componentOneView);
	componentOne->getComponentInstantiations()->append(cimpOne);

    QSharedPointer<QFile> componentOneVerilogFile = createEmptyFile("changingWithView.v");

    componentOneFiles.clear();
    componentOneFiles.append(componentOneVerilogFile->fileName());
    componentOneFiles.append(componentOneVhdlFile->fileName());
    QSharedPointer<FileSet> componentOneOtherFileset = createTestFileset("otherFiles", componentOneFiles);
    componentOne->getFileSets()->append(componentOneOtherFileset);

	QSharedPointer<View> componentOneOtherView (new View("restView"));
	QSharedPointer<ComponentInstantiation> cimpOneOther( new ComponentInstantiation("ONE-OTHER-INSTANTIATION") );
	cimpOneOther->getFileSetReferences()->append(QSharedPointer<FileSetRef>(new FileSetRef(componentOneOtherFileset->name())));
	componentOneOtherView->setComponentInstantiationRef(cimpOneOther->name());
	componentOne->getViews()->append(componentOneOtherView);
	componentOne->getComponentInstantiations()->append(cimpOneOther);

    QSharedPointer<Component> componentTwo = createTestComponent("componentTwo");
    QSharedPointer<QFile> componentTwoQipFile = createEmptyFile("componentTwoQip.qip");

    QStringList componentTwoFiles;
    componentTwoFiles.append(componentTwoQipFile->fileName());
    QSharedPointer<FileSet> componentTwoFileset = createTestFileset("qipFiles", componentTwoFiles);
    componentTwo->getFileSets()->append(componentTwoFileset);

	QSharedPointer<View> componentTwoView (new View("qipView"));
	QSharedPointer<ComponentInstantiation> cimpTwo( new ComponentInstantiation("TWO-INSTANTIATION") );
	cimpTwo->getFileSetReferences()->append(QSharedPointer<FileSetRef>(new FileSetRef(componentTwoFileset->name())));
	componentTwoView->setComponentInstantiationRef(cimpTwo->name());
	componentTwo->getViews()->append(componentTwoView);
	componentTwo->getComponentInstantiations()->append(cimpTwo);

    QSharedPointer<View> componentTwoOtherView (new View("componentTwoEmptyView"));
    componentTwo->getViews()->append(componentTwoView);

    QList<QSharedPointer<Component> > componentsInDesign;
    componentsInDesign.append(componentOne);
    componentsInDesign.append(componentTwo);

    QSharedPointer<Design> quartusDesign = createTestDesign("quartusDesign", componentsInDesign);
    QSharedPointer<DesignConfiguration> quartusDesignConf = createTestDesignConfiguration("quartusDesignConf",
        quartusDesign);

    setViewOverridesForDesignConfiguration(quartusDesignConf, quartusDesign->getComponentInstances());

	QSharedPointer<DesignInstantiation> des( new DesignInstantiation("design" ) );
	QSharedPointer<ConfigurableVLNVReference> desRef( new ConfigurableVLNVReference( quartusDesign->getVlnv() ) );
	des->setDesignReference( desRef );
	topComponent_->getDesignInstantiations()->append(des);

	QSharedPointer<DesignConfigurationInstantiation> disg( new DesignConfigurationInstantiation("desgonf" ) );
	QSharedPointer<ConfigurableVLNVReference> desgConfRef( new ConfigurableVLNVReference( quartusDesignConf->getVlnv() ) );
	disg->setDesignConfigurationReference( desgConfRef );
	topComponent_->getDesignConfigurationInstantiations()->append(disg);

	QSharedPointer<View> quartusDesignView (new View("quartusView"));
	quartusDesignView->setDesignInstantiationRef(des->name());
	quartusDesignView->setDesignConfigurationInstantiationRef(disg->name());
    topComponent_->getViews()->append(quartusDesignView);

    QString currentTime = QDateTime::currentDateTime().toString(QString("hh:mm:ss dd.MM.yyyy"));
    quartusGenerator_->replaceTime(currentTime);

    quartusGenerator_->parseFiles(topComponent_, quartusDesignView);
    quartusGenerator_->generateProject(targetPath_, targetEntity_, generatorInformation_);

    QString expectedProjectOutput( getQuartusHeader(currentTime) + 
        "QUARTUS_VERSION = \"10.0\"\n"
        "\n"
        "# Revisions\n"
        "\n"
        "PROJECT_REVISION = \"generatorOutput\"");

    compareOutputToExpected(projectFile(), expectedProjectOutput);

    QString expectedSettingsOutput ( getQuartusHeader(currentTime) +
        "set_global_assignment -name TOP_LEVEL_ENTITY generatorOutput\n"
        "set_global_assignment -name ORIGINAL_QUARTUS_VERSION \"10.0 SP1\"\n"
        "set_global_assignment -name VERILOG_FILE " + QFileInfo(*componentOneVerilogFile).absoluteFilePath() + "\n"
        "set_global_assignment -name VHDL_FILE " + QFileInfo(*componentOneVhdlFile).absoluteFilePath() + "\n"
        );

    compareOutputToExpected(settingsFile(), expectedSettingsOutput);

    QFile::remove(componentOneVhdlFile->fileName());
    QFile::remove(componentTwoQipFile->fileName());
    QFile::remove(componentOneVerilogFile->fileName());
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::testGeneratorInInstancesWithoutActiveViews()
//-----------------------------------------------------------------------------
void tst_QuartusProjectGenerator::testGeneratorInInstancesWithoutActiveViews()
{
    QSharedPointer<Component> componentOne = createTestComponent("componentOne");
    QSharedPointer<QFile> componentOneVhdlFile = createEmptyFile("componentOneVHDL.vhdl");

    QStringList componentOneFiles;
    componentOneFiles.append(componentOneVhdlFile->fileName());
    QSharedPointer<FileSet> componentOneFileset = createTestFileset("vhdlFiles", componentOneFiles);
    componentOne->getFileSets()->append(componentOneFileset);

    QSharedPointer<QFile> componentOneVerilogFile = createEmptyFile("componentOneVerilog.v");

    componentOneFiles.clear();
    componentOneFiles.append(componentOneVerilogFile->fileName());
    componentOneFiles.append(componentOneVhdlFile->fileName());
    QSharedPointer<FileSet> componentOneOtherFileset = createTestFileset("otherFiles", componentOneFiles);
    componentOne->getFileSets()->append(componentOneOtherFileset);

    QSharedPointer<Component> componentTwo = createTestComponent("componentTwo");
    QSharedPointer<QFile> componentTwoQipFile = createEmptyFile("componentTwoQip.qip");

    QStringList componentTwoFiles;
    componentTwoFiles.append(componentTwoQipFile->fileName());
    QSharedPointer<FileSet> componentTwoFileset = createTestFileset("qipFiles", componentTwoFiles);
    componentTwo->getFileSets()->append(componentTwoFileset);

    componentTwoFiles.append(componentOneVerilogFile->fileName());
    componentTwoFiles.append(componentOneVhdlFile->fileName());
    QSharedPointer<FileSet> componentTwoOtherFileset = createTestFileset("allFiles", componentTwoFiles);
    componentTwo->getFileSets()->append(componentTwoOtherFileset);

	QSharedPointer<ComponentInstantiation> cimpTwo( new ComponentInstantiation("Two-INSTANTIATION") );
	cimpTwo->getFileSetReferences()->append(QSharedPointer<FileSetRef>(new FileSetRef(componentTwoFileset->name())));
	componentTwo->getComponentInstantiations()->append(cimpTwo);

	QSharedPointer<View> componentTwoView (new View("qipView"));
	componentTwoView->setComponentInstantiationRef(cimpTwo->name());
	componentTwo->getViews()->append(componentTwoView);

    QSharedPointer<View> componentTwoOtherView (new View("componentTwoEmptyView"));
    componentTwo->getViews()->append(componentTwoOtherView);

    QList<QSharedPointer<Component> > componentsInDesign;
    componentsInDesign.append(componentOne);
    componentsInDesign.append(componentTwo);

    QSharedPointer<QStringList> fileTypes( new QStringList );
    fileTypes->append("vhdlSource");
    fileTypes->append("verilogSource");
    setFileTypesForFileSet(componentOneFileset, fileTypes);
    setFileTypesForFileSet(componentOneOtherFileset, fileTypes);
    setFileTypesForFileSet(componentTwoFileset, fileTypes);
    setFileTypesForFileSet(componentTwoOtherFileset, fileTypes);

    QSharedPointer<Design> quartusDesign = createTestDesign("quartusDesign", componentsInDesign);
    QSharedPointer<DesignConfiguration> quartusDesignConf = createTestDesignConfiguration("quartusDesignConf",
        quartusDesign);

	QSharedPointer<QList<QSharedPointer<ViewConfiguration> > > viewConfigurations( new QList<QSharedPointer<ViewConfiguration> > );
    foreach (QSharedPointer<ComponentInstance> instance, *quartusDesign->getComponentInstances())
    {
        viewConfigurations->append(QSharedPointer<ViewConfiguration>( new ViewConfiguration( instance->getInstanceName() ) ) );
    }
    quartusDesignConf->setViewConfigurations(viewConfigurations);

	QSharedPointer<DesignConfigurationInstantiation> disg( new DesignConfigurationInstantiation("DESG-CONF-INSTANTIATION") );
	topComponent_->getDesignConfigurationInstantiations()->append(disg);
	QSharedPointer<ConfigurableVLNVReference> designConfRef( new ConfigurableVLNVReference( quartusDesignConf->getVlnv() ) );
	disg->setDesignConfigurationReference( designConfRef );

	QSharedPointer<DesignInstantiation> dis( new DesignInstantiation("DESIGN-INSTANTIATION") );
	topComponent_->getDesignInstantiations()->append(dis);
	QSharedPointer<ConfigurableVLNVReference> designRef( new ConfigurableVLNVReference( quartusDesign->getVlnv() ) );
	dis->setDesignReference( designRef );

	QSharedPointer<View> quartusDesignView (new View("quartusView"));
	quartusDesignView->setDesignConfigurationInstantiationRef(disg->name());
	quartusDesignView->setComponentInstantiationRef(dis->name());
	topComponent_->getViews()->append(quartusDesignView);

    QString currentTime = QDateTime::currentDateTime().toString(QString("hh:mm:ss dd.MM.yyyy"));
    quartusGenerator_->replaceTime(currentTime);

    quartusGenerator_->parseFiles(topComponent_, quartusDesignView);
    quartusGenerator_->generateProject(targetPath_, targetEntity_, generatorInformation_);

    QString expectedProjectOutput( getQuartusHeader(currentTime) + 
        "QUARTUS_VERSION = \"10.0\"\n"
        "\n"
        "# Revisions\n"
        "\n"
        "PROJECT_REVISION = \"generatorOutput\"");

    compareOutputToExpected(projectFile(), expectedProjectOutput);

    QString expectedSettingsOutput ( getQuartusHeader(currentTime) +
        "set_global_assignment -name TOP_LEVEL_ENTITY generatorOutput\n"
        "set_global_assignment -name ORIGINAL_QUARTUS_VERSION \"10.0 SP1\"\n"
        "set_global_assignment -name VHDL_FILE " + QFileInfo(*componentOneVhdlFile).absoluteFilePath() + "\n"
        "set_global_assignment -name VERILOG_FILE " + QFileInfo(*componentOneVerilogFile).absoluteFilePath() + "\n"
        "set_global_assignment -name QIP_FILE " + QFileInfo(*componentTwoQipFile).absoluteFilePath() + "\n"
        );

    compareOutputToExpected(settingsFile(), expectedSettingsOutput);

    QFile::remove(componentOneVhdlFile->fileName());
    QFile::remove(componentTwoQipFile->fileName());
    QFile::remove(componentOneVerilogFile->fileName());
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::readOutputFile()
//-----------------------------------------------------------------------------
void tst_QuartusProjectGenerator::readOutputFile(QString const& fileType)
{
    output_.clear();

    QFile outputFile (targetPath_ + targetEntity_ + fileType);

    QVERIFY(outputFile.open(QIODevice::ReadOnly));

    output_ = outputFile.readAll();
    outputFile.close();
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::compareOutputToExpected()
//-----------------------------------------------------------------------------
void tst_QuartusProjectGenerator::compareOutputToExpected(QString const& fileType, QString const& expectedOutput)
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
// Function: tst_QuartusProjectGenerator::getQuartusHeader()
//-----------------------------------------------------------------------------
QString tst_QuartusProjectGenerator::getQuartusHeader(QString const& currentTime)
{
    QString header(
        "# ----------------------------------------------------------- #\n"
        "\n"
        "# Quartus project generated by Kactus2\n"
        "# Date created " + currentTime + "\n"
        "# This file was generated with " + generatorInformation_ + "\n"
        "\n"
        "# ----------------------------------------------------------- #\n"
        "\n");

    return header;
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::createTestComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_QuartusProjectGenerator::createTestComponent(QString const& componentName)
{
    VLNV newVlnv(VLNV::COMPONENT, "TUT", "TestLibrary", componentName, "1.0");
    QSharedPointer<Component> newComponent = QSharedPointer<Component>(new Component(newVlnv, Document::Revision::Std14));

    library_.writeModelToFile(QFileInfo(".").absoluteFilePath() + "/", newComponent);
    library_.addComponent(newComponent);

    return newComponent;
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::createTestDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> tst_QuartusProjectGenerator::createTestDesign(QString const& designName,
    QList<QSharedPointer<Component> > containedComponents)
{
    VLNV designVLNV(VLNV::DESIGN, "TUT", "TestLibrary", designName, "1.0");
    QSharedPointer<Design> newDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    library_.writeModelToFile(QFileInfo(".").absoluteFilePath() + "/", newDesign);
    library_.addComponent(newDesign);

    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > componentInstances(
        new QList<QSharedPointer<ComponentInstance> >());
    QMap<QString, int> usedInstances;
    for (auto const& currentComponent : containedComponents)
    {
        int instanceNumber = usedInstances.value(currentComponent->getVlnv().getName(), 0);
        QString instanceName = currentComponent->getVlnv().getName() + "_" + QString::number(instanceNumber);
        
		QSharedPointer<ConfigurableVLNVReference> vlnvRef(new ConfigurableVLNVReference(currentComponent->getVlnv()));
        QSharedPointer<ComponentInstance> newInstance (new ComponentInstance(instanceName, vlnvRef));
        newInstance->setUuid(instanceName + "ID");

        componentInstances->append(newInstance);
        
        usedInstances.insert(currentComponent->getVlnv().getName(), instanceNumber + 1);
    }
    newDesign->setComponentInstances(componentInstances);

    return newDesign;
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::createTestDesignConfiguration()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfiguration> tst_QuartusProjectGenerator::createTestDesignConfiguration(QString const& name,
    QSharedPointer<Design> referencedDesign)
{
    VLNV designConfigurationVLNV(VLNV::DESIGNCONFIGURATION, "TUT", "TestLibrary", name, "1.0");
    QSharedPointer<DesignConfiguration> newDesignConfiguration =
        QSharedPointer<DesignConfiguration>(new DesignConfiguration(designConfigurationVLNV, Document::Revision::Std14));

    library_.writeModelToFile(QFileInfo(".").absoluteFilePath() + "/", newDesignConfiguration);
    library_.addComponent(newDesignConfiguration);

    newDesignConfiguration->setDesignRef(referencedDesign->getVlnv());

    for (auto const& currentInstance : *referencedDesign->getComponentInstances())
    {
        VLNV componentReference = *currentInstance->getComponentRef();
        QSharedPointer<Component> referencedComponent =
            library_.getModel(componentReference).dynamicCast<Component>();

        if (referencedComponent && referencedComponent->getModel()->getViews()->size() > 0)
        {
            newDesignConfiguration->addViewConfiguration(currentInstance->getInstanceName(),
                referencedComponent->getViews()->at(0)->name());
        }
    }

    return newDesignConfiguration;
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::createTestFileset()
//-----------------------------------------------------------------------------
QSharedPointer<FileSet> tst_QuartusProjectGenerator::createTestFileset(QString const& filesetName, QStringList filesToAdd)
{
    QSharedPointer<FileSet> newFileSet (new FileSet(filesetName));

    for (auto const& fileName : filesToAdd)
    {
        QSharedPointer<File> newFileForFileSet (new File(fileName));

        newFileSet->addFile(newFileForFileSet);
    }

    return newFileSet;
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::createEmptyFile()
//-----------------------------------------------------------------------------
QSharedPointer<QFile> tst_QuartusProjectGenerator::createEmptyFile(QString const& fileName)
{
    QSharedPointer<QFile> newFile(new QFile(fileName));
    newFile->open(QFile::WriteOnly);
    newFile->close();

    return newFile;
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::setViewOverridesForDesignConfiguration()
//-----------------------------------------------------------------------------
void tst_QuartusProjectGenerator::setViewOverridesForDesignConfiguration( QSharedPointer<DesignConfiguration> testDesignConfiguration,
	QSharedPointer<QList<QSharedPointer<ComponentInstance> > > componentInstances)
{
    QMap<QString, QString> viewOverrides;
    for (auto const& instance : *componentInstances)
    {
        VLNV componentReferenceVLNV = *instance->getComponentRef();
        QSharedPointer<Component> referencedComponent =
            library_.getModel(componentReferenceVLNV).dynamicCast<Component>();

		if (referencedComponent && referencedComponent->getModel()->getViews()->size() > 1)
		{
            viewOverrides.insert(instance->getUuid(), referencedComponent->getViews()->at(1)->name());
        }
    }

    testDesignConfiguration->setKactus2ViewOverrides(viewOverrides);
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::setFileTypesForFileSet()
//-----------------------------------------------------------------------------
void tst_QuartusProjectGenerator::setFileTypesForFileSet(QSharedPointer<FileSet> selectedFileset, QSharedPointer<QStringList> fileTypes)
{
    QSharedPointer<QList<FileType> > fileTypeObjs(new QList<FileType>());

    for (auto const& fileType : *fileTypes)
    {
        fileTypeObjs->append(FileType(fileType));
    }

    for (auto const& currentFile : *selectedFileset->getFiles())
    {
        currentFile->setFileTypes(fileTypeObjs);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::projectFile()
//-----------------------------------------------------------------------------
QString tst_QuartusProjectGenerator::projectFile() const
{
    static const QString PROJECTFILE(".qpf");
    return PROJECTFILE;
}

//-----------------------------------------------------------------------------
// Function: tst_QuartusProjectGenerator::settingsFile()
//-----------------------------------------------------------------------------
QString tst_QuartusProjectGenerator::settingsFile() const
{
    static const QString SETTINGSFILE(".qsf");
    return SETTINGSFILE;
}

//QTEST_APPLESS_MAIN(tst_QuartusProjectGenerator)
QTEST_MAIN(tst_QuartusProjectGenerator)

#include "tst_QuartusProjectGenerator.moc"
