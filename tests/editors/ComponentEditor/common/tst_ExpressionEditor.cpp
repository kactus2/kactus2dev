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

Q_DECLARE_METATYPE(Qt::Key);

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

    void testCompletionChangesWithCursor();

    void testCompleteChangesInAllValuesWithKeyNavigation();
    void testCompleteChangesInAllValuesWithKeyNavigation_data();

    void testCompleteChangesInAllValuesWithMouseNavigation();

    void testCutCopyPasteIsDisabled();
    void testCutCopyPasteIsDisabled_data();

    void testAutomaticCompletionForSingleOption();

    void testColorsAreSetWhenWritingText();

    void testColorsAreSetWhenRemovingText();

    void testEditingInTheMiddleOfReferenceName();
    
    void testRemovingExpression();
    void testRemovingReference();
    void testRemovingReferenceWithBackSpaceAndDelete();

    void testReplaceReferenceWithAnother();

    void testRemoveAndReplaceSelection();
    void testRemoveAndReplaceSelectionWithReference();
    void testRemoveAndReplaceSelectionWithReference_data();

    void testEditingConstantExpression();
    void testEditingConstantExpression_data();

private:

    ExpressionEditor* createEditorWithoutFinder();
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
    ExpressionEditor* editor = createEditorWithoutFinder();

    QTest::keyClicks(editor, "one t");

    QCOMPARE(editor->toPlainText(), QString("one t"));

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testSelectedCompletionIsAppendedToText()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testSelectedCompletionIsAppendedToText()
{
    ExpressionEditor* editor = createEditorWithoutFinder();

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

    ExpressionEditor* editor = createEditorWithoutFinder();

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
    ExpressionEditor* editor = createEditorWithoutFinder();

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
    ExpressionEditor* editor = createEditorWithoutFinder();

    QStringList availableWords("one");

    QCompleter* completer = new QCompleter(availableWords);
    editor->setAppendingCompleter(completer);

    QVERIFY2(!completer->popup()->isVisible(), "Completer popup should not be visible before editing.");

    QTest::keyClick(editor, Qt::Key_Space, Qt::ControlModifier);

    QVERIFY2(completer->popup()->isVisible(), "Completer popup should be visible when Ctrl+Space is pressed.");

    QCOMPARE(editor->toPlainText(), QString(""));

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
    targetComponent->getParameters()->append(parameters);

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

    QTest::newRow("Parameter id can be uuid") << "testParameter" << "uuid_12345678_90ab_cdef_ffff_abcdef000000" 
        << "uuid_12345678_90ab_cdef_ffff_abcdef000000" << "testParameter" << "darkGreen";

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

    QString uuidString = uuid.toString();
    uuidString.remove('{');
    uuidString.remove('}');
    uuidString.replace('-','_');
    uuidString.prepend("uuid_");

    QSharedPointer<Parameter> uuidParameter(new Parameter());
    uuidParameter->setName("alpha");
    uuidParameter->setValueId(uuidString);
    uuidParameter->setValue("a");

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("testParameter");
    testParameter->setValueId("id");
    testParameter->setValue("32");

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(uuidParameter);
    parameters.append(testParameter);

    QSharedPointer<Component> targetComponent(new Component());
    targetComponent->getParameters()->append(parameters);

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

    QTest::newRow("Uuid as parameter id") << "uuid_12345678_90ab_cdef_ffff_abcdef000000" << "-1" 
        << "uuid_12345678_90ab_cdef_ffff_abcdef000000-1" << "alpha-1";
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testCompletionChangesWithCursor()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testCompletionChangesWithCursor()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("testParameter");
    testParameter->setValueId("id");
    testParameter->setValue("32");

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(testParameter);

    QSharedPointer<Component> targetComponent(new Component());
    targetComponent->getParameters()->append(parameters);

    ExpressionEditor* editor = createEditorForComponent(targetComponent);

    editor->show();
    QVERIFY(QTest::qWaitForWindowActive(editor));

    editor->setExpression("id+1+id+\"text\"");

    QTextCursor cursor = editor->textCursor();
    cursor.setPosition(4);

    QTest::mouseClick(editor->viewport(), Qt::LeftButton, 0, editor->cursorRect(cursor).center());

    QVERIFY2(editor->completer()->popup()->isVisible(), "Completer popup should be visible after clicking a word.");
    QCOMPARE(editor->completer()->currentCompletion(), QString("testParameter"));

    cursor.setPosition(testParameter->getName().length() + 1);

    QTest::mouseClick(editor->viewport(), Qt::LeftButton, 0, editor->cursorRect(cursor).center());

    QVERIFY2(!editor->completer()->popup()->isVisible(), "Completer popup should not be visible after clicking a constant.");

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testCompleteChangesInAllValuesWithKeyNavigationRight()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testCompleteChangesInAllValuesWithKeyNavigation()
{
    QFETCH(QString, expectedValue);
    QFETCH(QList<Qt::Key>, keysUsed);
    QFETCH(int, numberOfRounds);

    ExpressionEditor* editor = createEditorWithoutFinder();

    editor->show();
    QVERIFY(QTest::qWaitForWindowActive(editor));

    editor->setExpression("1+1+1+1");
    QTextCursor cursor = editor->textCursor();
    cursor.setPosition(0);

    QTest::keyClick(editor, keysUsed.at(0));

    for (int i = 0; i < numberOfRounds; ++i)
    {
        for (int j = 1; j < keysUsed.size(); ++j)
        {
            QTest::keyClick(editor, keysUsed.at(j));
            if (keysUsed.at(j) == Qt::Key_Delete)
            {
                QTest::keyClicks(editor, "20");
            }
        }
    }

    editor->finishEditingCurrentWord();

    QCOMPARE(editor->toPlainText(), expectedValue);
    QCOMPARE(editor->getExpression(), expectedValue);

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testCompleteChangesInAllValuesWithKeyNavigation_data()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testCompleteChangesInAllValuesWithKeyNavigation_data()
{
    QTest::addColumn<QString>("expectedValue");
    QTest::addColumn<QList < Qt::Key > >("keysUsed");
    QTest::addColumn<int>("numberOfRounds");

    QList<Qt::Key> keysUsedInTest;
    keysUsedInTest << Qt::Key_Home << Qt::Key_Delete << Qt::Key_Right;
    QTest::newRow("Complete all changes when using right key to navigate") << "20+20+20+20" << keysUsedInTest << 4;

    keysUsedInTest.clear();
    keysUsedInTest << Qt::Key_End << Qt::Key_Left << Qt::Key_Delete << Qt::Key_Left << Qt::Key_Left << Qt::Key_Left;
    QTest::newRow("Complete all changes when using left key to navigate") << "20+20+20+20" << keysUsedInTest << 4;

    keysUsedInTest.clear();
    keysUsedInTest << Qt::Key_End << Qt::Key_Left << Qt::Key_Delete << Qt::Key_Home << Qt::Key_Delete;
    QTest::newRow("Complete all changes when using end and home keys to navigate") << "20+1+1+20" << keysUsedInTest << 1;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testCompleteChangesInAllValuesWithMouseNavigation()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testCompleteChangesInAllValuesWithMouseNavigation()
{
    ExpressionEditor* editor = createEditorWithoutFinder();
    editor->show();
    QVERIFY(QTest::qWaitForWindowActive(editor));

    editor->setExpression("1+1+1+1");
    QTextCursor cursor = editor->textCursor();

    cursor.setPosition(0);
    QPoint clickPoint = editor->cursorRect(cursor).center();
    QTest::mouseClick(editor->viewport(), Qt::LeftButton, 0, clickPoint);
    QTest::keyClick(editor, Qt::Key_Delete);
    QTest::keyClicks(editor, "20");

    cursor.setPosition(5);
    clickPoint = editor->cursorRect(cursor).center();
    QTest::mouseClick(editor->viewport(), Qt::LeftButton, 0, clickPoint);
    QTest::keyClick(editor, Qt::Key_Delete);
    QTest::keyClicks(editor, "20");

    cursor.setPosition(3);
    clickPoint = editor->cursorRect(cursor).center();
    QTest::mouseClick(editor->viewport(), Qt::LeftButton, 0, clickPoint);
    QTest::keyClick(editor, Qt::Key_Delete);
    QTest::keyClicks(editor, "20");

    editor->finishEditingCurrentWord();

    QString expectedValue("20+20+20+1");

    QCOMPARE(editor->toPlainText(), expectedValue);
    QCOMPARE(editor->getExpression(), expectedValue);

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testCutCopyPasteIsDisabled()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testCutCopyPasteIsDisabled()
{
    QFETCH(Qt::Key, keyForCopying);

    ExpressionEditor* editor = createEditorWithoutFinder();
    editor->show();
    QVERIFY(QTest::qWaitForWindowActive(editor));

    editor->setExpression("1+2+2+1");
    QTextCursor cursor = editor->textCursor();

    QTest::keyClick(editor, Qt::Key_Home);
    QTest::keyClick(editor, Qt::Key_Right);
    for (int i = 0; i < 4; ++i)
    {
        QTest::keyClick(editor, Qt::Key_Right, Qt::ShiftModifier);
    }
    QTest::keyClick(editor, keyForCopying, Qt::ControlModifier);

    QTest::keyClick(editor, Qt::Key_End);
    QTest::keyClick(editor, Qt::Key_V, Qt::ControlModifier);

    editor->finishEditingCurrentWord();

    QString expectedValue("1+2+2+1");

    QCOMPARE(editor->toPlainText(), expectedValue);
    QCOMPARE(editor->getExpression(), expectedValue);

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testCutCopyPasteIsDisabled_data()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testCutCopyPasteIsDisabled_data()
{
    QTest::addColumn<Qt::Key>("keyForCopying");

    QTest::newRow("Copying is disabled inside the editor") << Qt::Key_C;
    QTest::newRow("Cutting is disabled inside the editor") << Qt::Key_X;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testAutomaticCompletionForSingleOption(()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testAutomaticCompletionForSingleOption()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("testParameter");
    testParameter->setValueId("id");

    QSharedPointer<Parameter> nonUniqueParameter(new Parameter());
    nonUniqueParameter->setName("none");
    nonUniqueParameter->setValueId("noId");

    QSharedPointer<Parameter> nonUniqueParameter2(new Parameter());
    nonUniqueParameter->setName("none2");
    nonUniqueParameter->setValueId("noId2");

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(testParameter);

    QSharedPointer<Component> targetComponent(new Component());
    targetComponent->getParameters()->append(parameters);

    ExpressionEditor* editor = createEditorForComponent(targetComponent);

    QTest::keyClicks(editor, "testParameter ");

    QCOMPARE(editor->getExpression(), QString("id "));

    QTextCursor cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::PreviousCharacter);
    QCOMPARE(cursor.charFormat().foreground().color(), QColor("darkGreen"));

    editor->setExpression("");

    QTest::keyClicks(editor, "none ");

    QCOMPARE(editor->getExpression(), QString("none "));

    cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::PreviousCharacter);
    QCOMPARE(cursor.charFormat().foreground().color(), QColor("red"));

    delete editor;
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
    targetComponent->getParameters()->append(parameters);

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

    QCOMPARE(editor->getExpression(), QString("id+8'hff-id "));

    QTextCursor cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::PreviousCharacter);
    QCOMPARE(cursor.charFormat().foreground().color(), QColor("darkGreen"));

    QTest::keyClicks(editor, "+ \"text\"");

    editor->finishEditingCurrentWord();

    QCOMPARE(editor->getExpression(), QString("id+8'hff-id + \"text\""));
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
    targetComponent->getParameters()->append(parameters);

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
// Function: tst_ExpressionEditor::testEditingInTheMiddleOfReferenceName()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testEditingInTheMiddleOfReferenceName()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("testParameter");
    testParameter->setValueId("id");
    testParameter->setValue("32");

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(testParameter);

    QSharedPointer<Component> targetComponent(new Component());
    targetComponent->getParameters()->append(parameters);

    ExpressionEditor* editor = createEditorForComponent(targetComponent);
    QSignalSpy referenceDecreases(editor, SIGNAL(decreaseReference(QString const&)));

    editor->setExpression("id");
    
    QTextCursor cursor = editor->textCursor();
    cursor.setPosition(4);
    editor->setTextCursor(cursor);

    QTest::keyClick(editor, Qt::Key_Space);
    QCOMPARE(editor->toPlainText(), QString("test Parameter"));
    QCOMPARE(editor->getExpression(), QString("test Parameter"));

    QCOMPARE(referenceDecreases.first().count(), 1);

    QTest::keyClick(editor, Qt::Key_Delete);
    editor->finishEditingCurrentWord();
    QCOMPARE(editor->toPlainText(), QString("test arameter"));
    QCOMPARE(editor->getExpression(), QString("test arameter"));

    QTest::keyClick(editor, Qt::Key_Backspace);
    editor->finishEditingCurrentWord();
    QCOMPARE(editor->toPlainText(), QString("testarameter"));
    QCOMPARE(editor->getExpression(), QString("testarameter"));

    QTest::keyClicks(editor, "+P");
    editor->finishEditingCurrentWord();
    QCOMPARE(editor->toPlainText(), QString("test+Parameter"));
    QCOMPARE(editor->getExpression(), QString("test+Parameter"));

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testRemovingExpression()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testRemovingExpression()
{
    QSharedPointer<Component> emptyComponent(new Component());

    ExpressionEditor* editor = createEditorForComponent(emptyComponent);
    editor->setExpression("8'hff*2");

    QSignalSpy referenceDecreases(editor, SIGNAL(decreaseReference(QString const&)));

    QTextCursor cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::End);
    editor->setTextCursor(cursor);

    for (int i = 0; i < 7; i++)
    {    
        QTest::keyClick(editor, Qt::Key_Backspace);
    }

    QCOMPARE(editor->toPlainText(), QString(""));
    QCOMPARE(editor->getExpression(), QString(""));
    QCOMPARE(referenceDecreases.count(), 0);


    editor->setExpression("2*2+2/2");

    cursor.movePosition(QTextCursor::Start);
    editor->setTextCursor(cursor);

    for (int i = 0; i < 7; i++)
    {    
        QTest::keyClick(editor, Qt::Key_Delete);
    }

    QCOMPARE(editor->toPlainText(), QString(""));
    QCOMPARE(editor->getExpression(), QString(""));
    QCOMPARE(referenceDecreases.count(), 0);

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
    targetComponent->getParameters()->append(parameters);

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
// Function: tst_ExpressionEditor::testRemovingReferenceWithBackSpaceAndDelete()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testRemovingReferenceWithBackSpaceAndDelete()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("testParameter");
    testParameter->setValueId("id");
    testParameter->setValue("32");

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(testParameter);

    QSharedPointer<Component> targetComponent(new Component());
    targetComponent->getParameters()->append(parameters);

    ExpressionEditor* editor = createEditorForComponent(targetComponent);

    QSignalSpy referenceDecreases(editor, SIGNAL(decreaseReference(QString const&)));

    editor->setExpression("id");

    QTextCursor cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::End);
    editor->setTextCursor(cursor);

    const int numberOfCharacters = QString("testParameter").length();
    for (int i = 0; i < numberOfCharacters; i++)
    {    
        QTest::keyClick(editor, Qt::Key_Backspace);
    }
    
    QCOMPARE(editor->toPlainText(), QString(""));
    QCOMPARE(editor->getExpression(), QString(""));

    QCOMPARE(referenceDecreases.first().first().toString(), QString("id"));


    referenceDecreases.clear();
    editor->setExpression("id");

    cursor.movePosition(QTextCursor::Start);
    editor->setTextCursor(cursor);

    for (int i = 0; i < numberOfCharacters; i++)
    {    
        QTest::keyClick(editor, Qt::Key_Delete);
    }

    QCOMPARE(editor->toPlainText(), QString(""));
    QCOMPARE(editor->getExpression(), QString(""));

    QCOMPARE(referenceDecreases.first().first().toString(), QString("id"));

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testReplaceReferenceWithAnother()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testReplaceReferenceWithAnother()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("testParameter");
    testParameter->setValueId("id");
    testParameter->setValue("32");

    QSharedPointer<Parameter> otherParameter(new Parameter());
    otherParameter->setName("testParameter2");
    otherParameter->setValueId("otherId");
    otherParameter->setValue("32");

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(testParameter);
    parameters.append(otherParameter);

    QSharedPointer<Component> targetComponent(new Component());
    targetComponent->getParameters()->append(parameters);

    ExpressionEditor* editor = createEditorForComponent(targetComponent);

    QSignalSpy referenceIncreases(editor, SIGNAL(increaseReference(QString const&)));
    QSignalSpy referenceDecreases(editor, SIGNAL(decreaseReference(QString const&)));

    editor->setExpression("id");
    
    QTextCursor cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::End);
    editor->setTextCursor(cursor);

    QTest::keyClicks(editor, "2");

    QVERIFY(editor->completer()->popup()->isVisible());
    QTest::keyClick(editor->completer()->popup(), Qt::Key_Down);
    QTest::keyClick(editor->completer()->popup(), Qt::Key_Return);

    QCOMPARE(editor->toPlainText(), QString("testParameter2"));
    QCOMPARE(editor->getExpression(), QString("otherId"));

    QCOMPARE(referenceIncreases.first().first().toString(), QString("otherId"));
    QCOMPARE(referenceDecreases.first().first().toString(), QString("id"));

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testRemoveAndReplaceSelection()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testRemoveAndReplaceSelection()
{
    QSharedPointer<Component> emptyComponent(new Component());

    ExpressionEditor* editor = createEditorForComponent(emptyComponent);

    editor->setExpression("8'hff+8'hff");

    QTextCursor cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    cursor.setPosition(5, QTextCursor::KeepAnchor);
    editor->setTextCursor(cursor);

    QTest::keyClick(editor, Qt::Key_Delete);
    editor->finishEditingCurrentWord();

    QCOMPARE(editor->toPlainText(), QString("+8'hff"));
    QCOMPARE(editor->getExpression(), QString("+8'hff"));

    cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    editor->setTextCursor(cursor);

    QVERIFY(editor->textCursor().hasSelection());
    QTest::keyClicks(editor, "abc");

    editor->finishEditingCurrentWord();

    QCOMPARE(editor->toPlainText(), QString("abc"));
    QCOMPARE(editor->getExpression(), QString("abc"));

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testRemoveAndReplaceSelectionWithReference()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testRemoveAndReplaceSelectionWithReference()
{
    QFETCH(QString, initialExpression);    
    QFETCH(QString, selectedText);
    QFETCH(Qt::Key, input);
    QFETCH(QString, expectedText);
    QFETCH(QString, expectedExpression);
    QFETCH(int, numberOfReferencesInSelection);

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("testParameter");
    testParameter->setValueId("veryLongReferenceName");
    testParameter->setValue("32");

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(testParameter);

    QSharedPointer<Component> targetComponent(new Component());
    targetComponent->getParameters()->append(parameters);

    ExpressionEditor* editor = createEditorForComponent(targetComponent);
    QSignalSpy referenceDecreases(editor, SIGNAL(decreaseReference(QString const&)));

    editor->setExpression(initialExpression);

    QTextCursor cursor = editor->textCursor();
    cursor.setPosition(editor->toPlainText().indexOf(selectedText), QTextCursor::MoveAnchor);
    cursor.setPosition(cursor.position() + selectedText.length(), QTextCursor::KeepAnchor);
    editor->setTextCursor(cursor);

    QTest::keyClick(editor, input);
    editor->finishEditingCurrentWord();

    QCOMPARE(editor->toPlainText(), expectedText);
    QCOMPARE(editor->getExpression(), expectedExpression);
    QCOMPARE(referenceDecreases.count(), numberOfReferencesInSelection);

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testRemoveAndReplaceSelectionWithReference_data()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testRemoveAndReplaceSelectionWithReference_data()
{
    QTest::addColumn<QString>("initialExpression");    
    QTest::addColumn<QString>("selectedText");
    QTest::addColumn<Qt::Key>("input");
    QTest::addColumn<QString>("expectedText");
    QTest::addColumn<QString>("expectedExpression");
    QTest::addColumn<int>("numberOfReferencesInSelection");

    QTest::newRow("delete whole expression") << "veryLongReferenceName+veryLongReferenceName" << 
        "testParameter+testParameter" << Qt::Key_Delete << "" << "" << 2;

    QTest::newRow("delete reference from expression") << "veryLongReferenceName+1" << 
        "testParameter+" << Qt::Key_Delete << "1" << "1" << 1;

    QTest::newRow("delete part of reference") << "1+veryLongReferenceName+1" << 
        "Parameter" << Qt::Key_Delete << "1+test+1" << "1+test+1" << 1;

    QTest::newRow("delete part of reference") << "veryLongReferenceName+1+2" << 
        "Parameter+1" << Qt::Key_Delete << "test+2" << "test+2" << 1;

    QTest::newRow("delete parts from two references") << "veryLongReferenceName+veryLongReferenceName" << 
        "Parameter+t" << Qt::Key_Delete << "testestParameter" << "testestParameter" << 2;

    QTest::newRow("replace reference") << "veryLongReferenceName" << 
        "testParameter" << Qt::Key_0 << "0" << "0" << 1;

    QTest::newRow("replace part of reference") << "veryLongReferenceName" << 
        "test" << Qt::Key_1 << "1Parameter" << "1Parameter" << 1;

    QTest::newRow("replace parts of two references") << "veryLongReferenceName+veryLongReferenceName" << 
        "Parameter+t" << Qt::Key_T << "testtestParameter" << "testtestParameter" << 2;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testEditingConstantExpression()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testEditingConstantExpression()
{
    QFETCH(QString, initialExpression);
    QFETCH(int, indexForInput);
    QFETCH(QString, input);
    QFETCH(QString, expectedExpression);

    QSharedPointer<Component> emptyComponent(new Component());

    ExpressionEditor* editor = createEditorForComponent(emptyComponent);

    editor->setExpression(initialExpression);

    QTextCursor cursor = editor->textCursor();
    cursor.setPosition(indexForInput);
    editor->setTextCursor(cursor);

    QTest::keyClicks(editor, input);

    editor->finishEditingCurrentWord();

    QCOMPARE(editor->toPlainText(), expectedExpression);
    QCOMPARE(editor->getExpression(), expectedExpression);

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::testEditingConstantExpression_data()
//-----------------------------------------------------------------------------
void tst_ExpressionEditor::testEditingConstantExpression_data()
{
    QTest::addColumn<QString>("initialExpression");
    QTest::addColumn<int>("indexForInput");
    QTest::addColumn<QString>("input");
    QTest::addColumn<QString>("expectedExpression");

    QTest::newRow("Inserting space before last term") << "1+1" << 2 << " " << "1+ 1";
    QTest::newRow("Inserting operator in constant term") << "11" << 1 << "+" << "1+1";
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::createEditorWithoutResolver()
//-----------------------------------------------------------------------------
ExpressionEditor* tst_ExpressionEditor::createEditorWithoutFinder()
{
    return new ExpressionEditor(QSharedPointer<ParameterFinder>());
}

//-----------------------------------------------------------------------------
// Function: tst_ExpressionEditor::createEditorForComponent()
//-----------------------------------------------------------------------------
ExpressionEditor* tst_ExpressionEditor::createEditorForComponent(QSharedPointer<Component> component)
{
    QSharedPointer<ParameterFinder> finder(new ComponentParameterFinder(component));

    ComponentParameterModel* parameterModel = new ComponentParameterModel(0, finder);
    parameterModel->setExpressionParser(QSharedPointer<IPXactSystemVerilogParser>(new IPXactSystemVerilogParser(finder)));

    QCompleter* completer = new QCompleter(this);
    completer->setModel(parameterModel);
    
    ExpressionEditor* editor = new ExpressionEditor(finder);
    editor->setAppendingCompleter(completer);

    completer->setParent(editor);
    return editor;
}


QTEST_MAIN(tst_ExpressionEditor)

#include "tst_ExpressionEditor.moc"
