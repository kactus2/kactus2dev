//-----------------------------------------------------------------------------
// File: tst_ParameterReferenceTree.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 21.01.2015
//
// Description:
// Unit test for class ParameterReferenceTree.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <common/KactusColors.h>

#include <editors/ComponentEditor/parameterReferenceTree/ParameterReferenceTree.h>
#include <editors/ComponentEditor/parameterReferenceTree/ComponentParameterReferenceTree.h>
#include <editors/ComponentEditor/referenceCounter/ComponentParameterReferenceCounter.h>

#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/ComponentParameterFinder.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/common/FileBuilder.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/FieldReset.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/MirroredSlaveInterface.h>
#include <IPXACTmodels/Component/MasterInterface.h>
#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/RemapState.h>
#include <IPXACTmodels/Component/RemapPort.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/BuildCommand.h>

#include <QSharedPointer>

class tst_ParameterReferenceTree : public QObject
{
    Q_OBJECT

public:
    tst_ParameterReferenceTree();

private slots:
    void testEmptyComponentInConstructor();
    void testColumnCount();
    void testNoReferencesFoundAddsOneRow();

    void testReferenceInFileSetFileBuilderAddsFiveRows();

    void testReferenceInFileBuildCommandAddsSixRows();

    void testReferenceInParameterValueAddsThreeRows();
    void testMultipleReferencesInOneParameter();
    void testMultipleReferencesInMultipleParameters();

    void testReferenceAsAnExpressionInParameter();
    void testMultipleReferencesInSameExpression();

    void testReferenceInViewParameterValueAddsFiveRows();
    void testReferenceInViewModuleParameterValueAddsFiveRows();

    void testReferenceInComponentInstantiationFileBuilderAddsFiveRows();

    void testReferenceInPortRightboundAddsThreeRows();
    void testReferenceInPortArraysAddsThreeRows();

    void testReferenceInAddressBlockBaseAddressAddsSevenRows();
    void testReferenceInAddressBlockNoReferenceInRegister();

    void testReferenceInRegisterDimensionAddsNineRows();
    void testReferenceInRegisterIsPresentAddsNineRows();
    void testReferenceInMultipleRegistersInTwoAddressBlocks();
    void testRegisterwithNoReferences();

    void testReferenceInAddressSpaceAddressBlockAddsSixRows();
    void testReferenceInAddressSpaceSegmentAddsSixRows();

    void testReferenceInRegisterFieldAddsElevenRows();

    void testReferenceInBusInterfaceParameterAddsFourRows();

    void testReferenceInBusInterfaceMasterAddsFourRows();

    void testReferenceInBusInterfaceMirroredSlaveAddsFourRows();

    void testReferenceInRemapStateAddsFourRows();

    void testRerefencesInMultiplePlaces();

private:
    QSharedPointer<Parameter> createTestParameter(QString const& name, QString const& value, 
        QString const& bitWidthLeft, QString const& bitWidthRight, QString const& arrayLeft,
        QString const& arrayRight);

    QSharedPointer<ModuleParameter> createTestModuleParameter(QString const& name, QString const& value,
        QString const& bitWidthLeft, QString const& bitWidthRight, QString const& arrayLeft,
        QString const& arrayRight);

    QSharedPointer<Port> createTestPort(QString const& name, QString const& leftExpression,
        QString const& rightExpression, QString const& defaultValue, QString const& arrayLeft,
        QString const& arrayRight);

    QSharedPointer<Register> createTestRegister(QString const& name, QString const& offSet,
        QString const& dimension);

    QSharedPointer<AddressBlock> createTestAddressBlock(QString const& name, QString const& baseAddress,
        QString const& range, QString const& width);

    QSharedPointer<MemoryMap> createTestMemoryMap(QString const& name);

    QSharedPointer<MemoryRemap> createTestMemoryRemap(QString const& name);

    QSharedPointer<Field> createTestField(QString const& name, QString const& offset, QString const& width,
        QString const& presenceExpression);

    QSharedPointer<FieldReset> createTestFieldReset(QString const& resetValue, QString const& resetMask);

    QSharedPointer<AddressSpace> createTestAddressSpace(QString const& name);

    QSharedPointer<FileBuilder> createTestFileBuilder(QString const& fileType, QString const& replaceDefaultFlags);

    QSharedPointer<FileSet> createTestFileSet(QString const& name, QSharedPointer<FileBuilder> builder);

    QSharedPointer<BuildCommand> createTestBuildCommand(QString const& replaceDefaultFlags);

    QSharedPointer<File> createTestFile(QString const& name, QSharedPointer<BuildCommand> buildCommand);

    QSharedPointer<ExpressionFormatter> createTestExpressionFormatter(QSharedPointer<Component> component);   

