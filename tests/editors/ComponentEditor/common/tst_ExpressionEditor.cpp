//-----------------------------------------------------------------------------
// File: tst_ExpressionEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.12.2014
//
// Description:
// Unit test for class ExpressionEditor.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <QAbstractItemView>
#include <QCompleter>
#include <QLineEdit>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/ComponentParameterFinder.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <IPXACTmodels/component.h>

class tst_ExpressionEditor : public QObject
{
    Q_OBJECT

public:
    tst_ExpressionEditor();

private slots:

    void testWithoutSettingCompleter();


    void testLastWrittenWordIsCompleted();
    void testLastWrittenWordIsCompleted_data();
    
    void testSelectedCompletionIsAppendedToText();

    void testWordUnderCursorIsCompleted();
    void testWordUnderCursorIsCompleted_data();
    
    void testOnlyWordUnderCursorIsReplaced();
    void testNoCompletionIsShownForEmptyInput();
    void testCompletionsAreShownOnCtrlSpace();
    void testReferenceIDIsReplacedWithName();

    void testReferenceIDIsReplacedWithName_data();
    void testExpressionIsUpdatedForSelectedCompletion();
    void testExpressionIsUpdatedForSelectedCompletion_data();

    void testEditingExpression();
    void testEditingExpression_data();
private:

