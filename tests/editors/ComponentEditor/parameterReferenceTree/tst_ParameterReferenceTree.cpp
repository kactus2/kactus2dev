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

#include <editors/ComponentEditor/parameterReferenceTree/ParameterReferenceTree.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/parameter.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/register.h>
#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/memorymap.h>
#include <IPXACTmodels/MemoryRemap.h>
#include <IPXACTmodels/AbstractMemoryMap.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/mirroredslaveinterface.h>
#include <IPXACTmodels/field.h>
#include <IPXACTmodels/addressspace.h>
#include <IPXACTmodels/remapstate.h>
#include <IPXACTmodels/remapport.h>

#include <IPXACTmodels/kactusExtensions/ModuleParameter.h>

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

    void testReferenceInParameterValueAddsThreeRows();
    void testMultipleReferencesInOneParameter();
    void testMultipleReferencesInMultipleParameters();

    void testReferenceAsAnExpressionInParameter();
    void testMultipleReferencesInSameExpression();

    void testReferenceInModelParameterValueAddsThreeRows();
    void testReferencesInParametersAndModelParameters();

    void testReferenceInViewParameterValueAddsFiveRows();
    void testReferenceInViewModuleParameterValueAddsFiveRows();

    void testReferenceInPortRightboundAddsThreeRows();
    void testReferenceInPortArraysAddsThreeRows();

    void testReferenceInAddressBlockBaseAddressAddsSevenRows();
    void testReferenceInAddressBlockNoReferenceInRegister();

    void testReferenceInRegisterDimensionAddsEightRows();
    void testReferenceInMultipleRegistersInTwoAddressBlocks();
    void testRegisterwithNoReferences();

    void testReferenceInAddressSpaceAddressBlockAddsSixRows();

    void testReferenceInRegisterFieldAddsTenRows();

    void testReferenceInBusInterfaceParameterAddsFourRows();

    void testReferenceInBusInterfaceMirroredSlaveAddsFourRows();

    void testReferenceInRemapStateAddsFourRows();

    void testRerefencesInMultiplePlaces();

private:
    QSharedPointer<Parameter> createTestParameter(QString const& name, QString const& value, 
        QString const& bitWidthLeft, QString const& bitWidthRight, QString const& arrayLeft,
        QString const& arrayRight);

    QSharedPointer<ModelParameter> createTestModelParameter(QString const& name, QString const& value,
        QString const& bitWidthLeft, QString const& bitWidthRight, QString const& arrayLeft,
        QString const& arrayRight);

    QSharedPointer<Port> createTestPort(QString const& name, QString const& leftExpression,
        QString const& rightExpression, QString const& defaultValue, QString const& arrayLeft,
        QString const& arrayRight);

    QSharedPointer<Register> createTestRegister(QString const& name, QString const& offSet,
        QString const& dimension);

    QSharedPointer<AddressBlock> createTestAddressBlock(QString const& name, QString const& baseAddress,
        QString const& range, QString const& width, QList <QSharedPointer<Register> > registers);

    QSharedPointer<MemoryMap> createTestMemoryMap(QString const& name,
        QList <QSharedPointer<AddressBlock> > addressBlocks);

    QSharedPointer<MemoryRemap> createTestMemoryRemap(QString const& name,
        QList<QSharedPointer<AddressBlock> > addressBlocks);

    QSharedPointer<Field> createTestField(QString const& name, QString const& offset, QString const& width,
        QString const& presenceExpression);

    QSharedPointer<AddressSpace> createTestAddressSpace(QString const& name,
        QSharedPointer<MemoryMap> localMemoryMap);

    QSharedPointer<ExpressionFormatter> createTestExpressionFormatter(QSharedPointer<Component> component);
};

