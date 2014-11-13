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

    void testValueFontColorForBoolFormat();
    void testValueFontColorForBoolFormat_data();

    void testValueFontColorForBitStringFormat();
    void testValueFontColorForBitStringFormat_data();

    void testValueFontColorForLongFormat();
    void testValueFontColorForLongFormat_data();

    void testValueFontColorForFloatFormat();
    void testValueFontColorForFloatFormat_data();


private:

    ModelParameterModel* createModelWithSimpleModelParameter();

    QSharedPointer<Choice> createChoice(QString const& choiceName) const;

    QSharedPointer<Enumeration> createEnumeration(QString const& value, QString const& text) const;

    void testFontColorForValue();

    void setFormat(QString const& format);

    QModelIndex formatIndex();

    void setValue(QString const& value);

    QModelIndex valueIndex();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    QSharedPointer<QList<QSharedPointer<Choice> > > choices_;

    QStyleOptionViewItem option_;

    ModelParameterModel* tableModel_;

    QSharedPointer<ModelParameter> modelParameter_;

};

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::tst_ModelParameterDelegate()
//-----------------------------------------------------------------------------
tst_ModelParameterDelegate::tst_ModelParameterDelegate():  
    choices_(new QList<QSharedPointer<Choice> >()), option_(), tableModel_(0), modelParameter_()
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
    modelParameter_.clear();
    choices_->clear();
    delete tableModel_;
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testValueFontColorForBoolFormat()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testValueFontColorForBoolFormat()
{
    setFormat("bool");
    testFontColorForValue();
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testFontColorForValue()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testFontColorForValue()
{
    QFETCH(QString, value);
    QFETCH(QString, expectedColor);

    setValue(value);

    QVERIFY2(tableModel_->data(valueIndex(), Qt::ForegroundRole).canConvert<QColor>(),
        "Cannot convert foreground data to color");

    QVERIFY2(tableModel_->data(valueIndex(), Qt::ForegroundRole).value<QColor>() == QColor(expectedColor),
        "Font color in value cell does not match the expected");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testValueFontColorForBoolFormat_data()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testValueFontColorForBoolFormat_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<QString>("expectedColor");

    QTest::newRow("True is acceptable") << "true" << "black";
    QTest::newRow("False is acceptable") << "false" << "black";
    QTest::newRow("Other text is not acceptable") << "int" << "red";
    QTest::newRow("Number is not acceptable") << "1" << "red";
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testValueFontColorForBitStringFormat()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testValueFontColorForBitStringFormat()
{
    setFormat("bitString");
    testFontColorForValue();
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testValueFontColorForBitStringFormat_data()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testValueFontColorForBitStringFormat_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<QString>("expectedColor");

    QTest::newRow("empty input") << "" << "red";
    QTest::newRow("single zero") << "0" << "black";
    QTest::newRow("single number") << "9" << "red";
    QTest::newRow("single character") << "c" << "red";
    QTest::newRow("text") <<"invalid input" << "red";
    QTest::newRow("zeros and ones") << "01010101" << "black";
    QTest::newRow("zeros and other numbers") << "002003008" << "red";
    QTest::newRow("zeros and ones in double quotes") << "\"0011\"" << "black";
    QTest::newRow("zeros and ones missing first double quote") << "0011\"" << "red";
    QTest::newRow("zeros and ones missing last double quote") << "\"0011" << "red";
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testValueFontColorForLongFormat()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testValueFontColorForLongFormat()
{
    setFormat("long");
    testFontColorForValue();
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testValueFontColorForLongFormat_data()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testValueFontColorForLongFormat_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<QString>("expectedColor");

    QTest::newRow("empty input") << "" << "red";
    QTest::newRow("single number") << "0" << "black";
    QTest::newRow("negative number") << "-1" << "black";
    QTest::newRow("positive number") << "+1" << "black";
    QTest::newRow("long number") << "12345678910" << "black";
    QTest::newRow("expression") << "12+12" << "red";

    QTest::newRow("single hexadecimal character") << "A" << "black";
    QTest::newRow("single non-hexadecimal character") << "z" << "red";
    QTest::newRow("only characters a-f are valid") << "nonsense" << "red";
    QTest::newRow("hexadecimal with preceding 0x") << "0xFFFF" << "black";
    QTest::newRow("hexadecimal with preceding 0X") << "0XAAAA" << "black";
    QTest::newRow("hexadecimal with invalid prefix") << "000x000" << "red";
    QTest::newRow("hexadecimal with preceding #") << "#010" << "black";

    QTest::newRow("number with magnitude suffix k") << "10k" << "black";
    QTest::newRow("number with magnitude suffix K") << "10K" << "black";
    QTest::newRow("number with magnitude suffix m") << "10m" << "black";
    QTest::newRow("number with magnitude suffix M") << "10M" << "black";
    QTest::newRow("number with magnitude suffix g") << "10g" << "black";
    QTest::newRow("number with magnitude suffix G") << "10G" << "black";
    QTest::newRow("number with magnitude suffix t") << "10t" << "black";
    QTest::newRow("number with magnitude suffix T") << "10T" << "black";
    QTest::newRow("number with invalid magnitude suffix") << "10Q" << "red";
    QTest::newRow("number with multiple magnitude suffixes") << "10Mk" << "red";
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testValueFontColorForFloatFormat()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testValueFontColorForFloatFormat()
{
    setFormat("float");
    testFontColorForValue();
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::testValueFontColorForFloatFormat_data()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::testValueFontColorForFloatFormat_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<QString>("expectedColor");

    QTest::newRow("single number") << "0" << "black";
    QTest::newRow("negative number") << "-1" << "black";
    QTest::newRow("positive number") << "+1" << "black";
    QTest::newRow("long number") << "12345678910" << "black";
    QTest::newRow("expression") << "12+12" << "red";

    QTest::newRow("single decimal") << "1.0" << "black";
    QTest::newRow("four decimals") << "0.0005" << "black";
    QTest::newRow("scientific format") << "2e5" << "black";
    QTest::newRow("scientific format with negative exponent") << "1E-3" << "black";
    QTest::newRow("text is not valid") << "nonsense" << "red";
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::createModelWithEmptyModelParameter()
//-----------------------------------------------------------------------------
ModelParameterModel* tst_ModelParameterDelegate::createModelWithSimpleModelParameter()
{
    QSharedPointer<Model> model(new Model());
    modelParameter_ = QSharedPointer<ModelParameter>(new ModelParameter());
    modelParameter_->setName("param1");
    modelParameter_->setUsageType("nontyped");

    model->addModelParameter(modelParameter_);

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

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::setFormat()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::setFormat(QString const& format)
{
    QVERIFY2(tableModel_->setData(formatIndex(), format, Qt::EditRole), "Could not set format.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::formatIndex()
//-----------------------------------------------------------------------------
QModelIndex tst_ModelParameterDelegate::formatIndex()
{
    return tableModel_->QAbstractTableModel::index(0, ModelParameterColumns::FORMAT, QModelIndex());
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::setValue()
//-----------------------------------------------------------------------------
void tst_ModelParameterDelegate::setValue(QString const& value)
{
    QVERIFY2(tableModel_->setData(valueIndex(), value, Qt::EditRole), "Could not set value.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterDelegate::valueIndex()
//-----------------------------------------------------------------------------
QModelIndex tst_ModelParameterDelegate::valueIndex()
{
    return tableModel_->QAbstractTableModel::index(0, ModelParameterColumns::VALUE, QModelIndex());
}


QTEST_MAIN(tst_ModelParameterDelegate)

#include "tst_ModelParameterModel.moc"
