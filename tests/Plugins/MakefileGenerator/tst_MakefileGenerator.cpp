//-----------------------------------------------------------------------------
// File: tst_MakefileGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 02.09.2014
//
// Description:
// Unit test for classes MakefileParser and MakefileGenerator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/ComponentInstance.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/SWView.h>
#include <IPXACTmodels/SystemView.h>
#include <IPXACTmodels/file.h>
#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/ApiInterface.h>

#include <tests/MockObjects/LibraryMock.h>

#include <Plugins/MakefileGenerator/MakefileParser.h>
#include <Plugins/MakefileGenerator/MakefileGenerator.h>
#include <Plugins/PluginSystem/PluginUtilityAdapter.h>

#include <QRegExp>

class tst_MakefileGenerator : public QObject
{
    Q_OBJECT

public:
    tst_MakefileGenerator();

private slots:
    void baseCase();

    void fileBuildOverride();
    void fileSetBuildOverride();

    void fileFlagReplace();

    void fileSetFlagReplace();
    void includeFile();
    void swSWViewFlagReplace();
    void hwBuilder();
    void hwBuilderWithNoSoftView();
    void hwRef();
    void hwandswRef();
    void instanceHeaders();

    void multipleFiles();
    void multipleFileSets();
    void multipleComponents();
    void noFilesComponent();
    void multipleHardWare();
    void multipleHardWareMedRefs();

    void noHardWare();
    void multipleInstances();
    void apiUsage();
    void threeLevelStack();
    void fullCircularapiUsage();
    void circularapiUsage();

private:
    void getFile(QSharedPointer<File>& file, QSharedPointer<Component> component, QString fileName);

    QSharedPointer<Component> createSW(QString swName, QString hwInstanceName, QSharedPointer<Design> design, QString softViewName, QMap<QString,QString>& activeViews, SWView*& softView, QString swInstanceName);

    QSharedPointer<FileSet> addFileSet(QSharedPointer<Component> component, QString fileSetName, SWView*& view);

    void addFileToSet(QSharedPointer<FileSet> fileSet, QString fileName, QString fileType="cSource", bool isInclude=false);

    void createSWSWView(SWView* softView, QString softViewName, QSharedPointer<Component> sw);

    QSharedPointer<Component> createHW(QString hwInstanceName, QSharedPointer<Design> design, QString hardViewName, QMap<QString,QString>& activeViews, SWView*& hardView, QString hwName="hardware");

    QSharedPointer<Component> createDesign(QSharedPointer<Design> &design, QSharedPointer<DesignConfiguration> &desgconf);

    void verifyOutputContains(QString instanceName, QString const& expectedOutput);

    void setFileBuilder(QSharedPointer<File> file, QString command, QString flags, bool replace);

    void addFileSetBuilder(QSharedPointer<FileSet> fileSet, QString command, QString fileType, QString flags, bool replace);

    void addCmd2View(SWView* view, QString command, QString fileType, QString flags, bool replace);

    //! The test mock for library interface.
    LibraryMock library_;

    //! Output directory for the generator
    QString outputDir_;

    //! Mock utility to be used in tests
    PluginUtilityAdapter utilityMock_;
};

tst_MakefileGenerator::tst_MakefileGenerator(): library_( this ), utilityMock_(&library_, 0, this)
{
}

// A rather basic and secure case testing that all the needed lines come for one file.
void tst_MakefileGenerator::baseCase()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* softView;

    createHW("hardware_0", design, "firmware", activeViews, hardView);
    addCmd2View(hardView, "gcc", "cSource", "-hw", false);

    QSharedPointer<Component> sw = createSW("software", "hardware_0", design, "default", activeViews, softView,"software_0");

    QString fileSetName = "someFileSet";
    addFileSet(sw, fileSetName, softView);

    QSharedPointer<FileSet> fileSet = sw->getFileSet(fileSetName);
    addFileToSet(fileSet, "array.c");
    addCmd2View(softView, "gcc", "cSource", "-sw", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design, outputDir_ );
    MakefileGenerator generator( parser, &utilityMock_ );
    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("software_0", "_OBJ= array.c.o");
    verifyOutputContains("software_0", "OBJ= $(patsubst %,$(ODIR)/%,$(_OBJ))");
    verifyOutputContains("software_0", "ODIR= obj");
    verifyOutputContains("software_0", "ENAME= software_0");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES)");
    verifyOutputContains("software_0", "EBUILDER= gcc");
    verifyOutputContains("software_0", "$(ENAME): $(OBJ)\n\t$(EBUILDER) -o bin_$(ENAME) $(OBJ) $(EFLAGS)");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -sw -hw");
}

// Case used to test if file builder properly overrides other builders.
void tst_MakefileGenerator::fileBuildOverride()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, activeViews, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softView);

    addFileToSet(fileSet, "array.c");
    addCmd2View(softView, "gcc -o", "cSource", "-sw", false);

    QSharedPointer<File> file;
    getFile(file, sw, "array.c");
    setFileBuilder(file, "python", "-l", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );
    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) -sw");
    verifyOutputContains("software_0", "python -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -l -sw");
}

// Case used to test if file set builder properly overrides other builders.
void tst_MakefileGenerator::fileSetBuildOverride()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, activeViews, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softView);

    addFileToSet(fileSet, "array.c");
    addCmd2View(softView, "gcc -o", "cSource", "-sw", false);
    addFileSetBuilder(fileSet, "javac -beef", "cSource", "-lrt", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) -sw");
    verifyOutputContains("software_0", "javac -beef -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -lrt -sw");
}

