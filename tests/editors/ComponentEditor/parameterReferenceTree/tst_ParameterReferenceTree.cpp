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

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/parameter.h>

#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>

#include <IPXACTmodels/register.h>
#include <IPXACTmodels/addressblock.h>

#include <IPXACTmodels/memorymap.h>

#include <QSharedPointer>

class tst_ParameterReferenceTree : public QObject
{
    Q_OBJECT

public:
    tst_ParameterReferenceTree();

private slots:
    void testEmptyComponentInConstructor();
    void testColumnCount();

    void testReferenceInParameterValueAddsThreeRows();
    void testMultipleReferencesInOneParameter();
    void testMultipleReferencesInMultipleParameters();

    void testReferenceAsAnExpressionInParameter();
    void testMultipleReferencesInSameExpression();

    void testReferenceInModelParameterValueAddsThreeRows();
    void testReferencesInParametersAndModelParameters();

    void testReferenceInViewParameterValueAddsFourRows();

    void testReferenceInPortRightboundAddsThreeRows();

    void testReferenceInRegisterDimensionAddsFiveRows();
    void testReferenceInMultipleRegistersInTwoAddressBlocks();

    void testRerefencesInMultiplePlaces();

private:
    QSharedPointer<Parameter> createTestParameter(QString const& name, QString const& value, 
        QString const& bitWidth, QString const& arraySize, QString const& arrayOffset);

    QSharedPointer<ModelParameter> createTestModelParameter(QString const& name, QString const& value,
        QString const& bitWidth, QString const& arraySize, QString const& arrayOffset);

    QSharedPointer<Port> createTestPort(QString const&name, QString const& leftExpression,
        QString const& rightExpression, QString const& defaultValue);

    QSharedPointer<Register> createTestRegister(QString const& name, QString const& offSet,
        QString const& dimension);

    QSharedPointer<AddressBlock> createTestAddressBlock(QString const& name,
        QList <QSharedPointer<Register> > registers);

    QSharedPointer<MemoryMap> createTestMemoryMap(QString const& name,
        QList <QSharedPointer<AddressBlock> > addressBlocks);
};

