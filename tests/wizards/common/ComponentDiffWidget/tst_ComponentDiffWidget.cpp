//-----------------------------------------------------------------------------
// File: tst_ComponentDiffTreeModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.10.2014
//
// Description:
// Unit test for class ComponentDiffTreeModel.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <wizards/common/ComponentDiffWidget/ComponentDiffWidget.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>

class tst_ComponentDiffWidget : public QObject
{
    Q_OBJECT

public:
    tst_ComponentDiffWidget();

    //! Enumeration of the tree columns.
    enum COLUMNS
    {
        NAME_COLUMN = 0,
        ELEMENT_COLUMN,
        PREVIOUS_VALUE_COLUMN,
        UPDATED_VALUE_COLUMN
    };

private slots:
    void testHasFourColumns();
    void testTreeIsEmptyForNullComponents();
    void testAddingNewModelParameterCreatesTwoLevels();
    void testAddingTwoModelParameterCreatesTwoLevels();
    void testModificationShowsElementPreviousAndCurrentValue();
    void testModificationShowsElementPreviousAndCurrentValueAsChilds();
    void testOneModifiedModelParameterCreatesOneModification();
     
    void testAddedElementHasIcon();
    void testRemovedElementHasIcon();
    void testModifiedElementHasIcon();
private:
    
    void verifyItemColumnTexts(QTreeWidgetItem* item, QString const& name, QString const& element, QString const& previousValue, QString const& newValue);
   