// Case used to test if file flags properly replace other flags if needed.
void tst_MakefileGenerator::fileFlagReplace()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* softView;

    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, activeViews, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softView);

    addFileToSet(fileSet, "array.c");
    addCmd2View(softView, "gcc", "cSource", "-sw", false);

    QSharedPointer<File> file;
    getFile(file, sw, "array.c");
    setFileBuilder(file, "", "-u", true);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) -sw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -u");
}

// Case used to test if file set flags properly replace other flags if needed.
void tst_MakefileGenerator::fileSetFlagReplace()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, activeViews, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    QSharedPointer<FileSet> fileSet = addFileSet(sw, fileSetName, softView);

    addFileToSet(fileSet, "array.c");
    addCmd2View(softView, "gcc", "cSource", "-sw", false);
    addFileSetBuilder(fileSet, "javac -beef", "cSource", "-lrt", true);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) -sw");
    verifyOutputContains("software_0", "javac -beef -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -lrt");
}

// Include files shoudl not yield object files.
void tst_MakefileGenerator::includeFile()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, activeViews, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    addFileSet(sw, fileSetName, softView);

    QSharedPointer<FileSet> fileSet = sw->getFileSet(fileSetName);
    addFileToSet(fileSet, "array.c");
    addFileToSet(fileSet, "array.h", "cSource", true);

    addCmd2View(hardView, "gcc", "cSource", "-hw", false);
    addCmd2View(softView, "gcc", "cSource", "-sw", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("software_0", "_OBJ= array.c.o\n");
    verifyOutputContains("software_0", "EBUILDER= gcc");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) -hw -sw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -sw -hw");
}

// Softwaree view of the software component should be able to replace those of hardware component.
void tst_MakefileGenerator::swSWViewFlagReplace()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, activeViews, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    addFileSet(sw, fileSetName, softView);

    QSharedPointer<FileSet> fileSet = sw->getFileSet(fileSetName);
    addFileToSet(fileSet, "array.c");

    addCmd2View(hardView, "gcc", "cSource", "-hw", false);
    addCmd2View(softView, "gcc", "cSource", "-sw", true);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("software_0", "EBUILDER= gcc");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) -hw -sw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -sw\n");
}

// See if hardware builder is taken account properly.
void tst_MakefileGenerator::hwBuilder()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, activeViews, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    addFileSet(sw, fileSetName, softView);

    QSharedPointer<FileSet> fileSet = sw->getFileSet(fileSetName);
    addFileToSet(fileSet, "array.c");
    addFileSetBuilder(fileSet, "", "cSource", "-lrt", false);

    addCmd2View(hardView, "super_asm", "cSource", "-hw", false);
    addCmd2View(softView, "", "cSource", "-sw", false);

    QSharedPointer<File> file;
    getFile(file, sw, "array.c");
    setFileBuilder(file, "", "-u", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("software_0", "EBUILDER= super_asm");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) -hw -sw");
    verifyOutputContains("software_0", "super_asm -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -u -lrt -sw -hw");
}

// See if hardware builder works if there are no software view for the software component.
void tst_MakefileGenerator::hwBuilderWithNoSoftView()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, activeViews, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    addFileSet(sw, fileSetName, softView);

    QSharedPointer<FileSet> fileSet = sw->getFileSet(fileSetName);
    addFileToSet(fileSet, "array.c");
    addFileSetBuilder(fileSet, "", "cSource", "-lrt", false);

    addCmd2View(hardView, "super_asm", "cSource", "-hw", false);

    QSharedPointer<File> file;
    getFile(file, sw, "array.c");
    setFileBuilder(file, "", "-u", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("software_0", "EBUILDER= super_asm");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) -hw");
    verifyOutputContains("software_0", "super_asm -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -u -lrt -hw");
}

// See if hard ware's own file set references function.
void tst_MakefileGenerator::hwRef()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    QSharedPointer<Component> hw = createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    createSW("software", hwInstanceName, design, softViewName, activeViews, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    addFileSet(hw, fileSetName, hardView);

    QSharedPointer<FileSet> fileSet = hw->getFileSet(fileSetName);
    addFileToSet(fileSet, "array.c");
    addFileSetBuilder(fileSet, "", "cSource", "-lrt", false);

    addCmd2View(hardView, "super_asm", "cSource", "-hw", false);
    addCmd2View(softView, "", "cSource", "-sw", false);

    QSharedPointer<File> file;
    getFile(file, hw, "array.c");
    setFileBuilder(file, "", "-u", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("software_0", "EBUILDER= super_asm");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) -hw");
    verifyOutputContains("software_0", "super_asm -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -u -lrt -hw");
}

// See if hard ware's own file set references function, while there are those of software component.
void tst_MakefileGenerator::hwandswRef()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    QSharedPointer<Component> hw = createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<FileSet> hfileSet = addFileSet(hw, "hardFileSet", hardView);

    addFileToSet(hfileSet, "harray.c");
    addFileSetBuilder(hfileSet, "", "cSource", "-hset", false);
    addCmd2View(hardView, "super_asm", "cSource", "-hw", false);

    QSharedPointer<File> hfile;
    getFile(hfile, hw, "harray.c");
    setFileBuilder(hfile, "", "-hu", false);

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, activeViews, softView,swInstanceName);

    QString sfileSetName = "softFileSet";
    addFileSet(sw, sfileSetName, softView);

    QSharedPointer<FileSet> sfileSet = sw->getFileSet(sfileSetName);
    addFileToSet(sfileSet, "sarray.c");
    addFileSetBuilder(sfileSet, "", "cSource", "-sset", false);
    addCmd2View(softView, "", "cSource", "-sw", false);

    QSharedPointer<File> sfile;
    getFile(sfile, sw, "sarray.c");
    setFileBuilder(sfile, "", "-su", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("software_0", "_OBJ= sarray.c.o harray.c.o");
    verifyOutputContains("software_0", "EBUILDER= super_asm");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) -hw -sw");
    verifyOutputContains("software_0", "super_asm -c -o $(ODIR)/sarray.c.o ../../sarray.c $(INCLUDES) -su -sset -sw -hw");
    verifyOutputContains("software_0", "super_asm -c -o $(ODIR)/harray.c.o ../../harray.c $(INCLUDES) -hu -hset -hw");
}