tst_ParameterReferenceTree::tst_ParameterReferenceTree()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testHasTwoColumns()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testEmptyComponentInConstructor()
{
    ParameterReferenceTree tree(QSharedPointer<Component>(0), QString(), 0);

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
    ParameterReferenceTree tree(QSharedPointer<Component>(0), QString(), 0);

    QCOMPARE(tree.columnCount(), 2);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testParameterReferenceAddsThreeRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInParameterValueAddsThreeRows()
{
    QString searchedID = "searched";
    QList <QSharedPointer<Parameter> > componentParameters;

    QSharedPointer<Parameter> referencingParameter = createTestParameter(
        "referencingParameter", "searched", "", "", "");

    componentParameters.append(referencingParameter);

    QSharedPointer<Component> component(new Component);
    component->setParameters(componentParameters);

    ParameterReferenceTree tree(component, searchedID);

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
        referencingParameter->getValue());

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testMultipleReferencesInParameter()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testMultipleReferencesInOneParameter()
{
    QString searchedID = "searched";
    QList <QSharedPointer<Parameter> > componentParameters;

    // Four references in a parameter
    QSharedPointer<Parameter> referencer = createTestParameter("refParam", "searched", "searched", "searched",
        "searched");

    componentParameters.append(referencer);

    QSharedPointer<Component> component(new Component);
    component->setParameters(componentParameters);

    ParameterReferenceTree tree(component, searchedID);

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Parameters"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), referencer->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 4);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Value"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        referencer->getValue());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME), QString("Bit Width"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        referencer->getBitWidth());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(2)->text(ParameterReferenceTree::ITEM_NAME), QString("Array Size"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(2)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        referencer->getAttribute("arraySize"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(3)->text(ParameterReferenceTree::ITEM_NAME), QString("Array Offset"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(3)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        referencer->getAttribute("arrayOffset"));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->childCount(), 0);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(2)->childCount(), 0);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(3)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testMultipleReferencesInMultipleParameters()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testMultipleReferencesInMultipleParameters()
{
    QString searchedID = "searched";
    QList <QSharedPointer<Parameter> > componentParameters;

    // Two references, in array size and array offset.
    QSharedPointer<Parameter> firstRef = createTestParameter("ref1", "", "", "searched", "searched");
    componentParameters.append(firstRef);
    // Three references, in bit width, array size and array offset.
    QSharedPointer<Parameter> secondRef = createTestParameter("ref2", "", "searched", "searched", "searched");
    componentParameters.append(secondRef);

    QSharedPointer<Component> component(new Component);
    component->setParameters(componentParameters);

    ParameterReferenceTree tree(component, searchedID);

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
        QString("Array Size"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        firstRef->getAttribute("arraySize"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Offset"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        firstRef->getAttribute("arrayOffset"));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->childCount(), 0);

    QCOMPARE(tree.topLevelItem(0)->child(1)->childCount(), 3);
    QCOMPARE(tree.topLevelItem(0)->child(1)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Bit Width"));
    QCOMPARE(tree.topLevelItem(0)->child(1)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        secondRef->getBitWidth());
    QCOMPARE(tree.topLevelItem(0)->child(1)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Size"));
    QCOMPARE(tree.topLevelItem(0)->child(1)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        secondRef->getAttribute("arraySize"));
    QCOMPARE(tree.topLevelItem(0)->child(1)->child(2)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Offset"));
    QCOMPARE(tree.topLevelItem(0)->child(1)->child(2)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        secondRef->getAttribute("arrayOffset"));

    QCOMPARE(tree.topLevelItem(0)->child(1)->child(0)->childCount(), 0);
    QCOMPARE(tree.topLevelItem(0)->child(1)->child(1)->childCount(), 0);
    QCOMPARE(tree.topLevelItem(0)->child(1)->child(2)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceAsAnExpressionInParameter()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceAsAnExpressionInParameter()
{
    QString searchedID = "searched";
    QList <QSharedPointer<Parameter> > componentParameters;

    // One reference, in value.
    QSharedPointer<Parameter> firstRef = createTestParameter("ref", "search + searched", "", "", "");
    componentParameters.append(firstRef);

    QSharedPointer<Component> component(new Component);
    component->setParameters(componentParameters);

    ParameterReferenceTree tree(component, searchedID);

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Parameters"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), firstRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Value"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        firstRef->getValue());

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testMultipleReferencesInSameExpression()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testMultipleReferencesInSameExpression()
{
    QString searchedID = "searched";
    QList <QSharedPointer<Parameter> > componentParameters;

    // Three references, in value.
    QSharedPointer<Parameter> firstRef = createTestParameter("ref", "searched + searched * searched", "", "", "");
    componentParameters.append(firstRef);

    QSharedPointer<Component> component(new Component);
    component->setParameters(componentParameters);

    ParameterReferenceTree tree(component, searchedID);

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Parameters"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), firstRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Value"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        firstRef->getValue());

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInModelParameterValueAddsThreeRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInModelParameterValueAddsThreeRows()
{
    QString searchedID = "searched";

    // One reference, in value.
    QSharedPointer<ModelParameter> modelRef = createTestModelParameter("modelRef", "searched", "", "" , "");

    Model* componentModel(new Model);
    componentModel->addModelParameter(modelRef);

    QSharedPointer<Component> component(new Component);
    component->setModel(componentModel);

    ParameterReferenceTree tree(component, searchedID);

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Model Parameters"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), modelRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Value"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        modelRef->getValue());

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferencesInParamersAndModelParameters()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferencesInParametersAndModelParameters()
{
    QString searchedID = "searched";

    QList<QSharedPointer<Parameter> > componentParameters;
    // One reference, in array offset.
    QSharedPointer<Parameter> paramRef = createTestParameter("paramRef", "", "", "", "searched");
    componentParameters.append(paramRef);

    // Two references, in bit width and array size.
    QSharedPointer<ModelParameter> modelRef = createTestModelParameter("modelRef", "", "searched", "searched", "");
    Model* componentModel(new Model);
    componentModel->addModelParameter(modelRef);
    
    QSharedPointer<Component> component(new Component);
    component->setParameters(componentParameters);
    component->setModel(componentModel);

    ParameterReferenceTree tree(component, searchedID);

    QCOMPARE(tree.topLevelItemCount(), 2);

    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Model Parameters"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), modelRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 2);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Bit Width"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        modelRef->getBitWidth());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Size"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        modelRef->getAttribute("arraySize"));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->childCount(), 0);

    QCOMPARE(tree.topLevelItem(1)->text(ParameterReferenceTree::ITEM_NAME), QString("Parameters"));
    QCOMPARE(tree.topLevelItem(1)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(1)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(1)->child(0)->text(ParameterReferenceTree::ITEM_NAME), paramRef->getName());
    QCOMPARE(tree.topLevelItem(1)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(1)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(1)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Offset"));
    QCOMPARE(tree.topLevelItem(1)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        paramRef->getAttribute("arrayOffset"));

    QCOMPARE(tree.topLevelItem(1)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInViewParameterValueAddsFourRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInViewParameterValueAddsFourRows()
{
    QString searchedID = "searched";

    QList<QSharedPointer<Parameter> > componentParameters;
    // One reference, in array offset.
    QSharedPointer<Parameter> paramRef = createTestParameter("paramRef", "searched", "", "", "");
    componentParameters.append(paramRef);

    View *viewRef(new View("viewRef"));
    viewRef->setParameters(componentParameters);

    QSharedPointer<Component> component(new Component);
    component->addView(viewRef);

    ParameterReferenceTree tree(component, searchedID);

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Views"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), viewRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        paramRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Value"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        paramRef->getValue());

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInPortRightboundAddsThreeRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInPortRightboundAddsThreeRows()
{
    QString searchedID = "searched";

    QSharedPointer<Port> portRef = createTestPort("portRef", "", "searched", "");

    QSharedPointer<Component> component(new Component);
    component->addPort(portRef);

    ParameterReferenceTree tree(component, searchedID);

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
        portRef->getRightBoundExpression());

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInRegisterDimensionAddsFiveRows()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInRegisterDimensionAddsFiveRows()
{
    QString searchedID = "searched";

    QList <QSharedPointer<Register> > registers;
    QSharedPointer<Register> registerRef = createTestRegister("registerRef", "test", "searched");
    registers.append(registerRef);

    QList <QSharedPointer<AddressBlock> > addressBlocks;
    QSharedPointer<AddressBlock> addressRef = createTestAddressBlock("addressBlockRef", registers);
    addressBlocks.append(addressRef);

    QList <QSharedPointer<MemoryMap> > memoryMaps;

    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef", addressBlocks);

    memoryMaps.append(memoryMapRef);

    QSharedPointer<Component> component(new Component);
    component->setMemoryMaps(memoryMaps);

    ParameterReferenceTree tree(component, searchedID);

    QCOMPARE(tree.topLevelItemCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Memory maps"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        addressRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        registerRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Dimension"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), registerRef->getDimensionExpression());

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferenceInMultipleRegisters()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testReferenceInMultipleRegistersInTwoAddressBlocks()
{
    QString searchedID = "searched";

    QList <QSharedPointer <Register> > firstRegisters;
    QSharedPointer <Register> registerOne_1 = createTestRegister("registerOne_1", "", "searched");
    firstRegisters.append(registerOne_1);
    QSharedPointer <Register> registerTwo_1 = createTestRegister("registerTwo_1", "searched", "");
    firstRegisters.append(registerTwo_1);

    QList <QSharedPointer <AddressBlock> > addressBlocks;
    QSharedPointer <AddressBlock> addressBlockOne = createTestAddressBlock("addressBlockOne", firstRegisters);
    addressBlocks.append(addressBlockOne);

    QList <QSharedPointer <Register> > secondRegisters;
    QSharedPointer <Register> registerOne_2 = createTestRegister("registerOne_2", "searched", "searched");
    secondRegisters.append(registerOne_2);

    QSharedPointer <AddressBlock> addressBlockTwo = createTestAddressBlock("addressBlockTwo", secondRegisters);
    addressBlocks.append(addressBlockTwo);

    QList <QSharedPointer<MemoryMap> > memoryMaps;
    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef", addressBlocks);
    memoryMaps.append(memoryMapRef);

    QSharedPointer<Component> component(new Component);
    component->setMemoryMaps(memoryMaps);

    ParameterReferenceTree tree(component, searchedID);

    QCOMPARE(tree.topLevelItemCount(), 1);

    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Memory maps"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);

    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 2);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        addressBlockOne->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 2);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        registerOne_1->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->childCount(), 1);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Dimension"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        registerOne_1->getDimensionExpression());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(0)->child(0)->childCount(), 0);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        registerTwo_1->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(1)->childCount(), 1);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(1)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Offset"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(1)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        registerTwo_1->getAddressOffset());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->child(1)->child(0)->childCount(), 0);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        addressBlockTwo->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->childCount(), 1);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        registerOne_2->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString(""));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->child(0)->childCount(), 2);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Offset"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), registerOne_2->getAddressOffset());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->child(0)->child(0)->childCount(), 0);

    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Dimension"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->child(0)->child(1)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), registerOne_2->getDimensionExpression());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(1)->child(0)->child(1)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::testReferencesInMultiplePlaces()
//-----------------------------------------------------------------------------
void tst_ParameterReferenceTree::testRerefencesInMultiplePlaces()
{
    QString searchedID = "searched";

    QList <QSharedPointer<Parameter> > componentParameters;
    // Parameter: Two references, in array size and array offset.
    QSharedPointer<Parameter> paramRef = createTestParameter("ref1", "", "", "searched", "searched");
    componentParameters.append(paramRef);

    // Model Parameter: One reference, in value.
    QSharedPointer<ModelParameter> modelRef = createTestModelParameter("modelRef", "searched", "", "" , "");
    Model* componentModel(new Model);
    componentModel->addModelParameter(modelRef);


    QList<QSharedPointer<Parameter> > viewParameters;
    // Parameter in view: One reference, in array offset.
    QSharedPointer<Parameter> viewParamRef = createTestParameter("viewParamRef", "searched", "", "", "");
    viewParameters.append(viewParamRef);
    View *viewRef(new View("viewRef"));
    viewRef->setParameters(viewParameters);

    // Port: One reference, in default value.
    QSharedPointer<Port> portRef = createTestPort("portRef", "", "test", "searched");

    // Register: One reference, in offset

    QList <QSharedPointer<Register> > registers;
    QSharedPointer<Register> registerRef = createTestRegister("registerRef", "searched", "test");
    registers.append(registerRef);

    QList <QSharedPointer <AddressBlock> > addressBlocks;
    QSharedPointer<AddressBlock> addressRef = createTestAddressBlock("addressBlockRef", registers);
    addressBlocks.append(addressRef);
    
    QList <QSharedPointer<MemoryMap> > memoryMaps;
    QSharedPointer<MemoryMap> memoryMapRef = createTestMemoryMap("memoryMapRef", addressBlocks);
    memoryMaps.append(memoryMapRef);

    QSharedPointer<Component> component(new Component);
    component->setParameters(componentParameters);
    component->setModel(componentModel);
    component->addView(viewRef);
    component->addPort(portRef);
    component->setMemoryMaps(memoryMaps);

    ParameterReferenceTree tree(component, searchedID);

    QCOMPARE(tree.topLevelItemCount(), 5);

    // Test model parameters.
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_NAME), QString("Model Parameters"));
    QCOMPARE(tree.topLevelItem(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME), modelRef->getName());
    QCOMPARE(tree.topLevelItem(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Value"));
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        modelRef->getValue());
    QCOMPARE(tree.topLevelItem(0)->child(0)->child(0)->childCount(), 0);

    // Test parameters.
    QCOMPARE(tree.topLevelItem(1)->text(ParameterReferenceTree::ITEM_NAME), QString("Parameters"));
    QCOMPARE(tree.topLevelItem(1)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(1)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(1)->child(0)->text(ParameterReferenceTree::ITEM_NAME), paramRef->getName());
    QCOMPARE(tree.topLevelItem(1)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(1)->child(0)->childCount(), 2);
    QCOMPARE(tree.topLevelItem(1)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Size"));
    QCOMPARE(tree.topLevelItem(1)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        paramRef->getAttribute("arraySize"));
    QCOMPARE(tree.topLevelItem(1)->child(0)->child(0)->childCount(), 0);

    QCOMPARE(tree.topLevelItem(1)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Array Offset"));
    QCOMPARE(tree.topLevelItem(1)->child(0)->child(1)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        paramRef->getAttribute("arrayOffset"));
    QCOMPARE(tree.topLevelItem(1)->child(0)->child(1)->childCount(), 0);

    // Test memory maps.
    QCOMPARE(tree.topLevelItem(2)->text(ParameterReferenceTree::ITEM_NAME), QString("Memory maps"));
    QCOMPARE(tree.topLevelItem(2)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(2)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(2)->child(0)->text(ParameterReferenceTree::ITEM_NAME), memoryMapRef->getName());
    QCOMPARE(tree.topLevelItem(2)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(2)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        addressRef->getName());
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        registerRef->getName());
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        QString(""));

    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Offset"));
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->text(
        ParameterReferenceTree::ITEM_EXPRESSION), registerRef->getAddressOffset());
    QCOMPARE(tree.topLevelItem(2)->child(0)->child(0)->child(0)->child(0)->childCount(), 0);

    // Test views.
    QCOMPARE(tree.topLevelItem(3)->text(ParameterReferenceTree::ITEM_NAME), QString("Views"));
    QCOMPARE(tree.topLevelItem(3)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(3)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(3)->child(0)->text(ParameterReferenceTree::ITEM_NAME), viewRef->getName());
    QCOMPARE(tree.topLevelItem(3)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(3)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        viewParamRef->getName());
    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Value"));
    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        viewParamRef->getValue());
    QCOMPARE(tree.topLevelItem(3)->child(0)->child(0)->child(0)->childCount(), 0);

    // Test ports.
    QCOMPARE(tree.topLevelItem(4)->text(ParameterReferenceTree::ITEM_NAME), QString("Ports"));
    QCOMPARE(tree.topLevelItem(4)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(4)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(4)->child(0)->text(ParameterReferenceTree::ITEM_NAME), portRef->getName());
    QCOMPARE(tree.topLevelItem(4)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION), QString(""));

    QCOMPARE(tree.topLevelItem(4)->child(0)->childCount(), 1);
    QCOMPARE(tree.topLevelItem(4)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_NAME),
        QString("Default Value"));
    QCOMPARE(tree.topLevelItem(4)->child(0)->child(0)->text(ParameterReferenceTree::ITEM_EXPRESSION),
        portRef->getDefaultValue());
    QCOMPARE(tree.topLevelItem(4)->child(0)->child(0)->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> tst_ParameterReferenceTree::createTestParameter (QString const& name, 
    QString const& value, QString const& bitWidth, QString const& arraySize, QString const& arrayOffset)
{
    QSharedPointer<Parameter> referencingParameter(new Parameter);
    referencingParameter->setName(name);
    referencingParameter->setValue(value);
    referencingParameter->setBitWidth(bitWidth);
    referencingParameter->setAttribute("arraySize", arraySize);
    referencingParameter->setAttribute("arrayOffset", arrayOffset);

    return referencingParameter;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestModelParameter()
//-----------------------------------------------------------------------------
QSharedPointer<ModelParameter> tst_ParameterReferenceTree::createTestModelParameter(QString const& name,
    QString const& value, QString const& bitWidth, QString const& arraySize, QString const& arrayOffset)
{
    QSharedPointer<ModelParameter> referencingModelParameter(new ModelParameter);
    referencingModelParameter->setName(name);
    referencingModelParameter->setValue(value);
    referencingModelParameter->setBitWidth(bitWidth);
    referencingModelParameter->setAttribute("arraySize", arraySize);
    referencingModelParameter->setAttribute("arrayOffset", arrayOffset);

    return referencingModelParameter;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReferenceTree::createTestPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> tst_ParameterReferenceTree::createTestPort(QString const&name,
    QString const& leftExpression, QString const& rightExpression, QString const& defaultValue)
{
    QSharedPointer<Port> referencingPort(new Port);
    referencingPort->setName(name);
    referencingPort->setLeftBoundExpression(leftExpression);
    referencingPort->setRightBoundExpression(rightExpression);
    referencingPort->setDefaultValue(defaultValue);

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
    QList<QSharedPointer<Register> > registers)
{
    QSharedPointer<AddressBlock> addressRef(new AddressBlock);
    addressRef->setName(name);
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

QTEST_MAIN(tst_ParameterReferenceTree)

#include "tst_ParameterReferenceTree.moc"
