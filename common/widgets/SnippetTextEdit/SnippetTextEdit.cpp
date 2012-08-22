//-----------------------------------------------------------------------------
// File: SnippetTextEdit.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Määttä
// Date: 22.08.2012
//
// Description:
// Text edit for writing snippets that can use magic words.
//-----------------------------------------------------------------------------

#include "SnippetTextEdit.h"

#include "SnippetHighlighter.h"

//-----------------------------------------------------------------------------
// Function: SnippetTextEdit::SnippetTextEdit()
//-----------------------------------------------------------------------------
SnippetTextEdit::SnippetTextEdit(QWidget* parent)
    : QPlainTextEdit(parent),
      highlighter_(new SnippetHighlighter(document())),
      magicWordsMap_()
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint const&)),
            this, SLOT(showContextMenu(QPoint const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: SnippetTextEdit::~SnippetTextEdit()
//-----------------------------------------------------------------------------
SnippetTextEdit::~SnippetTextEdit()
{
}

//-----------------------------------------------------------------------------
// Function: SnippetTextEdit::addMagicWords()
//-----------------------------------------------------------------------------
void SnippetTextEdit::addMagicWords(QStringList const& magicWords, QString const& category /*= QString("")*/)
{
    if (!magicWordsMap_.contains(category))
    {
        magicWordsMap_.insert(category, QStringList());
    }

    magicWordsMap_[category].append(magicWords);
    magicWordsMap_[category].sort();

    highlighter_->addMagicWords(magicWords);
}

//-----------------------------------------------------------------------------
// Function: SnippetTextEdit::clearMagicWords()
//-----------------------------------------------------------------------------
void SnippetTextEdit::clearMagicWords()
{
    magicWordsMap_.clear();
    highlighter_->clearMagicWords();
}

//-----------------------------------------------------------------------------
// Function: SnippetTextEdit::showContextMenu()
//-----------------------------------------------------------------------------
void SnippetTextEdit::showContextMenu(QPoint const& pt)
{
    QMenu* menu = createStandardContextMenu();
    QMenu* insertMenu = menu->addMenu(tr("Insert"));

    QMapIterator<QString, QStringList> iter(magicWordsMap_);

    bool foundUnsorted = false;
    QActionGroup* actionGroup = new QActionGroup(menu);

    while (iter.hasNext())
    {
        iter.next();
        
        if (iter.key() == "")
        {
            foundUnsorted = true;
        }
        else
        {
            QMenu* categoryMenu = insertMenu->addMenu(iter.key());

            foreach (QString const& name, iter.value())
            {
                QAction* action = new QAction(name, menu);
                categoryMenu->addAction(action);
                actionGroup->addAction(action);
            }
        }
    }

    if (foundUnsorted)
    {
        foreach (QString const& name, magicWordsMap_[""])
        {
            QAction* action = new QAction(name, menu);
            insertMenu->addAction(action);
            actionGroup->addAction(action);
        }
    }

    connect(actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(insertMagicWord(QAction*)));

    menu->exec(mapToGlobal(pt));
    delete menu;
}

//-----------------------------------------------------------------------------
// Function: SnippetTextEdit::insertMagicWord()
//-----------------------------------------------------------------------------
void SnippetTextEdit::insertMagicWord(QAction* action)
{
    insertPlainText(QString("$") + action->text() + QString("$"));
}