// Instance specific header files must be included to the make file.
void tst_MakefileGenerator::instanceHeaders()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, activeViews, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    addFileSet(sw, fileSetName, softView);
    QSharedPointer<FileSet> fileSet = sw->getFileSet(fileSetName);
    addFileToSet(fileSet, "array.c");

    QString topFileSetName = "software_0_headers";
    QSharedPointer<FileSet> topFileSet = topComponent->getFileSet(topFileSetName);
    addFileToSet(topFileSet, "array.h", "cSource", true);
    QString topFileSetName2 = "esa-pekka";
    QSharedPointer<FileSet> topFileSet2 = topComponent->getFileSet(topFileSetName2);
    addFileToSet(topFileSet2, "joku.h", "cSource", true);

    addCmd2View(hardView, "gcc", "cSource", "-hw", false);
    addCmd2View(softView, "gcc", "cSource", "-sw", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);
}

// Generate makefile with multiple source and header files.
void tst_MakefileGenerator::multipleFiles()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, activeViews, softView,swInstanceName);

    QString fileSetName = "someFileSet";
    addFileSet(sw, fileSetName, softView);

    QSharedPointer<FileSet> fileSet = sw->getFileSet(fileSetName);
    addFileToSet(fileSet, "array.c");
    addFileToSet(fileSet, "support.c");
    addFileToSet(fileSet, "additional.c");
    addFileToSet(fileSet, "hiterbehn.c");
    addFileToSet(fileSet, "array.h", "cSource", true);
    addFileToSet(fileSet, "support.h", "cSource", true);

    addCmd2View(hardView, "gcc", "cSource", "-hw", false);
    addCmd2View(softView, "gcc", "cSource", "-sw", false);

    QSharedPointer<File> file;
    getFile(file, sw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("software_0", "_OBJ= array.c.o support.c.o additional.c.o hiterbehn.c.o");
    verifyOutputContains("software_0", "EBUILDER= gcc");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) -hw -sw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -sw -hw");
    verifyOutputContains("software_0", "continental -c -o $(ODIR)/support.c.o ../../support.c $(INCLUDES) -y -sw -hw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) -sw -hw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) -sw -hw");
}

// Generate makefile when multiple file sets are featured.
void tst_MakefileGenerator::multipleFileSets()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QString swInstanceName = "software_0";
    QString softViewName = "default";

    QSharedPointer<Component> sw = createSW("software", hwInstanceName, design, softViewName, activeViews, softView,swInstanceName);

    QSharedPointer<FileSet> afileSet = addFileSet(sw, "alphaFileSet", softView);
    QSharedPointer<FileSet> bfileSet = addFileSet(sw, "betaFileSet", softView);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addCmd2View(hardView, "gcc", "cSource", "-hw", false);
    addCmd2View(softView, "gcc", "cSource", "-sw", false);

    QSharedPointer<File> file;
    getFile(file, sw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("software_0", "_OBJ= array.c.o support.c.o additional.c.o hiterbehn.c.o");
    verifyOutputContains("software_0", "EBUILDER= gcc");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) -hw -sw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -sw -hw");
    verifyOutputContains("software_0", "continental -c -o $(ODIR)/support.c.o ../../support.c $(INCLUDES) -y -sw -hw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) -sw -hw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) -sw -hw");
}

// Generate makefile when multiple components are featured.
void tst_MakefileGenerator::multipleComponents()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* asoftView;
    SWView* bsoftView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QSharedPointer<Component> asw = createSW("crapware", hwInstanceName, design, "default", activeViews, asoftView,"crapware_0");
    QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", activeViews, bsoftView,"stackware_0");

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addCmd2View(hardView, "hopo", "cSource", "-hw", false);
    addCmd2View(bsoftView, "asm-meister", "cSource", "-bmw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("crapware_0", "EBUILDER= hopo");
    verifyOutputContains("crapware_0", "EFLAGS= $(INCLUDES) -hw");
    verifyOutputContains("crapware_0", "_OBJ= array.c.o support.c.o");
    verifyOutputContains("crapware_0", "hopo -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -hw");
    verifyOutputContains("crapware_0", "continental -c -o $(ODIR)/support.c.o ../../support.c $(INCLUDES) -y -hw");

    verifyOutputContains("stackware_0", "EBUILDER= hopo");
    verifyOutputContains("stackware_0", "EFLAGS= $(INCLUDES) -hw -bmw");
    verifyOutputContains("stackware_0", "_OBJ= additional.c.o hiterbehn.c.o");
    verifyOutputContains("stackware_0", "asm-meister -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) -bmw -hw");
    verifyOutputContains("stackware_0", "asm-meister -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) -bmw -hw");
}

// Yield no makefile, if no files are present.
void tst_MakefileGenerator::noFilesComponent()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* asoftView;
    SWView* bsoftView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QSharedPointer<Component> asw = createSW("whatware", hwInstanceName, design, "default", activeViews, asoftView,"whatware_0");
    QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", activeViews, bsoftView,"stackware_0");

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);

    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addCmd2View(hardView, "hopo", "cSource", "-hw", false);
    addCmd2View(bsoftView, "asm-meister", "cSource", "-bmw", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("stackware_0", "EBUILDER= hopo");
    verifyOutputContains("stackware_0", "EFLAGS= $(INCLUDES) -hw -bmw");
    verifyOutputContains("stackware_0", "_OBJ= additional.c.o hiterbehn.c.o");
    verifyOutputContains("stackware_0", "asm-meister -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) -bmw -hw");
    verifyOutputContains("stackware_0", "asm-meister -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) -bmw -hw");

    QFile outputFile(outputDir_ + "/sw/whatware_0/Makefile");

    QVERIFY(!outputFile.open(QIODevice::ReadOnly));
}