    ComponentParameterReferenceTree* createTestTree(QSharedPointer<Component> component);

};

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::tst_ParameterReferenceTree()
//-----------------------------------------------------------------------------
tst_ParameterReferenceTree::tst_ParameterReferenceTree()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testEmptyComponentInConstructor()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testEmptyComponentInConstructor()
{
    QSharedPointer<Component> component(0);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QString());

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->childCount(), 0);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Component does not exist."));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testColumnCount()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testColumnCount()
{
    QSharedPointer<Component> component(new Component());

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));

    QCOMPARE(tree->columnCount(), 2);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testNoReferencesFoundAddsOneRow()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testNoReferencesFoundAddsOneRow()
{
    QSharedPointer <Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    // Parameter: No references.
    QSharedPointer<Parameter> paramRef = createTestParameter("ref1", "", "", "", "test", "test");

    // Parameter in view: No references.
    QSharedPointer<Parameter> viewParameter = createTestParameter("viewParamRef", "test", "", "", "", "");

    QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instantiation"));
    instantiation->getParameters()->append(viewParameter);

    QSharedPointer<View> viewRef (new View("viewRef"));
    viewRef->setComponentInstantiationRef(instantiation->name());

    // Port: No references.
    QSharedPointer<Port> portRef = createTestPort("portRef", "", "test", "test", "", "");

    // Memory Maps: No references.
    QSharedPointer<Register> registerRef = createTestRegister("registerRef", "test", "test");

    QSharedPointer<AddressBlock> addressRef = createTestAddressBlock("addressBlockRef", "test", "", "4");
    addressRef->getRegisterData()->append(registerRef);

    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef");
    memoryMapRef->getMemoryBlocks()->append(addressRef);

    // Address space: No references.
    QSharedPointer <AddressBlock> spaceBlock = createTestAddressBlock("spaceBlockRef", "test", "", "15");

    QSharedPointer<MemoryMapBase> spaceMemMap (new MemoryMapBase("spaceMemMap"));
    spaceMemMap->getMemoryBlocks()->append(spaceBlock);

    QSharedPointer<AddressSpace> addressSpaceRef (new AddressSpace("addressSpaceRef"));
    addressSpaceRef->setLocalMemoryMap(spaceMemMap);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(searched);
    component->getParameters()->append(paramRef);
    component->getViews()->append(viewRef);
    component->getComponentInstantiations()->append(instantiation);
    component->getPorts()->append(portRef);
    component->getMemoryMaps()->append(memoryMapRef);
    component->getAddressSpaces()->append(addressSpaceRef);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("No references found."));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(tree->topLevelItem(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInFileSetFileBuilderAddsFourRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInFileSetFileBuilderAddsFiveRows()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QSharedPointer<FileBuilder> referencingFileBuilder = createTestFileBuilder("testBuilder", "searched");
    QSharedPointer<FileSet> referencingFileSet = createTestFileSet("testSet", referencingFileBuilder);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(searched);
    component->getFileSets()->append(referencingFileSet);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("File sets"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), 
        referencingFileSet->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Default file build commands"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("File type: ") + referencingFileBuilder->getUserFileType());
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Replace default flags"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(referencingFileBuilder->getReplaceDefaultFlags()));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInFileBuildCommandAddsSixRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInFileBuildCommandAddsSixRows()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QSharedPointer<BuildCommand> referencingBuild = createTestBuildCommand("searched * Saitama");

    QSharedPointer<File> referencingFile = createTestFile("One", referencingBuild);

    QSharedPointer<FileSet> referencingFileSet = createTestFileSet("testSet", QSharedPointer<FileBuilder>());
    referencingFileSet->getFiles()->append(referencingFile);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(searched);
    component->getFileSets()->append(referencingFileSet);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);

    QTreeWidgetItem* topItem = tree->topLevelItem(0);
    QCOMPARE(topItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("File sets"));
    QCOMPARE(topItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(topItem->childCount(), 1);

    QTreeWidgetItem* fileSetItem = topItem->child(0);
    QCOMPARE(fileSetItem->text(ParameterReferenceTree::ITEM_NAME), referencingFileSet->name());
    QCOMPARE(fileSetItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(fileSetItem->childCount(), 1);

    QTreeWidgetItem* filesItem = fileSetItem->child(0);
    QCOMPARE(filesItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Files"));
    QCOMPARE(filesItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(filesItem->childCount(), 1);

    QTreeWidgetItem* singleFileItem = filesItem->child(0);
    QCOMPARE(singleFileItem->text(ParameterReferenceTree::ITEM_NAME), referencingFile->name());
    QCOMPARE(singleFileItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(singleFileItem->childCount(), 1);

    QTreeWidgetItem* buildCommandItem = singleFileItem->child(0);
    QCOMPARE(buildCommandItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Build command"));
    QCOMPARE(buildCommandItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(buildCommandItem->childCount(), 1);

    QTreeWidgetItem* flagsItem = buildCommandItem->child(0);
    QCOMPARE(flagsItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Replace default flags"));
    QCOMPARE(flagsItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(referencingBuild->getReplaceDefaultFlags()));
    QCOMPARE(flagsItem->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testParameterReferenceAddsThreeRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInParameterValueAddsThreeRows()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList <QSharedPointer<Parameter> > componentParameters;
    QSharedPointer<Parameter> referencingParameter = createTestParameter(
        "referencingParameter", "searched", "", "", "", "");
    componentParameters.append(referencingParameter);

    componentParameters.append(searched);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Parameters"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), 
        referencingParameter->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Value"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(referencingParameter->getValue()));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testMultipleReferencesInParameter()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testMultipleReferencesInOneParameter()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList <QSharedPointer<Parameter> > componentParameters;
    componentParameters.append(searched);
    // Four references in a parameter
    QSharedPointer<Parameter> referencer = createTestParameter("refParam", "searched", "searched", "searched",
        "searched", "searched");
    componentParameters.append(referencer);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Parameters"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), referencer->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 5);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Value"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(referencer->getValue()));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Bit Width Left"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(referencer->getVectorLeft()));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(2)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Bit Width Right"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(2)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(referencer->getVectorRight()));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(3)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Array Left"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(3)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(referencer->getAttribute("kactus2:arrayLeft")));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(4)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Array Right"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(4)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(referencer->getAttribute("kactus2:arrayRight")));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->childCount(), 0);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(1)->childCount(), 0);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(2)->childCount(), 0);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(3)->childCount(), 0);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(4)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testMultipleReferencesInMultipleParameters()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testMultipleReferencesInMultipleParameters()
{
    QSharedPointer <Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList <QSharedPointer<Parameter> > componentParameters;
    componentParameters.append(searched);

    // Two references, in array size and array offset.
    QSharedPointer<Parameter> firstRef = createTestParameter("ref1", "", "", "", "searched", "searched");
    componentParameters.append(firstRef);
    // Three references, in bit width, array size and array offset.
    QSharedPointer<Parameter> secondRef = createTestParameter("ref2", "", "searched", "", "searched", "searched");
    componentParameters.append(secondRef);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Parameters"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->childCount(), 2);
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), firstRef->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(tree->topLevelItem(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME), secondRef->name());
    QCOMPARE(tree->topLevelItem(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 2);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Array Left"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(firstRef->getAttribute("kactus2:arrayLeft")));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Array Right"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(firstRef->getAttribute("kactus2:arrayRight")));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->childCount(), 0);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(1)->childCount(), 0);

    QCOMPARE(tree->topLevelItem(0)->child(1)->childCount(), 3);
    QCOMPARE(tree->topLevelItem(0)->child(1)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Bit Width Left"));
    QCOMPARE(tree->topLevelItem(0)->child(1)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(secondRef->getVectorLeft()));

    QCOMPARE(tree->topLevelItem(0)->child(1)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Array Left"));
    QCOMPARE(tree->topLevelItem(0)->child(1)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(secondRef->getAttribute("kactus2:arrayLeft")));

    QCOMPARE(tree->topLevelItem(0)->child(1)->child(2)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Array Right"));
    QCOMPARE(tree->topLevelItem(0)->child(1)->child(2)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(secondRef->getAttribute("kactus2:arrayRight")));

    QCOMPARE(tree->topLevelItem(0)->child(1)->child(0)->childCount(), 0);
    QCOMPARE(tree->topLevelItem(0)->child(1)->child(1)->childCount(), 0);
    QCOMPARE(tree->topLevelItem(0)->child(1)->child(2)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceAsAnExpressionInParameter()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceAsAnExpressionInParameter()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList <QSharedPointer<Parameter> > componentParameters;
    componentParameters.append(searched);

    // One reference, in value.
    QSharedPointer<Parameter> firstRef = createTestParameter("ref", "search + searched", "", "", "", "");
    componentParameters.append(firstRef);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Parameters"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), firstRef->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Value"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(firstRef->getValue()));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testMultipleReferencesInSameExpression()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testMultipleReferencesInSameExpression()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList <QSharedPointer<Parameter> > componentParameters;
    componentParameters.append(searched);

    // Three references, in value.
    QSharedPointer<Parameter> firstRef = createTestParameter("ref", "searched + searched * searched", "", "", "",
        "");
    componentParameters.append(firstRef);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Parameters"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), firstRef->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Value"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(firstRef->getValue()));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInViewParameterValueAddsFiveRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInViewParameterValueAddsFiveRows()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    // One reference, in array offset.
    QSharedPointer<Parameter> paramRef = createTestParameter("paramRef", "searched", "", "", "", "");

    QSharedPointer<ComponentInstantiation> componentInstantiation (new ComponentInstantiation("componentI"));
    componentInstantiation->getParameters()->append(paramRef);

    QSharedPointer<View> viewRef(new View("viewRef"));
    viewRef->setComponentInstantiationRef(componentInstantiation->name());

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(searched);
    component->getViews()->append(viewRef);
    component->getComponentInstantiations()->append(componentInstantiation);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));
        
    QCOMPARE(tree->topLevelItemCount(), 1);
    QTreeWidgetItem* instantiationsItem = tree->topLevelItem(0);
    QCOMPARE(instantiationsItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Instantiations"));
    QCOMPARE(instantiationsItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());   
    QCOMPARE(instantiationsItem->background(ParameterReferenceTree::ITEM_EXPRESSION).color(),
        KactusColors::STRONG_FIELD);

    QTreeWidgetItem* componentInstantiationsItem = instantiationsItem->child(0);
    QCOMPARE(componentInstantiationsItem->text(ParameterReferenceTree::ITEM_NAME), 
        QStringLiteral("Component instantiations"));
    QCOMPARE(componentInstantiationsItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(componentInstantiationsItem->childCount(), 1);
    QTreeWidgetItem* singleComponentInstantiationItem = componentInstantiationsItem->child(0);
    QCOMPARE(singleComponentInstantiationItem->text(ParameterReferenceTree::ITEM_NAME),
        componentInstantiation->name());
    QCOMPARE(singleComponentInstantiationItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(singleComponentInstantiationItem->childCount(), 1);
    QTreeWidgetItem* viewParametersItem = singleComponentInstantiationItem->child(0);
    QCOMPARE(viewParametersItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Parameters"));
    QCOMPARE(viewParametersItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(viewParametersItem->background(ParameterReferenceTree::ITEM_EXPRESSION).color(),
        KactusColors::STRONG_FIELD);

    QCOMPARE(singleComponentInstantiationItem->childCount(), 1);
    QTreeWidgetItem* parameterItem = viewParametersItem->child(0);
    QCOMPARE(parameterItem->text(ParameterReferenceTree::ITEM_NAME), paramRef->name());
    QCOMPARE(parameterItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(parameterItem->childCount(), 1);
    QTreeWidgetItem* valueItem = parameterItem->child(0);
    QCOMPARE(valueItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Value"));
    QCOMPARE(valueItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(paramRef->getValue()));

    QCOMPARE(valueItem->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInViewModuleParameterValueAddsFiveRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInViewModuleParameterValueAddsFiveRows()
{
    QSharedPointer<Parameter> searched(new Parameter());
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(searched);  

    QSharedPointer<ModuleParameter> moduleParameterRef(new ModuleParameter());
    moduleParameterRef->setName("paramRef");
    moduleParameterRef->setValue(searched->getValueId());

    QSharedPointer<ComponentInstantiation> instantiation (new ComponentInstantiation("compInstantiation"));
    instantiation->getModuleParameters()->append(moduleParameterRef);

    QSharedPointer<View> viewRef(new View("viewRef"));
    viewRef->setComponentInstantiationRef(instantiation->name());

    component->getViews()->append(viewRef);
    component->getComponentInstantiations()->append(instantiation);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QTreeWidgetItem* instantiationsItem = tree->topLevelItem(0);

    QCOMPARE(instantiationsItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Instantiations"));
    QCOMPARE(instantiationsItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(instantiationsItem->background(ParameterReferenceTree::ITEM_EXPRESSION).color(), 
        KactusColors::STRONG_FIELD);

    QTreeWidgetItem* componentInstantiationsItem = instantiationsItem->child(0);
    QCOMPARE(componentInstantiationsItem->text(ParameterReferenceTree::ITEM_NAME), 
        QStringLiteral("Component instantiations"));
    QCOMPARE(componentInstantiationsItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
  
    QCOMPARE(componentInstantiationsItem->childCount(), 1);
    QTreeWidgetItem* singleInstantiationItem = componentInstantiationsItem->child(0);
    QCOMPARE(singleInstantiationItem->text(ParameterReferenceTree::ITEM_NAME), instantiation->name());
    QCOMPARE(singleInstantiationItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(singleInstantiationItem->childCount(), 1);
    QTreeWidgetItem* viewModuleParametersItem = singleInstantiationItem->child(0);
    QCOMPARE(viewModuleParametersItem->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Module Parameters"));
    QCOMPARE(viewModuleParametersItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(viewModuleParametersItem->background(ParameterReferenceTree::ITEM_EXPRESSION).color(), 
        KactusColors::STRONG_FIELD);

    QCOMPARE(singleInstantiationItem->childCount(), 1);
    QTreeWidgetItem* parameterItem = viewModuleParametersItem->child(0);
    QCOMPARE(parameterItem->text(ParameterReferenceTree::ITEM_NAME), moduleParameterRef->name());
    QCOMPARE(parameterItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(parameterItem->childCount(), 1);
    QTreeWidgetItem* valueItem = parameterItem->child(0);
    QCOMPARE(valueItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Value"));
    QCOMPARE(valueItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(moduleParameterRef->getValue()));

    QCOMPARE(valueItem->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInComponentInstantiationFileBuilderAddsFiveRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInComponentInstantiationFileBuilderAddsFiveRows()
{
    QSharedPointer<Parameter> searched(new Parameter());
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(searched);  

    QSharedPointer<FileBuilder> instantiationFileBuilder = createTestFileBuilder("systemCSource", "searched-2");

    QSharedPointer<ComponentInstantiation> instantiation (new ComponentInstantiation("compInstantiation"));
    instantiation->getDefaultFileBuilders()->append(instantiationFileBuilder);

    QSharedPointer<View> viewRef(new View("viewRef"));
    viewRef->setComponentInstantiationRef(instantiation->name());

    component->getViews()->append(viewRef);
    component->getComponentInstantiations()->append(instantiation);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QTreeWidgetItem* instantiationsItem = tree->topLevelItem(0);
    QCOMPARE(instantiationsItem->text(ParameterReferenceTree::ITEM_NAME), 
        QStringLiteral("Instantiations"));
    QCOMPARE(instantiationsItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(instantiationsItem->background(ParameterReferenceTree::ITEM_EXPRESSION).color(),
        KactusColors::STRONG_FIELD);

    QTreeWidgetItem* componentInstantiationsItem = instantiationsItem->child(0);
    QCOMPARE(componentInstantiationsItem->text(ParameterReferenceTree::ITEM_NAME), 
        QStringLiteral("Component instantiations"));
    QCOMPARE(componentInstantiationsItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(componentInstantiationsItem->childCount(), 1);
    QTreeWidgetItem* singleInstantiationItem = componentInstantiationsItem->child(0);
    QCOMPARE(singleInstantiationItem->text(ParameterReferenceTree::ITEM_NAME), instantiation->name());
    QCOMPARE(singleInstantiationItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(singleInstantiationItem->childCount(), 1);
    QTreeWidgetItem* defaultFileBuildersItem = singleInstantiationItem->child(0);
    QCOMPARE(defaultFileBuildersItem->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Default file build commands"));
    QCOMPARE(defaultFileBuildersItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(defaultFileBuildersItem->childCount(), 1);
    QTreeWidgetItem* fileBuilderItem = defaultFileBuildersItem->child(0);
    QCOMPARE(fileBuilderItem->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("File type: ") + instantiationFileBuilder->getFileType());
    QCOMPARE(fileBuilderItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(fileBuilderItem->childCount(), 1);
    QTreeWidgetItem* replaceFlagsItem = fileBuilderItem->child(0);
    QCOMPARE(replaceFlagsItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Replace default flags"));
    QCOMPARE(replaceFlagsItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(instantiationFileBuilder->getReplaceDefaultFlags()));

    QCOMPARE(replaceFlagsItem->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInPortRightboundAddsThreeRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInPortRightboundAddsThreeRows()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList < QSharedPointer<Parameter> > componentParameters;
    componentParameters.append(searched);

    QSharedPointer<Port> portRef = createTestPort("portRef", "", "searched", "", "", "");

    QSharedPointer<Component> component(new Component);
    component->getPorts()->append(portRef);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);
  
    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Ports"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), portRef->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Right Bound"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(portRef->getRightBound()));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInPortArraysAddsThreeRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInPortArraysAddsThreeRows()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList<QSharedPointer<Parameter> > componentParameters;
    componentParameters.append(searched);

    QSharedPointer<Port> portRef = createTestPort("portRef", "", "", "", "searched", "searched");
    
    QSharedPointer<Component> component(new Component);
    component->getPorts()->append(portRef);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Ports"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), portRef->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 2);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Array Left"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QStringLiteral("searchedParameter"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->childCount(), 0);

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Array Right"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QStringLiteral("searchedParameter"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(1)->childCount(), 0);

}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInAddressBlockBaseAddressAddsSevenRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInAddressBlockBaseAddressAddsSevenRows()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QSharedPointer<AddressBlock> addressRef = createTestAddressBlock("addressBlockRef", "searched", "2", "32");

    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef");
    memoryMapRef->getMemoryBlocks()->append(addressRef);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(searched);
    component->getMemoryMaps()->append(memoryMapRef);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Memory maps"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Memory remaps"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Default"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Address blocks"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), addressRef->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), QStringLiteral("Base Address"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), expressionFormatter->formatReferringExpression(
        addressRef->getBaseAddress()));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInAddressBlockNoReferenceInRegister()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInAddressBlockNoReferenceInRegister()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList < QSharedPointer <Parameter> > componentParameters;
    componentParameters.append(searched);

    QSharedPointer<Register> registerRef = createTestRegister("registerRef", "test", "testing");

    QSharedPointer<AddressBlock> addressRef = createTestAddressBlock("addressBlockRef", "searched", "2", "32");
    addressRef->getRegisterData()->append(registerRef);

    QSharedPointer<MemoryRemap> memoryRemapRef = createTestMemoryRemap("memoryRemapRef");
    memoryRemapRef->getMemoryBlocks()->append(addressRef);

    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef");
    memoryMapRef->getMemoryRemaps()->append(memoryRemapRef);

    QSharedPointer<Component> component(new Component);
    component->getMemoryMaps()->append(memoryMapRef);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Memory maps"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Memory remaps"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("memoryRemapRef"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Address blocks"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), addressRef->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), QStringLiteral("Base Address"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), expressionFormatter->formatReferringExpression(
        addressRef->getBaseAddress()));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInRegisterDimensionAddsEightRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInRegisterDimensionAddsNineRows()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList < QSharedPointer <Parameter> > componentParameters;
    componentParameters.append(searched);

    QSharedPointer<Register> registerRef = createTestRegister("registerRef", "test", "searched");

    QSharedPointer<AddressBlock> addressRef = createTestAddressBlock("addressBlockRef", "test", "", "");
    addressRef->getRegisterData()->append(registerRef);

    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef");
    memoryMapRef->getMemoryBlocks()->append(addressRef);

    QSharedPointer<Component> component(new Component);
    component->getMemoryMaps()->append(memoryMapRef);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QTreeWidgetItem* memoryMapsItem = tree->topLevelItem(0);
    QCOMPARE(memoryMapsItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Memory maps"));
    QCOMPARE(memoryMapsItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(memoryMapsItem->childCount(), 1);
    QTreeWidgetItem* memoryMapItem = memoryMapsItem->child(0);
    QCOMPARE(memoryMapItem->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->name());
    QCOMPARE(memoryMapItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(memoryMapItem->childCount(), 1);
    QTreeWidgetItem* remapsItem = memoryMapItem->child(0);
    QCOMPARE(remapsItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Memory remaps"));
    QCOMPARE(remapsItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(remapsItem->childCount(), 1);
    QTreeWidgetItem* defaultRemap = remapsItem->child(0);
    QCOMPARE(defaultRemap->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Default"));
    QCOMPARE(defaultRemap->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(defaultRemap->childCount(), 1);
    QTreeWidgetItem* addressBlocksItem = defaultRemap->child(0);
    QCOMPARE(addressBlocksItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Address blocks"));
    QCOMPARE(addressBlocksItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(addressBlocksItem->childCount(), 1);
    QTreeWidgetItem* addressBlock = addressBlocksItem->child(0);
    QCOMPARE(addressBlock->text(ParameterReferenceTree::ITEM_NAME), addressRef->name());
    QCOMPARE(addressBlock->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(addressBlock->childCount(), 1);
    QTreeWidgetItem* registerListItem = addressBlock->child(0);
    QCOMPARE(registerListItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Registers"));
    QCOMPARE(registerListItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QTreeWidgetItem* registerItem = registerListItem->child(0);
    QCOMPARE(registerItem->text(ParameterReferenceTree::ITEM_NAME), registerRef->name());
    QCOMPARE(registerItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(registerItem->childCount(), 1);
    QTreeWidgetItem* dimensionItem = registerItem->child(0);
    QCOMPARE(dimensionItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Dimension"));
    QCOMPARE(dimensionItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(registerRef->getDimension()));

    QCOMPARE(dimensionItem->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInRegisterIsPresentAddsEightRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInRegisterIsPresentAddsNineRows()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList < QSharedPointer <Parameter> > componentParameters;
    componentParameters.append(searched);

    QSharedPointer<Register> registerRef = createTestRegister("registerRef", "test", "1");
    registerRef->setIsPresent("searched");

    QSharedPointer<AddressBlock> addressRef = createTestAddressBlock("addressBlockRef", "test", "", "");
    addressRef->getRegisterData()->append(registerRef);

    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef");
    memoryMapRef->getMemoryBlocks()->append(addressRef);

    QSharedPointer<Component> component(new Component);
    component->getMemoryMaps()->append(memoryMapRef);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QTreeWidgetItem* memoryMapsItem = tree->topLevelItem(0);
    QCOMPARE(memoryMapsItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Memory maps"));
    QCOMPARE(memoryMapsItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(memoryMapsItem->childCount(), 1);
    QTreeWidgetItem* memoryMapItem = memoryMapsItem->child(0);
    QCOMPARE(memoryMapItem->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->name());
    QCOMPARE(memoryMapItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(memoryMapItem->childCount(), 1);
    QTreeWidgetItem* remapsItem = memoryMapItem->child(0);
    QCOMPARE(remapsItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Memory remaps"));
    QCOMPARE(remapsItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(remapsItem->childCount(), 1);
    QTreeWidgetItem* defaultRemap = remapsItem->child(0);
    QCOMPARE(defaultRemap->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Default"));
    QCOMPARE(defaultRemap->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(defaultRemap->childCount(), 1);
    QTreeWidgetItem* addressBlocksItem = defaultRemap->child(0);
    QCOMPARE(addressBlocksItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Address blocks"));
    QCOMPARE(addressBlocksItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(addressBlocksItem->childCount(), 1);
    QTreeWidgetItem* addressBlock = addressBlocksItem->child(0);
    QCOMPARE(addressBlock->text(ParameterReferenceTree::ITEM_NAME), addressRef->name());
    QCOMPARE(addressBlock->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(addressBlock->childCount(), 1);
    QTreeWidgetItem* registerListItem = addressBlock->child(0);
    QCOMPARE(registerListItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Registers"));
    QCOMPARE(registerListItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QTreeWidgetItem* registerItem = registerListItem->child(0);
    QCOMPARE(registerItem->text(ParameterReferenceTree::ITEM_NAME), registerRef->name());
    QCOMPARE(registerItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(registerItem->childCount(), 1);
    QTreeWidgetItem* isPresentItem = registerItem->child(0);
    QCOMPARE(isPresentItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Is present"));
    QCOMPARE(isPresentItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(registerRef->getIsPresent()));

    QCOMPARE(isPresentItem->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInMultipleRegisters()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInMultipleRegistersInTwoAddressBlocks()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList<QSharedPointer<Parameter> > componentParameters;
    componentParameters.append(searched);

    QSharedPointer <Register> registerOne_1 = createTestRegister("registerOne_1", "", "searched");
    QSharedPointer <Register> registerTwo_1 = createTestRegister("registerTwo_1", "searched", "");

    QSharedPointer <AddressBlock> addressBlockOne = createTestAddressBlock("addressBlockOne", "", "", "");
    addressBlockOne->getRegisterData()->append(registerOne_1);
    addressBlockOne->getRegisterData()->append(registerTwo_1);

    QSharedPointer <Register> registerOne_2 = createTestRegister("registerOne_2", "searched", "searched");

    QSharedPointer <AddressBlock> addressBlockTwo = createTestAddressBlock("addressBlockTwo", "", "", "");
    addressBlockTwo->getRegisterData()->append(registerOne_2);

    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef");
    memoryMapRef->getMemoryBlocks()->append(addressBlockOne);
    memoryMapRef->getMemoryBlocks()->append(addressBlockTwo);

    QSharedPointer<Component> component(new Component);
    component->getMemoryMaps()->append(memoryMapRef);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);

    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Memory maps"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);

    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 1);

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Memory remaps"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->childCount(), 1);

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Default"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString());
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Address blocks"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 2);

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), addressBlockOne->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QTreeWidgetItem* registerListItem =
        tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0);
    QCOMPARE(registerListItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Registers"));
    QCOMPARE(registerListItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(registerListItem->childCount(), 2);

    QTreeWidgetItem* registerItemOne = registerListItem->child(0);
    QCOMPARE(registerItemOne->text(ParameterReferenceTree::ITEM_NAME), registerOne_1->name());
    QCOMPARE(registerItemOne->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(registerItemOne->childCount(), 1);

    QTreeWidgetItem* registerOneDimensionItem = registerItemOne->child(0);
    QCOMPARE(registerOneDimensionItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Dimension"));
    QCOMPARE(registerOneDimensionItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(registerOne_1->getDimension()));
    QCOMPARE(registerOneDimensionItem->childCount(), 0);

    QTreeWidgetItem* registerItemTwo = registerListItem->child(1);
    QCOMPARE(registerItemTwo->text(ParameterReferenceTree::ITEM_NAME), registerTwo_1->name());
    QCOMPARE(registerItemTwo->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(registerItemTwo->childCount(), 1);

    QTreeWidgetItem* offsetItem = registerItemTwo->child(0);
    QCOMPARE(offsetItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Offset"));
    QCOMPARE(offsetItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(registerTwo_1->getAddressOffset()));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(1)->text(
        ParameterReferenceTree::ITEM_NAME), addressBlockTwo->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(1)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(1)->childCount(), 1);

    registerListItem = tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(0);
    QCOMPARE(registerListItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Registers"));
    QCOMPARE(registerListItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(registerListItem->childCount(), 1);

    registerItemOne = registerListItem->child(0);
    QCOMPARE(registerItemOne->text(ParameterReferenceTree::ITEM_NAME), registerOne_2->name());
    QCOMPARE(registerItemOne->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(registerItemOne->childCount(), 2);

    offsetItem = registerItemOne->child(0);
    QCOMPARE(offsetItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Offset"));
    QCOMPARE(offsetItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(registerOne_2->getAddressOffset()));
    QCOMPARE(offsetItem->childCount(), 0);

    QTreeWidgetItem* dimensionItem = registerItemOne->child(1);
    QCOMPARE(dimensionItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Dimension"));
    QCOMPARE(dimensionItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(registerOne_2->getDimension()));
    QCOMPARE(dimensionItem->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testUnreferencingRegister()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testRegisterwithNoReferences()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList <QSharedPointer<Parameter> > componentParameters;
    componentParameters.append(searched);

    QSharedPointer <Register> noRefRegister = createTestRegister("noRefRegister", "test", "test");
    QSharedPointer <Register> refRegister = createTestRegister("refRegister", "test", "searched");

    QSharedPointer <AddressBlock> addressBlockRef = createTestAddressBlock("addressBlockRef", "", "", "");
    addressBlockRef->getRegisterData()->append(refRegister);
    addressBlockRef->getRegisterData()->append(noRefRegister);

    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef");
    memoryMapRef->getMemoryBlocks()->append(addressBlockRef);

    QSharedPointer<Component> component(new Component);
    component->getMemoryMaps()->append(memoryMapRef);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));
    
    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Memory maps"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Memory remaps"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Default"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Address blocks"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), addressBlockRef->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);

    QTreeWidgetItem* registerListItem =
        tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0);
    QCOMPARE(registerListItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Registers"));
    QCOMPARE(registerListItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(registerListItem->childCount(), 1);

    QTreeWidgetItem* registerItem = registerListItem->child(0);
    QCOMPARE(registerItem->text(ParameterReferenceTree::ITEM_NAME), QString(refRegister->name()));
    QCOMPARE(registerItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(registerItem->childCount(), 1);

    QTreeWidgetItem* dimensionItem = registerItem->child(0);
    QCOMPARE(dimensionItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Dimension"));
    QCOMPARE(dimensionItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(refRegister->getDimension()));
    QCOMPARE(dimensionItem->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInAddressSpaceAddressBlockAddsSixRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInAddressSpaceAddressBlockAddsSixRows()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList <QSharedPointer<Parameter> > componentParameters;
    componentParameters.append(searched);

    QSharedPointer <AddressBlock> addressBlockRef = createTestAddressBlock("addressBlockRef", "searched", "", "");

    QSharedPointer<MemoryMapBase> localMemoryMap (new MemoryMapBase("localMemMap"));
    localMemoryMap->getMemoryBlocks()->append(addressBlockRef);

    QSharedPointer <AddressSpace> addressSpaceRef = createTestAddressSpace("addressSpaceRef");
    addressSpaceRef->setLocalMemoryMap(localMemoryMap);

    QSharedPointer<Component> component(new Component);
    component->getAddressSpaces()->append(addressSpaceRef);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Address Spaces"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), addressSpaceRef->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        localMemoryMap->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Address blocks"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        addressBlockRef->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), QStringLiteral("Base Address"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), expressionFormatter->formatReferringExpression(
        addressBlockRef->getBaseAddress()));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInAddressSpaceSegmentAddsSixRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInAddressSpaceSegmentAddsSixRows()
{
    QSharedPointer <AddressSpace> addressSpace = createTestAddressSpace("addressSpace");
    addressSpace->setWidth("searched");
    addressSpace->setRange("searched+1");

    QSharedPointer<Component> component(new Component);
    component->getAddressSpaces()->append(addressSpace);

    QSharedPointer<Segment> testSegment(new Segment());
    testSegment->setName("testSegment");
    testSegment->setOffset("searched");
    testSegment->setRange("searched+2");

    addressSpace->getSegments()->append(testSegment);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QTreeWidgetItem* addressSpacesItem = tree->topLevelItem(0);
    QCOMPARE(addressSpacesItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Address Spaces"));
    QCOMPARE(addressSpacesItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);
    QTreeWidgetItem* testSpaceItem = tree->topLevelItem(0)->child(0);
    QCOMPARE(testSpaceItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("addressSpace"));
    QCOMPARE(testSpaceItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(testSpaceItem->childCount(), 3);

    QTreeWidgetItem* widthItem = testSpaceItem->child(0);
    QCOMPARE(widthItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Width"));
    QCOMPARE(widthItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QStringLiteral("searched"));

    QTreeWidgetItem* rangeItem = testSpaceItem->child(1);
    QCOMPARE(rangeItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Range"));
    QCOMPARE(rangeItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QStringLiteral("searched+1"));

    QTreeWidgetItem* segmentsItem = testSpaceItem->child(2);
    QCOMPARE(segmentsItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Segments"));
    QCOMPARE(segmentsItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
   
    QCOMPARE(segmentsItem->childCount(), 1);
    QTreeWidgetItem* singleSegmentItem = segmentsItem->child(0);
    QCOMPARE(singleSegmentItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("testSegment"));
    QCOMPARE(singleSegmentItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(singleSegmentItem->childCount(), 2);
    QTreeWidgetItem* segmentOffsetItem = singleSegmentItem->child(0);
    QCOMPARE(segmentOffsetItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Offset"));
    QCOMPARE(segmentOffsetItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QStringLiteral("searched"));
    QCOMPARE(segmentOffsetItem->childCount(), 0);

    QTreeWidgetItem* segmentRangeItem = singleSegmentItem->child(1);
    QCOMPARE(segmentRangeItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Range"));
    QCOMPARE(segmentRangeItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QStringLiteral("searched+2"));
    QCOMPARE(segmentRangeItem->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInRegisterFieldAddsTenRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInRegisterFieldAddsElevenRows()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList <QSharedPointer<Parameter> > componentParameters;
    componentParameters.append(searched);

    QSharedPointer<WriteValueConstraint> refWriteConstraint (new WriteValueConstraint());
    refWriteConstraint->setType(WriteValueConstraint::MIN_MAX);
    refWriteConstraint->setMinimum("searched - 1");
    refWriteConstraint->setMaximum("searched + 1");

    QSharedPointer<Field> refField = createTestField("fieldRef", "searched", "nothing", "searched");
    refField->setWriteConstraint(refWriteConstraint);

    QSharedPointer<FieldReset> testReset = createTestFieldReset("searched", "searched");
    refField->getResets()->append(testReset);

    QSharedPointer <Register> refRegister = createTestRegister("refRegister", "test", "test");
    refRegister->getFields()->append(refField);

    QSharedPointer <AddressBlock> addressBlockRef = createTestAddressBlock("addressBlockRef", "", "", "");
    addressBlockRef->getRegisterData()->append(refRegister);

    QSharedPointer<MemoryRemap> memoryRemapRef = createTestMemoryRemap("memoryRemapRef");
    memoryRemapRef->getMemoryBlocks()->append(addressBlockRef);

    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef");
    memoryMapRef->getMemoryRemaps()->append(memoryRemapRef);

    QSharedPointer<Component> component(new Component);
    component->getMemoryMaps()->append(memoryMapRef);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Memory maps"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Memory remaps"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("memoryRemapRef"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Address blocks"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString());
    
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), addressBlockRef->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);

    QTreeWidgetItem* registerListItem =
        tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0);
    QCOMPARE(registerListItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Registers"));
    QCOMPARE(registerListItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(registerListItem->childCount(), 1);

    QTreeWidgetItem* registerItem = registerListItem->child(0);
    QCOMPARE(registerItem->text(ParameterReferenceTree::ITEM_NAME), refRegister->name());
    QCOMPARE(registerItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(registerItem->childCount(), 1);

    QTreeWidgetItem* fieldListItem = registerItem->child(0);
    QCOMPARE(fieldListItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Fields"));
    QCOMPARE(fieldListItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(fieldListItem->childCount(), 1);

    QTreeWidgetItem* fieldItem = fieldListItem->child(0);
    QCOMPARE(fieldItem->text(ParameterReferenceTree::ITEM_NAME), refField->name());
    QCOMPARE(fieldItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(fieldItem->childCount(), 5);

    QTreeWidgetItem* offsetItem = fieldItem->child(0);
    QCOMPARE(offsetItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Offset"));
    QCOMPARE(offsetItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(refField->getBitOffset()));
    QCOMPARE(offsetItem->childCount(), 0);

    QTreeWidgetItem* isPresentItem = fieldItem->child(1);
    QCOMPARE(isPresentItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Is Present"));
    QCOMPARE(isPresentItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(refField->getIsPresent()));
    QCOMPARE(isPresentItem->childCount(), 0);

    QTreeWidgetItem* resetsItem = fieldItem->child(2);
    QCOMPARE(resetsItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Resets"));
    QCOMPARE(resetsItem->childCount(), 1);

    QTreeWidgetItem* hardResetItem = resetsItem->child(0);
    QCOMPARE(hardResetItem->text(ParameterReferenceTree::ITEM_NAME), QLatin1String("HARD"));
    QCOMPARE(hardResetItem->childCount(), 2);

    QTreeWidgetItem* hardResetValue = hardResetItem->child(0);
    QCOMPARE(hardResetValue->text(ParameterReferenceTree::ITEM_NAME), QLatin1String("Reset value"));
    QCOMPARE(hardResetValue->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(testReset->getResetValue()));
    QCOMPARE(hardResetValue->childCount(), 0);

    QTreeWidgetItem* hardResetMask = hardResetItem->child(1);
    QCOMPARE(hardResetMask->text(ParameterReferenceTree::ITEM_NAME), QLatin1String("Reset mask"));
    QCOMPARE(hardResetMask->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(testReset->getResetMask()));
    QCOMPARE(hardResetMask->childCount(), 0);

    QTreeWidgetItem* constraintMinItem = fieldItem->child(3);
    QCOMPARE(constraintMinItem->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Write constraint minimum"));
    QCOMPARE(constraintMinItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(refField->getWriteConstraint()->getMinimum()));
    QCOMPARE(constraintMinItem->childCount(), 0);

    QTreeWidgetItem* constraintMaxItem = fieldItem->child(4);
    QCOMPARE(constraintMaxItem->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Write constraint maximum"));
    QCOMPARE(constraintMaxItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(refField->getWriteConstraint()->getMaximum()));
    QCOMPARE(constraintMaxItem->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInBusInterfaceParameterAddsFourRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInBusInterfaceParameterAddsFourRows()
{
    QSharedPointer<Parameter> searched = createTestParameter("searchedParameter", "", "", "", "", "");
    searched->setValueId("searched");

    QList <QSharedPointer<Parameter> > componentParameters;
    componentParameters.append(searched);

    QSharedPointer<Parameter> busIFParameter = createTestParameter("busIFParameterRef", "", "", "", "searched", "");

    QSharedPointer<BusInterface> refBusInterface(new BusInterface);
    refBusInterface->setName("refBusInterface");
    refBusInterface->getParameters()->append(busIFParameter);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(componentParameters);
    component->getBusInterfaces()->append(refBusInterface);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Bus Interfaces"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QTreeWidgetItem* busInterfaceItem = tree->topLevelItem(0)->child(0);
    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);
    QCOMPARE(busInterfaceItem->text(ParameterReferenceTree::ITEM_NAME), refBusInterface->name());
    QCOMPARE(busInterfaceItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QTreeWidgetItem* parametersItem = busInterfaceItem->child(0);
    QCOMPARE(busInterfaceItem->childCount(), 1);
    QCOMPARE(parametersItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Parameters"));
    QCOMPARE(parametersItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QTreeWidgetItem* busInterfaceParameterItem = parametersItem->child(0);
    QCOMPARE(busInterfaceParameterItem->text(ParameterReferenceTree::ITEM_NAME), busIFParameter->name());
    QCOMPARE(busInterfaceParameterItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QTreeWidgetItem* arrayItem = busInterfaceParameterItem->child(0);
    QCOMPARE(busInterfaceParameterItem->childCount(), 1);
    QCOMPARE(arrayItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Array Left"));
    QCOMPARE(arrayItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(busIFParameter->getAttribute("kactus2:arrayLeft")));

    QCOMPARE(arrayItem->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInBusInterfaceMasterAddsFourRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInBusInterfaceMasterAddsFourRows()
{
    QSharedPointer<Parameter> searched = createTestParameter("searchedParameter", "4", "", "", "", "");
    searched->setValueId("searched");
    QList<QSharedPointer<Parameter> > componentParameters;
    componentParameters.append(searched);

    QSharedPointer<MasterInterface> masterInterface (new MasterInterface);
    masterInterface->setBaseAddress(searched->getValueId());

    QSharedPointer<BusInterface> masterBus(new BusInterface);
    masterBus->setName("masterBus");
    masterBus->setInterfaceMode(General::MASTER);
    masterBus->setMaster(masterInterface);

    QSharedPointer<BusInterface> mirroredMaster(new BusInterface);
    mirroredMaster->setName("mirroredMaster");
    mirroredMaster->setInterfaceMode(General::MIRROREDMASTER);
    mirroredMaster->setMaster(masterInterface);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(componentParameters);
    component->getBusInterfaces()->append(masterBus);
    component->getBusInterfaces()->append(mirroredMaster);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Bus Interfaces"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->childCount(), 2);
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), masterBus->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Master Interface"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Base Address"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(masterInterface->getBaseAddress()));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 0);

    QCOMPARE(tree->topLevelItem(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME), mirroredMaster->name());
    QCOMPARE(tree->topLevelItem(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(1)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(1)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Mirrored Master Interface"));
    QCOMPARE(tree->topLevelItem(0)->child(1)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(1)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(1)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Base Address"));
    QCOMPARE(tree->topLevelItem(0)->child(1)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(masterInterface->getBaseAddress()));

    QCOMPARE(tree->topLevelItem(0)->child(1)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInBusInterfaceMirroredSlaveAddsFourRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInBusInterfaceMirroredSlaveAddsFourRows()
{
    QSharedPointer<Parameter> searched = createTestParameter("searchedParameter", "", "", "", "", "");
    searched->setValueId("searched");

    QList <QSharedPointer<Parameter> > componentParameters;
    componentParameters.append(searched);
    
    QSharedPointer<BusInterface> refBusInterface(new BusInterface);
    refBusInterface->setName("refBusInterface");

    refBusInterface->setInterfaceMode(General::MIRROREDSLAVE);

    QSharedPointer<MirroredSlaveInterface> mirrorSlave(new MirroredSlaveInterface);
    mirrorSlave->setRange(searched->getValueId());
    refBusInterface->setMirroredSlave(mirrorSlave);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(componentParameters);
    component->getBusInterfaces()->append(refBusInterface);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Bus Interfaces"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), refBusInterface->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Mirrored Slave Interface"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Range"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(mirrorSlave->getRange()));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInRemapStateAddsFourRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInRemapStateAddsFourRows()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QSharedPointer<RemapPort> refRemapPort(new RemapPort);
    refRemapPort->setPortNameRef("remapPort");
    refRemapPort->setPortIndex("1");
    refRemapPort->setValue("searched");

    QSharedPointer<RemapPort> secondRemapPort(new RemapPort);
    secondRemapPort->setPortNameRef("remapPort");
    secondRemapPort->setPortIndex("3");
    secondRemapPort->setValue("searched + 1");

    QSharedPointer<RemapPort> thirdRemapPort(new RemapPort);
    thirdRemapPort->setPortNameRef("otherRemapPort");
    thirdRemapPort->setValue("searched * 2");

    QSharedPointer<RemapState> refRemapState(new RemapState());
    refRemapState->setName("refRemapState");
    refRemapState->getRemapPorts()->append(refRemapPort);
    refRemapState->getRemapPorts()->append(secondRemapPort);
    refRemapState->getRemapPorts()->append(thirdRemapPort);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(searched);
    component->getRemapStates()->append(refRemapState);

    QSharedPointer<ExpressionFormatter> formatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Remap States"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);

    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("refRemapState"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 1);

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Remap Ports"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->childCount(), 3);

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("remapPort[1]"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QStringLiteral("searchedParameter"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 0);

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("remapPort[3]"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QStringLiteral("searchedParameter + 1"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(1)->childCount(), 0);

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(2)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("otherRemapPort"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(2)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QStringLiteral("searchedParameter * 2"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(2)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferencesInMultiplePlaces()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testRerefencesInMultiplePlaces()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QSharedPointer<Parameter> paramRef = createTestParameter("ref1", "", "", "", "searched", "searched");

    // View: One reference: in parameter.
    QSharedPointer<Parameter> viewParamRef = createTestParameter("viewParamRef", "searched", "", "", "", "");

    QSharedPointer<FileBuilder> instantiationBuilder = createTestFileBuilder("Lordgenome", "searched + Lazengann");

    QSharedPointer<ComponentInstantiation> componentInstantiation (new ComponentInstantiation("compInstantiation"));
    componentInstantiation->getParameters()->append(viewParamRef);
    componentInstantiation->getDefaultFileBuilders()->append(instantiationBuilder);

    QSharedPointer<View> viewRef (new View("viewRef"));
    viewRef->setComponentInstantiationRef(componentInstantiation->name());

    // Port: One reference, in default value.
    QSharedPointer<Port> portRef = createTestPort("portRef", "", "test", "searched", "", "");

    QSharedPointer<WriteValueConstraint> constraintRef (new WriteValueConstraint());
    constraintRef->setType(WriteValueConstraint::MIN_MAX);
    constraintRef->setMinimum("Saitama");
    constraintRef->setMaximum("searched + Saitama");

    QSharedPointer<Field> refField = createTestField("fieldRef", "", "searched", "");
    refField->setWriteConstraint(constraintRef);

    QSharedPointer<FieldReset> testReset = createTestFieldReset("1", "searched");
    testReset->setResetTypeReference("SOFT");
    refField->getResets()->append(testReset);

    // Register: One reference, in offset
    QSharedPointer<Register> registerRef = createTestRegister("registerRef", "searched", "test");
    registerRef->getFields()->append(refField);

    QSharedPointer<AddressBlock> addressRef = createTestAddressBlock("addressBlockRef", "'h4", "searched", "10");
    addressRef->getRegisterData()->append(registerRef);

    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef");
    memoryMapRef->getMemoryBlocks()->append(addressRef);

    QSharedPointer<Parameter> busIFParameter = createTestParameter("busIFParameterRef", "searched", "", "", "", "");

    QSharedPointer<MirroredSlaveInterface> mirrorSlave(new MirroredSlaveInterface);
    mirrorSlave->setRange(searched->getValueId());

    QSharedPointer<BusInterface> refBusInterface(new BusInterface);
    refBusInterface->setName("refBusInterface");
    refBusInterface->getParameters()->append(busIFParameter);
    refBusInterface->setInterfaceMode(General::MIRROREDSLAVE);
    refBusInterface->setMirroredSlave(mirrorSlave);

    QSharedPointer<MasterInterface> masterInterface(new MasterInterface);
    masterInterface->setBaseAddress(searched->getValueId() + "*2");

    QSharedPointer<BusInterface> masterBus(new BusInterface);
    masterBus->setInterfaceMode(General::MASTER);
    masterBus->setMaster(masterInterface);

    QSharedPointer <AddressBlock> spaceBlockRef = createTestAddressBlock("spaceBlockRef", "searched", "", "");

    QSharedPointer<MemoryMapBase> localMemoryMap (new MemoryMapBase("localMemMap"));
    localMemoryMap->getMemoryBlocks()->append(spaceBlockRef);

    QSharedPointer <AddressSpace> addressSpaceRef = createTestAddressSpace("addressSpaceRef");
    addressSpaceRef->setLocalMemoryMap(localMemoryMap);

    QSharedPointer<RemapPort> refRemapPort(new RemapPort);
    refRemapPort->setPortNameRef("remapPort");
    refRemapPort->setPortIndex("2");
    refRemapPort->setValue("searched");

    QSharedPointer<RemapState> refRemapState(new RemapState());
    refRemapState->setName("refRemapState");
    refRemapState->getRemapPorts()->append(refRemapPort);

    QSharedPointer<BuildCommand> referencingBuildCommand = createTestBuildCommand("searched+Dandy");
    QSharedPointer<File> referencingFile = createTestFile("referencingFile", referencingBuildCommand);

    QSharedPointer<FileBuilder> refFileBuilder = createTestFileBuilder("systemCSource", "searched+1");
    QSharedPointer<FileSet> refFileSet = createTestFileSet("refFiles", refFileBuilder);
    refFileSet->getFiles()->append(referencingFile);

    QSharedPointer<Component> component(new Component);
    component->getFileSets()->append(refFileSet);
    component->getParameters()->append(searched);
    component->getParameters()->append(paramRef);
    component->getViews()->append(viewRef);
    component->getComponentInstantiations()->append(componentInstantiation);
    component->getPorts()->append(portRef);
    component->getMemoryMaps()->append(memoryMapRef);
    component->getBusInterfaces()->append(refBusInterface);
    component->getBusInterfaces()->append(masterBus);
    component->getAddressSpaces()->append(addressSpaceRef);
    component->getRemapStates()->append(refRemapState);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    QScopedPointer<ComponentParameterReferenceTree> tree(createTestTree(component));
    tree->openReferenceTree(QStringLiteral("searched"));

    QCOMPARE(tree->topLevelItemCount(), 8);

    //! Test file sets.
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("File sets"));
    QCOMPARE(tree->topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), refFileSet->name());
    QCOMPARE(tree->topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->childCount(), 2);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Default file build commands"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("File type: ") + refFileBuilder->getFileType());
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString());

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Replace default flags"));
    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(refFileBuilder->getReplaceDefaultFlags()));

    QCOMPARE(tree->topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 0);

    QTreeWidgetItem* filesItem = tree->topLevelItem(0)->child(0)->child(1);
    QCOMPARE(filesItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Files"));
    QCOMPARE(filesItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(filesItem->childCount(), 1);

    QTreeWidgetItem* singleFileItem = filesItem->child(0);
    QCOMPARE(singleFileItem->text(ParameterReferenceTree::ITEM_NAME), referencingFile->name());
    QCOMPARE(singleFileItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(singleFileItem->childCount(), 1);

    QTreeWidgetItem* buildCommandItem = singleFileItem->child(0);
    QCOMPARE(buildCommandItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Build command"));
    QCOMPARE(buildCommandItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(buildCommandItem->childCount(), 1);

    QTreeWidgetItem* flagsItem = buildCommandItem->child(0);
    QCOMPARE(flagsItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Replace default flags"));
    QCOMPARE(flagsItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(referencingBuildCommand->getReplaceDefaultFlags()));
    QCOMPARE(flagsItem->childCount(), 0);

    //! Test parameters.
    QCOMPARE(tree->topLevelItem(1)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Parameters"));
    QCOMPARE(tree->topLevelItem(1)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(1)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(1)->child(0)->text(ParameterReferenceTree::ITEM_NAME), paramRef->name());
    QCOMPARE(tree->topLevelItem(1)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(1)->child(0)->childCount(), 2);
    QCOMPARE(tree->topLevelItem(1)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Array Left"));
    QCOMPARE(tree->topLevelItem(1)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(paramRef->getAttribute("kactus2:arrayLeft")));
    QCOMPARE(tree->topLevelItem(1)->child(0)->child(0)->childCount(), 0);

    QCOMPARE(tree->topLevelItem(1)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Array Right"));
    QCOMPARE(tree->topLevelItem(1)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(paramRef->getAttribute("kactus2:arrayRight")));
    QCOMPARE(tree->topLevelItem(1)->child(0)->child(1)->childCount(), 0);

    //! Test memory maps.
    QCOMPARE(tree->topLevelItem(2)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Memory maps"));
    QCOMPARE(tree->topLevelItem(2)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(2)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(2)->child(0)->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->name());
    QCOMPARE(tree->topLevelItem(2)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(2)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(2)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Memory remaps"));
    QCOMPARE(tree->topLevelItem(2)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(2)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(2)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Default"));
    QCOMPARE(tree->topLevelItem(2)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString());

    QCOMPARE(tree->topLevelItem(2)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Address blocks"));
    QCOMPARE(tree->topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(2)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), addressRef->name());
    QCOMPARE(tree->topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 2);
    QCOMPARE(tree->topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), QStringLiteral("Range"));
    QCOMPARE(tree->topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), expressionFormatter->formatReferringExpression(
        addressRef->getRange()));
    QCOMPARE(tree->topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 0);

    QTreeWidgetItem* registerListItem =
        tree->topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1);
    QCOMPARE(registerListItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Registers"));
    QCOMPARE(registerListItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(registerListItem->childCount(), 1);

    QTreeWidgetItem* registerItem = registerListItem->child(0);
    QCOMPARE(registerItem->text(ParameterReferenceTree::ITEM_NAME), registerRef->name());
    QCOMPARE(registerItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(registerItem->childCount(), 2);

    //! Test registers.
    QTreeWidgetItem* offsetItem = registerItem->child(0);
    QCOMPARE(offsetItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Offset"));
    QCOMPARE(offsetItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(registerRef->getAddressOffset()));
    QCOMPARE(offsetItem->childCount(), 0);

    //! Test register fields.
    QTreeWidgetItem* fieldListItem = registerItem->child(1);
    QCOMPARE(fieldListItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Fields"));
    QCOMPARE(fieldListItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(fieldListItem->childCount(), 1);

    QTreeWidgetItem* fieldItem = fieldListItem->child(0);
    QCOMPARE(fieldItem->text(ParameterReferenceTree::ITEM_NAME), refField->name());
    QCOMPARE(fieldItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(fieldItem->childCount(), 3);

    QTreeWidgetItem* widthItem = fieldItem->child(0);
    QCOMPARE(widthItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Width"));
    QCOMPARE(widthItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(refField->getBitWidth()));
    QCOMPARE(widthItem->childCount(), 0);

    QTreeWidgetItem* resetsItem = fieldItem->child(1);
    QCOMPARE(resetsItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Resets"));
    QCOMPARE(resetsItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(resetsItem->childCount(), 1);

    QTreeWidgetItem* softResetItem = resetsItem->child(0);
    QCOMPARE(softResetItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("SOFT"));
    QCOMPARE(softResetItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(softResetItem->childCount(), 1);
    
    QTreeWidgetItem* softResetMaskItem = softResetItem->child(0);
    QCOMPARE(softResetMaskItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Reset mask"));
    QCOMPARE(softResetMaskItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(testReset->getResetMask()));
    QCOMPARE(softResetMaskItem->childCount(), 0);

    QTreeWidgetItem* maxConstraintItem = fieldItem->child(2);
    QCOMPARE(maxConstraintItem->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Write constraint maximum"));
    QCOMPARE(maxConstraintItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(refField->getWriteConstraint()->getMaximum()));
    QCOMPARE(maxConstraintItem->childCount(), 0);

    //! Test address Spaces.
    QCOMPARE(tree->topLevelItem(3)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Address Spaces"));
    QCOMPARE(tree->topLevelItem(3)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(3)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(3)->child(0)->text(ParameterReferenceTree::ITEM_NAME), addressSpaceRef->name());
    QCOMPARE(tree->topLevelItem(3)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(3)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(3)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        localMemoryMap->name());
    QCOMPARE(tree->topLevelItem(3)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(3)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(3)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Address blocks"));
    QCOMPARE(tree->topLevelItem(3)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString());

    QCOMPARE(tree->topLevelItem(3)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(3)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        spaceBlockRef->name());
    QCOMPARE(tree->topLevelItem(3)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(3)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(3)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), QStringLiteral("Base Address"));
    QCOMPARE(tree->topLevelItem(3)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), expressionFormatter->formatReferringExpression(
        spaceBlockRef->getBaseAddress()));
    QCOMPARE(tree->topLevelItem(3)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 0);

    //! Test views.
    QTreeWidgetItem* instantiationsItem = tree->topLevelItem(4);
    QCOMPARE(instantiationsItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Instantiations"));
    QCOMPARE(instantiationsItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(instantiationsItem->childCount(), 1);
    QTreeWidgetItem* componentInstantiationsItem = instantiationsItem->child(0);
    QCOMPARE(componentInstantiationsItem->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Component instantiations"));
    QCOMPARE(componentInstantiationsItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(componentInstantiationsItem->childCount(), 1);
    QTreeWidgetItem* singleComponentInstantiationItem = componentInstantiationsItem->child(0);
    QCOMPARE(singleComponentInstantiationItem->text(ParameterReferenceTree::ITEM_NAME),
        componentInstantiation->name());
    QCOMPARE(singleComponentInstantiationItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(singleComponentInstantiationItem->childCount(), 2);
    QTreeWidgetItem* parametersItem = singleComponentInstantiationItem->child(0);
    QCOMPARE(parametersItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Parameters"));
    QCOMPARE(parametersItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(parametersItem->childCount(), 1);
    QTreeWidgetItem* viewParameterItem = parametersItem->child(0);
    QCOMPARE(viewParameterItem->text(ParameterReferenceTree::ITEM_NAME), viewParamRef->name());
    QCOMPARE(viewParameterItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(viewParameterItem->childCount(), 1);
    QTreeWidgetItem* valueItem = viewParameterItem->child(0);
    QCOMPARE(valueItem->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Value"));
    QCOMPARE(valueItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(viewParamRef->getValue()));
    QCOMPARE(valueItem->childCount(), 0);

    QTreeWidgetItem* defaultFileBuildersItem = singleComponentInstantiationItem->child(1);
    QCOMPARE(defaultFileBuildersItem->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Default file build commands"));
    QCOMPARE(defaultFileBuildersItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(defaultFileBuildersItem->childCount(), 1);
    QTreeWidgetItem* fileBuilderItem = defaultFileBuildersItem->child(0);
    QCOMPARE(fileBuilderItem->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("File type: ") + instantiationBuilder->getUserFileType());
    QCOMPARE(fileBuilderItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(fileBuilderItem->childCount(), 1);
    QTreeWidgetItem* replaceFlagsItem = fileBuilderItem->child(0);
    QCOMPARE(replaceFlagsItem->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Replace default flags"));
    QCOMPARE(replaceFlagsItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(instantiationBuilder->getReplaceDefaultFlags()));

    QCOMPARE(replaceFlagsItem->childCount(), 0);

    //! Test ports.
    QCOMPARE(tree->topLevelItem(5)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Ports"));
    QCOMPARE(tree->topLevelItem(5)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(5)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(5)->child(0)->text(ParameterReferenceTree::ITEM_NAME), portRef->name());
    QCOMPARE(tree->topLevelItem(5)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(5)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(5)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Default Value"));
    QCOMPARE(tree->topLevelItem(5)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(portRef->getDefaultValue()));
    QCOMPARE(tree->topLevelItem(5)->child(0)->child(0)->childCount(), 0);

    //! Test bus interfaces.
    QCOMPARE(tree->topLevelItem(6)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Bus Interfaces"));
    QCOMPARE(tree->topLevelItem(6)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(6)->childCount(), 2);
    QTreeWidgetItem* busInterfaceItem = tree->topLevelItem(6)->child(0);
    QCOMPARE(busInterfaceItem->text(ParameterReferenceTree::ITEM_NAME), refBusInterface->name());
    QCOMPARE(busInterfaceItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(busInterfaceItem->childCount(), 2);

    //! Test mirrored slave.
    QCOMPARE(busInterfaceItem->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Mirrored Slave Interface"));
    QCOMPARE(busInterfaceItem->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(6)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(6)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Range"));
    QCOMPARE(tree->topLevelItem(6)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(mirrorSlave->getRange()));
    QCOMPARE(tree->topLevelItem(6)->child(0)->child(0)->child(0)->childCount(), 0);

    //! Test bus interface parameters.
    QTreeWidgetItem* busInterfaceParametersItem = busInterfaceItem->child(1);
    QCOMPARE(busInterfaceParametersItem->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Parameters"));
    QCOMPARE(busInterfaceParametersItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QTreeWidgetItem* parameterItem = busInterfaceParametersItem->child(0);
    QCOMPARE(parameterItem->text(ParameterReferenceTree::ITEM_NAME),
        busIFParameter->name());
    QCOMPARE(parameterItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    
    QCOMPARE(parameterItem->childCount(), 1);
    QCOMPARE(parameterItem->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Value"));
    QCOMPARE(parameterItem->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(busIFParameter->getValue()));

    QCOMPARE(parameterItem->child(0)->childCount(), 0);

    //! Test master interface.
    QCOMPARE(tree->topLevelItem(6)->child(1)->text(ParameterReferenceTree::ITEM_NAME), masterBus->name());
    QCOMPARE(tree->topLevelItem(6)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(6)->child(1)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(6)->child(1)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Master Interface"));
    QCOMPARE(tree->topLevelItem(6)->child(1)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());

    QCOMPARE(tree->topLevelItem(6)->child(1)->child(0)->childCount(), 1);
    QCOMPARE(tree->topLevelItem(6)->child(1)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Base Address"));
    QCOMPARE(tree->topLevelItem(6)->child(1)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(masterInterface->getBaseAddress()));

    QCOMPARE(tree->topLevelItem(6)->child(1)->child(0)->child(0)->childCount(), 0);

    //! Test remap states.
    QCOMPARE(tree->topLevelItem(7)->text(ParameterReferenceTree::ITEM_NAME), QStringLiteral("Remap States"));
    QCOMPARE(tree->topLevelItem(7)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(tree->topLevelItem(7)->childCount(), 1);

    QCOMPARE(tree->topLevelItem(7)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("refRemapState"));
    QCOMPARE(tree->topLevelItem(7)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(tree->topLevelItem(7)->child(0)->childCount(), 1);

    QCOMPARE(tree->topLevelItem(7)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("Remap Ports"));
    QCOMPARE(tree->topLevelItem(7)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString());
    QCOMPARE(tree->topLevelItem(7)->child(0)->child(0)->childCount(), 1);

    QCOMPARE(tree->topLevelItem(7)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QStringLiteral("remapPort[2]"));
    QCOMPARE(tree->topLevelItem(7)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QStringLiteral("searchedParameter"));
    QCOMPARE(tree->topLevelItem(7)->child(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> tst_ParameterReferenceTree::createTestParameter(QString const& name,
    QString const& value, QString const& bitWidthLeft, QString const& bitWidthRight, QString const& arrayLeft,
    QString const& arrayRight)
{
    QSharedPointer<Parameter> referencingParameter(new Parameter);
    referencingParameter->setName(name);
    referencingParameter->setValue(value);
    referencingParameter->setVectorLeft(bitWidthLeft);
    referencingParameter->setVectorRight(bitWidthRight);
    referencingParameter->setAttribute("kactus2:arrayLeft", arrayLeft);
    referencingParameter->setAttribute("kactus2:arrayRight", arrayRight);

    return referencingParameter;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestModuleParameter()
//-----------------------------------------------------------------------------
QSharedPointer<ModuleParameter> tst_ParameterReferenceTree::createTestModuleParameter(QString const& name,
    QString const& value, QString const& bitWidthLeft, QString const& bitWidthRight, QString const& arrayLeft,
    QString const& arrayRight)
{
    QSharedPointer<ModuleParameter> referencingModuleParameter(new ModuleParameter());
    referencingModuleParameter->setName(name);
    referencingModuleParameter->setValue(value);
    referencingModuleParameter->setVectorLeft(bitWidthLeft);
    referencingModuleParameter->setVectorRight(bitWidthRight);
    referencingModuleParameter->setAttribute("kactus2:arrayLeft", arrayLeft);
    referencingModuleParameter->setAttribute("kactus2:arrayRight", arrayRight);

    return referencingModuleParameter;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> tst_ParameterReferenceTree::createTestPort(QString const& name, QString const& leftExpression,
    QString const& rightExpression, QString const& defaultValue, QString const& arrayLeft,
    QString const& arrayRight)
{
    QSharedPointer<Port> referencingPort(new Port);
    referencingPort->setName(name);
    referencingPort->setLeftBound(leftExpression);
    referencingPort->setRightBound(rightExpression);
    referencingPort->setDefaultValue(defaultValue);
    referencingPort->setArrayLeft(arrayLeft);
    referencingPort->setArrayRight(arrayRight);

    return referencingPort;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestRegister()
//-----------------------------------------------------------------------------
QSharedPointer<Register> tst_ParameterReferenceTree::createTestRegister(QString const& name, QString const& offSet,
    QString const& dimension)
{
    QSharedPointer<Register> registerRef(new Register);
    registerRef->setName(name);
    registerRef->setAddressOffset(offSet);
    registerRef->setDimension(dimension);

    return registerRef;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestAddressBlock()
//-----------------------------------------------------------------------------
QSharedPointer<AddressBlock> tst_ParameterReferenceTree::createTestAddressBlock(QString const& name,
    QString const& baseAddress, QString const& range, QString const& width)
{
    QSharedPointer<AddressBlock> addressRef(new AddressBlock);
    addressRef->setName(name);
    addressRef->setBaseAddress(baseAddress);
    addressRef->setRange(range);
    addressRef->setWidth(width);

    return addressRef;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestMemoryMap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMap> tst_ParameterReferenceTree::createTestMemoryMap(QString const& name)
{
    QSharedPointer<MemoryMap> memoryMapRef(new MemoryMap);
    memoryMapRef->setName(name);

    return memoryMapRef;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestMemoryRemap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryRemap> tst_ParameterReferenceTree::createTestMemoryRemap(QString const& name)
{
    QSharedPointer<MemoryRemap> memoryRemapRef(new MemoryRemap);
    memoryRemapRef->setName(name);

    return memoryRemapRef;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestField()
//-----------------------------------------------------------------------------
QSharedPointer<Field> tst_ParameterReferenceTree::createTestField(QString const& name, QString const& offset,
    QString const& width, QString const& presenceExpression)
{
    QSharedPointer<Field> fieldRef(new Field);
    fieldRef->setName(name);
    fieldRef->setBitOffset(offset);
    fieldRef->setBitWidth(width);
    fieldRef->setIsPresent(presenceExpression);

    return fieldRef;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestFieldReset()
//-----------------------------------------------------------------------------
QSharedPointer<FieldReset> tst_ParameterReferenceTree::createTestFieldReset(QString const& resetValue,
    QString const& resetMask)
{
    QSharedPointer<FieldReset> testReset(new FieldReset());
    testReset->setResetValue(resetValue);
    testReset->setResetMask(resetMask);

    return testReset;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestAddressSpace()
//-----------------------------------------------------------------------------
QSharedPointer<AddressSpace> tst_ParameterReferenceTree::createTestAddressSpace(QString const& name)
{
    QSharedPointer<AddressSpace> addressSpaceRef(new AddressSpace);
    addressSpaceRef->setName(name);

    return addressSpaceRef;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestFileBuilder()
//-----------------------------------------------------------------------------
QSharedPointer<FileBuilder> tst_ParameterReferenceTree::createTestFileBuilder(QString const& fileType,
    QString const& replaceDefaultFlags)
{
    QSharedPointer<FileBuilder> fileBuilderReference(new FileBuilder(fileType));
    fileBuilderReference->setReplaceDefaultFlags(replaceDefaultFlags);

    return fileBuilderReference;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestFileSet()
//-----------------------------------------------------------------------------
QSharedPointer<FileSet> tst_ParameterReferenceTree::createTestFileSet(QString const& name,
    QSharedPointer<FileBuilder> builder)
{
    QSharedPointer<FileSet> referencingFileSet (new FileSet(name));

    if (builder)
    {
        referencingFileSet->getDefaultFileBuilders()->append(builder);
    }

    return referencingFileSet;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestBuildCommand()
//-----------------------------------------------------------------------------
QSharedPointer<BuildCommand> tst_ParameterReferenceTree::createTestBuildCommand(QString const& replaceDefaultFlags)
{
    QSharedPointer<BuildCommand> newCommand (new BuildCommand());
    newCommand->setReplaceDefaultFlags(replaceDefaultFlags);

    return newCommand;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestFile()
//-----------------------------------------------------------------------------
QSharedPointer<File> tst_ParameterReferenceTree::createTestFile(QString const& name,
    QSharedPointer<BuildCommand> buildCommand)
{
    QSharedPointer<File> newFile (new File(name));
    newFile->setBuildcommand(buildCommand);

    return newFile;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestExpressionFormatter()
//-----------------------------------------------------------------------------
QSharedPointer<ExpressionFormatter> tst_ParameterReferenceTree::createTestExpressionFormatter(
    QSharedPointer<Component> component)
{
    QSharedPointer<ParameterFinder> parameterFinder(new ComponentParameterFinder(component));
    QSharedPointer<ExpressionFormatter> expressionFormatter (new ExpressionFormatter(parameterFinder));

    return expressionFormatter;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestTree()
//-----------------------------------------------------------------------------
ComponentParameterReferenceTree* tst_ParameterReferenceTree::createTestTree(QSharedPointer<Component> component)
{
    QSharedPointer<ParameterFinder> finder(new ComponentParameterFinder(component));

    ComponentParameterReferenceTree* tree = new ComponentParameterReferenceTree(component, 
        createTestExpressionFormatter(component), 
        QSharedPointer<ComponentParameterReferenceCounter>(
        new ComponentParameterReferenceCounter(finder, component)), 0);

    return tree;
}

QTEST_MAIN(tst_ParameterReferenceTree)



#include "tst_ParameterReferenceTree.moc"
