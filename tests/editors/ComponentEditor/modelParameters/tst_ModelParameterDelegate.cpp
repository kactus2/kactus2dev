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
#include <QLineEdit>
#include <QModelIndex>
#include <QList>
#include <QSharedPointer>
#include <QObject>
#include <QWidget>

#include <editors/ComponentEditor/modelParameters/ModelParameterDelegate.h>
#include <editors/ComponentEditor/modelParameters/modelparametermodel.h>
#include <editors/ComponentEditor/modelParameters/ModelParameterColumns.h>
#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <IPXACTmodels/component.h>
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
    void testResolveIsSelectedFromPredifinedValues();
    void testTypeIsSelectedFromPredifinedValues();
    void testChoiceIsSelectedFromChoiceNames();
    void testValueIsSelectedUsingChoiceEnumerations();
    void testValueIsSelectedUsingChoiceEnumerationsIfFormatSelected();

private:

    ModelParameterDelegate* createDelegate();

    QWidget* createEditorForColumn(int column);

    ModelParameterModel* createModelWithSimpleModelParameter();

    QSharedPointer<Choice> createChoice(QString const& choiceName) const;

    QSharedPointer<Enumeration> createEnumeration(QString const& value, QString const& text) const;

    void setType(QString const& format);

    void testValueForEditingForFormat(QString const& format);

    void testMinimumForEditingForFormat(QString const& format);

    void testMaximumForEditingForFormat(QString const& format);

    void testKeyInputForColumn(int column);
   
    void bitStringFormatTestData();

    void longFormatTestData();

    void floatFormatTestData();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    QWidget parentWidget_;

    QSharedPointer<QList<QSharedPointer<Choice> > > choices_;

    QStyleOptionViewItem option_;

    ModelParameterModel* tableModel_;
};

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::tst_ModelParameterDelegate()
//-----------------------------------------------------------------------------
tst_ModelParameterDelegate::tst_ModelParameterDelegate(): parentWidget_(), 
    choices_(new QList<QSharedPointer<Choice> >()), option_(), tableModel_(0)
{

}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::init()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::init()
{
    tableModel_ = createModelWithSimpleModelParameter();
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::cleanup()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::cleanup()
{
    choices_->clear();
    delete tableModel_;
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testUsageIsSelectedFromPredifinedValues()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testUsageIsSelectedFromPredifinedValues()
{
    QWidget* editor = createEditorForColumn(ModelParameterColumns::USAGE_TYPE);

    QComboBox* usageSelector = qobject_cast<QComboBox*>(editor);

    QVERIFY2(usageSelector != 0, "Expected combo box for selecting usage type.");

    QVERIFY2(usageSelector->findText("typed") != -1, "Expected combo box to have selection for typed.");
    QVERIFY2(usageSelector->findText("nontyped") != -1, "Expected combo box to have selection for nontyped.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testResolveIsSelectedFromPredifinedValues()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testResolveIsSelectedFromPredifinedValues()
{
    QWidget* editor = createEditorForColumn(ModelParameterColumns::RESOLVE);

    QComboBox* usageSelector = qobject_cast<QComboBox*>(editor);

    QVERIFY2(usageSelector != 0, "Expected combo box for selecting resolve type.");

    QVERIFY2(usageSelector->findText("immediate") != -1, "Expected combo box to have selection for immediate.");
    QVERIFY2(usageSelector->findText("user") != -1, "Expected combo box to have selection for user.");
    QVERIFY2(usageSelector->findText("generated") != -1, "Expected combo box to have selection for generated.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testTypeIsSelectedFromPredifinedValues()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testTypeIsSelectedFromPredifinedValues()
{
    QWidget* editor = createEditorForColumn(ModelParameterColumns::TYPE);

    QComboBox* formatSelector = qobject_cast<QComboBox*>(editor);

    QVERIFY2(formatSelector != 0, "Expected combo box for selecting format.");

    QVERIFY2(formatSelector->findText("") != -1, "Expected combo box to have selection for empty format.");
    QVERIFY2(formatSelector->findText("bit") != -1, "Expected combo box to have selection for bit.");
    QVERIFY2(formatSelector->findText("byte") != -1, "Expected combo box to have selection for byte.");
    QVERIFY2(formatSelector->findText("shortint") != -1, "Expected combo box to have selection for shortint.");
    QVERIFY2(formatSelector->findText("int") != -1, "Expected combo box to have selection for int.");
    QVERIFY2(formatSelector->findText("longint") != -1, "Expected combo box to have selection for longint.");
    QVERIFY2(formatSelector->findText("shortreal") != -1, "Expected combo box to have selection for shortreal.");
    QVERIFY2(formatSelector->findText("real") != -1, "Expected combo box to have selection for real.");
    QVERIFY2(formatSelector->findText("string") != -1, "Expected combo box to have selection for string.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testChoiceIsSelectedFromChoiceNames()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testChoiceIsSelectedFromChoiceNames()
{
    createChoice("choice1");
    createChoice("choice2");
    createChoice("choice3");
  
    QWidget* editor = createEditorForColumn(ModelParameterColumns::CHOICE);

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
    QSharedPointer<Choice> choice = createChoice("choice1");
    choice->enumerations()->append(createEnumeration("1", ""));
    choice->enumerations()->append(createEnumeration("2", "two"));

    QModelIndex choiceIndex = tableModel_->QAbstractTableModel::index(0, ModelParameterColumns::CHOICE, QModelIndex());
    QVERIFY2(tableModel_->setData(choiceIndex, "choice1", Qt::EditRole), "Could not set choice reference.");

    QWidget* editor = createEditorForColumn(ModelParameterColumns::VALUE);

    QComboBox* choiceSelector = qobject_cast<QComboBox*>(editor);

    QVERIFY2(choiceSelector != 0, "Expected combo box for selecting value using choice.");

    QVERIFY2(choiceSelector->findText("1") != -1, "Expected combo box to have selection for no choice.");
    QVERIFY2(choiceSelector->findText("2:two") != -1, "Expected combo box to have selection for choice1.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testValueIsSelectedUsingChoiceEnumerationsIfFormatSelected()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testValueIsSelectedUsingChoiceEnumerationsIfFormatSelected()
{
    setType("bool");
    testValueIsSelectedUsingChoiceEnumerations();
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testKeyInputForColumn()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testKeyInputForColumn(int column)
{
    QLineEdit* valueEditor = qobject_cast<QLineEdit*>(createEditorForColumn(column));

    QFETCH(QString, input);
    QFETCH(QString, expectedTextInEditor);

    QTest::keyClicks(valueEditor, input);

    QCOMPARE(valueEditor->text(), expectedTextInEditor);
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::createDelegate()
//-----------------------------------------------------------------------------
ModelParameterDelegate* tst_ModelParameterDelegate::createDelegate()
{
    return new ModelParameterDelegate(choices_, 0, QSharedPointer<ParameterFinder>(),
        QSharedPointer<ExpressionFormatter>(), this);
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::createModelWithSimpleModelParameter()
//-----------------------------------------------------------------------------
ModelParameterModel* tst_ModelParameterDelegate::createModelWithSimpleModelParameter()
{
    QSharedPointer<QList<QSharedPointer<ModelParameter> > > modelParameters(
        new QList<QSharedPointer<ModelParameter> >());
    modelParameters->append(QSharedPointer<ModelParameter>(new ModelParameter()));

    QSharedPointer<Component> component (new Component);
    QSharedPointer<ParameterFinder> parameterFinder(new ComponentParameterFinder(component));
    QSharedPointer<ExpressionFormatter> expressionFormatter(new ExpressionFormatter(parameterFinder));

    QSharedPointer<IPXactSystemVerilogParser> parser (new IPXactSystemVerilogParser(parameterFinder));

    return new ModelParameterModel(modelParameters, choices_, parser, parameterFinder, expressionFormatter, this);
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::createChoice()
//-----------------------------------------------------------------------------
QSharedPointer<Choice> tst_ModelParameterDelegate::createChoice(QString const& choiceName) const
{
    QSharedPointer<Choice> choice(new Choice());
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

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::createEditorForColumn()
//-----------------------------------------------------------------------------
QWidget* tst_ModelParameterDelegate::createEditorForColumn(int column)
{
    QModelIndex index = tableModel_->QAbstractTableModel::index(0, column, QModelIndex());
    
    ModelParameterDelegate* delegate = createDelegate();

    return delegate->createEditor(&parentWidget_, option_, index);	
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::setType()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::setType(QString const& format)
{
    QModelIndex choiceIndex = tableModel_->QAbstractTableModel::index(0, ModelParameterColumns::TYPE, QModelIndex());
    QVERIFY2(tableModel_->setData(choiceIndex, format, Qt::EditRole), "Could not set type.");
}


//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::bitStringFormatTestData()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::bitStringFormatTestData()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expectedTextInEditor");

    QTest::newRow("empty input") << "" << "";
    QTest::newRow("single zero") << "0" << "0";
    QTest::newRow("single number") << "9" << "";
    QTest::newRow("single character") << "c" << "";
    QTest::newRow("text") << "invalid input" << "";
    QTest::newRow("zeros and ones") << "01010101" << "01010101";
    QTest::newRow("zeros and other numbers") << "002003008" << "000000";
    QTest::newRow("zeros and ones in double quotes") << "\"0011\"" << "\"0011\"";
    QTest::newRow("zeros and ones missing first double quote") << "0011\"" << "0011";
    QTest::newRow("zeros and ones missing last double quote") << "\"0011" << "\"0011";
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::longFormatTestData()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::longFormatTestData()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expectedTextInEditor");

    QTest::newRow("empty input") << "" << "";
    QTest::newRow("single number") << "0" << "0";
    QTest::newRow("negative number") << "-1" << "-1";
    QTest::newRow("positive number") << "+1" << "+1";
    QTest::newRow("long number") << "12345678910" << "12345678910";
    QTest::newRow("expression") << "12+12" << "1212";

    QTest::newRow("single hexadecimal character") << "A" << "A";
    QTest::newRow("single non-hexadecimal character") << "z" << "";
    QTest::newRow("only characters a-f are valid") << "nonsense" << "ee";
    QTest::newRow("hexadecimal with preceding 0x") << "0xFFFF" << "0xFFFF";
    QTest::newRow("hexadecimal with preceding 0X") << "0XAAAA" << "0XAAAA";
    QTest::newRow("hexadecimal with invalid prefix") << "000x000" << "000000";
    QTest::newRow("hexadecimal with preceding #") << "#010" << "#010";

    QTest::newRow("number with magnitude suffix k") << "10k" << "10k";
    QTest::newRow("number with magnitude suffix K") << "10K" << "10K";
    QTest::newRow("number with magnitude suffix m") << "10m" << "10m";
    QTest::newRow("number with magnitude suffix M") << "10M" << "10M";
    QTest::newRow("number with magnitude suffix g") << "10g" << "10g";
    QTest::newRow("number with magnitude suffix G") << "10G" << "10G";
    QTest::newRow("number with magnitude suffix t") << "10t" << "10t";
    QTest::newRow("number with magnitude suffix T") << "10T" << "10T";
    QTest::newRow("number with invalid magnitude suffix") << "10Q" << "10";
    QTest::newRow("number with multiple magnitude suffixes") << "10Mk" << "10M";
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::floatFormatTestData()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::floatFormatTestData()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expectedTextInEditor");

    QTest::newRow("empty input") << "" << "";
    QTest::newRow("single number") << "0" << "0";
    QTest::newRow("negative number") << "-1" << "-1";
    QTest::newRow("positive number") << "+1" << "+1";
    QTest::newRow("long number") << "12345678910" << "12345678910";
    QTest::newRow("expression") << "12+12" << "1212";

    QTest::newRow("single decimal") << "1.0" << "1.0";
    QTest::newRow("four decimals") << "0.0005" << "0.0005";
    QTest::newRow("scientific format") << "2e5" << "2e5";
    QTest::newRow("scientific format with negative exponent") << "1E-3" << "1E-3";
    QTest::newRow("text is not valid") << "nonsense" << "";
}

QTEST_MAIN(tst_ModelParameterDelegate)

#include "tst_ModelParameterDelegate.moc"