// Must work if there is multiple hardware where instances are mapped to.
void tst_MakefileGenerator::multipleHardWare()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* ahardView;
    SWView* bhardView;
    SWView* asoftView;
    SWView* bsoftView;

    createHW("Lenit_0", design, "firmware", activeViews, ahardView, "Lenit");
    createHW("MAR_0", design, "firmware", activeViews, bhardView, "MAR");

    QSharedPointer<Component> asw = createSW("crapware", "Lenit_0", design, "default", activeViews, asoftView,"crapware_0");
    QSharedPointer<Component> bsw = createSW("stackware", "MAR_0", design, "default", activeViews, bsoftView,"stackware_0");

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(afileSet, "array.h", "cSource", true);

    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addCmd2View(ahardView, "hopo", "cSource", "-ahw", false);
    addCmd2View(bhardView, "juu -f", "cSource", "-bhw", false);

    addCmd2View(bsoftView, "asm-meister", "cSource", "-bmw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("crapware_0", "EBUILDER= hopo");
    verifyOutputContains("crapware_0", "EFLAGS= $(INCLUDES) -ahw");
    verifyOutputContains("crapware_0", "_OBJ= array.c.o support.c.o");
    verifyOutputContains("crapware_0", "hopo -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -ahw");
    verifyOutputContains("crapware_0", "continental -c -o $(ODIR)/support.c.o ../../support.c $(INCLUDES) -y -ahw");

    verifyOutputContains("stackware_0", "EBUILDER= juu -f");
    verifyOutputContains("stackware_0", "EFLAGS= $(INCLUDES) -bhw -bmw");
    verifyOutputContains("stackware_0", "_OBJ= additional.c.o hiterbehn.c.o");
    verifyOutputContains("stackware_0", "asm-meister -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) -bmw -bhw");
    verifyOutputContains("stackware_0", "asm-meister -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) -bmw -bhw");
}

// Multiple hardware, along with their references to their own files.
void tst_MakefileGenerator::multipleHardWareMedRefs()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* ahardView;
    SWView* bhardView;
    SWView* asoftView;
    SWView* bsoftView;

    QSharedPointer<Component> hw = createHW("Lenit_0", design, "firmware", activeViews, ahardView, "Lenit");
    createHW("MAR_0", design, "firmware", activeViews, bhardView, "MAR");

    QSharedPointer<Component> asw = createSW("crapware", "Lenit_0", design, "default", activeViews, asoftView,"crapware_0");
    QSharedPointer<Component> bsw = createSW("stackware", "MAR_0", design, "default", activeViews, bsoftView,"stackware_0");

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(afileSet, "array.h", "cSource", true);

    QString hfileSetName = "hardFileSet";
    QSharedPointer<FileSet> hfileSet = addFileSet(hw, hfileSetName, ahardView);

    addFileToSet(hfileSet, "harray.c");
    addFileSetBuilder(hfileSet, "", "cSource", "-hset", false);

    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addCmd2View(ahardView, "hopo", "cSource", "-ahw", false);
    addCmd2View(bhardView, "juu", "cSource", "-bhw", false);

    addCmd2View(bsoftView, "asm-meister", "cSource", "-bmw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("crapware_0", "EBUILDER= hopo");
    verifyOutputContains("crapware_0", "EFLAGS= $(INCLUDES) -ahw");
    verifyOutputContains("crapware_0", "_OBJ= array.c.o support.c.o");
    verifyOutputContains("crapware_0", "hopo -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -ahw");
    verifyOutputContains("crapware_0", "continental -c -o $(ODIR)/support.c.o ../../support.c $(INCLUDES) -y -ahw");

    verifyOutputContains("stackware_0", "EBUILDER= juu");
    verifyOutputContains("stackware_0", "EFLAGS= $(INCLUDES) -bhw -bmw");
    verifyOutputContains("stackware_0", "_OBJ= additional.c.o hiterbehn.c.o");
    verifyOutputContains("stackware_0", "asm-meister -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) -bmw -bhw");
    verifyOutputContains("stackware_0", "asm-meister -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) -bmw -bhw");
}

// How it works without hardware. NOTICE: presently shall have no compiler!
void tst_MakefileGenerator::noHardWare()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* softView;

    QSharedPointer<Component> sw = createSW("software", "", design, "default", activeViews, softView,"software_0");

    QString fileSetName = "someFileSet";
    addFileSet(sw, fileSetName, softView);

    QSharedPointer<FileSet> fileSet = sw->getFileSet(fileSetName);
    addFileToSet(fileSet, "array.c");
    addCmd2View(softView, "gcc", "cSource", "-sw", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("software_0", "_OBJ= array.c.o");
    verifyOutputContains("software_0", "OBJ= $(patsubst %,$(ODIR)/%,$(_OBJ))");
    verifyOutputContains("software_0", "EBUILDER= ");
    verifyOutputContains("software_0", "EFLAGS= $(INCLUDES) -sw");
    verifyOutputContains("software_0", "gcc -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -sw");
}

