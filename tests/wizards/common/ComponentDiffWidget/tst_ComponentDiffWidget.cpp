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
    void testAddingModelParameterCreatesTwoLevels();

    void testAddingTwoModelParameterCreatesTwoLevels();
    void testModificationShowsElementPreviousAndCurrentValue();
    void testModificationShowsElementPreviousAndCurrentValueAsChilds();
    void testOneModifiedModelParameterCreatesOneModification();
     
    void testAddedElementHasIcon();
    void testRemovedElementHasIcon();
    void testModifiedElementHasIcon();

    void testAddingViewCreatesTwoLevels();
    void testViewModelNameChangeShowsPreviousAndCurrentValue();
    
     void testViewChangesShowsAsChilds();

    void testAddingDifferentTypeElements();
private:
    
    void verifyItemColumnTexts(QTreeWidgetItem* item, QString const& name, QString const& element,
        QString const& previousValue, QString const& newValue);
   
    void addModelParameter(QSharedPointer<Component> component, QString const& name, QString const& value) const;
   
    void addView(QSharedPointer<Component> component, QString const& viewName);
   
    QTreeWidgetItem * getModelParametersItem(ComponentDiffWidget const& widget);

    void verifyModelParametersItem(ComponentDiffWidget const& widget);

    void verifyModelParameterChangeItem(ComponentDiffWidget const& widget, int itemIndex, QString const& expectedName);

    QTreeWidgetItem* getViewsItem(ComponentDiffWidget const& widget) const;
    QTreeWidgetItem* getTopLevelItemByName(ComponentDiffWidget const& widget, QString const& name) const;

    QTreeWidgetItem* getChildItemByName(QString const& name, QTreeWidgetItem* parentItem) const;


    void verifyViewsItem(ComponentDiffWidget const& widget);
   
    void verifyHasChildWithColumns(QTreeWidgetItem* viewChangeItem, QString const& expectedName,
        QString const& expectedElement, QString const& expectedOldValue, QString const& expectedNewValue) const;

    QTreeWidgetItem* getChildItemByElement(QString const& elementName, QTreeWidgetItem* parentItem);

    void addPort(QSharedPointer<Component> component, QString const& portName);

    void verifyPortsItem(ComponentDiffWidget const& widget);

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
void tst_ComponentDiffWidget::testAddingModelParameterCreatesTwoLevels()
{    
    QSharedPointer<Component> reference(new Component());

    QSharedPointer<Component> subject(new Component());
    addModelParameter(subject, "testParameter", "");

    ComponentDiffWidget widget(0);
    widget.setComponents(reference, subject);

    QCOMPARE(widget.topLevelItemCount(), 1);

    verifyModelParametersItem(widget);

    QCOMPARE(getModelParametersItem(widget)->childCount(), 1);

    verifyModelParameterChangeItem(widget, 0, "testParameter");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::testAddingTwoModelParameterCreatesTwoLevels()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::testAddingTwoModelParameterCreatesTwoLevels()
{    
    QSharedPointer<Component> reference(new Component());

    QSharedPointer<Component> subject(new Component());
    addModelParameter(subject, "testParameter", "");
    addModelParameter(subject, "parameterCount", "");

    ComponentDiffWidget widget(0);
    widget.setComponents(reference, subject);

    QCOMPARE(widget.topLevelItemCount(), 1);

    verifyModelParametersItem(widget);
    QCOMPARE(getModelParametersItem(widget)->childCount(), 2);

    verifyModelParameterChangeItem(widget, 0, "testParameter");
    verifyModelParameterChangeItem(widget, 1, "parameterCount");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::testModificationShowsElementPreviousAndCurrentValue()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::testModificationShowsElementPreviousAndCurrentValue()
{    
    QSharedPointer<Component> reference(new Component());
    addModelParameter(reference, "testParameter", "");

    QSharedPointer<Component> subject(new Component());
    addModelParameter(subject, "testParameter", "1");

    ComponentDiffWidget widget(0);
    widget.setComponents(reference, subject);

    QCOMPARE(widget.topLevelItemCount(), 1);

    verifyModelParametersItem(widget);
    QCOMPARE(getModelParametersItem(widget)->childCount(), 1);

    QTreeWidgetItem* testParameterChangeItem = getChildItemByName("testParameter", getModelParametersItem(widget));
    verifyItemColumnTexts(testParameterChangeItem, "testParameter", "value", "", "1");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::testModificationShowsElementPreviousAndCurrentValueAsChilds()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::testModificationShowsElementPreviousAndCurrentValueAsChilds()
{    
    QSharedPointer<Component> reference(new Component());
    addModelParameter(reference, "testParameter", "");

    QSharedPointer<Component> subject(new Component());
    addModelParameter(subject, "testParameter", "1");
    
    QSharedPointer<ModelParameter> modifiedModelParameter = subject->getModelParameters().first();
    modifiedModelParameter->setUsageType("nontyped");

    ComponentDiffWidget widget(0);
    widget.setComponents(reference, subject);

    QCOMPARE(widget.topLevelItemCount(), 1);

    verifyModelParametersItem(widget);
    QCOMPARE(getModelParametersItem(widget)->childCount(), 1);

    verifyHasChildWithColumns(getModelParametersItem(widget), "testParameter", "[multiple changes]", "", "");

    QTreeWidgetItem* testParameterChangeItem = getChildItemByName("testParameter", getModelParametersItem(widget));
    QCOMPARE(testParameterChangeItem->childCount(), 2);

    QTreeWidgetItem* valueChangeItem =  getChildItemByElement("value", testParameterChangeItem);
    verifyItemColumnTexts(valueChangeItem, "" , "value", "", "1");    

    QTreeWidgetItem* usageChangeItem = getChildItemByElement("usage type", testParameterChangeItem);
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

    verifyModelParametersItem(widget);
    QCOMPARE(getModelParametersItem(widget)->childCount(), 1);
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

    QTreeWidgetItem* addedItem = getModelParametersItem(widget)->child(0);
    QVERIFY2(!addedItem->icon(NAME_COLUMN).isNull(), "No icon set for added element.");
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

    QTreeWidgetItem* addedItem = getModelParametersItem(widget)->child(0);
    QVERIFY2(!addedItem->icon(NAME_COLUMN).isNull(), "No icon set for removed element.");
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

    QTreeWidgetItem* addedItem = getModelParametersItem(widget)->child(0);
    QVERIFY2(!addedItem->icon(NAME_COLUMN).isNull(), "No icon set for modified element.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::testAddingViewCreatesTwoLevels()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::testAddingViewCreatesTwoLevels()
{
    QSharedPointer<Component> reference(new Component());

    QSharedPointer<Component> subject(new Component());    
    addView(subject, "testView");

    ComponentDiffWidget widget(0);
    widget.setComponents(reference, subject);

    verifyViewsItem(widget);

    QCOMPARE(getViewsItem(widget)->childCount(), 1);

    QTreeWidgetItem* modelParameterChangeItem = getViewsItem(widget)->child(0);
    verifyItemColumnTexts(modelParameterChangeItem, "testView", "", "", "");   
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::testViewModelNameChangeShowsPreviousAndCurrentValue()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::testViewModelNameChangeShowsPreviousAndCurrentValue()
{    
    QSharedPointer<Component> reference(new Component());
    addView(reference, "view1");
    reference->getViews().first()->setModelName("old");

    QSharedPointer<Component> subject(new Component());
    addView(subject, "view1");
    subject->getViews().first()->setModelName("new");

    ComponentDiffWidget widget(0);
    widget.setComponents(reference, subject);

    verifyViewsItem(widget);
    QCOMPARE(getViewsItem(widget)->childCount(), 1);

    verifyHasChildWithColumns(getViewsItem(widget), "view1", "model name", "old", "new");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::testViewChangesShowsAsChilds()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::testViewChangesShowsAsChilds()
{    
    QSharedPointer<Component> reference(new Component());
    View* referenceView = new View("view1");
    referenceView->setEnvIdentifiers(QStringList("old Env"));
    referenceView->setLanguage("old language");
    referenceView->setFileSetRefs(QStringList("old FileSetRef"));
    referenceView->setModelName("old model");
    reference->addView(referenceView);

    QSharedPointer<Component> subject(new Component());
    View* subjectView = new View("view1");
    subjectView->setEnvIdentifiers(QStringList("new Env"));
    subjectView->setLanguage("new language");
    subjectView->setFileSetRefs(QStringList("new FileSetRef"));
    subjectView->setModelName("new model");
    subject->addView(subjectView);

    ComponentDiffWidget widget(0);
    widget.setComponents(reference, subject);

    verifyViewsItem(widget);

    QTreeWidgetItem* viewChangeItem = getViewsItem(widget)->child(0);
    verifyItemColumnTexts(viewChangeItem, "view1", "[multiple changes]", "", "");

    verifyHasChildWithColumns(viewChangeItem, "", "environment identifiers", "old Env", "new Env");
    verifyHasChildWithColumns(viewChangeItem, "", "language", "old language", "new language");
    verifyHasChildWithColumns(viewChangeItem, "", "model name", "old model", "new model");        
    verifyHasChildWithColumns(viewChangeItem, "", "file set ref", "old FileSetRef", "new FileSetRef");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::testAddingDifferentTypeElements()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::testAddingDifferentTypeElements()
{
    QSharedPointer<Component> reference(new Component());

    QSharedPointer<Component> subject(new Component());    
    addModelParameter(subject, "testModelParameter", "");
    addView(subject, "testView");
    addPort(subject, "testPort");

    ComponentDiffWidget widget(0);
    widget.setComponents(reference, subject);

    QCOMPARE(widget.topLevelItemCount(), 3);

    verifyModelParametersItem(widget);
    QCOMPARE(getModelParametersItem(widget)->childCount(), 1);

    verifyViewsItem(widget);
    QCOMPARE(getViewsItem(widget)->childCount(), 1);

    verifyPortsItem(widget);
    QCOMPARE(getTopLevelItemByName(widget, "Ports")->childCount(), 1);
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

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::addPort()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::addPort(QSharedPointer<Component> component, QString const& portName)
{
    QSharedPointer<Port> port(new Port());
    port->setName(portName);

    component->addPort(port);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::addView()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::addView(QSharedPointer<Component> component, QString const& viewName)
{
    component->addView(new View(viewName));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::verifyModelParametersItem()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::verifyModelParametersItem(ComponentDiffWidget const& widget)
{
    verifyHasChildWithColumns(widget.invisibleRootItem(), "Model parameters", "", "", "");
    for (int i = 0; i < widget.columnCount(); i++)
    {
        QVERIFY2(getModelParametersItem(widget)->backgroundColor(i) == QColor(230, 230, 230), 
            "Item 'Model parameters' does not have gray background."); 
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::getModelParametersItem()
//-----------------------------------------------------------------------------
QTreeWidgetItem * tst_ComponentDiffWidget::getModelParametersItem(ComponentDiffWidget const& widget)
{
    return getTopLevelItemByName(widget, "Model parameters");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::verfyModelParameterChangeItem()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::verifyModelParameterChangeItem(ComponentDiffWidget const& widget, int itemIndex,
    QString const& expectedName)
{
    QTreeWidgetItem* modelParameterChangeItem = getModelParametersItem(widget)->child(itemIndex);
    verifyItemColumnTexts(modelParameterChangeItem, expectedName, "", "", "");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::verifyViewsItem()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::verifyViewsItem(ComponentDiffWidget const& widget)
{
    verifyHasChildWithColumns(widget.invisibleRootItem(), "Views", "", "", "");
    for (int i = 0; i < widget.columnCount(); i++)
    {
        QVERIFY2(getViewsItem(widget)->backgroundColor(i) == QColor(230, 230, 230), 
            "Item 'Views' does not have gray background."); 
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::getViewsItem()
//-----------------------------------------------------------------------------
QTreeWidgetItem* tst_ComponentDiffWidget::getViewsItem(ComponentDiffWidget const& widget) const
{
    return getTopLevelItemByName(widget, "Views");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::getTopLevelItemByName()
//-----------------------------------------------------------------------------
QTreeWidgetItem* tst_ComponentDiffWidget::getTopLevelItemByName(ComponentDiffWidget const& widget, 
    QString const& name) const
{
    for (int i = 0; i < widget.topLevelItemCount(); i++)
    {
        if (widget.topLevelItem(i)->text(NAME_COLUMN) == name)
        {
            return widget.topLevelItem(i);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::verifyHasChildWithColumns()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::verifyHasChildWithColumns(QTreeWidgetItem* viewChangeItem, 
    QString const& expectedName, QString const& expectedElement, 
    QString const& expectedOldValue, QString const& expectedNewValue) const
{
    for (int i = 0; i < viewChangeItem->childCount(); i++)
    {
        QTreeWidgetItem* childItem = viewChangeItem->child(i);
        if (childItem->text(NAME_COLUMN) == expectedName &&
            childItem->text(ELEMENT_COLUMN) == expectedElement &&
            childItem->text(PREVIOUS_VALUE_COLUMN) == expectedOldValue &&
            childItem->text(UPDATED_VALUE_COLUMN) == expectedNewValue)
        {
            return;
        }
    }

     QFAIL(QString("Item '%1' has no child with name '%2', element '%3', previous value of '%4' and "
         "updated value of '%5'").arg(viewChangeItem->text(NAME_COLUMN), expectedName, expectedElement, 
         expectedOldValue, expectedOldValue).toLocal8Bit());
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::getChildItemByName()
//-----------------------------------------------------------------------------
QTreeWidgetItem* tst_ComponentDiffWidget::getChildItemByName(QString const& name, 
    QTreeWidgetItem* parentItem) const
{
    for (int i = 0; i < parentItem->childCount(); i++)
    {
        if (parentItem->child(i)->text(NAME_COLUMN) == name)
        {
            return parentItem->child(i);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::getChildItemByElement()
//-----------------------------------------------------------------------------
QTreeWidgetItem* tst_ComponentDiffWidget::getChildItemByElement(QString const& elementName, QTreeWidgetItem* parentItem)
{
    for (int i = 0; i < parentItem->childCount(); i++)
    {
        if (parentItem->child(i)->text(ELEMENT_COLUMN) == elementName)
        {
            return parentItem->child(i);
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentDiffWidget::verifyPortsItem()
//-----------------------------------------------------------------------------
void tst_ComponentDiffWidget::verifyPortsItem(ComponentDiffWidget const& widget)
{
    verifyHasChildWithColumns(widget.invisibleRootItem(), "Ports", "", "", "");
    for (int i = 0; i < widget.columnCount(); i++)
    {
        QVERIFY2(getViewsItem(widget)->backgroundColor(i) == QColor(230, 230, 230), 
            "Item 'Ports' does not have gray background."); 
    }
}

QTEST_MAIN(tst_ComponentDiffWidget)

#include "tst_ComponentDiffWidget.moc"