    void addModelParameter(QSharedPointer<Component> component, QString const& name, QString const& value) const;
   

};

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::tst_ComponentDiffWidget()
//-----------------------------------------------------------------------------
tst_ComponentDiffWidget::tst_ComponentDiffWidget()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::testHasFourColumns()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::testHasFourColumns()
{
    ComponentDiffWidget widget(0);
    widget.setComponents(QSharedPointer<Component>(0), QSharedPointer<Component>(0));

    QCOMPARE(widget.columnCount(), 4);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::testTreeIsEmptyForNullComponents()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::testTreeIsEmptyForNullComponents()
{
    ComponentDiffWidget widget(0);
    
    widget.setComponents(QSharedPointer<Component>(0), QSharedPointer<Component>(0));
    
    QCOMPARE(widget.topLevelItemCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::testAddingNewModelParameterCreatesTwoLevels()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::testAddingNewModelParameterCreatesTwoLevels()
{    
    QSharedPointer<Component> reference(new Component());

    QSharedPointer<Component> subject(new Component());
    QSharedPointer<ModelParameter> modelParameter(new ModelParameter());
    modelParameter->setName("testParameter");
    subject->getModel()->addModelParameter(modelParameter);

    ComponentDiffWidget widget(0);
    widget.setComponents(reference, subject);

    QCOMPARE(widget.topLevelItemCount(), 1);

    QTreeWidgetItem* modelParametersItem = widget.topLevelItem(0);
    verifyItemColumnTexts(modelParametersItem, "Model parameters", "", "", "");    
    QCOMPARE(modelParametersItem->childCount(), 1);

    QTreeWidgetItem* modelParameterChangeItem = modelParametersItem->child(0);
    verifyItemColumnTexts(modelParameterChangeItem, "testParameter", "", "", "");   
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::testAddingTwoModelParameterCreatesTwoLevels()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::testAddingTwoModelParameterCreatesTwoLevels()
{    
    QSharedPointer<Component> reference(new Component());

    QSharedPointer<Component> subject(new Component());
    QSharedPointer<ModelParameter> modelParameter(new ModelParameter());
    modelParameter->setName("testParameter");
    subject->getModel()->addModelParameter(modelParameter);

    QSharedPointer<ModelParameter> secondParameter(new ModelParameter());
    secondParameter->setName("parameterCount");
    subject->getModel()->addModelParameter(secondParameter);

    ComponentDiffWidget widget(0);
    widget.setComponents(reference, subject);

    QCOMPARE(widget.topLevelItemCount(), 1);

    QTreeWidgetItem* modelParametersItem = widget.topLevelItem(0);
    verifyItemColumnTexts(modelParametersItem, "Model parameters", "", "", "");
    QCOMPARE(modelParametersItem->childCount(), 2);

    QTreeWidgetItem* testParameterChangeItem = modelParametersItem->child(0);
    verifyItemColumnTexts(testParameterChangeItem, "testParameter", "", "", "");

    QTreeWidgetItem* parameterCountChangeItem = modelParametersItem->child(1);
    verifyItemColumnTexts(parameterCountChangeItem, "parameterCount", "", "", "");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::testModificationShowsElementPreviousAndCurrentValue()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::testModificationShowsElementPreviousAndCurrentValue()
{    
    QSharedPointer<Component> reference(new Component());
    QSharedPointer<ModelParameter> modelParameter(new ModelParameter());
    modelParameter->setName("testParameter");
    reference->getModel()->addModelParameter(modelParameter);

    QSharedPointer<Component> subject(new Component());
    QSharedPointer<ModelParameter> modifiedModelParameter(new ModelParameter());
    modifiedModelParameter->setName("testParameter");
    modifiedModelParameter->setValue("1");    
    subject->getModel()->addModelParameter(modifiedModelParameter);

    ComponentDiffWidget widget(0);
    widget.setComponents(reference, subject);

    QCOMPARE(widget.topLevelItemCount(), 1);

    QTreeWidgetItem* modelParametersItem = widget.topLevelItem(0);
    verifyItemColumnTexts(modelParametersItem, "Model parameters", "", "", "");    
    QCOMPARE(modelParametersItem->childCount(), 1);

    QTreeWidgetItem* testParameterChangeItem = modelParametersItem->child(0);    
    verifyItemColumnTexts(testParameterChangeItem,"testParameter", "value", "", "1");
    QCOMPARE(testParameterChangeItem->childCount(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::testModificationShowsElementPreviousAndCurrentValueAsChilds()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::testModificationShowsElementPreviousAndCurrentValueAsChilds()
{    
    QSharedPointer<Component> reference(new Component());
    QSharedPointer<ModelParameter> modelParameter(new ModelParameter());
    modelParameter->setName("testParameter");
    reference->getModel()->addModelParameter(modelParameter);

    QSharedPointer<Component> subject(new Component());
    QSharedPointer<ModelParameter> modifiedModelParameter(new ModelParameter());
    modifiedModelParameter->setName("testParameter");
    modifiedModelParameter->setUsageType("nontyped");
    modifiedModelParameter->setValue("1");    
    subject->getModel()->addModelParameter(modifiedModelParameter);

    ComponentDiffWidget widget(0);
    widget.setComponents(reference, subject);

    QCOMPARE(widget.topLevelItemCount(), 1);

    QTreeWidgetItem* modelParametersItem = widget.topLevelItem(0);
    verifyItemColumnTexts(modelParametersItem, "Model parameters", "", "", "");    
    QCOMPARE(modelParametersItem->childCount(), 1);

    QTreeWidgetItem* testParameterChangeItem = modelParametersItem->child(0);
    verifyItemColumnTexts(testParameterChangeItem, "testParameter", "[multiple changes]", "", "");    

    QCOMPARE(testParameterChangeItem->childCount(), 2);

    QTreeWidgetItem* valueChangeItem = testParameterChangeItem->child(0);
    verifyItemColumnTexts(valueChangeItem, "" , "value", "", "1");    

    QTreeWidgetItem* usageChangeItem = testParameterChangeItem->child(1);
    verifyItemColumnTexts(usageChangeItem, "" , "usage type", "", "nontyped");    
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::testOneModifiedModelParameterCreatesOneModification()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::testOneModifiedModelParameterCreatesOneModification()
{
    QSharedPointer<Component> reference(new Component());
    addModelParameter(reference, "addr_width", "1");
    addModelParameter(reference, "data_width", "1");

    QSharedPointer<Component> subject(new Component());
    addModelParameter(subject, "addr_width", "32");
    addModelParameter(subject, "data_width", "1");

    ComponentDiffWidget widget(0);
    widget.setComponents(reference, subject);

    QCOMPARE(widget.topLevelItemCount(), 1);

    QTreeWidgetItem* modelParametersItem = widget.topLevelItem(0);
    verifyItemColumnTexts(modelParametersItem, "Model parameters", "", "", "");    
    QCOMPARE(modelParametersItem->childCount(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::testAddedElementHasIcon()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::testAddedElementHasIcon()
{
    QSharedPointer<Component> reference(new Component());

    QSharedPointer<Component> subject(new Component());
    addModelParameter(subject, "", "");

    ComponentDiffWidget widget(0);
    widget.setComponents(reference, subject);

    QCOMPARE(widget.topLevelItemCount(), 1);

    QTreeWidgetItem* modelParametersItem = widget.topLevelItem(0);
    QCOMPARE(modelParametersItem->childCount(), 1);

    QTreeWidgetItem* addedItem = modelParametersItem->child(0);
    QVERIFY2(!addedItem->icon(0).isNull(), "No icon set for added element.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::testRemovedElementHasIcon()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::testRemovedElementHasIcon()
{
    QSharedPointer<Component> reference(new Component());
    addModelParameter(reference, "", "");

    QSharedPointer<Component> subject(new Component());    

    ComponentDiffWidget widget(0);
    widget.setComponents(reference, subject);

    QCOMPARE(widget.topLevelItemCount(), 1);

    QTreeWidgetItem* modelParametersItem = widget.topLevelItem(0);
    QCOMPARE(modelParametersItem->childCount(), 1);

    QTreeWidgetItem* addedItem = modelParametersItem->child(0);
    QVERIFY2(!addedItem->icon(0).isNull(), "No icon set for removed element.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::testModifiedElementHasIcon()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::testModifiedElementHasIcon()
{
    QSharedPointer<Component> reference(new Component());
    addModelParameter(reference, "id", "0");

    QSharedPointer<Component> subject(new Component());    
    addModelParameter(subject, "id", "1");

    ComponentDiffWidget widget(0);
    widget.setComponents(reference, subject);

    QCOMPARE(widget.topLevelItemCount(), 1);

    QTreeWidgetItem* modelParametersItem = widget.topLevelItem(0);
    QCOMPARE(modelParametersItem->childCount(), 1);

    QTreeWidgetItem* addedItem = modelParametersItem->child(0);
    QVERIFY2(!addedItem->icon(0).isNull(), "No icon set for modified element.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::verifyItemColumnTexts()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::verifyItemColumnTexts(QTreeWidgetItem* item, QString const& name, 
    QString const& element, QString const& previousValue, QString const& newValue)
{
    QCOMPARE(item->text(NAME_COLUMN), name);
    QCOMPARE(item->text(ELEMENT_COLUMN), element);
    QCOMPARE(item->text(PREVIOUS_VALUE_COLUMN), previousValue);
    QCOMPARE(item->text(UPDATED_VALUE_COLUMN), newValue);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::addModelParameter()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::addModelParameter(QSharedPointer<Component> component, QString const& name, 
    QString const& value) const
{
    QSharedPointer<ModelParameter> otherModelParameter(new ModelParameter());
    otherModelParameter->setName(name);
    otherModelParameter->setValue(value);
    component->getModel()->addModelParameter(otherModelParameter);
}

QTEST_MAIN(tst_ComponentDiffWidget)

#include "tst_ComponentDiffWidget.moc"