    ExpressionEditor* createEditorWithoutResolver();
    ExpressionEditor* createEditorForComponent(QSharedPointer<Component> component);
   
};

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::tst_ExpressionEditor()
//-----------------------------------------------------------------------------
tst_ExpressionEditor::tst_ExpressionEditor()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testWithoutSettingCompleter()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testWithoutSettingCompleter()
{
    ExpressionEditor* editor = createEditorWithoutResolver();

    QTest::keyClicks(editor, "one t");

    QCOMPARE(editor->getDisplayText(), QString("one t"));

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testLastWrittenWordIsCompleted()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testLastWrittenWordIsCompleted()
{
    QFETCH(QString, input);
    QFETCH(QString, expectedCompletion);

    ExpressionEditor* editor = createEditorWithoutResolver();

    QStringList availableWords;
    availableWords << "one" << "two";

    QCompleter* completer = new QCompleter(availableWords);
    editor->setAppendingCompleter(completer);

    QTest::keyClicks(editor, input);

    QVERIFY2(completer->popup()->isVisible(), "Completer popup not visible.");
    QCOMPARE(completer->currentCompletion(), expectedCompletion);

    QCOMPARE(editor->getDisplayText(), input);
    QCOMPARE(editor->getExpression(), input);

    delete editor;
    delete completer;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testLastWrittenWordIsCompleted_data()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testLastWrittenWordIsCompleted_data()
{
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expectedCompletion");

    QTest::newRow("single character input") << "o" << "one";
    QTest::newRow("second word completion") << "one t" << "two";
    QTest::newRow("third word completion") << "one two o" << "one";

    QTest::newRow("plus as a word separator") << "1+o" << "one";
    QTest::newRow("minus as a word separator") << "1-o" << "one";
    QTest::newRow("multiply as a word separator") << "1*o" << "one";
    QTest::newRow("power as a word separator") << "1**o" << "one";
    QTest::newRow("division as a word separator") << "1/o" << "one";
    QTest::newRow("opening parenthesis as a word separator") << "1+(o" << "one";
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testSelectedCompletionIsAppendedToText()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testSelectedCompletionIsAppendedToText()
{
    ExpressionEditor* editor = createEditorWithoutResolver();

    QStringList availableWords;
    availableWords << "one" << "two";

    QCompleter* completer = new QCompleter(availableWords);
    editor->setAppendingCompleter(completer);

    QTest::keyClicks(editor, "one t");

    QVERIFY2(completer->popup()->isVisible(), "Completer popup not visible.");
    QCOMPARE(completer->currentCompletion(), QString("two"));

    QTest::keyClick(completer->popup(), Qt::Key_Down);
    QTest::keyClick(completer->popup(), Qt::Key_Return);

    QCOMPARE(editor->getDisplayText(), QString("one two"));
    QCOMPARE(editor->getExpression(), QString("one two"));

    delete editor;
    delete completer;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testWordUnderCursorIsCompleted()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testWordUnderCursorIsCompleted()
{
    QFETCH(QString, initialText);
    QFETCH(int, selectedPosition);
    QFETCH(QString, input);
    QFETCH(QString, expectedCompletion);
    QFETCH(QString, expectedFinalText);

    ExpressionEditor* editor = createEditorWithoutResolver();

    QStringList availableWords;
    availableWords << "one" << "two";

    QCompleter* completer = new QCompleter(availableWords);
    editor->setAppendingCompleter(completer);

    editor->setText(initialText);
    
    QSignalSpy referenceUpdaterSpy(editor, SIGNAL(increaseReference(QString const&)));

    
    QTextCursor cursor = editor->textCursor();
    cursor.setPosition(selectedPosition);
    editor->setTextCursor(cursor);

    QTest::keyClicks(editor, input);

    QVERIFY2(completer->popup()->isVisible(), "Completer popup not visible.");
    QCOMPARE(completer->currentCompletion(), expectedCompletion);

    QTest::keyClick(completer->popup(), Qt::Key_Down);
    QTest::keyClick(completer->popup(), Qt::Key_Return);

    QCOMPARE(editor->getDisplayText(), expectedFinalText);
    QCOMPARE(editor->getExpression(), expectedFinalText);

    QCOMPARE(referenceUpdaterSpy.count(), 1);
    QCOMPARE(referenceUpdaterSpy.first().first().toString(), expectedCompletion);

    delete editor;
    delete completer;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testWordUnderCursorIsCompleted_data()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testWordUnderCursorIsCompleted_data()
{
    QTest::addColumn<QString>("initialText");
    QTest::addColumn<int>("selectedPosition");
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expectedCompletion");
    QTest::addColumn<QString>("expectedFinalText");

    QTest::newRow("Complete first word") << "o word" << 1 << "n" << "one" << "one word";

    QTest::newRow("Complete first word before plus") << "o+1" << 1 << "n" << "one" << "one+1";
    QTest::newRow("Complete first word before minus") << "o-1" << 1 << "n" << "one" << "one-1";
    QTest::newRow("Complete first word before multiply") << "o*1" << 1 << "n" << "one" << "one*1";
    QTest::newRow("Complete first word before power") << "o**1" << 1 << "n" << "one" << "one**1";
    QTest::newRow("Complete first word before division") << "o/1" << 1 << "n" << "one" << "one/1";

    QTest::newRow("Complete word in parentheses") << "(t)" << 2 << "w" << "two" << "(two)";
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testOnlyWordUnderCursorIsReplaced()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testOnlyWordUnderCursorIsReplaced()
{
    ExpressionEditor* editor = createEditorWithoutResolver();

    QStringList availableWords("one");

    QCompleter* completer = new QCompleter(availableWords);
    editor->setAppendingCompleter(completer);

    editor->setText("o on only");

    QTextCursor cursor = editor->textCursor();
    cursor.setPosition(1);
    editor->setTextCursor(cursor);

    QTest::keyClicks(editor, " o");

    QVERIFY2(completer->popup()->isVisible(), "Completer popup not visible.");
    QCOMPARE(completer->currentCompletion(), QString("one"));

    QTest::keyClick(completer->popup(), Qt::Key_Down);
    QTest::keyClick(completer->popup(), Qt::Key_Return);

    QCOMPARE(editor->getDisplayText(), QString("o one on only"));
    QCOMPARE(editor->textCursor().position(), 5);

    delete editor;
    delete completer;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testNoCompletionIsShownForEmptyInput()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testNoCompletionIsShownForEmptyInput()
{
    ExpressionEditor* editor = createEditorWithoutResolver();

    QStringList availableWords("one");

    QCompleter* completer = new QCompleter(availableWords);
    editor->setAppendingCompleter(completer);

    editor->setText("");

    QVERIFY2(!completer->popup()->isVisible(), "Completer popup should not be visible before editing.");

    QTest::keyClick(editor, Qt::Key_O);
    QTest::keyClick(editor, Qt::Key_Backspace);

    QVERIFY2(!completer->popup()->isVisible(), "Completer popup should not be visible for emptied editor.");

    delete editor;
    delete completer;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testCompletionsAreShownOnCtrlSpace()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testCompletionsAreShownOnCtrlSpace()
{
    ExpressionEditor* editor = createEditorWithoutResolver();

    QStringList availableWords("one");

    QCompleter* completer = new QCompleter(availableWords);
    editor->setAppendingCompleter(completer);

    QVERIFY2(!completer->popup()->isVisible(), "Completer popup should not be visible before editing.");

    QTest::keyClick(editor, Qt::Key_Space, Qt::ControlModifier);

    QVERIFY2(completer->popup()->isVisible(), "Completer popup should be visible when Ctrl+Space is pressed.");

    delete editor;
    delete completer;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testReferenceIDIsReplacedWithName()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testReferenceIDIsReplacedWithName()
{
    QFETCH(QString, parameterName);
    QFETCH(QString, parameterId);
    QFETCH(QString, expression);
    QFETCH(QString, expectedDisplayText);



    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName(parameterName);
    testParameter->setValueId(parameterId);
    testParameter->setValue("32");

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(testParameter);

    QSharedPointer<Component> targetComponent(new Component());
    targetComponent->setParameters(parameters);

    ExpressionEditor* editor = createEditorForComponent(targetComponent);

    editor->setExpression(expression);
    QCOMPARE(editor->getDisplayText(), expectedDisplayText);
    QCOMPARE(editor->getExpression(), expression);



    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testReferenceIDIsReplacedWithName_data()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testReferenceIDIsReplacedWithName_data()
{
    QTest::addColumn<QString>("parameterName");
    QTest::addColumn<QString>("parameterId");
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedDisplayText");

    QTest::newRow("Parameter id is replaced with name") << "testParameter" << "id" << "id" << "testParameter";

    QTest::newRow("Parameter id is replaced with name in addition") << "width" << "width_id" 
        << "width_id + 1" << "width + 1";

    QTest::newRow("Parameter id is not replaced if part of other word") << "testParameter" << "id" 
        << "pid + 1" << "pid + 1";
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testExpressionIsUpdatedForSelectedCompletion()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testExpressionIsUpdatedForSelectedCompletion()
{
    QFETCH(QString, initialExpression);
    QFETCH(QString, input);
    QFETCH(QString, expectedExpression);
    QFETCH(QString, expectedDisplayText);

    QUuid uuid("{12345678-90ab-cdef-ffff-abcdef000000}");

    QSharedPointer<Parameter> uuidParameter(new Parameter());
    uuidParameter->setName("alpha");
    uuidParameter->setValueId(uuid.toString());
    uuidParameter->setValue("a");

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("testParameter");
    testParameter->setValueId("id");
    testParameter->setValue("32");

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(uuidParameter);
    parameters.append(testParameter);

    QSharedPointer<Component> targetComponent(new Component());
    targetComponent->setParameters(parameters);

    ExpressionEditor* editor = createEditorForComponent(targetComponent);

    editor->setExpression(initialExpression);


    QTest::keyClick(editor, Qt::Key_End);
    QTest::keyClicks(editor, input);

    QTest::keyClick(editor->completer()->popup(), Qt::Key_Down);
    QTest::keyClick(editor->completer()->popup(), Qt::Key_Return);

    QCOMPARE(editor->getDisplayText(), expectedDisplayText);
    QCOMPARE(editor->getExpression(), expectedExpression);



    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testExpressionIsUpdatedForSelectedCompletion_data()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testExpressionIsUpdatedForSelectedCompletion_data()
{
    QTest::addColumn<QString>("initialExpression");
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expectedExpression");
    QTest::addColumn<QString>("expectedDisplayText");

    QTest::newRow("Parameter id is replaced with name") << "" << "te" << "id" << "testParameter";

    QTest::newRow("Adding text does not remove id") << "id" << "-1" << "id-1" << "testParameter-1";

    QTest::newRow("Adding another reference does not remove id") << "id" << "+testP" 
        << "id+id" << "testParameter+testParameter";

    QTest::newRow("Uuid as parameter id") << "{12345678-90ab-cdef-ffff-abcdef000000}" << "-1" 
        << "{12345678-90ab-cdef-ffff-abcdef000000}-1" << "alpha-1";
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testEditingExpression()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testEditingExpression()
{
    QFETCH(QString, initialExpression);
    QFETCH(int, selectedIndex);
    QFETCH(QString, input);
    QFETCH(QString, expectedExpression);
    QFETCH(QString, expectedDisplayText);

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("testParameter");
    testParameter->setValueId("{12345678-90ab-cdef-ffff-abcdef000000}");
    testParameter->setValue("32");

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(testParameter);

    QSharedPointer<Component> targetComponent(new Component());
    targetComponent->setParameters(parameters);

    ExpressionEditor* editor = createEditorForComponent(targetComponent);

    editor->setExpression(initialExpression);

    QTextCursor cursor = editor->textCursor();
    cursor.setPosition(selectedIndex);
    editor->setTextCursor(cursor);

    QTest::keyClicks(editor, input);

    QCOMPARE(editor->getDisplayText(), expectedDisplayText);
    QCOMPARE(editor->getExpression(), expectedExpression);
    QCOMPARE(editor->textColor(), QColor("red"));

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testEditingExpression_data()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testEditingExpression_data()
{
    QTest::addColumn<QString>("initialExpression");
    QTest::addColumn<int>("selectedIndex");
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expectedExpression");
    QTest::addColumn<QString>("expectedDisplayText");

    QTest::newRow("Ending non-referencing word inserts red color") << "nonReferencing" << 14 << " "
        << "nonReferencing " << "nonReferencing ";
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::createEditorWithoutResolver()
//-----------------------------------------------------------------------------
ExpressionEditor* tst_ExpressionEditor::createEditorWithoutResolver()
{
    return new ExpressionEditor(QSharedPointer<ParameterFinder>());
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::createEditorForComponent()
//-----------------------------------------------------------------------------
ExpressionEditor* tst_ExpressionEditor::createEditorForComponent(QSharedPointer<Component> component)
{
    QSharedPointer<ParameterFinder> resolver(new ComponentParameterFinder(component));

    ComponentParameterModel* parameterModel = new ComponentParameterModel(0, resolver);
    parameterModel->setExpressionParser(QSharedPointer<IPXactSystemVerilogParser>(new IPXactSystemVerilogParser(component)));

    QCompleter* completer = new QCompleter(this);
    completer->setModel(parameterModel);
    
    ExpressionEditor* editor = new ExpressionEditor(resolver);
    editor->setAppendingCompleter(completer);

    completer->setParent(editor);
    return editor;
}


QTEST_MAIN(tst_ExpressionEditor)

#include "tst_ExpressionEditor.moc"
