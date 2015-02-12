//-----------------------------------------------------------------------------
// File: DrawingBoard.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.3.2012
//
// Description:
// The drawing board for holding various IP-XACT documents.
//-----------------------------------------------------------------------------

#include "DrawingBoard.h"

#include <common/widgets/tabDocument/TabDocument.h>

#include <IPXACTmodels/vlnv.h>

#include <QCoreApplication>
#include <QKeyEvent>
#include <QObject>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: DrawingBoard::DrawingBoard()
//-----------------------------------------------------------------------------
DrawingBoard::DrawingBoard(QWidget* parent) : QTabWidget(parent)
{
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeAndRemoveDocument(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: DrawingBoard::~DrawingBoard()
//-----------------------------------------------------------------------------
DrawingBoard::~DrawingBoard()
{

}

//-----------------------------------------------------------------------------
// Function: DrawingBoard::addAndOpenDocument()
//-----------------------------------------------------------------------------
void DrawingBoard::addAndOpenDocument(TabDocument* doc, bool forceUnlocked)
{
    if (forceUnlocked)
    {
        doc->setProtection(false);
    }
    else
    {
        // Open in unlocked mode by default only if the version is draft.
        doc->setProtection(doc->getDocumentVLNV().getVersion() != "draft");
    }

    connect(doc, SIGNAL(errorMessage(QString const&)),
        this, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);
    connect(doc, SIGNAL(noticeMessage(QString const&)),
        this, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);

    connect(doc, SIGNAL(helpUrlRequested(QString const&)),
        this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);

    connect(doc, SIGNAL(contentChanged()), this, SIGNAL(documentContentChanged()), Qt::UniqueConnection);
    connect(doc, SIGNAL(modifiedChanged(bool)), this, SIGNAL(documentModifiedChanged(bool)), Qt::UniqueConnection);
    connect(doc, SIGNAL(documentSaved(TabDocument*)),
        this, SLOT(onDocumentSaved(TabDocument*)), Qt::UniqueConnection);
    connect(doc, SIGNAL(titleChanged(TabDocument*, QString const&)),
        this, SLOT(onDocumentTitleChanged(TabDocument*, QString const&)), Qt::UniqueConnection);

    if (doc->getEditProvider() != 0)
    {
        connect(doc->getEditProvider(), SIGNAL(editStateChanged()), this, SIGNAL(documentEditStateChanged()));
    }

    addTab(doc, doc->getTitle());
    setCurrentWidget(doc);
}

//-----------------------------------------------------------------------------
// Function: DrawingBoard::saveCurrentDocument()
//-----------------------------------------------------------------------------
void DrawingBoard::saveCurrentDocument()
{
    TabDocument* doc = static_cast<TabDocument*>(currentWidget());

    if (doc != 0 && shouldSave(doc))
    {
        doc->save();
        emit currentChanged(currentIndex());
    }
}

//-----------------------------------------------------------------------------
// Function: DrawingBoard::saveCurrentDocumentAs()
//-----------------------------------------------------------------------------
void DrawingBoard::saveCurrentDocumentAs()
{
    TabDocument* doc = static_cast<TabDocument*>(currentWidget());

    if (doc != 0 && shouldSave(doc) && doc->saveAs())
    {
        emit currentChanged(currentIndex());
        doc->refresh();
    }
}

//-----------------------------------------------------------------------------
// Function: DrawingBoard::saveAll()
//-----------------------------------------------------------------------------
void DrawingBoard::saveAll()
{
    // Go through all documents and save those that have been modified.
    for (int i = 0; i < count(); ++i)
    {
        TabDocument* doc = static_cast<TabDocument*>(widget(i));

        if (doc != 0 && doc->isModified())
        {
            if (!shouldSave(doc))
            {
                return;
            }

            doc->save();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DrawingBoard::printCurrentDocument()
//-----------------------------------------------------------------------------
void DrawingBoard::printCurrentDocument()
{
    TabDocument* doc = static_cast<TabDocument*>(currentWidget());
    Q_ASSERT(doc != 0);

    doc->print();
}

//-----------------------------------------------------------------------------
// Function: DrawingBoard::refreshCurrentDocument()
//-----------------------------------------------------------------------------
void DrawingBoard::refreshCurrentDocument()
{
    TabDocument* doc = static_cast<TabDocument*>(currentWidget());

    if (doc == 0)
    {
        return;
    }

    if (doc->isModified())
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            tr("The document has been modified. Save changes before refresh?"),
            QMessageBox::Yes | QMessageBox::No, this);

        if (msgBox.exec() == QMessageBox::Yes)
        {
            doc->save();
        }
    }

    doc->refresh();
}

//-----------------------------------------------------------------------------
// Function: DrawingBoard::closeAndRemoveDocument()
//-----------------------------------------------------------------------------
void DrawingBoard::closeAndRemoveDocument(int index)
{	
    Q_ASSERT(widget(index) != 0);
    TabDocument* doc = static_cast<TabDocument*>(widget(index));

    // If the document has been modified, ask a confirmation from the user.
    if (doc->isModified())
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            "Do you want to save changes to " + doc->getDocumentName() + "?",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);

        int saveChanges = msgBox.exec();
        if (saveChanges == QMessageBox::Yes)
        {
            // Try to save and if it fails, cancel the close operation.
            if (!doc->save())
            {
                return;
            }
        }
        else if (saveChanges == QMessageBox::No)
        {
            doc->setModified(false);
        }
        else if (saveChanges == QMessageBox::Cancel)
        {
            // Cancel the close operation.
            return;
        }
    }

    removeTab(index);
    delete doc;
    doc = 0;

    if (count() == 0)
    {
        emit lastDocumentClosed();
    }
}

//-----------------------------------------------------------------------------
// Function: DrawingBoard::eventFilter()
//-----------------------------------------------------------------------------
bool DrawingBoard::event(QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->modifiers() == Qt::ControlModifier)
        {
            if (keyEvent->key() == Qt::Key_Tab)
            {
                int nextPage = (currentIndex() + 1) % count();
                setCurrentIndex(nextPage);
                return true;
            }
            else if (keyEvent->key() == Qt::Key_W)
            {
                if (currentIndex() >= 0 && currentIndex() < count())
                {
                    emit tabCloseRequested(currentIndex());
                }

                return true;
            }
        }
    }

    return QTabWidget::event(event);
}

