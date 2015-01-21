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
    
    void testSelectedCompletionIsAppendedToText();

    void testWordUnderCursorIsCompleted();
    void testWordUnderCursorIsCompleted_data();

    void testNoCompletionIsShownForEmptyInput();
    void testCompletionsAreShownOnCtrlSpace();

    void testExpressionInitialization();
    void testExpressionInitialization_data();

    void testExpressionIsUpdatedForSelectedCompletion();
    void testExpressionIsUpdatedForSelectedCompletion_data();

    void testColorsAreSetWhenWritingText();

    void testColorsAreSetWhenRemovingText();

    void testReferenceCannotBeEditedInTheMiddleOfTheName();
    
    void testRemovingReference();

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

    QCOMPARE(editor->toPlainText(), QString("one t"));

    delete editor;
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

    QCOMPARE(editor->getExpression(), QString("one "));

    QVERIFY2(completer->popup()->isVisible(), "Completer popup not visible.");
    QCOMPARE(completer->currentCompletion(), QString("two"));

    QTest::keyClick(completer->popup(), Qt::Key_Down);
    QTest::keyClick(completer->popup(), Qt::Key_Return);

    QCOMPARE(editor->toPlainText(), QString("one two"));
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

    editor->setExpression(initialText);
    
    QSignalSpy referenceUpdaterSpy(editor, SIGNAL(increaseReference(QString const&)));
    
    QTextCursor cursor = editor->textCursor();
    cursor.setPosition(selectedPosition);
    editor->setTextCursor(cursor);

    QTest::keyClicks(editor, input);

    QVERIFY2(completer->popup()->isVisible(), "Completer popup not visible.");
    QCOMPARE(completer->currentCompletion(), expectedCompletion);

    QTest::keyClick(completer->popup(), Qt::Key_Down);
    QTest::keyClick(completer->popup(), Qt::Key_Return);

    QCOMPARE(editor->toPlainText(), expectedFinalText);
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

    QTest::newRow("Complete first word after opening parenthesis") << "1+(o" << 4 << "n" << "one" << "1+(one";
    QTest::newRow("Complete word in parentheses") << "(t)" << 2 << "w" << "two" << "(two)";
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

    QTest::keyClicks(editor, "o");
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
// Function: tst_ExpressionEditor::testExpressionInitialization()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testExpressionInitialization()
{
    QFETCH(QString, parameterName);
    QFETCH(QString, parameterId);
    QFETCH(QString, expression);
    QFETCH(QString, expectedDisplayText);
    QFETCH(QString, expectedColor);

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

    QCOMPARE(editor->toPlainText(), expectedDisplayText);
    QCOMPARE(editor->getExpression(), expression);
    QCOMPARE(editor->textColor(), QColor(expectedColor));

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testExpressionInitialization_data()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testExpressionInitialization_data()
{
    QTest::addColumn<QString>("parameterName");
    QTest::addColumn<QString>("parameterId");
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedDisplayText");
    QTest::addColumn<QString>("expectedColor");

    QTest::newRow("Parameter id is replaced with name and shown in darkGreen") << "testParameter" << "id" 
        << "id" << "testParameter" << "darkGreen";

    QTest::newRow("Parameter id can be uuid") << "testParameter" << "{12345678-90ab-cdef-ffff-abcdef000000}" 
        << "{12345678-90ab-cdef-ffff-abcdef000000}" << "testParameter" << "darkGreen";

    QTest::newRow("Constant is written in black") << "testParameter" << "id" 
        << "1" << "1" << "black";

    QTest::newRow("Constant with size and base is written in black") << "testParameter" << "id" 
        << "8'hFF" << "8'hFF" << "black";

    QTest::newRow("String is written in black") << "testParameter" << "id" 
        << "\"testParameter\"" << "\"testParameter\"" << "black";

    QTest::newRow("Operator is shown in black") << "testParameter" << "id" 
        << "+" << "+" << "black";

    QTest::newRow("Non-referencing word is shown in red") << "testParameter" << "id" 
        << "nonReferencing" << "nonReferencing" << "red";

    QTest::newRow("Parameter id is not replaced if part of other word") << "testParameter" << "id" 
        << "pid + 1" << "pid + 1" << "black";
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

    editor->finishEditingCurrentWord();

    QCOMPARE(editor->toPlainText(), expectedDisplayText);
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

    QTest::newRow("Adding text does not remove id") << "id" << "-8" << "id-8" << "testParameter-8";

    QTest::newRow("Adding another reference does not remove id") << "id" << "+testP" 
        << "id+id" << "testParameter+testParameter";

    QTest::newRow("Uuid as parameter id") << "{12345678-90ab-cdef-ffff-abcdef000000}" << "-1" 
        << "{12345678-90ab-cdef-ffff-abcdef000000}-1" << "alpha-1";
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testColorsAreSetWhenWritingText()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testColorsAreSetWhenWritingText()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("testParameter");
    testParameter->setValueId("id");
    testParameter->setValue("32");

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(testParameter);

    QSharedPointer<Component> targetComponent(new Component());
    targetComponent->setParameters(parameters);

    ExpressionEditor* editor = createEditorForComponent(targetComponent);

    QTest::keyClicks(editor, "tes");

    QTest::keyClick(editor->completer()->popup(), Qt::Key_Down);
    QTest::keyClick(editor->completer()->popup(), Qt::Key_Return);

    QCOMPARE(editor->getExpression(), QString("id"));
    QCOMPARE(editor->textCursor().charFormat().foreground().color(), QColor("darkGreen"));

    QTest::keyClicks(editor, "+");

    QCOMPARE(editor->getExpression(), QString("id+"));
    QCOMPARE(editor->textCursor().charFormat().foreground().color(), QColor("black"));

    QTest::keyClicks(editor, "8");
    QCOMPARE(editor->textCursor().charFormat().foreground().color(), QColor("black"));

    QTest::keyClicks(editor, "'hff");
    QCOMPARE(editor->textCursor().charFormat().foreground().color(), QColor("black"));

    QTest::keyClicks(editor, "-testParameter");

    QCOMPARE(editor->getExpression(), QString("id+8'hff-"));
    QCOMPARE(editor->textCursor().charFormat().foreground().color(), QColor("black"));

    QTest::keyClicks(editor, " ");

    QCOMPARE(editor->getExpression(), QString("id+8'hff-testParameter "));

    QTextCursor cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::PreviousCharacter);
    QCOMPARE(cursor.charFormat().foreground().color(), QColor("red"));

    QTest::keyClicks(editor, "+ \"text\"");

    editor->finishEditingCurrentWord();

    QCOMPARE(editor->getExpression(), QString("id+8'hff-testParameter + \"text\""));
    QCOMPARE(editor->textCursor().charFormat().foreground().color(), QColor("black"));

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testColorsAreSetWhenRemovingText()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testColorsAreSetWhenRemovingText()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("testParameter");
    testParameter->setValueId("id");
    testParameter->setValue("32");

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(testParameter);

    QSharedPointer<Component> targetComponent(new Component());
    targetComponent->setParameters(parameters);

    ExpressionEditor* editor = createEditorForComponent(targetComponent);

    QSignalSpy referenceDecreases(editor, SIGNAL(decreaseReference(QString const&)));

    QTest::keyClicks(editor, "tes");

    QTest::keyClick(editor->completer()->popup(), Qt::Key_Down);
    QTest::keyClick(editor->completer()->popup(), Qt::Key_Return);

    QCOMPARE(editor->getExpression(), QString("id"));
    QCOMPARE(editor->textCursor().charFormat().foreground().color(), QColor("darkGreen"));

    QTest::keyClick(editor, Qt::Key_Backspace);

    QCOMPARE(editor->getExpression(), QString("id"));

    QTest::keyClicks(editor, " ");

    QCOMPARE(editor->getExpression(), QString("testParamete "));
    QCOMPARE(referenceDecreases.first().first().toString(), QString("id"));

    QTextCursor cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::PreviousWord);
    QCOMPARE(cursor.charFormat().foreground().color(), QColor("red"));

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testReferenceCannotBeEditedInTheMiddleOfTheName()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testReferenceCannotBeEditedInTheMiddleOfTheName()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("testParameter");
    testParameter->setValueId("id");
    testParameter->setValue("32");

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(testParameter);

    QSharedPointer<Component> targetComponent(new Component());
    targetComponent->setParameters(parameters);

    ExpressionEditor* editor = createEditorForComponent(targetComponent);

    editor->setExpression("id");
    
    QTextCursor cursor = editor->textCursor();
    cursor.setPosition(4);
    editor->setTextCursor(cursor);

    QTest::keyClick(editor, Qt::Key_Space);

    QCOMPARE(editor->toPlainText(), QString("testParameter"));
    QCOMPARE(editor->getExpression(), QString("id"));

    QTest::keyClick(editor, Qt::Key_Delete);

    QCOMPARE(editor->toPlainText(), QString("testParameter"));
    QCOMPARE(editor->getExpression(), QString("id"));

    QTest::keyClick(editor, Qt::Key_Backspace);

    QCOMPARE(editor->toPlainText(), QString("testParameter"));
    QCOMPARE(editor->getExpression(), QString("id"));

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testRemovingReference()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testRemovingReference()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("testParameter");
    testParameter->setValueId("id");
    testParameter->setValue("32");

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(testParameter);

    QSharedPointer<Component> targetComponent(new Component());
    targetComponent->setParameters(parameters);

    ExpressionEditor* editor = createEditorForComponent(targetComponent);

    QSignalSpy referenceDecreases(editor, SIGNAL(decreaseReference(QString const&)));

    editor->setExpression("id");

    QTextCursor cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::End);
    editor->setTextCursor(cursor);

    QTest::keyClick(editor, Qt::Key_Backspace);

    QCOMPARE(editor->toPlainText(), QString("testParamete"));
    QCOMPARE(editor->getExpression(), QString("id"));

    editor->finishEditingCurrentWord();

    QCOMPARE(editor->toPlainText(), QString("testParamete"));
    QCOMPARE(editor->getExpression(), QString("testParamete"));

    editor->setExpression("id");

    cursor.movePosition(QTextCursor::Start);
    editor->setTextCursor(cursor);

    QTest::keyClick(editor, Qt::Key_Delete);

    QCOMPARE(editor->toPlainText(), QString("estParameter"));
    QCOMPARE(editor->getExpression(), QString("id"));

    editor->finishEditingCurrentWord();

    QCOMPARE(editor->toPlainText(), QString("estParameter"));
    QCOMPARE(editor->getExpression(), QString("estParameter"));

    delete editor;
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