// Must work with multiple instances of the same component.
void tst_MakefileGenerator::multipleInstances()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* softView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QSharedPointer<Component> sw = createSW("stackware", hwInstanceName, design, "default", activeViews, softView,"stackware_0");

    VLNV* swvlvnv = sw->getVlnv();
    SWInstance softInstance;
    softInstance.setInstanceName("stackware_1");
    softInstance.setComponentRef(*swvlvnv);
    softInstance.setMapping(hwInstanceName);

    activeViews.insert("stackware_1","default");

    QList<SWInstance> swInstances = design->getSWInstances();
    swInstances.append(softInstance);
    design->setSWInstances(swInstances);
    library_.writeModelToFile("polku/stackware",sw);

    QSharedPointer<FileSet> fileSet = addFileSet(sw, "FileSet", softView);

    addFileToSet(fileSet, "additional.c");
    addFileToSet(fileSet, "hiterbehn.c");
    addFileToSet(fileSet, "array.h", "cSource", true);
    addFileToSet(fileSet, "support.h", "cSource", true);

    addCmd2View(hardView, "hopo", "cSource", "-hw", false);
    addCmd2View(softView, "asm-meister", "cSource", "-bmw", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("stackware_0", "EBUILDER= hopo");
    verifyOutputContains("stackware_0", "EFLAGS= $(INCLUDES) -hw -bmw");

    verifyOutputContains("stackware_1", "EBUILDER= hopo");
    verifyOutputContains("stackware_1", "EFLAGS= $(INCLUDES) -hw -bmw");
}

// Must include API dependencies to the make file.
void tst_MakefileGenerator::apiUsage()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* asoftView;
    SWView* bsoftView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QSharedPointer<Component> asw = createSW("crapware", hwInstanceName, design, "default", activeViews, asoftView,"crapware_0");
    QSharedPointer<ApiInterface> apina = QSharedPointer<ApiInterface>(new ApiInterface );
    apina->setDependencyDirection(DEPENDENCY_REQUESTER);
    apina->setName("apina");
    asw->addApiInterface( apina );

    QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", activeViews, bsoftView,"stackware_0");
    QSharedPointer<ApiInterface> banaani = QSharedPointer<ApiInterface>(new ApiInterface );
    banaani->setDependencyDirection(DEPENDENCY_PROVIDER);
    banaani->setName("banaani");
    bsw->addApiInterface( banaani );

    QList<ApiConnection> deps = design->getApiDependencies();
    ApiConnection dependency;
    dependency.setInterface1(ApiInterfaceRef("crapware_0","apina"));
    dependency.setInterface2(ApiInterfaceRef("stackware_0","banaani"));
    deps.append(dependency);
    design->setApiDependencies(deps);

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addCmd2View(hardView, "hopo -s", "cSource", "-hw", false);
    addCmd2View(bsoftView, "asm-meister", "cSource", "-bmw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("crapware_0", "EBUILDER= hopo -s");
    verifyOutputContains("crapware_0", "EFLAGS= $(INCLUDES) -hw");
    verifyOutputContains("crapware_0", "_OBJ= array.c.o support.c.o additional.c.o hiterbehn.c.o");
    verifyOutputContains("crapware_0", "hopo -s -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -hw");
    verifyOutputContains("crapware_0", "continental -c -o $(ODIR)/support.c.o ../../support.c $(INCLUDES) -y -hw");

    verifyOutputContains("crapware_0", "asm-meister -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) -bmw -hw");
    verifyOutputContains("crapware_0", "asm-meister -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) -bmw -hw");
}

