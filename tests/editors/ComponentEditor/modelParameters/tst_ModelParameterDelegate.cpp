//-----------------------------------------------------------------------------
// File: tst_ModelParameterDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.11.2014
//
// Description:
// Unit test for class ModelParameterDelegate.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <QAbstractTableModel>
#include <QComboBox>
#include <QModelIndex>
#include <QList>
#include <QSharedPointer>
#include <QObject>
#include <QWidget>

#include <editors/ComponentEditor/modelParameters/ModelParameterDelegate.h>
#include <editors/ComponentEditor/modelParameters/modelparametermodel.h>
#include <editors/ComponentEditor/modelParameters/ModelParameterColumns.h>

#include <IPXACTmodels/choice.h>
#include <IPXACTmodels/Enumeration.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>

class tst_ModelParameterDelegate : public QObject
{
    Q_OBJECT

public:
    tst_ModelParameterDelegate();

private slots:
    void init();
    void cleanup();
    void testUsageIsSelectedFromPredifinedValues();
   
        void testFormatIsSelectedFromPredifinedValues();
    void testChoiceIsSelectedFromChoiceNames();

        void testValueIsSelectedUsingChoiceEnumerations();
        QSharedPointer<Enumeration> createEnumeration(QString const& value, QString const& text) const;

private:

     ModelParameterDelegate* createDelegate();

    ModelParameterModel* createModelWithEmptyModelParameter();

    QSharedPointer<Choice> createChoice(QString const& choiceName) const;
    QWidget parentWidget_;

    QSharedPointer<QList<QSharedPointer<Choice> > > choices_;