//-----------------------------------------------------------------------------
// Function: DrawingBoard::onDocumentNameChanged()
//-----------------------------------------------------------------------------
void DrawingBoard::onDocumentTitleChanged(TabDocument* document, QString const& newTitle)
{
    int index = indexOf(document);

    if (index >= 0)
    {
        setTabText(index, newTitle);
    }
}

//-----------------------------------------------------------------------------
// Function: DrawingBoard::onDocumentSaved()
//-----------------------------------------------------------------------------
void DrawingBoard::onDocumentSaved(TabDocument* doc)
{
    VLNV savedVLNV = doc->getDocumentVLNV();

    int documentCount =  count();
    for (int i = 0; i < documentCount; i++)
    {
        TabDocument* otherDoc = static_cast<TabDocument*>(widget(i));

        if (otherDoc != doc && otherDoc->getRelatedVLNVs().contains(savedVLNV))
        {
            otherDoc->requestRefresh();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: DrawingBoard::shouldSave()
//-----------------------------------------------------------------------------
bool DrawingBoard::shouldSave(TabDocument* doc)
{
    bool save = true;

    QStringList errorList;
    if (!doc->validate(errorList))
    {
        // If the document contained errors, inform the user and give options to save or cancel.
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            tr("Document %1 contained %2 error(s). The document can be saved but "
            "may not be opened with other IP-XACT tools. Continue save?").arg(doc->getDocumentName(), 
            QString::number(errorList.size())), QMessageBox::Yes | QMessageBox::No, this);

        msgBox.setDetailedText("The document contained the following error(s):\n* " + 
            errorList.join("\n* "));

        save = (msgBox.exec() == QMessageBox::Yes);
    }

    return save;
}