// A more complex situation with API dependency.
void tst_MakefileGenerator::threeLevelStack()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* asoftView;
    SWView* bsoftView;
    SWView* gsoftView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QSharedPointer<Component> asw = createSW("crapware", hwInstanceName, design, "default", activeViews, asoftView,"crapware_0");
    QSharedPointer<ApiInterface> apina = QSharedPointer<ApiInterface>(new ApiInterface );
    apina->setDependencyDirection(DEPENDENCY_REQUESTER);
    apina->setName("apina");
    asw->addApiInterface( apina );

    QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", activeViews, bsoftView,"stackware_0");
    QSharedPointer<ApiInterface> banaani = QSharedPointer<ApiInterface>(new ApiInterface );
    banaani->setDependencyDirection(DEPENDENCY_PROVIDER);
    banaani->setName("banaani");
    bsw->addApiInterface( banaani );

    QSharedPointer<Component> gsw = createSW("driver", hwInstanceName, design, "default", activeViews, gsoftView,"driver_0");

    QSharedPointer<ApiInterface> stackDriver = QSharedPointer<ApiInterface>(new ApiInterface );
    stackDriver->setDependencyDirection(DEPENDENCY_REQUESTER);
    stackDriver->setName("stackDriver");
    bsw->addApiInterface( stackDriver );
    QSharedPointer<ApiInterface> driverStack = QSharedPointer<ApiInterface>(new ApiInterface );
    driverStack->setDependencyDirection(DEPENDENCY_PROVIDER);
    driverStack->setName("driverStack");
    gsw->addApiInterface( driverStack );

    QList<ApiConnection> deps = design->getApiDependencies();
    ApiConnection dependency1;
    dependency1.setInterface1(ApiInterfaceRef("crapware_0","apina"));
    dependency1.setInterface2(ApiInterfaceRef("stackware_0","banaani"));
    deps.append(dependency1);
    ApiConnection dependency2;
    dependency2.setInterface1(ApiInterfaceRef("stackware_0","stackDriver"));
    dependency2.setInterface2(ApiInterfaceRef("driver_0","driverStack"));
    deps.append(dependency2);
    design->setApiDependencies(deps);

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);
    QSharedPointer<FileSet> cfileSet = addFileSet(gsw, "gamaFileSet", gsoftView);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addFileToSet(cfileSet, "ok.c");
    addFileToSet(cfileSet, "ok.h", "cSource", true);

    addCmd2View(hardView, "hopo", "cSource", "-hw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);
    addFileSetBuilder(bfileSet,"jaska","cSource","-g",false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("crapware_0", "EBUILDER= hopo");
    verifyOutputContains("crapware_0", "EFLAGS= $(INCLUDES) -hw");
    verifyOutputContains("crapware_0", "_OBJ= array.c.o support.c.o additional.c.o hiterbehn.c.o ok.c.o");
    verifyOutputContains("crapware_0", "hopo -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -hw");
    verifyOutputContains("crapware_0", "continental -c -o $(ODIR)/support.c.o ../../support.c $(INCLUDES) -y -hw");

    verifyOutputContains("crapware_0", "jaska -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) -g -hw");
    verifyOutputContains("crapware_0", "jaska -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) -g -hw");

    verifyOutputContains("crapware_0", "hopo -c -o $(ODIR)/ok.c.o ../../ok.c $(INCLUDES) -hw");
}

// If no top level in the stack, there can be no executable!
void tst_MakefileGenerator::fullCircularapiUsage()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* asoftView;
    SWView* bsoftView;
    SWView* gsoftView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QSharedPointer<Component> asw = createSW("topware", hwInstanceName, design, "default", activeViews, asoftView,"topware_0");
    QSharedPointer<ApiInterface> apina = QSharedPointer<ApiInterface>(new ApiInterface );
    apina->setDependencyDirection(DEPENDENCY_REQUESTER);
    apina->setName("apina");
    asw->addApiInterface( apina );

    QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", activeViews, bsoftView,"stackware_0");
    QSharedPointer<ApiInterface> banaani = QSharedPointer<ApiInterface>(new ApiInterface );
    banaani->setDependencyDirection(DEPENDENCY_PROVIDER);
    banaani->setName("banaani");
    bsw->addApiInterface( banaani );

    QSharedPointer<Component> gsw = createSW("driver", hwInstanceName, design, "default", activeViews, gsoftView,"driver_0");

    QSharedPointer<ApiInterface> stackDriver = QSharedPointer<ApiInterface>(new ApiInterface );
    stackDriver->setDependencyDirection(DEPENDENCY_REQUESTER);
    stackDriver->setName("stackDriver");
    bsw->addApiInterface( stackDriver );
    QSharedPointer<ApiInterface> driverStack = QSharedPointer<ApiInterface>(new ApiInterface );
    driverStack->setDependencyDirection(DEPENDENCY_PROVIDER);
    driverStack->setName("driverStack");
    gsw->addApiInterface( driverStack );

    QSharedPointer<ApiInterface> bottomUp = QSharedPointer<ApiInterface>(new ApiInterface );
    bottomUp->setDependencyDirection(DEPENDENCY_REQUESTER);
    bottomUp->setName("bottomUp");
    gsw->addApiInterface( bottomUp );
    QSharedPointer<ApiInterface> upBottom = QSharedPointer<ApiInterface>(new ApiInterface );
    upBottom->setDependencyDirection(DEPENDENCY_PROVIDER);
    upBottom->setName("upBottom");
    asw->addApiInterface( upBottom );

    QList<ApiConnection> deps = design->getApiDependencies();
    ApiConnection dependency1;
    dependency1.setInterface1(ApiInterfaceRef("topware_0","apina"));
    dependency1.setInterface2(ApiInterfaceRef("stackware_0","banaani"));
    deps.append(dependency1);
    ApiConnection dependency2;
    dependency2.setInterface1(ApiInterfaceRef("stackware_0","stackDriver"));
    dependency2.setInterface2(ApiInterfaceRef("driver_0","driverStack"));
    deps.append(dependency2);
    ApiConnection dependency3;
    dependency3.setInterface1(ApiInterfaceRef("topware_0","upBottom"));
    dependency3.setInterface2(ApiInterfaceRef("driver_0","bottomUp"));
    deps.append(dependency3);
    design->setApiDependencies(deps);

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);
    QSharedPointer<FileSet> cfileSet = addFileSet(gsw, "gamaFileSet", gsoftView);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addFileToSet(cfileSet, "ok.c");
    addFileToSet(cfileSet, "ok.h", "cSource", true);

    addCmd2View(hardView, "hopo", "cSource", "-hw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);
    addFileSetBuilder(bfileSet,"jaska","cSource","-g",false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    QFile outputFile(outputDir_ + "/sw/topware_0/Makefile");

    QVERIFY(!outputFile.open(QIODevice::ReadOnly));
}

// Must be able to handle circular dependencies in lower levels.
void tst_MakefileGenerator::circularapiUsage()
{
    QSharedPointer<Design> design;
    QSharedPointer<DesignConfiguration> desgconf;
    QSharedPointer<Component> topComponent = createDesign(design, desgconf);

    QMap<QString,QString> activeViews;
    SWView* hardView;
    SWView* asoftView;
    SWView* bsoftView;
    SWView* gsoftView;

    QString hwInstanceName = "hardware_0";
    QString hardViewName = "firmware";
    createHW(hwInstanceName, design, hardViewName, activeViews, hardView);

    QSharedPointer<Component> asw = createSW("crapware", hwInstanceName, design, "default", activeViews, asoftView,"crapware_0");
    QSharedPointer<ApiInterface> apina = QSharedPointer<ApiInterface>(new ApiInterface );
    apina->setDependencyDirection(DEPENDENCY_REQUESTER);
    apina->setName("apina");
    asw->addApiInterface( apina );

    QSharedPointer<Component> bsw = createSW("stackware", hwInstanceName, design, "default", activeViews, bsoftView,"stackware_0");
    QSharedPointer<ApiInterface> banaani = QSharedPointer<ApiInterface>(new ApiInterface );
    banaani->setDependencyDirection(DEPENDENCY_PROVIDER);
    banaani->setName("banaani");
    bsw->addApiInterface( banaani );

    QSharedPointer<Component> gsw = createSW("driver", hwInstanceName, design, "default", activeViews, gsoftView,"driver_0");

    QSharedPointer<ApiInterface> stackDriver = QSharedPointer<ApiInterface>(new ApiInterface );
    stackDriver->setDependencyDirection(DEPENDENCY_REQUESTER);
    stackDriver->setName("stackDriver");
    bsw->addApiInterface( stackDriver );
    QSharedPointer<ApiInterface> driverStack = QSharedPointer<ApiInterface>(new ApiInterface );
    driverStack->setDependencyDirection(DEPENDENCY_PROVIDER);
    driverStack->setName("driverStack");
    gsw->addApiInterface( driverStack );

    QSharedPointer<ApiInterface> bottomUp = QSharedPointer<ApiInterface>(new ApiInterface );
    bottomUp->setDependencyDirection(DEPENDENCY_REQUESTER);
    bottomUp->setName("bottomUp");
    gsw->addApiInterface( bottomUp );
    QSharedPointer<ApiInterface> upBottom = QSharedPointer<ApiInterface>(new ApiInterface );
    upBottom->setDependencyDirection(DEPENDENCY_PROVIDER);
    upBottom->setName("upBottom");
    bsw->addApiInterface( upBottom );

    QList<ApiConnection> deps = design->getApiDependencies();
    ApiConnection dependency1;
    dependency1.setInterface1(ApiInterfaceRef("crapware_0","apina"));
    dependency1.setInterface2(ApiInterfaceRef("stackware_0","banaani"));
    deps.append(dependency1);
    ApiConnection dependency2;
    dependency2.setInterface1(ApiInterfaceRef("stackware_0","stackDriver"));
    dependency2.setInterface2(ApiInterfaceRef("driver_0","driverStack"));
    deps.append(dependency2);
    ApiConnection dependency3;
    dependency3.setInterface1(ApiInterfaceRef("stackware_0","upBottom"));
    dependency3.setInterface2(ApiInterfaceRef("driver_0","bottomUp"));
    deps.append(dependency3);
    design->setApiDependencies(deps);

    QSharedPointer<FileSet> afileSet = addFileSet(asw, "alphaFileSet", asoftView);
    QSharedPointer<FileSet> bfileSet = addFileSet(bsw, "betaFileSet", bsoftView);
    QSharedPointer<FileSet> cfileSet = addFileSet(gsw, "gamaFileSet", gsoftView);

    addFileToSet(afileSet, "array.c");
    addFileToSet(afileSet, "support.c");
    addFileToSet(bfileSet, "additional.c");
    addFileToSet(bfileSet, "hiterbehn.c");
    addFileToSet(afileSet, "array.h", "cSource", true);
    addFileToSet(bfileSet, "support.h", "cSource", true);

    addFileToSet(cfileSet, "ok.c");
    addFileToSet(cfileSet, "ok.h", "cSource", true);

    addCmd2View(hardView, "hopo", "cSource", "-hw", false);

    QSharedPointer<File> file;
    getFile(file, asw, "support.c");
    setFileBuilder(file, "continental", "-y", false);
    addFileSetBuilder(bfileSet,"jaska","cSource","-g",false);

    desgconf->setViewConfigurations(activeViews);

    MakefileParser parser;
    parser.parse( &library_, topComponent, desgconf, design );
    MakefileGenerator generator( parser, &utilityMock_ );

    generator.generate(outputDir_,outputDir_);

    verifyOutputContains("crapware_0", "EBUILDER= hopo");
    verifyOutputContains("crapware_0", "EFLAGS= $(INCLUDES) -hw");
    verifyOutputContains("crapware_0", "_OBJ= array.c.o support.c.o additional.c.o hiterbehn.c.o ok.c.o");
    verifyOutputContains("crapware_0", "hopo -c -o $(ODIR)/array.c.o ../../array.c $(INCLUDES) -hw");
    verifyOutputContains("crapware_0", "continental -c -o $(ODIR)/support.c.o ../../support.c $(INCLUDES) -y -hw");

    verifyOutputContains("crapware_0", "jaska -c -o $(ODIR)/additional.c.o ../../additional.c $(INCLUDES) -g -hw");
    verifyOutputContains("crapware_0", "jaska -c -o $(ODIR)/hiterbehn.c.o ../../hiterbehn.c $(INCLUDES) -g -hw");

    verifyOutputContains("crapware_0", "hopo -c -o $(ODIR)/ok.c.o ../../ok.c $(INCLUDES) -hw");
}

QSharedPointer<Component> tst_MakefileGenerator::createDesign(QSharedPointer<Design> &design,
    QSharedPointer<DesignConfiguration> &desgconf)
{
    VLNV dvlnv("","kyytto","raision kirjasto","design","1.0");
    design = QSharedPointer<Design>(new Design(dvlnv));
    VLNV dgvlnv("","kyytto","raision kirjasto","design-conf","1.0");
    desgconf = QSharedPointer<DesignConfiguration>(new DesignConfiguration(dgvlnv));
    desgconf->setDesignRef(dvlnv);

    VLNV topvlvnv("","kyytto","raision kirjasto","master-plan","1.0");
    QSharedPointer<Component> top = QSharedPointer<Component>(new Component(topvlvnv));
    top->setComponentImplementation(KactusAttribute::KTS_SYS);
    library_.addComponent(top);
    library_.writeModelToFile("polku/master-plan",top);

    outputDir_ = QFileInfo(library_.getPath(*top->getVlnv())).absolutePath();

    SystemView* sw = new SystemView("sysview");
    sw->setHierarchyRef(dgvlnv);
    top->addSystemView(sw);

    return top;
}

void tst_MakefileGenerator::addFileToSet(QSharedPointer<FileSet> fileSet, QString fileName, QString fileType/*="cSource"*/, bool isInclude/*=false*/)
{
    QSettings settings;

    QSharedPointer<File> file;
    QStringList types;
    types.append(fileType);

    file = fileSet->addFile(fileName, settings);
    file->setAllFileTypes( types );
    file->setIncludeFile( isInclude );
}

QSharedPointer<Component> tst_MakefileGenerator::createSW(QString swName, QString hwInstanceName, QSharedPointer<Design> design, QString softViewName, QMap<QString,QString>& activeViews, SWView*& softView, QString swInstanceName)
{
    VLNV swvlvnv("","kyytto","raision kirjasto",swName,"1.0");
    QSharedPointer<Component> sw = QSharedPointer<Component>(new Component(swvlvnv));
    SWInstance softInstance;
    softInstance.setInstanceName(swInstanceName);
    softInstance.setComponentRef(swvlvnv);
    softInstance.setMapping(hwInstanceName);

    QList<SWInstance> swInstances = design->getSWInstances();
    swInstances.append(softInstance);
    design->setSWInstances(swInstances);
    library_.addComponent(sw);
    library_.writeModelToFile("polku/" + swName,sw);

    softView = new SWView();
    softView->setName(softViewName);
    sw->addSWView(softView);

    activeViews.insert(swInstanceName,softViewName);

    return sw;
}

QSharedPointer<Component> tst_MakefileGenerator::createHW(QString hwInstanceName, QSharedPointer<Design> design, QString hardViewName, QMap<QString,QString>& activeViews, SWView*& hardView, QString hwName/*="hardware"*/)
{
    VLNV hwvlvnv("","kyytto","raision kirjasto",hwName,"1.0");
    QSharedPointer<Component> hw = QSharedPointer<Component>(new Component(hwvlvnv));
    ComponentInstance hwInstance(hwInstanceName,"","esim",hwvlvnv,QPointF(),hwInstanceName);

    QList<ComponentInstance> instances = design->getComponentInstances();
    instances.append(hwInstance);
    design->setComponentInstances(instances);
    library_.addComponent(hw);
    library_.writeModelToFile("polku/" + hwName,hw);

    hardView = new SWView;
    hardView->setName(hardViewName);
    hw->addSWView(hardView);

    activeViews.insert(hwInstanceName,hardViewName);

    return hw;
}

//-----------------------------------------------------------------------------
// Function: tst_MakefileGenerator::verifyOutputContains()
//-----------------------------------------------------------------------------
void tst_MakefileGenerator::verifyOutputContains(QString instanceName, QString const& expectedOutput)
{
    QFile outputFile(outputDir_ + "/sw/" + instanceName + "/Makefile");

    QVERIFY(outputFile.open(QIODevice::ReadOnly));

    QString outputString = outputFile.readAll();
    outputFile.close();

    outputString = outputString.replace( QRegExp("[ ]+"), " " );

    if (!outputString.contains(expectedOutput))
    {
        QStringList outputLines = outputString.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            QCOMPARE(outputString, expectedOutput);
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
    else if(outputString.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " +
            QString::number(outputString.count(expectedOutput)) + " times in output.").toLocal8Bit());
    }
}