        QStyleOptionViewItem option_;
};

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::tst_ModelParameterDelegate()
//-----------------------------------------------------------------------------
tst_ModelParameterDelegate::tst_ModelParameterDelegate(): parentWidget_(), 
    choices_(new QList<QSharedPointer<Choice> >()), option_()
{

}
//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::init()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::init()
{
    choices_->clear();
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::cleanup()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::cleanup()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testUsageIsSelectedFromPredifinedValues()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testUsageIsSelectedFromPredifinedValues()
{
    ModelParameterDelegate* delegate = createDelegate();

    ModelParameterModel* tableModel = createModelWithEmptyModelParameter();

    QModelIndex usageIndex = tableModel->QAbstractTableModel::index(0, ModelParameterColumns::USAGE_TYPE, QModelIndex());

    QWidget* editor = delegate->createEditor(&parentWidget_, option_, usageIndex);

    QComboBox* usageSelector = qobject_cast<QComboBox*>(editor);

    QVERIFY2(usageSelector != 0, "Expected combo box for selecting usage type.");

    QVERIFY2(usageSelector->findText("typed") != -1, "Expected combo box to have selection for typed.");
    QVERIFY2(usageSelector->findText("nontyped") != -1, "Expected combo box to have selection for nontyped.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testFormatIsSelectedFromPredifinedValues()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testFormatIsSelectedFromPredifinedValues()
{
    ModelParameterDelegate* delegate = createDelegate();

    ModelParameterModel* tableModel = createModelWithEmptyModelParameter();

    QModelIndex formatIndex = tableModel->QAbstractTableModel::index(0, ModelParameterColumns::FORMAT, QModelIndex());
    QWidget* editor = delegate->createEditor(&parentWidget_, option_, formatIndex);

    QComboBox* formatSelector = qobject_cast<QComboBox*>(editor);

    QVERIFY2(formatSelector != 0, "Expected combo box for selecting format.");

    QVERIFY2(formatSelector->findText("") != -1, "Expected combo box to have selection for empty format.");
    QVERIFY2(formatSelector->findText("bitString") != -1, "Expected combo box to have selection for bitString.");
    QVERIFY2(formatSelector->findText("bool") != -1, "Expected combo box to have selection for bool.");
    QVERIFY2(formatSelector->findText("float") != -1, "Expected combo box to have selection for float.");
    QVERIFY2(formatSelector->findText("long") != -1, "Expected combo box to have selection for long.");
    QVERIFY2(formatSelector->findText("string") != -1, "Expected combo box to have selection for string.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testChoiceIsSelectedFromChoiceNames()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testChoiceIsSelectedFromChoiceNames()
{
    ModelParameterDelegate* delegate = createDelegate();

    ModelParameterModel* tableModel = createModelWithEmptyModelParameter();
    createChoice("choice1");
    createChoice("choice2");
    createChoice("choice3");
  
    QModelIndex choiceIndex = tableModel->QAbstractTableModel::index(0, ModelParameterColumns::CHOICE, QModelIndex());
    QWidget* editor = delegate->createEditor(&parentWidget_, option_, choiceIndex);

    QComboBox* choiceSelector = qobject_cast<QComboBox*>(editor);

    QVERIFY2(choiceSelector != 0, "Expected combo box for selecting choice.");

    QVERIFY2(choiceSelector->findText("<none>") != -1, "Expected combo box to have selection for no choice.");
    QVERIFY2(choiceSelector->findText("choice1") != -1, "Expected combo box to have selection for choice1.");
    QVERIFY2(choiceSelector->findText("choice2") != -1, "Expected combo box to have selection for choice2.");
    QVERIFY2(choiceSelector->findText("choice3") != -1, "Expected combo box to have selection for choice3.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testValueIsSelectedUsingChoiceEnumerations()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testValueIsSelectedUsingChoiceEnumerations()
{
    ModelParameterDelegate* delegate = createDelegate();

    ModelParameterModel* tableModel = createModelWithEmptyModelParameter();

    QSharedPointer<Choice> choice = createChoice("choice1");
    choice->enumerations()->append(createEnumeration("1", ""));
    choice->enumerations()->append(createEnumeration("2", "two"));

    QModelIndex choiceIndex = tableModel->QAbstractTableModel::index(0, ModelParameterColumns::CHOICE, QModelIndex());
    QVERIFY2(tableModel->setData(choiceIndex, "choice1", Qt::EditRole), "Could not set choice reference.");

    QModelIndex valueIndex = tableModel->QAbstractTableModel::index(0, ModelParameterColumns::VALUE, QModelIndex());
    QWidget* editor = delegate->createEditor(&parentWidget_, option_, valueIndex);

    QComboBox* choiceSelector = qobject_cast<QComboBox*>(editor);

    QVERIFY2(choiceSelector != 0, "Expected combo box for selecting value using choice.");

    QVERIFY2(choiceSelector->findText("1") != -1, "Expected combo box to have selection for no choice.");
    QVERIFY2(choiceSelector->findText("2:two") != -1, "Expected combo box to have selection for choice1.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::createDelegate()
//-----------------------------------------------------------------------------
ModelParameterDelegate* tst_ModelParameterDelegate::createDelegate()
{
    return new ModelParameterDelegate(choices_, this);
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::createModelWithEmptyModelParameter()
//-----------------------------------------------------------------------------
ModelParameterModel* tst_ModelParameterDelegate::createModelWithEmptyModelParameter()
{
    QSharedPointer<Model> model(new Model());
    model->addModelParameter(QSharedPointer<ModelParameter>(new ModelParameter()));

    return new ModelParameterModel(model, choices_, this);
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::createChoice()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> tst_ModelParameterDelegate::createChoice(QString const& choiceName) const
{
    QSharedPointer<Choice> choice(new Choice(QDomNode()));
    choice->setName(choiceName);

    choices_->append(choice);

    return choice;
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::createEnumeration()
//-----------------------------------------------------------------------------
QSharedPointer<Enumeration> tst_ModelParameterDelegate::createEnumeration(QString const& value, 
    QString const& text) const
{
    QSharedPointer<Enumeration> enumerationWithText(new Enumeration());
    enumerationWithText->setValue(value);
    enumerationWithText->setText(text);

    return enumerationWithText;
}

QTEST_MAIN(tst_ModelParameterDelegate)

#include "tst_ModelParameterDelegate.moc"
