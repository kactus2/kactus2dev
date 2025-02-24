//-----------------------------------------------------------------------------
// File: DrawingBoard.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 28.3.2012
//
// Description:
// The drawing board for holding various IP-XACT documents.
//-----------------------------------------------------------------------------

#include "DrawingBoard.h"

#include <common/widgets/tabDocument/TabDocument.h>

#include <IPXACTmodels/common/VLNV.h>

#include <QCoreApplication>
#include <QKeyEvent>
#include <QObject>
#include <QMessageBox>
#include <QMenu>
#include <QTabBar>
#include <QSettings>

//-----------------------------------------------------------------------------
// Function: DrawingBoard::DrawingBoard()
//-----------------------------------------------------------------------------
DrawingBoard::DrawingBoard(QWidget* parent) : QTabWidget(parent)
{
    connect(this, SIGNAL(tabCloseRequested(int)), this, SLOT(closeAndRemoveDocument(int)), Qt::UniqueConnection);

    tabBar()->installEventFilter(this);
}

//-----------------------------------------------------------------------------
// Function: DrawingBoard::addAndOpenDocument()
//-----------------------------------------------------------------------------
void DrawingBoard::addAndOpenDocument(TabDocument* doc)
{    
    // Open in unlocked mode by default if locking isn't enabled in settings.
    QSettings settings; 
    doc->setProtection(settings.value("General/EnableLocking").toBool());

    connect(doc, SIGNAL(errorMessage(QString const&)),
        this, SIGNAL(errorMessage(QString const&)), Qt::UniqueConnection);
    connect(doc, SIGNAL(noticeMessage(QString const&)),
        this, SIGNAL(noticeMessage(QString const&)), Qt::UniqueConnection);

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
        connect(doc->getEditProvider().data(), SIGNAL(editStateChanged()), this, SIGNAL(documentEditStateChanged()));
    }

    addTab(doc, doc->getTitle());
    setCurrentWidget(doc);
}

//-----------------------------------------------------------------------------
// Function: DrawingBoard::applySettings()
//-----------------------------------------------------------------------------
void DrawingBoard::applySettings()
{
    QSettings settings;

    // Apply the settings to the open documents.
    for (int i = 0; i < count(); ++i)
    {
        TabDocument* doc = static_cast<TabDocument*>(widget(i));
        Q_ASSERT(doc != 0);

        doc->applySettings(settings);
    }
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
// Function: DrawingBoard::exportCurrentDocumentAsImage()
//-----------------------------------------------------------------------------
void DrawingBoard::exportCurrentDocumentAsImage()
{
    TabDocument* document = static_cast<TabDocument*>(currentWidget());
    if (document)
    {
        bool exportSucceeds = document->exportImage();
        QString documentTitle = document->getTitle();
        if (exportSucceeds)
        {
            emit noticeMessage(QStringLiteral("Image export complete for ") + documentTitle);
        }
        else
        {
            emit errorMessage(QStringLiteral("Failed to export image of ") + documentTitle);
        }
    }
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

    if (!doc->fileExists())
    {
        QString warningText = tr("The document has been deleted.");

        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(), warningText,
            QMessageBox::Close, this);

        if (msgBox.exec() == QMessageBox::Yes)
        {
            doc->save();
        }

        delete doc;

        return;
    }

    if (doc->isModified())
    {
        QVector<QString> errorList;
        bool documentIsValid = doc->validate(errorList);

        QString warningText = tr("The document has been modified. Save changes before refresh?");

        if (!documentIsValid)
        {
            QString errorText = tr("%1 error(s) located within the document.").arg(QString::number(errorList.size()));
            warningText.append("\n" + errorText);
        }

        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(), warningText,
            QMessageBox::Yes | QMessageBox::No, this);
        
        if (!documentIsValid)
        {
            QString errorText = "* " + QStringList(errorList.toList()).join("\n* ");
            msgBox.setDetailedText(errorText);
        }

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
    TabDocument* document = static_cast<TabDocument*>(widget(index));

    // If the document has been modified, ask a confirmation from the user.
    if (document->isModified())
    {
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            "Do you want to save changes to " + document->getDocumentName() + "?",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, this);

        int saveChanges = msgBox.exec();
        if (saveChanges == QMessageBox::Yes)
        {
            // Try to save and if it fails, cancel the close operation.
            if (!document->save())
            {
                return;
            }
        }
        else if (saveChanges == QMessageBox::No)
        {
            document->setModified(false);
        }
        else if (saveChanges == QMessageBox::Cancel)
        {
            // Cancel the close operation.
            return;
        }
    }

    // Deleting a document will automatically remove the associated tab from this widget.
    // Using removeTab() triggers showEvent() in the next tab before deleted() 
    // triggers MainWindow::onClearItemSelection. This wrong order of triggers would cause the instance 
    // details editor in design editor to appear empty for a previously selected component.
    delete document;
    document = 0;

    if (count() == 0)
    {
        emit lastDocumentClosed();
    }
}

//-----------------------------------------------------------------------------
// Function: DrawingBoard::closeAll()
//-----------------------------------------------------------------------------
void DrawingBoard::closeAll()
{
    int tabCount = count();
    for (int i = tabCount - 1; i >= 0; i--)
    {
        closeAndRemoveDocument(i);

        if (i != count())
        {
            return;
        }
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
// Function: DrawingBoard::eventFilter()
//-----------------------------------------------------------------------------
bool DrawingBoard::eventFilter(QObject* target, QEvent* event)
{
    if (target == tabBar() && event->type() == QEvent::ContextMenu)
    {
        QContextMenuEvent* menuEvent = static_cast<QContextMenuEvent*>(event);
        
        QMenu contextMenu;
        contextMenu.addAction(tr("Close All Tabs"), this, SLOT(closeAll()));
        contextMenu.exec(menuEvent->globalPos());
        return true;
    }

    return QTabWidget::eventFilter(target, event);
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
    QVector<QString> errorList;

    if (!doc->validate(errorList))
    {
        // If the document contained errors, inform the user and give options to save or cancel.
        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            tr("Document %1 contained %2 error(s). The document can be saved but "
            "may not be opened with other IP-XACT tools. Continue save?").arg(doc->getDocumentName(), 
            QString::number(errorList.size())), QMessageBox::Yes | QMessageBox::No, this);

        msgBox.setDetailedText("The document contained the following error(s):\n* " +
            QStringList(errorList.toList()).join("\n* "));

        save = (msgBox.exec() == QMessageBox::Yes);
    }

    return save;
}