void tst_MakefileGenerator::getFile(QSharedPointer<File>& file, QSharedPointer<Component> component, QString fileName)
{
    foreach( QSharedPointer<FileSet> fset, component->getFileSets())
    {
        foreach( QSharedPointer<File> possibleFile, fset->getFiles())
        {
            if ( possibleFile->getName() == fileName )
            {
                file = possibleFile;
                return;
            }
        }
    }
}

void tst_MakefileGenerator::addCmd2View(SWView* view, QString command, QString fileType,
    QString flags, bool replace)
{
    QList<QSharedPointer<SWBuildCommand> >& coms = view->getSWBuildCommands();
    QSharedPointer<SWBuildCommand> cmd = QSharedPointer<SWBuildCommand>(new SWBuildCommand());
    cmd->setCommand(command);
    cmd->setFileType(fileType);
    cmd->setFlags(flags);
    cmd->setReplaceDefaultFlags(replace);
    coms.append(cmd);
}

QSharedPointer<FileSet> tst_MakefileGenerator::addFileSet(QSharedPointer<Component> component, QString fileSetName, SWView*& view)
{
    QSharedPointer<FileSet> fileSet = component->getFileSet(fileSetName);
    fileSet->setGroups("sourceFiles");
    QStringList fileSets = view->getFileSetRefs();
    fileSets.append(fileSetName);
    view->setFileSetRefs(fileSets);

    return fileSet;
}

void tst_MakefileGenerator::addFileSetBuilder(QSharedPointer<FileSet> fileSet, QString command, QString fileType, QString flags, bool replace)
{
    QList<QSharedPointer<FileBuilder> > fblist = fileSet->getDefaultFileBuilders();
    QSharedPointer<FileBuilder> fb = QSharedPointer<FileBuilder>(new FileBuilder);
    fb->setCommand( command);
    fb->setFileType(fileType);
    fb->setFlags(flags);
    fb->setReplaceDefaultFlags(replace);
    fblist.append(fb);
    fileSet->setDefaultFileBuilders(fblist);
}

void tst_MakefileGenerator::setFileBuilder(QSharedPointer<File> file, QString command, QString flags, bool replace)
{
    BuildCommand* buildCmd = new BuildCommand();
    buildCmd->setCommand(command);
    buildCmd->setFlags(flags);
    buildCmd->setReplaceDefaultFlags(replace);

    file->setBuildcommand( buildCmd );
}

QTEST_APPLESS_MAIN(tst_MakefileGenerator)

#include "tst_MakefileGenerator.moc"