tst_ParameterReferenceTree::tst_ParameterReferenceTree()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testHasTwoColumns()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testEmptyComponentInConstructor()
{
    QSharedPointer<Component> component(0);

    ParameterReferenceTree tree(component, createTestExpressionFormatter(component), QString(), 0);

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->childCount(), 0);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Component does not exist."));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testColumnCount()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testColumnCount()
{
    QSharedPointer<Component> component(0);

    ParameterReferenceTree tree(component, createTestExpressionFormatter(component), QString(), 0);

    QCOMPARE(tree.columnCount(), 2);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testNoReferencesFoundAddsOneRow()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testNoReferencesFoundAddsOneRow()
{
    QSharedPointer <Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList <QSharedPointer<Parameter> > componentParameters;
    // Parameter: No references.
    QSharedPointer<Parameter> paramRef = createTestParameter("ref1", "", "", "", "test", "test");
    componentParameters.append(paramRef);

    // Model Parameter: No references.
    QSharedPointer<ModelParameter> modelRef = createTestModelParameter("modelRef", "test", "", "", "" , "");
    Model* componentModel(new Model);
    componentModel->addModelParameter(modelRef);


    QList<QSharedPointer<Parameter> > viewParameters;
    // Parameter in view: No references
    QSharedPointer<Parameter> viewParamRef = createTestParameter("viewParamRef", "test", "", "", "", "");
    viewParameters.append(viewParamRef);
    View *viewRef(new View("viewRef"));
    viewRef->getParameters()->append(viewParameters);

    // Port: No references
    QSharedPointer<Port> portRef = createTestPort("portRef", "", "test", "test", "", "");

    // Register: No references
    QList <QSharedPointer<Register> > registers;
    QSharedPointer<Register> registerRef = createTestRegister("registerRef", "test", "test");
    registers.append(registerRef);

    QList <QSharedPointer <AddressBlock> > addressBlocks;
    QSharedPointer<AddressBlock> addressRef = createTestAddressBlock("addressBlockRef", "test", "", "4",
        registers);
    addressBlocks.append(addressRef);

    QList <QSharedPointer<MemoryMap> > memoryMaps;
    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef", addressBlocks);
    memoryMaps.append(memoryMapRef);

    QList <QSharedPointer <AddressBlock> > addressSpaceBlocks;
    QSharedPointer <AddressBlock> spaceBlock = createTestAddressBlock("spaceBlockRef", "test", "", "15",
        QList<QSharedPointer<Register> > ());
    addressSpaceBlocks.append(spaceBlock);

    QSharedPointer <MemoryMap> spaceMemMap = createTestMemoryMap("spaceMemMap", addressBlocks);

    QList <QSharedPointer <AddressSpace> > addressSpaces;
    QSharedPointer <AddressSpace> addressSpaceRef = createTestAddressSpace("addressSpaceRef", spaceMemMap);
    addressSpaces.append(addressSpaceRef);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(componentParameters);
    component->setModel(componentModel);
    component->addView(viewRef);
    component->addPort(portRef);
    component->setMemoryMaps(memoryMaps);
    component->setAddressSpaces(addressSpaces);

    ParameterReferenceTree tree(component, createTestExpressionFormatter(component), searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("No references found."));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->childCount(), 0);
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

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Parameters"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), 
        referencingParameter->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Value"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(referencingParameter->getValue()));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);
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

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Parameters"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), referencer->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 5);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Value"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(referencer->getValue()));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Bit Width Left"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(referencer->getBitWidthLeft()));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(2)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Bit Width Right"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(2)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(referencer->getBitWidthRight()));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(3)->text(ParameterReferenceTree::ITEM_NAME), QString("Array Left"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(3)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(referencer->getAttribute("kactus2:arrayLeft")));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(4)->text(ParameterReferenceTree::ITEM_NAME), QString("Array Right"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(4)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(referencer->getAttribute("kactus2:arrayRight")));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->childCount(), 0);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(2)->childCount(), 0);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(3)->childCount(), 0);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(4)->childCount(), 0);
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

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Parameters"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 2);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), firstRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME), secondRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 2);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Left"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(firstRef->getAttribute("kactus2:arrayLeft")));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Right"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(firstRef->getAttribute("kactus2:arrayRight")));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->childCount(), 0);

    QCOMPARE(tree.topLevelItem(0)->child(1)->childCount(), 3);
    QCOMPARE(tree.topLevelItem(0)->child(1)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Bit Width Left"));
    QCOMPARE(tree.topLevelItem(0)->child(1)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(secondRef->getBitWidthLeft()));

    QCOMPARE(tree.topLevelItem(0)->child(1)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Left"));
    QCOMPARE(tree.topLevelItem(0)->child(1)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(secondRef->getAttribute("kactus2:arrayLeft")));

    QCOMPARE(tree.topLevelItem(0)->child(1)->child(2)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Right"));
    QCOMPARE(tree.topLevelItem(0)->child(1)->child(2)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(secondRef->getAttribute("kactus2:arrayRight")));

    QCOMPARE(tree.topLevelItem(0)->child(1)->child(0)->childCount(), 0);
    QCOMPARE(tree.topLevelItem(0)->child(1)->child(1)->childCount(), 0);
    QCOMPARE(tree.topLevelItem(0)->child(1)->child(2)->childCount(), 0);
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

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Parameters"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), firstRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Value"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(firstRef->getValue()));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);
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

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Parameters"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), firstRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Value"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(firstRef->getValue()));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInModelParameterValueAddsThreeRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInModelParameterValueAddsThreeRows()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList <QSharedPointer <Parameter> > componentParameters;
    componentParameters.append(searched);

    // One reference, in value.
    QSharedPointer<ModelParameter> modelRef = createTestModelParameter("modelRef", "searched", "", "", "" , "");

    Model* componentModel(new Model);
    componentModel->addModelParameter(modelRef);

    QSharedPointer<Component> component(new Component);
    component->setModel(componentModel);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Model Parameters"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), modelRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Value"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(modelRef->getValue()));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferencesInParamersAndModelParameters()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferencesInParametersAndModelParameters()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList<QSharedPointer<Parameter> > componentParameters;
    componentParameters.append(searched);

    // One reference, in array offset.
    QSharedPointer<Parameter> paramRef = createTestParameter("paramRef", "", "", "", "", "searched");
    componentParameters.append(paramRef);

    // Two references, in bit width and array size.
    QSharedPointer<ModelParameter> modelRef = createTestModelParameter("modelRef", "", "", "searched", "searched",
        "");
    Model* componentModel(new Model);
    componentModel->addModelParameter(modelRef);
    
    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(componentParameters);
    component->setModel(componentModel);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 2);

    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Model Parameters"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), modelRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 2);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Bit Width Right"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(modelRef->getBitWidthRight()));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Left"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(modelRef->getAttribute("kactus2:arrayLeft")));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->childCount(), 0);

    QCOMPARE(tree.topLevelItem(1)->text(ParameterReferenceTree::ITEM_NAME), QString("Parameters"));
    QCOMPARE(tree.topLevelItem(1)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(1)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(1)->child(0)->text(ParameterReferenceTree::ITEM_NAME), paramRef->getName());
    QCOMPARE(tree.topLevelItem(1)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(1)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(1)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Right"));
    QCOMPARE(tree.topLevelItem(1)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(paramRef->getAttribute("kactus2:arrayRight")));

    QCOMPARE(tree.topLevelItem(1)->child(0)->child(0)->childCount(), 0);
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

    View *viewRef(new View("viewRef"));
    viewRef->getParameters()->append(paramRef);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(searched);
    component->addView(viewRef);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());
        
    QCOMPARE(tree.topLevelItemCount(), 1);
    QTreeWidgetItem* viewsItem = tree.topLevelItem(0);
    QCOMPARE(viewsItem->text(ParameterReferenceTree::ITEM_NAME), QString("Views"));
    QCOMPARE(viewsItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(viewsItem->background(ParameterReferenceTree::ITEM_EXPRESSION).color(), QColor(230, 230, 230));

    QCOMPARE(viewsItem->childCount(), 1);
    QTreeWidgetItem* refViewItem = tree.topLevelItem(0)->child(0);
    QCOMPARE(refViewItem->text(ParameterReferenceTree::ITEM_NAME), viewRef->getName());
    QCOMPARE(refViewItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(refViewItem->childCount(), 1);
    QTreeWidgetItem* viewParametersItem = refViewItem->child(0);
    QCOMPARE(viewParametersItem->text(ParameterReferenceTree::ITEM_NAME), QString("Parameters"));
    QCOMPARE(viewParametersItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(viewParametersItem->background(ParameterReferenceTree::ITEM_EXPRESSION).color(), QColor(230, 230, 230));

    QCOMPARE(refViewItem->childCount(), 1);
    QTreeWidgetItem* parameterItem = viewParametersItem->child(0);
    QCOMPARE(parameterItem->text(ParameterReferenceTree::ITEM_NAME), paramRef->getName());
    QCOMPARE(parameterItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(parameterItem->childCount(), 1);
    QTreeWidgetItem* valueItem = parameterItem->child(0);
    QCOMPARE(valueItem->text(ParameterReferenceTree::ITEM_NAME), QString("Value"));
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

    View* viewRef(new View("viewRef"));
    component->addView(viewRef);

    QDomNode emptyNode;
    QSharedPointer<ModuleParameter> paramRef(new ModuleParameter(emptyNode));
    paramRef->setName("paramRef");
    paramRef->setValue(searched->getValueId());
    viewRef->getModuleParameters()->append(paramRef);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QTreeWidgetItem* viewsItem = tree.topLevelItem(0);
    QCOMPARE(viewsItem->text(ParameterReferenceTree::ITEM_NAME), QString("Views"));
    QCOMPARE(viewsItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(viewsItem->background(ParameterReferenceTree::ITEM_EXPRESSION).color(), QColor(230, 230, 230));

    QCOMPARE(viewsItem->childCount(), 1);
    QTreeWidgetItem* refViewItem = tree.topLevelItem(0)->child(0);
    QCOMPARE(refViewItem->text(ParameterReferenceTree::ITEM_NAME), viewRef->getName());
    QCOMPARE(refViewItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(refViewItem->childCount(), 1);
    QTreeWidgetItem* viewModuleParametersItem = refViewItem->child(0);
    QCOMPARE(viewModuleParametersItem->text(ParameterReferenceTree::ITEM_NAME), QString("Module Parameters"));
    QCOMPARE(viewModuleParametersItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(viewModuleParametersItem->background(ParameterReferenceTree::ITEM_EXPRESSION).color(), QColor(230, 230, 230));

    QCOMPARE(refViewItem->childCount(), 1);
    QTreeWidgetItem* parameterItem = viewModuleParametersItem->child(0);
    QCOMPARE(parameterItem->text(ParameterReferenceTree::ITEM_NAME), paramRef->getName());
    QCOMPARE(parameterItem->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(parameterItem->childCount(), 1);
    QTreeWidgetItem* valueItem = parameterItem->child(0);
    QCOMPARE(valueItem->text(ParameterReferenceTree::ITEM_NAME), QString("Value"));
    QCOMPARE(valueItem->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(paramRef->getValue()));

    QCOMPARE(valueItem->childCount(), 0);
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
    component->addPort(portRef);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Ports"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), portRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Right (Lower) Bound"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(portRef->getRightBoundExpression()));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);
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
    component->addPort(portRef);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Ports"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), portRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 2);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Left"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString("searchedParameter"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Right"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString("searchedParameter"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->childCount(), 0);

}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInAddressBlockBaseAddressAddsSevenRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInAddressBlockBaseAddressAddsSevenRows()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList < QSharedPointer <Parameter> > componentParameters;
    componentParameters.append(searched);

    QList <QSharedPointer<AddressBlock> > addressBlocks;
    QSharedPointer<AddressBlock> addressRef = createTestAddressBlock("addressBlockRef", "searched", "2", "32",
        QList<QSharedPointer<Register> > ());

    addressBlocks.append(addressRef);

    QList <QSharedPointer<MemoryMap> > memoryMaps;
    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef", addressBlocks);

    memoryMaps.append(memoryMapRef);

    QSharedPointer<Component> component(new Component);
    component->setMemoryMaps(memoryMaps);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Memory maps"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Memory remaps"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Default"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Address blocks"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), addressRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), QString("Base Address"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), expressionFormatter->formatReferringExpression(
        addressRef->getBaseAddress()));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 0);
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

    QList <QSharedPointer<Register> > registers;
    QSharedPointer<Register> registerRef = createTestRegister("registerRef", "test", "testing");
    registers.append(registerRef);

    QList <QSharedPointer<AddressBlock> > addressBlocks;
    QSharedPointer<AddressBlock> addressRef = createTestAddressBlock("addressBlockRef", "searched", "2", "32",
        registers);

    addressBlocks.append(addressRef);

    QList <QSharedPointer<MemoryMap> > memoryMaps;
    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef",
        QList<QSharedPointer<AddressBlock> > ());

    memoryMaps.append(memoryMapRef);

    QSharedPointer<MemoryRemap> memoryRemapRef = createTestMemoryRemap("memoryRemapRef", addressBlocks);
    memoryMapRef->getMemoryRemaps()->append(memoryRemapRef);

    QSharedPointer<Component> component(new Component);
    component->setMemoryMaps(memoryMaps);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Memory maps"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Memory remaps"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("memoryRemapRef"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Address blocks"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), addressRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), QString("Base Address"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), expressionFormatter->formatReferringExpression(
        addressRef->getBaseAddress()));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInRegisterDimensionAddsEightRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInRegisterDimensionAddsEightRows()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList < QSharedPointer <Parameter> > componentParameters;
    componentParameters.append(searched);

    QList <QSharedPointer<Register> > registers;
    QSharedPointer<Register> registerRef = createTestRegister("registerRef", "test", "searched");
    registers.append(registerRef);

    QList <QSharedPointer<AddressBlock> > addressBlocks;
    QSharedPointer<AddressBlock> addressRef = createTestAddressBlock("addressBlockRef", "test", "", "", registers);
    addressBlocks.append(addressRef);

    QList <QSharedPointer<MemoryMap> > memoryMaps;

    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef", addressBlocks);

    memoryMaps.append(memoryMapRef);

    QSharedPointer<Component> component(new Component);
    component->setMemoryMaps(memoryMaps);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Memory maps"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Memory remaps"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Default"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Address blocks"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), addressRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), registerRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), QString("Dimension"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(registerRef->getDimensionExpression()));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->
        childCount(), 0);
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

    QList <QSharedPointer <Register> > firstRegisters;
    QSharedPointer <Register> registerOne_1 = createTestRegister("registerOne_1", "", "searched");
    firstRegisters.append(registerOne_1);
    QSharedPointer <Register> registerTwo_1 = createTestRegister("registerTwo_1", "searched", "");
    firstRegisters.append(registerTwo_1);

    QList <QSharedPointer <AddressBlock> > addressBlocks;
    QSharedPointer <AddressBlock> addressBlockOne = createTestAddressBlock("addressBlockOne", "", "", "",
        firstRegisters);
    addressBlocks.append(addressBlockOne);

    QList <QSharedPointer <Register> > secondRegisters;
    QSharedPointer <Register> registerOne_2 = createTestRegister("registerOne_2", "searched", "searched");
    secondRegisters.append(registerOne_2);

    QSharedPointer <AddressBlock> addressBlockTwo = createTestAddressBlock("addressBlockTwo", "", "", "",
        secondRegisters);
    addressBlocks.append(addressBlockTwo);

    QList <QSharedPointer<MemoryMap> > memoryMaps;
    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef", addressBlocks);
    memoryMaps.append(memoryMapRef);

    QSharedPointer<Component> component(new Component);
    component->setMemoryMaps(memoryMaps);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);

    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Memory maps"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);

    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Memory remaps"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 1);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Default"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Address blocks"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 2);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), addressBlockOne->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 2);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), registerOne_1->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), QString("Dimension"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(registerOne_1->getDimensionExpression()));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->
        childCount(), 0);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->text(
        ParameterReferenceTree::ITEM_NAME), registerTwo_1->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->childCount(), 1);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), QString("Offset"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(registerTwo_1->getAddressOffset()));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(0)->
        childCount(), 0);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(1)->text(
        ParameterReferenceTree::ITEM_NAME), addressBlockTwo->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(1)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(1)->childCount(), 1);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), registerOne_2->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(0)->childCount(), 2);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), QString("Offset"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(registerOne_2->getAddressOffset()));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(0)->child(0)->
        childCount(), 0);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(0)->child(1)->text(
        ParameterReferenceTree::ITEM_NAME), QString("Dimension"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(0)->child(1)->text(
        ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(registerOne_2->getDimensionExpression()));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(0)->child(1)->
        childCount(), 0);
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

    QList <QSharedPointer <Register> > registers;
    QSharedPointer <Register> noRefRegister = createTestRegister("noRefRegister", "test", "test");
    QSharedPointer <Register> refRegister = createTestRegister("refRegister", "test", "searched");
    registers.append(refRegister);
    registers.append(noRefRegister);

    QList <QSharedPointer <AddressBlock> > addressBlocks;
    QSharedPointer <AddressBlock> addressBlockRef = createTestAddressBlock("addressBlockRef", "", "", "",
        registers);
    addressBlocks.append(addressBlockRef);

    QList <QSharedPointer<MemoryMap> > memoryMaps;
    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef", addressBlocks);
    memoryMaps.append(memoryMapRef);

    QSharedPointer<Component> component(new Component);
    component->setMemoryMaps(memoryMaps);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Memory maps"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Memory remaps"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Default"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Address blocks"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), addressBlockRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), refRegister->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), QString("Dimension"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(refRegister->getDimensionExpression()));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->
        childCount(), 0);
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

    QList <QSharedPointer <AddressBlock> > addressBlocks;
    QSharedPointer <AddressBlock> addressBlockRef = createTestAddressBlock("addressBlockRef", "searched", "", "",
        QList <QSharedPointer <Register> > ());
    addressBlocks.append(addressBlockRef);

    QSharedPointer<MemoryMap> localMemoryMap = createTestMemoryMap("localMemMap", addressBlocks);

    QList <QSharedPointer <AddressSpace> > addressSpaces;
    QSharedPointer <AddressSpace> addressSpaceRef = createTestAddressSpace("addressSpaceRef", localMemoryMap);

    addressSpaces.append(addressSpaceRef);
    QSharedPointer<Component> component(new Component);
    component->setAddressSpaces(addressSpaces);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Address Spaces"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), addressSpaceRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        localMemoryMap->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Address blocks"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        addressBlockRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), QString("Base Address"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), expressionFormatter->formatReferringExpression(
        addressBlockRef->getBaseAddress()));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInRegisterFieldAddsTenRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInRegisterFieldAddsTenRows()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList <QSharedPointer<Parameter> > componentParameters;
    componentParameters.append(searched);

    QList <QSharedPointer <Register> > registers;
    QSharedPointer <Register> refRegister = createTestRegister("refRegister", "test", "test");
    
    registers.append(refRegister);

    QSharedPointer<Field> refField = createTestField("fieldRef", "searched", "nothing", "searched");
    refRegister->getFields().append(refField);
    
    QList <QSharedPointer <AddressBlock> > addressBlocks;
    QSharedPointer <AddressBlock> addressBlockRef = createTestAddressBlock("addressBlockRef", "", "", "",
        registers);
    addressBlocks.append(addressBlockRef);

    QList <QSharedPointer<MemoryMap> > memoryMaps;

    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef",
        QList<QSharedPointer<AddressBlock> > ());
    memoryMaps.append(memoryMapRef);

    QSharedPointer<MemoryRemap> memoryRemapRef = createTestMemoryRemap("memoryRemapRef", addressBlocks);
    memoryMapRef->getMemoryRemaps()->append(memoryRemapRef);

    QSharedPointer<Component> component(new Component);
    component->setMemoryMaps(memoryMaps);
    component->getParameters()->append(componentParameters);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Memory maps"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Memory remaps"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("memoryRemapRef"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Address blocks"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), addressBlockRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), refRegister->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), QString("Fields"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->
        childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->
        text(ParameterReferenceTree::ITEM_NAME), refField->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->
        text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->
        childCount(), 2);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->
        child(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Offset"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->
        child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), expressionFormatter->formatReferringExpression(
        refField->getBitOffsetExpression()));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->
        child(0)->childCount(), 0);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->
        child(1)->text(ParameterReferenceTree::ITEM_NAME), QString("Is Present"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->
        child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION), expressionFormatter->formatReferringExpression(
        refField->getBitOffsetExpression()));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->
        child(1)->childCount(), 0);
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

    QSharedPointer<BusInterface> refBusInterface(new BusInterface);
    refBusInterface->setName("refBusInterface");
    QList<QSharedPointer<Parameter> > busInterfaceParameters;
    QSharedPointer<Parameter> busIFParameter = createTestParameter("busIFParameterRef", "", "", "", "searched", "");
    busInterfaceParameters.append(busIFParameter);

    refBusInterface->getParameters()->append(busInterfaceParameters);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(componentParameters);
    component->addBusInterface(refBusInterface);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Bus Interfaces"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), refBusInterface->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        busIFParameter->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Left"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(busIFParameter->getAttribute("kactus2:arrayLeft")));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 0);
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
    mirrorSlave->setRange("searchedParameter");
    mirrorSlave->setRangeID(searched->getValueId());
    refBusInterface->setMirroredSlave(mirrorSlave);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(componentParameters);
    component->addBusInterface(refBusInterface);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Bus Interfaces"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), refBusInterface->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Mirrored Slave Interface"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Range"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(mirrorSlave->getRangeID()));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 0);
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
    refRemapPort->setPortIndex(1);
    refRemapPort->setValue("searched");

    QSharedPointer<RemapPort> secondRemapPort(new RemapPort);
    secondRemapPort->setPortNameRef("remapPort");
    secondRemapPort->setPortIndex(3);
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

    ParameterReferenceTree tree(component, formatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Remap States"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);

    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), QString("refRemapState"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Remap Ports"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 3);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("remapPort[1]"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString("searchedParameter"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 0);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QString("remapPort[3]"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString("searchedParameter + 1"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(1)->childCount(), 0);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(2)->text(ParameterReferenceTree::ITEM_NAME),
        QString("otherRemapPort"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(2)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString("searchedParameter * 2"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(2)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferencesInMultiplePlaces()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testRerefencesInMultiplePlaces()
{
    QSharedPointer<Parameter> searched(new Parameter);
    searched->setName("searchedParameter");
    searched->setValueId("searched");

    QList <QSharedPointer<Parameter> > componentParameters;
    componentParameters.append(searched);

    // Parameter: Two references, in array size and array offset.
    QSharedPointer<Parameter> paramRef = createTestParameter("ref1", "", "", "", "searched", "searched");
    componentParameters.append(paramRef);

    // Model Parameter: One reference, in value.
    QSharedPointer<ModelParameter> modelRef = createTestModelParameter("modelRef", "searched", "", "", "" , "");
    Model* componentModel(new Model);
    componentModel->addModelParameter(modelRef);


    QList<QSharedPointer<Parameter> > viewParameters;
    // Parameter in view: One reference, in array offset.
    QSharedPointer<Parameter> viewParamRef = createTestParameter("viewParamRef", "searched", "", "", "", "");
    viewParameters.append(viewParamRef);
    View *viewRef(new View("viewRef"));
    viewRef->getParameters()->append(viewParameters);

    // Port: One reference, in default value.
    QSharedPointer<Port> portRef = createTestPort("portRef", "", "test", "searched", "", "");

    // Register: One reference, in offset
    QList <QSharedPointer<Register> > registers;
    QSharedPointer<Register> registerRef = createTestRegister("registerRef", "searched", "test");
    registers.append(registerRef);

    QSharedPointer<Field> refField = createTestField("fieldRef", "", "searched", "");
    registerRef->getFields().append(refField);

    QList <QSharedPointer <AddressBlock> > addressBlocks;
    QSharedPointer<AddressBlock> addressRef = createTestAddressBlock("addressBlockRef", "'h4", "searched", "10",
        registers);
    addressBlocks.append(addressRef);
    
    QList <QSharedPointer<MemoryMap> > memoryMaps;
    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef", addressBlocks);
    memoryMaps.append(memoryMapRef);


    QSharedPointer<BusInterface> refBusInterface(new BusInterface);
    refBusInterface->setName("refBusInterface");
    QList<QSharedPointer<Parameter> > busInterfaceParameters;
    QSharedPointer<Parameter> busIFParameter = createTestParameter("busIFParameterRef", "searched", "", "", "", "");
    busInterfaceParameters.append(busIFParameter);

    refBusInterface->getParameters()->append(busInterfaceParameters);

    refBusInterface->setInterfaceMode(General::MIRROREDSLAVE);

    QSharedPointer<MirroredSlaveInterface> mirrorSlave(new MirroredSlaveInterface);
    mirrorSlave->setRange("searchedParameter");
    mirrorSlave->setRangeID(searched->getValueId());
    refBusInterface->setMirroredSlave(mirrorSlave);

    QList <QSharedPointer <AddressBlock> > spaceBlocks;
    QSharedPointer <AddressBlock> spaceBlockRef = createTestAddressBlock("spaceBlockRef", "searched", "", "",
        QList <QSharedPointer <Register> > ());
    spaceBlocks.append(spaceBlockRef);

    QSharedPointer<MemoryMap> localMemoryMap = createTestMemoryMap("localMemMap", spaceBlocks);

    QList <QSharedPointer <AddressSpace> > addressSpaces;
    QSharedPointer <AddressSpace> addressSpaceRef = createTestAddressSpace("addressSpaceRef", localMemoryMap);

    addressSpaces.append(addressSpaceRef);

    QSharedPointer<RemapPort> refRemapPort(new RemapPort);
    refRemapPort->setPortNameRef("remapPort");
    refRemapPort->setPortIndex(2);
    refRemapPort->setValue("searched");

    QSharedPointer<RemapState> refRemapState(new RemapState());
    refRemapState->setName("refRemapState");
    refRemapState->getRemapPorts()->append(refRemapPort);

    QSharedPointer<Component> component(new Component);
    component->getParameters()->append(componentParameters);
    component->setModel(componentModel);
    component->addView(viewRef);
    component->addPort(portRef);
    component->setMemoryMaps(memoryMaps);
    component->addBusInterface(refBusInterface);
    component->setAddressSpaces(addressSpaces);
    component->getRemapStates()->append(refRemapState);

    QSharedPointer<ExpressionFormatter> expressionFormatter = createTestExpressionFormatter(component);

    ParameterReferenceTree tree(component, expressionFormatter, searched->getValueId());

    QCOMPARE(tree.topLevelItemCount(), 8);

    //! Test model parameters.
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Model Parameters"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), modelRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Value"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(modelRef->getValue()));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);

    //! Test parameters.
    QCOMPARE(tree.topLevelItem(1)->text(ParameterReferenceTree::ITEM_NAME), QString("Parameters"));
    QCOMPARE(tree.topLevelItem(1)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(1)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(1)->child(0)->text(ParameterReferenceTree::ITEM_NAME), paramRef->getName());
    QCOMPARE(tree.topLevelItem(1)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(1)->child(0)->childCount(), 2);
    QCOMPARE(tree.topLevelItem(1)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Left"));
    QCOMPARE(tree.topLevelItem(1)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(paramRef->getAttribute("kactus2:arrayLeft")));
    QCOMPARE(tree.topLevelItem(1)->child(0)->child(0)->childCount(), 0);

    QCOMPARE(tree.topLevelItem(1)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Right"));
    QCOMPARE(tree.topLevelItem(1)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(paramRef->getAttribute("kactus2:arrayRight")));
    QCOMPARE(tree.topLevelItem(1)->child(0)->child(1)->childCount(), 0);

    //! Test memory maps.
    QCOMPARE(tree.topLevelItem(2)->text(ParameterReferenceTree::ITEM_NAME), QString("Memory maps"));
    QCOMPARE(tree.topLevelItem(2)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(2)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(2)->child(0)->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->getName());
    QCOMPARE(tree.topLevelItem(2)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(2)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Memory remaps"));
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(2)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Default"));
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString(""));

    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Address blocks"));
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), addressRef->getName());
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 2);
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), QString("Range"));
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), expressionFormatter->formatReferringExpression(
        addressRef->getRange()));

    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 0);
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->text(
        ParameterReferenceTree::ITEM_NAME), registerRef->getName());
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    //! Test registers.
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->childCount(), 2);
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), QString("Offset"));
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(registerRef->getAddressOffset()));
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(0)->
        childCount(), 0);

    //! Test register fields.
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(1)->text(
        ParameterReferenceTree::ITEM_NAME), QString("Fields"));
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(1)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(1)->
        childCount(), 1);
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(1)->child(0)->
        text(ParameterReferenceTree::ITEM_NAME), refField->getName());
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(1)->child(0)->
        text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(1)->child(0)->
        childCount(), 1);
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(1)->child(0)->
        child(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Width"));
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(1)->child(0)->
        child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), expressionFormatter->formatReferringExpression(
        refField->getBitWidthExpression()));

    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->child(0)->child(1)->child(1)->child(0)->
        child(0)->childCount(), 0);

    //! Test address Spaces.
    QCOMPARE(tree.topLevelItem(3)->text(ParameterReferenceTree::ITEM_NAME), QString("Address Spaces"));
    QCOMPARE(tree.topLevelItem(3)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(3)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(3)->child(0)->text(ParameterReferenceTree::ITEM_NAME), addressSpaceRef->getName());
    QCOMPARE(tree.topLevelItem(3)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(3)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        localMemoryMap->getName());
    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Address blocks"));
    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString(""));

    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        spaceBlockRef->getName());
    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_NAME), QString("Base Address"));
    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), expressionFormatter->formatReferringExpression(
        spaceBlockRef->getBaseAddress()));
    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 0);

    //! Test views.
    QCOMPARE(tree.topLevelItem(4)->text(ParameterReferenceTree::ITEM_NAME), QString("Views"));
    QCOMPARE(tree.topLevelItem(4)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(4)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(4)->child(0)->text(ParameterReferenceTree::ITEM_NAME), viewRef->getName());
    QCOMPARE(tree.topLevelItem(4)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(4)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(4)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Parameters"));
    QCOMPARE(tree.topLevelItem(4)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(4)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(4)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        viewParamRef->getName());
    QCOMPARE(tree.topLevelItem(4)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(4)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(4)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Value"));
    QCOMPARE(tree.topLevelItem(4)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(viewParamRef->getValue()));
    QCOMPARE(tree.topLevelItem(4)->child(0)->child(0)->child(0)->child(0)->childCount(), 0);

    //! Test ports.
    QCOMPARE(tree.topLevelItem(5)->text(ParameterReferenceTree::ITEM_NAME), QString("Ports"));
    QCOMPARE(tree.topLevelItem(5)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(5)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(5)->child(0)->text(ParameterReferenceTree::ITEM_NAME), portRef->getName());
    QCOMPARE(tree.topLevelItem(5)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(5)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(5)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Default Value"));
    QCOMPARE(tree.topLevelItem(5)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(portRef->getDefaultValue()));
    QCOMPARE(tree.topLevelItem(5)->child(0)->child(0)->childCount(), 0);

    //! Test bus interfaces.
    QCOMPARE(tree.topLevelItem(6)->text(ParameterReferenceTree::ITEM_NAME), QString("Bus Interfaces"));
    QCOMPARE(tree.topLevelItem(6)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(6)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(6)->child(0)->text(ParameterReferenceTree::ITEM_NAME), refBusInterface->getName());
    QCOMPARE(tree.topLevelItem(6)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(6)->child(0)->childCount(), 2);

    //! Test mirrored slave.
    QCOMPARE(tree.topLevelItem(6)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Mirrored Slave Interface"));
    QCOMPARE(tree.topLevelItem(6)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(6)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(6)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Range"));
    QCOMPARE(tree.topLevelItem(6)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(mirrorSlave->getRangeID()));

    QCOMPARE(tree.topLevelItem(6)->child(0)->child(0)->child(0)->childCount(), 0);

    //! Test bus interface parameters.
    QCOMPARE(tree.topLevelItem(6)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        busIFParameter->getName());
    QCOMPARE(tree.topLevelItem(6)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    
    QCOMPARE(tree.topLevelItem(6)->child(0)->child(1)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(6)->child(0)->child(1)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Value"));
    QCOMPARE(tree.topLevelItem(6)->child(0)->child(1)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        expressionFormatter->formatReferringExpression(busIFParameter->getValue()));

    QCOMPARE(tree.topLevelItem(6)->child(0)->child(1)->child(0)->childCount(), 0);

    //! Test remap states.
    QCOMPARE(tree.topLevelItem(7)->text(ParameterReferenceTree::ITEM_NAME), QString("Remap States"));
    QCOMPARE(tree.topLevelItem(7)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(7)->childCount(), 1);

    QCOMPARE(tree.topLevelItem(7)->child(0)->text(ParameterReferenceTree::ITEM_NAME), QString("refRemapState"));
    QCOMPARE(tree.topLevelItem(7)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(7)->child(0)->childCount(), 1);

    QCOMPARE(tree.topLevelItem(7)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Remap Ports"));
    QCOMPARE(tree.topLevelItem(7)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(7)->child(0)->child(0)->childCount(), 1);

    QCOMPARE(tree.topLevelItem(7)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("remapPort[2]"));
    QCOMPARE(tree.topLevelItem(7)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString("searchedParameter"));
    QCOMPARE(tree.topLevelItem(7)->child(0)->child(0)->child(0)->childCount(), 0);
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
    referencingParameter->setBitWidthLeft(bitWidthLeft);
    referencingParameter->setBitWidthRight(bitWidthRight);
    referencingParameter->setAttribute("kactus2:arrayLeft", arrayLeft);
    referencingParameter->setAttribute("kactus2:arrayRight", arrayRight);

    return referencingParameter;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestModelParameter()
//-----------------------------------------------------------------------------
QSharedPointer<ModelParameter> tst_ParameterReferenceTree::createTestModelParameter(QString const& name,
    QString const& value, QString const& bitWidthLeft, QString const& bitWidthRight, QString const& arrayLeft,
    QString const& arrayRight)
{
    QSharedPointer<ModelParameter> referencingModelParameter(new ModelParameter);
    referencingModelParameter->setName(name);
    referencingModelParameter->setValue(value);
    referencingModelParameter->setBitWidthLeft(bitWidthLeft);
    referencingModelParameter->setBitWidthRight(bitWidthRight);
    referencingModelParameter->setAttribute("kactus2:arrayLeft", arrayLeft);
    referencingModelParameter->setAttribute("kactus2:arrayRight", arrayRight);

    return referencingModelParameter;
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
    referencingPort->setLeftBoundExpression(leftExpression);
    referencingPort->setRightBoundExpression(rightExpression);
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
    registerRef->setDimensionExpression(dimension);

    return registerRef;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestAddressBlock()
//-----------------------------------------------------------------------------
QSharedPointer<AddressBlock> tst_ParameterReferenceTree::createTestAddressBlock(QString const& name,
    QString const& baseAddress, QString const& range, QString const& width,
    QList<QSharedPointer<Register> > registers)
{
    QSharedPointer<AddressBlock> addressRef(new AddressBlock);
    addressRef->setName(name);
    addressRef->setBaseAddress(baseAddress);
    addressRef->setRange(range);
    addressRef->setWidthExpression(width);
    QList<QSharedPointer<RegisterModel> >& regModelRef(addressRef->getRegisterData());

    foreach (QSharedPointer <Register> targetRegister, registers)
    {
        regModelRef.append(targetRegister);
    }

    return addressRef;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestMemoryMap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMap> tst_ParameterReferenceTree::createTestMemoryMap(QString const& name,
    QList<QSharedPointer<AddressBlock> > addressBlocks)
{
    QSharedPointer<MemoryMap> memoryMapRef(new MemoryMap);
    memoryMapRef->setName(name);

    QList <QSharedPointer<MemoryMapItem> > memoryMapItems(memoryMapRef->getItems());

    foreach (QSharedPointer<AddressBlock> addressBlockReferer, addressBlocks)
    {
        memoryMapItems.append(addressBlockReferer);
    }

    memoryMapRef->setItems(memoryMapItems);

    return memoryMapRef;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestMemoryRemap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryRemap> tst_ParameterReferenceTree::createTestMemoryRemap(QString const& name,
    QList<QSharedPointer<AddressBlock> > addressBlocks)
{
    QSharedPointer<MemoryRemap> memoryRemapRef(new MemoryRemap);
    memoryRemapRef->setName(name);

    QList<QSharedPointer<MemoryMapItem> > memoryRemapItems(memoryRemapRef->getItems());

    foreach (QSharedPointer<AddressBlock> addressBlockReferer, addressBlocks)
    {
        memoryRemapItems.append(addressBlockReferer);
    }

    memoryRemapRef->setItems(memoryRemapItems);

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
    fieldRef->setBitOffsetExpression(offset);
    fieldRef->setBitWidthExpression(width);
    fieldRef->setIsPresentExpression(presenceExpression);

    return fieldRef;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestAddressSpace()
//-----------------------------------------------------------------------------
QSharedPointer<AddressSpace> tst_ParameterReferenceTree::createTestAddressSpace(QString const& name,
    QSharedPointer<MemoryMap> localMemoryMap)
{
    QSharedPointer<AddressSpace> addressSpaceRef(new AddressSpace);
    addressSpaceRef->setName(name);
    addressSpaceRef->setLocalMemoryMap(localMemoryMap);

    return addressSpaceRef;
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

QTEST_MAIN(tst_ParameterReferenceTree)

#include "tst_ParameterReferenceTree.moc"
