//-----------------------------------------------------------------------------
// File: tst_AppendingEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.12.2014
//
// Description:
// Unit test for class AppendingEditor.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <QAbstractItemView>
#include <QCompleter>
#include <QLineEdit>

#include <editors/ComponentEditor/common/AppendingEditor.h>

class tst_AppendingEditor : public QObject
{
    Q_OBJECT

public:
    tst_AppendingEditor();

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

};

//-----------------------------------------------------------------------------
// Function: tst_AppendingEditor::tst_AppendingEditor()
//-----------------------------------------------------------------------------
tst_AppendingEditor::tst_AppendingEditor()
{

}

//-----------------------------------------------------------------------------
// Function: tst_AppendingEditor::testWithoutSettingCompleter()
//-----------------------------------------------------------------------------
void tst_AppendingEditor::testWithoutSettingCompleter()
{
    AppendingEditor* editor = new AppendingEditor();

    QTest::keyClicks(editor, "one t");

    QCOMPARE(editor->text(), QString("one t"));

    QVERIFY2(editor->completer() == 0, "Should have no completer");

    delete editor;
}

//-----------------------------------------------------------------------------
// Function: tst_AppendingEditor::testLastWrittenWordIsCompleted()
//-----------------------------------------------------------------------------
void tst_AppendingEditor::testLastWrittenWordIsCompleted()
{
    QFETCH(QString, input);
    QFETCH(QString, expectedCompletion);

    AppendingEditor* editor = new AppendingEditor();

    QStringList availableWords;
    availableWords << "one" << "two";

    QCompleter* completer = new QCompleter(availableWords);
    editor->setAppendingCompleter(completer);

    QTest::keyClicks(editor, input);

    QVERIFY2(completer->popup()->isVisible(), "Completer popup not visible.");
    QCOMPARE(completer->currentCompletion(), expectedCompletion);

    QCOMPARE(editor->text(), input);

    delete editor;
    delete completer;
}

//-----------------------------------------------------------------------------
// Function: tst_AppendingEditor::testLastWrittenWordIsCompleted_data()
//-----------------------------------------------------------------------------
void tst_AppendingEditor::testLastWrittenWordIsCompleted_data()
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
// Function: tst_AppendingEditor::testSelectedCompletionIsAppendedToText()
//-----------------------------------------------------------------------------
void tst_AppendingEditor::testSelectedCompletionIsAppendedToText()
{
    AppendingEditor* editor = new AppendingEditor();

    QStringList availableWords;
    availableWords << "one" << "two";

    QCompleter* completer = new QCompleter(availableWords);
    editor->setAppendingCompleter(completer);

    QTest::keyClicks(editor, "one t");

    QVERIFY2(completer->popup()->isVisible(), "Completer popup not visible.");
    QCOMPARE(completer->currentCompletion(), QString("two"));

    QTest::keyClick(completer->popup(), Qt::Key_Down);
    QTest::keyClick(completer->popup(), Qt::Key_Return);

    QCOMPARE(editor->text(), QString("one two"));

    delete editor;
    delete completer;
}

//-----------------------------------------------------------------------------
// Function: tst_AppendingEditor::testWordUnderCursorIsCompleted()
//-----------------------------------------------------------------------------
void tst_AppendingEditor::testWordUnderCursorIsCompleted()
{
    QFETCH(QString, initialText);
    QFETCH(int, selectedPosition);
    QFETCH(QString, input);
    QFETCH(QString, expectedCompletion);
    QFETCH(QString, expectedFinalText);

    AppendingEditor* editor = new AppendingEditor();

    QStringList availableWords;
    availableWords << "one" << "two";

    QCompleter* completer = new QCompleter(availableWords);
    editor->setAppendingCompleter(completer);

    editor->setText(initialText);

    editor->setCursorPosition(selectedPosition);

    QTest::keyClicks(editor, input);

    QVERIFY2(completer->popup()->isVisible(), "Completer popup not visible.");
    QCOMPARE(completer->currentCompletion(), expectedCompletion);

    QTest::keyClick(completer->popup(), Qt::Key_Down);
    QTest::keyClick(completer->popup(), Qt::Key_Return);

    QCOMPARE(editor->text(), expectedFinalText);

    delete editor;
    delete completer;
}

//-----------------------------------------------------------------------------
// Function: tst_AppendingEditor::testWordUnderCursorIsCompleted_data()
//-----------------------------------------------------------------------------
void tst_AppendingEditor::testWordUnderCursorIsCompleted_data()
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
// Function: tst_AppendingEditor::testOnlyWordUnderCursorIsReplaced()
//-----------------------------------------------------------------------------
void tst_AppendingEditor::testOnlyWordUnderCursorIsReplaced()
{
    AppendingEditor* editor = new AppendingEditor();

    QStringList availableWords("one");

    QCompleter* completer = new QCompleter(availableWords);
    editor->setAppendingCompleter(completer);

    editor->setText("o on only");

    editor->setCursorPosition(1);

    QTest::keyClicks(editor, " o");

    QVERIFY2(completer->popup()->isVisible(), "Completer popup not visible.");
    QCOMPARE(completer->currentCompletion(), QString("one"));

    QTest::keyClick(completer->popup(), Qt::Key_Down);
    QTest::keyClick(completer->popup(), Qt::Key_Return);

    QCOMPARE(editor->text(), QString("o one on only"));
    QCOMPARE(editor->cursorPosition(), 5);

    delete editor;
    delete completer;
}

//-----------------------------------------------------------------------------
// Function: tst_AppendingEditor::testNoCompletionIsShownForEmptyInput()
//-----------------------------------------------------------------------------
void tst_AppendingEditor::testNoCompletionIsShownForEmptyInput()
{
    AppendingEditor* editor = new AppendingEditor();

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
// Function: tst_AppendingEditor::testCompletionsAreShownOnCtrlSpace()
//-----------------------------------------------------------------------------
void tst_AppendingEditor::testCompletionsAreShownOnCtrlSpace()
{
    AppendingEditor* editor = new AppendingEditor();

    QStringList availableWords("one");

    QCompleter* completer = new QCompleter(availableWords);
    editor->setAppendingCompleter(completer);

    QVERIFY2(!completer->popup()->isVisible(), "Completer popup should not be visible before editing.");

    QTest::keyClick(editor, Qt::Key_Space, Qt::ControlModifier);

    QVERIFY2(completer->popup()->isVisible(), "Completer popup should be visible when Ctrl+Space is pressed.");

    delete editor;
    delete completer;
}

QTEST_MAIN(tst_AppendingEditor)

#include "tst_AppendingEditor.moc"
