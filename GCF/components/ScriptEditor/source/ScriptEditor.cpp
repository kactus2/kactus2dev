/****************************************************************************
**
** Copyright (C) VCreate Logic Private Limited, Bangalore
**
** Use of this file is limited according to the terms specified by
** VCreate Logic Private Limited, Bangalore.  Details of those terms
** are listed in licence.txt included as part of the distribution package
** of this file. This file may not be distributed without including the
** licence.txt file.
**
** Contact info@vcreatelogic.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "ScriptEditor.h"
#include "ScriptEditorComponent.h"
#include "ScriptEngineManager.h"
#include "ScriptEngineManager.h"
#include "ui_FindTextDialog.h"
#include "ui_FindAndReplaceDialog.h"

#include <QFont>
#include <QMetaObject>
#include <QMetaMethod>
#include <QPainter>
#include <QPalette>
#include <QScrollBar>
#include <QMouseEvent>
#include <QHelpEvent>
#include <QToolTip>
#include <QInputDialog>
#include <QDialog>
#include <QScriptEngine>
#include <QScriptValue>
#include <QCompleter>
#include <QStandardItemModel>
#include <QScriptValueIterator>
#include <QMessageBox>
#include <QAbstractTextDocumentLayout>
#include <QPaintEngine>
#include <QTreeView>
#include <QHeaderView>

namespace GCF
{
	namespace Components
	{
		struct FindTextInfo
		{
			FindTextInfo() : textToFind(), textToReplace(), regExp(false), findFromCursor(false), findFlags(0)
			{
			}

			QString textToFind;
			QString textToReplace;
			bool regExp;
			bool findFromCursor;
			QTextDocument::FindFlags findFlags;
			QDialog fnrDialog;
			Ui::FindAndReplaceDialog ui;
		};
	}
}

struct GCF::Components::ScriptEditorData
{
    ScriptEditorData() : tabSpace(0), showLineNumbers(false), timerId(-1), gotoLineNr(1),
        scriptEngineManager(0), completer(0), completerModel(0), completerView(0) { }

    int tabSpace;
    bool showLineNumbers;
    int leftMargin;
    int timerId;
    int gotoLineNr;
    QMap<int, QColor> lineMarkers;
    QMap<int, QString> lineToolTipMap;
    GCF::Components::FindTextInfo findTextInfo;
    GCF::Components::ScriptEngineManager* scriptEngineManager;

    // Used for context sensitive models.
    QCompleter* completer;
    QStandardItemModel* completerModel;
    QTreeView* completerView;
    int completerPos;

    void getContextOptions(const QString& context, QStandardItemModel* model);
    void prepareCompleter();
    bool isSignal(QObject* object, const QString& methodname);
    bool isPublic(QObject* object, const QString& methodname);
};

GCF::Components::ScriptEditor::ScriptEditor(QWidget* parent)
:QTextEdit(parent)
{
    d = new GCF::Components::ScriptEditorData;

    setTabSpace(4);
#if(defined(Q_OS_WIN32) || defined(Q_OS_WIN64))
    setFont(QFont("Courier New", 11));
#else
    setFont(QFont("Monospace", 11));
#endif
    setShowLineNumbers(true);
    setAcceptDrops(false);
    setAcceptRichText(false);
    setLineWrapMode(QTextEdit::NoWrap);
    setWordWrapMode(QTextOption::NoWrap);
    setAutoFormatting(QTextEdit::AutoNone);

    new GCF::Components::ScriptHighlighter(this);

    connect(this, SIGNAL(textChanged()), this, SIGNAL(projectPartModified()));
}

GCF::Components::ScriptEditor::~ScriptEditor()
{
    delete d;
}

void GCF::Components::ScriptEditor::setScriptingEngineManager(GCF::Components::ScriptEngineManager* manager)
{
    d->scriptEngineManager = manager;
}

GCF::Components::ScriptEngineManager* GCF::Components::ScriptEditor::scriptEngineManager() const
{
    return d->scriptEngineManager;
}

int GCF::Components::ScriptEditor::tabSpace() const
{
    return d->tabSpace;
}

void GCF::Components::ScriptEditor::setTabSpace(int space)
{
    if(d->tabSpace == space || space < 0)
        return;

    QFontMetrics fm = fontMetrics();
    QString tabStr;

    tabStr = tabStr.fill('_', space);
    int width = fm.width(tabStr);
    setTabStopWidth(width);
    d->tabSpace = space;
}

void GCF::Components::ScriptEditor::setFontName(const QString& fontName)
{
    QFont font = this->font();
    if( font.family() == fontName )
        return;

    font.setFamily( fontName );
    setFont( font );
}

QString GCF::Components::ScriptEditor::fontName() const
{
    return this->font().family();
}

void GCF::Components::ScriptEditor::setFontSize(int size)
{
    QFont font = this->font();
    if( font.pointSize() == size )
        return;

    font.setPointSize( size );
    setFont( font );
}

int GCF::Components::ScriptEditor::fontSize() const
{
    return this->font().pointSize();
}

void GCF::Components::ScriptEditor::setCode(QString code)
{
    setPlainText(code);
}

QString GCF::Components::ScriptEditor::code() const
{
    return toPlainText();
}

void GCF::Components::ScriptEditor::setShowLineNumbers(bool val)
{
    if(d->showLineNumbers == val)
        return;

    d->showLineNumbers = val;
    d->leftMargin = 0;

    // Minimize the line numbers area, and return
    if(!val)
    {
        setViewportMargins(0, 0, 0, 0);
        disconnect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(update()));
        return;
    }

    // Show some space for line numbers and quit. Lets assume that the
    // the user will edit a maximum of 999999 lines and nothing more.
    d->leftMargin = fontMetrics().width("999999")+2;
    setViewportMargins(d->leftMargin, 0, 0, 0);
    connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(update()));
    update();
}

bool GCF::Components::ScriptEditor::isShowLineNumbers() const
{
    return d->showLineNumbers;
}

void GCF::Components::ScriptEditor::gotoLine(int linrNr)
{
    int lineHeight = fontMetrics().height();
    int lineY = linrNr*lineHeight + qAbs(fontMetrics().leading());
    verticalScrollBar()->setValue(lineY);
}

void GCF::Components::ScriptEditor::markLine(int lineNr, QString toolTip, QColor color)
{
    d->lineMarkers[lineNr] = color;
    d->lineToolTipMap[lineNr] = toolTip;
    qWarning("%d, %s", lineNr, qPrintable(toolTip));
    update();
}

void GCF::Components::ScriptEditor::unmarkLine(int lineNr)
{
    d->lineMarkers.remove(lineNr);
    d->lineToolTipMap.remove(lineNr);
    update();
}

void GCF::Components::ScriptEditor::toggleMarkLine(int lineNr, QColor color)
{
    if(d->lineMarkers.contains(lineNr))
        d->lineMarkers.remove(lineNr);
    else
        d->lineMarkers[lineNr] = color;
    update();
}

void GCF::Components::ScriptEditor::unmarkAllLines()
{
    d->lineMarkers.clear();
    d->lineToolTipMap.clear();
    update();
}

void GCF::Components::ScriptEditor::showGotoLineDialog()
{
    bool ok;
    int lineNr = QInputDialog::getInteger(this, "Goto Line", "Enter Line Number", d->gotoLineNr, 1, 2147483647, 1, &ok);
    if(!ok)
        return;
    gotoLine(lineNr);
    markLine(lineNr, QString(), Qt::green);
    d->gotoLineNr = lineNr;
    d->timerId = startTimer(4000);
}

void GCF::Components::ScriptEditor::showFindDialog()
{
    QDialog findDialog;
    Ui::FindTextDialog ui;
    ui.setupUi(&findDialog);
    Qt::WindowFlags flags = Qt::Tool;
    findDialog.setWindowFlags(flags);

    if(d->findTextInfo.fnrDialog.isVisible())
        d->findTextInfo.fnrDialog.hide();

    ui.txtFindText->setText(d->findTextInfo.textToFind);
    ui.chkCaseSensitive->setCheckState((d->findTextInfo.findFlags & QTextDocument::FindCaseSensitively)?(Qt::Checked):(Qt::Unchecked));
    ui.chkFindBackwards->setCheckState((d->findTextInfo.findFlags & QTextDocument::FindBackward)?Qt::Checked:Qt::Unchecked);
    ui.chkFromCursor->setCheckState((d->findTextInfo.findFromCursor)?Qt::Checked:Qt::Unchecked);
    ui.chkRegularExpression->setCheckState((d->findTextInfo.regExp)?Qt::Checked:Qt::Unchecked);
    ui.chkWholeWordsOnly->setCheckState((d->findTextInfo.findFlags & QTextDocument::FindWholeWords)?Qt::Checked:Qt::Unchecked);

    int dialogCode = findDialog.exec();
    if(dialogCode != QDialog::Accepted)
        return;

    d->findTextInfo.textToFind = ui.txtFindText->text();
    d->findTextInfo.findFlags = 0;

    if (ui.chkFindBackwards->checkState() == Qt::Checked)
        d->findTextInfo.findFlags |= QTextDocument::FindBackward;

    if (ui.chkCaseSensitive->checkState() == Qt::Checked)
        d->findTextInfo.findFlags |= QTextDocument::FindCaseSensitively;

    if (ui.chkWholeWordsOnly->checkState() == Qt::Checked)
        d->findTextInfo.findFlags |= QTextDocument::FindWholeWords;

    d->findTextInfo.regExp = (ui.chkRegularExpression->checkState() == Qt::Checked)?1:0;
    d->findTextInfo.findFromCursor = (ui.chkFromCursor->checkState() == Qt::Checked)?1:0;

    QTextCursor  cursor = this->textCursor();
    if(d->findTextInfo.findFromCursor)
    {
        if(d->findTextInfo.regExp)
            cursor = this->document()->find(QRegExp(d->findTextInfo.textToFind), cursor.position(), d->findTextInfo.findFlags);
        else
            cursor = this->document()->find(d->findTextInfo.textToFind, cursor.position(), d->findTextInfo.findFlags);
    }
    else
    {
        if(d->findTextInfo.regExp)
            cursor = this->document()->find(QRegExp(d->findTextInfo.textToFind),0,d->findTextInfo.findFlags);
        else
            cursor = this->document()->find(d->findTextInfo.textToFind,0, d->findTextInfo.findFlags);
    }
    int pos = cursor.position();

    if(pos >= 0)
    {
        this->setTextCursor(cursor);
        cursor.select(QTextCursor::WordUnderCursor);
    }
    else
    {
        QMessageBox::information(this, tr("VW"), tr("End of document reached!!!"));
    }
}

void GCF::Components::ScriptEditor::findNext()
{
    int pos = 0;
    QTextCursor  cursor = this->textCursor();
    pos = cursor.position();


    if(d->findTextInfo.fnrDialog.isVisible())
    {
        d->findTextInfo.textToFind = d->findTextInfo.ui.txtFindText->text();
        d->findTextInfo.textToReplace = d->findTextInfo.ui.txtReplaceText->text();

        d->findTextInfo.findFlags = 0;
        if (d->findTextInfo.ui.chkFindBackwards->checkState() == Qt::Checked)
            d->findTextInfo.findFlags |= QTextDocument::FindBackward;

        if (d->findTextInfo.ui.chkCaseSensitive->checkState() == Qt::Checked)
            d->findTextInfo.findFlags |= QTextDocument::FindCaseSensitively;

        if (d->findTextInfo.ui.chkWholeWordsOnly->checkState() == Qt::Checked)
            d->findTextInfo.findFlags |= QTextDocument::FindWholeWords;

        d->findTextInfo.regExp = (d->findTextInfo.ui.chkRegularExpression->checkState() == Qt::Checked)?1:0;
        d->findTextInfo.findFromCursor = (d->findTextInfo.ui.chkFromCursor->checkState() == Qt::Checked)?1:0;
    }
    if(pos < 0)
    {
        cursor.setPosition(0);
        pos = 0;
    }

    if(cursor.hasSelection())
    {
        cursor.clearSelection();
        this->setTextCursor(cursor);
    }

    if(d->findTextInfo.regExp)
        cursor = this->document()->find(QRegExp(d->findTextInfo.textToFind), pos, d->findTextInfo.findFlags);
    else
        cursor = this->document()->find(d->findTextInfo.textToFind, pos, d->findTextInfo.findFlags);

    pos = cursor.position();

    if(pos >= 0)
    {
        this->setTextCursor(cursor);
        cursor.select(QTextCursor::WordUnderCursor);
    }
}

void GCF::Components::ScriptEditor::findPrevious()
{
    QTextCursor cursor = this->textCursor();
    int pos = cursor.position();
    QTextDocument::FindFlags flags = d->findTextInfo.findFlags;


    if(flags & QTextDocument::FindBackward)
    {
        flags &= !(QTextDocument::FindBackward);
    }
    else
    {
        flags |= QTextDocument::FindBackward;
        pos -= cursor.selectedText().length();
    }

    if(pos < 0)
    {
        cursor.setPosition(0);
        pos = 0;
    }

    if(d->findTextInfo.regExp)
        cursor = this->document()->find(QRegExp(d->findTextInfo.textToFind), pos, flags);
    else
        cursor = this->document()->find(d->findTextInfo.textToFind, pos, flags);

    pos = cursor.position();
    if(pos >= 0)
    {
        this->setTextCursor(cursor);
        cursor.select(QTextCursor::WordUnderCursor);
    }
    else
    {
        QMessageBox::information(this, tr("VW"), tr("End of document reached!!!"));
    }
}

void GCF::Components::ScriptEditor::showFindAndReplaceDialog()
{
    d->findTextInfo.ui.setupUi(&(d->findTextInfo.fnrDialog));
    Qt::WindowFlags flags = Qt::WindowStaysOnTopHint | Qt::Tool;
    d->findTextInfo.fnrDialog.setWindowFlags(flags);
    Qt::WindowModality modality = Qt::WindowModal;
    d->findTextInfo.fnrDialog.setWindowModality(modality);

    connect(d->findTextInfo.ui.findNextButton, SIGNAL(clicked()), this, SLOT(findNext()));
    connect(d->findTextInfo.ui.replaceWithButton, SIGNAL(clicked()), this, SLOT(replaceText()));
    connect(d->findTextInfo.ui.replaceAllButton, SIGNAL(clicked()), this, SLOT(replaceAll()));

    d->findTextInfo.ui.txtFindText->setText(d->findTextInfo.textToFind);
    d->findTextInfo.ui.chkCaseSensitive->setCheckState((d->findTextInfo.findFlags & QTextDocument::FindCaseSensitively)?(Qt::Checked):(Qt::Unchecked));
    d->findTextInfo.ui.chkFindBackwards->setCheckState((d->findTextInfo.findFlags & QTextDocument::FindBackward)?Qt::Checked:Qt::Unchecked);
    d->findTextInfo.ui.chkFromCursor->setCheckState((d->findTextInfo.findFromCursor)?Qt::Checked:Qt::Unchecked);
    d->findTextInfo.ui.chkRegularExpression->setCheckState((d->findTextInfo.regExp)?Qt::Checked:Qt::Unchecked);
    d->findTextInfo.ui.chkWholeWordsOnly->setCheckState((d->findTextInfo.findFlags & QTextDocument::FindWholeWords)?Qt::Checked:Qt::Unchecked);

    int dialogCode = d->findTextInfo.fnrDialog.exec();
    qWarning(qPrintable(QString("%1").arg(dialogCode)));
}

void GCF::Components::ScriptEditor::replaceText()
{
    QTextCursor cursor = this->textCursor();
    if(cursor.position() < 0 || !cursor.hasSelection())
        findNext();
    else
    {
        cursor = this->textCursor();
        if(cursor.hasSelection())
        {
            cursor.insertText(d->findTextInfo.textToReplace);
            cursor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
            findNext();
        }
    }
}

void GCF::Components::ScriptEditor::replaceAll()
{
    QTextCursor cursor = this->textCursor();
    cursor.movePosition(QTextCursor::Start, QTextCursor::KeepAnchor);

    while(1)
    {
        int pos = cursor.position();
        if(pos < 0 || !cursor.hasSelection())
            break;
        else
        {
            findNext();
            cursor = this->textCursor();
            if(cursor.hasSelection())
            {
                cursor.insertText(d->findTextInfo.textToReplace);
                cursor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
            }
        }
    }
}

void GCF::Components::ScriptEditor::insertCompletion(const QString& completion)
{
    QTextCursor tc = textCursor();
    QTextCursor oldCursor(document());
    oldCursor.setPosition(d->completerPos);
    oldCursor.setPosition(tc.position(), QTextCursor::KeepAnchor);
    tc.setPosition(d->completerPos);
    oldCursor.removeSelectedText();

    d->completerPos = -1;

    QString text = completion.section('(', 0, 0);
    tc.insertText(text);
    setTextCursor(tc);

    d->completer->popup()->hide();
}

bool GCF::Components::ScriptEditor::event(QEvent* e)
{
    if(!d->showLineNumbers)
    {
        if(!d->completer || (d->completer && !d->completer->popup()->isVisible()))
            return QTextEdit::event(e);
    }

    bool success = QTextEdit::event(e);

    switch(e->type())
    {
    case QEvent::Paint: {
            QPaintEvent* pe = (QPaintEvent*)e;
            myPaintEvent(pe);
        } break;
    case QEvent::MouseButtonPress: {
            QMouseEvent* me = (QMouseEvent*)e;
            myMousePressEvent(me);
        } break;
    case QEvent::WhatsThis:
    case QEvent::ToolTip: {
            QHelpEvent* he = (QHelpEvent*)e;
            myHelpEvent(he);
        } break;
    default:
        break;
    }

    return success;
}

void GCF::Components::ScriptEditor::myPaintEvent(QPaintEvent* pe)
{
    Q_UNUSED(pe);

    QColor lineMarkBg = palette().button().color();

    // Show line numbers
    QRect r(0, 0, d->leftMargin, height());
    QPainter paint(this);
    paint.fillRect(r, lineMarkBg);
    paint.drawLine(r.topRight(), r.bottomRight());

    int lineHeight = fontMetrics().height();
    int curLine = verticalScrollBar()->value() / lineHeight;
    int nrLines = 1 + (height() / lineHeight);
    int y = viewport()->y() + curLine*lineHeight - verticalScrollBar()->value() + qAbs(fontMetrics().leading());
    QPen defaultPen = paint.pen();
    QFont defaultFont = paint.font();
    QFont boldFont = defaultFont;
    boldFont.setBold(true);

    for(int i=curLine; i<curLine+nrLines; i++)
    {
        QRect lineRect(0, y, d->leftMargin-2, lineHeight);
        if(d->lineMarkers.contains(i))
        {
            paint.setPen(d->lineMarkers[i]);
            paint.setFont(boldFont);
            paint.fillRect(lineRect, lineMarkBg);
        }
        else
        {
            paint.setPen(defaultPen);
            paint.setFont(defaultFont);
        }
        paint.drawText(lineRect, Qt::AlignVCenter|Qt::AlignRight, QString("%1").arg(i+1));
        y += lineHeight;
    }
}

void GCF::Components::ScriptEditor::myMousePressEvent(QMouseEvent* me)
{
    if(me->x() >= d->leftMargin)
        return;

    int lineHeight = fontMetrics().height();
    int y = -verticalScrollBar()->value() + qAbs(fontMetrics().leading()) + viewport()->y();
    int lineNr = (me->y() - y) / lineHeight;

    toggleMarkLine(lineNr, Qt::blue);

    Q_UNUSED(me);
}

void GCF::Components::ScriptEditor::myHelpEvent(QHelpEvent* he)
{
    if(he->x() >= d->leftMargin)
        return;

    int lineHeight = fontMetrics().height();
    int y = -verticalScrollBar()->value() + qAbs(fontMetrics().leading()) + viewport()->y();
    int lineNr = (he->y() - y) / lineHeight;
    QRect lineNrRect(0, y+(lineNr*lineHeight), d->leftMargin, lineHeight);

    if(d->lineToolTipMap.contains(lineNr))
    {
        QString tip = d->lineToolTipMap[lineNr];
        if(tip.isEmpty())
            return;

        QToolTip::showText(QCursor::pos(), tip, this, lineNrRect);
    }
}

void GCF::Components::ScriptEditor::keyPressEvent(QKeyEvent* ke)
{
    QTextEdit::keyPressEvent(ke);

    if(d->completer && d->completer->popup()->isVisible())
    {
        if (d->completer && d->completer->popup()->isVisible()) {
            // The following keys are forwarded by the completer to the widget
           switch (ke->key()) {
           case Qt::Key_Enter:
           case Qt::Key_Return:
           case Qt::Key_Escape:
           case Qt::Key_Tab:
           case Qt::Key_Backtab:
                ke->ignore();
                return; // let the completer do default behavior
           default:
               break;
           }
        }
    }

    bool dotKey = (ke->text() == ".");
    if(dotKey)
        prepareCompleter();

    if(d->completer && (dotKey || d->completer->popup()->isVisible()))
    {
        QString completionPrefix = textUnderCursor();
        if (completionPrefix != d->completer->completionPrefix())
        {
            d->completer->setCompletionPrefix(completionPrefix);
            d->completer->popup()->setCurrentIndex(d->completer->completionModel()->index(0, 0));
        }
        QRect cr = cursorRect();
        cr.setWidth(d->completer->popup()->sizeHintForColumn(0)
                    + d->completer->popup()->verticalScrollBar()->sizeHint().width());
        d->completer->complete(cr); // popup it up!
    }
    else
        d->completerPos = -1;
}

void GCF::Components::ScriptEditor::timerEvent(QTimerEvent*)
{
    killTimer(d->timerId);
    unmarkLine(d->gotoLineNr);
}

QString GCF::Components::ScriptEditor::textUnderCursor() const
{
    if(d->completerPos < 0)
        return QString();

    QTextCursor tc = textCursor();
    QTextCursor cursor(document());
    cursor.setPosition(d->completerPos);
    cursor.setPosition(tc.position(), QTextCursor::KeepAnchor);
    return cursor.selectedText();
}

void GCF::Components::ScriptEditor::prepareCompleter()
{
    QTextCursor cursor = textCursor();
    d->completerPos = -1;

    // Keep going back in the line until you get a white space character..
    int pos = cursor.position();
    int oldPos = pos;
    while(1)
    {
        cursor.movePosition(QTextCursor::WordLeft, QTextCursor::KeepAnchor);
        pos = cursor.position();
        if(pos)
            --pos;
        cursor.setPosition(pos, QTextCursor::KeepAnchor);

        QString selText = cursor.selectedText();
        if(!pos || !selText.indexOf(QRegExp("\\s")))
            break;
    }

    // Figure out the current context.
    QString selText = cursor.selectedText();
    selText = selText.simplified();
    cursor.setPosition(oldPos);

    // Find out the options we are dealing with at the moment.
    if(!d->completerModel)
        d->completerModel = new QStandardItemModel(this);
    d->getContextOptions(selText, d->completerModel);
    if(!d->completerModel->rowCount())
        return;

    // Prepare a completer now
    if(!d->completer)
    {
        /*d->completerView = new QTreeView(this);
        d->completerView->header()->hide();
        d->completerView->setRootIsDecorated(false);*/

        d->completer = new QCompleter(this);
        d->completer->setWidget(this);
        d->completer->setCompletionMode(QCompleter::PopupCompletion);
        // d->completer->setPopup(d->completerView);
        // d->completer->popup()->setFont(this->font());
        d->completer->setModel(d->completerModel);
        d->completer->setCaseSensitivity(Qt::CaseInsensitive);
        d->completer->setCompletionColumn(0);
        d->completer->setCompletionRole(Qt::DisplayRole);
        connect(d->completer, SIGNAL(activated(const QString&)),
                this, SLOT(insertCompletion(const QString&)));
    }
    d->completerPos = cursor.position();
}

QObject* GCF::Components::ScriptEditor::containerObject()
{
    return this;
}

QString GCF::Components::ScriptEditor::projectPartID()
{
    return "Script";
}

bool GCF::Components::ScriptEditor::newProjectPart()
{
    this->clear();
    return true;
}

bool GCF::Components::ScriptEditor::loadProjectPart(QDomDocument doc, QDomElement partE)
{
    this->clear();

    QDomNodeList nodeList = partE.childNodes();
    for(int i=0; i<nodeList.count(); i++)
    {
        QDomNode node = nodeList.item(i);
        QDomCDATASection cdata = node.toCDATASection();
        if(cdata.isNull())
            continue;

        this->setPlainText( cdata.data() );
        return true;
    }

    return false;
}

bool GCF::Components::ScriptEditor::initializeProjectPart()
{
    GCF::Components::ScriptEditorComponent::instance().executeMainScript();
    return true;
}

bool GCF::Components::ScriptEditor::saveProjectPart(QDomDocument doc, QDomElement partParentE)
{
    QDomCDATASection codeSec = doc.createCDATASection(this->toPlainText());
    partParentE.appendChild(codeSec);
    return true;
}

bool GCF::Components::ScriptEditor::closeProjectPart()
{
    this->clear();
    return true;
}

void GCF::Components::ScriptEditorData::getContextOptions(const QString& context, QStandardItemModel* model)
{
    if(!model)
        return;

    // Reset the model first
    model->clear();

    if(!scriptEngineManager)
        return;

    QStringList comps = context.split('.');
    QScriptEngine* engine = scriptEngineManager->engine();
    QScriptValue value = engine->globalObject();

    if(comps.last().isEmpty())
        comps.pop_back();
    for(int i=0; i<comps.count(); i++)
    {
        value = value.property(comps[i]);
        if(!value.isValid())
            return;
    }

    QObject* object = value.isQObject() ? value.toQObject() : 0;

    QScriptValueIterator it(value);
    while(it.hasNext())
    {
        it.next();
        QString name = it.name();
        if( name.startsWith('_') || name.startsWith("on_") || name == "deleteLater" || name.startsWith("command_") )
            continue;

        QScriptValue val = it.value();
        // I think this is a bug in Qt 4.3.x.
        // isFunction() doesnt seem to work !
#if QT_VERSION >= 0x040400
        if( val.isFunction() && object )
        {
            if( isSignal(object, name) )
                continue;

            if( !isPublic(object, name) )
                continue;
        }
#endif

        QStandardItem* item = new QStandardItem;
        item->setText(name);

        static QIcon methodIcon(":/ScriptEditor/method.png");
        static QIcon propertyIcon(":/ScriptEditor/property.png");
        if( val.isFunction() )
            item->setIcon(methodIcon);
        else
            item->setIcon(propertyIcon);

        model->appendRow(item);
    }
}

bool GCF::Components::ScriptEditorData::isSignal(QObject* object, const QString& methodname)
{
    if( !object )
        return false;

    const QMetaObject* qmo = object->metaObject();
    for(int i=qmo->methodCount()-1; i>=0; i--)
    {
        QMetaMethod method = qmo->method(i);
        if( method.methodType() != QMetaMethod::Signal )
            continue;

        QString sig( method.signature() );
        if( sig.startsWith(methodname) )
            return true;
    }

    return false;
}

bool GCF::Components::ScriptEditorData::isPublic(QObject* object, const QString& methodname)
{
    if( !object )
        return true;

    const QMetaObject* qmo = object->metaObject();
    for(int i=qmo->methodCount()-1; i>=0; i--)
    {
        QMetaMethod method = qmo->method(i);
        if( method.access() != QMetaMethod::Public )
            continue;

        QString sig( method.signature() );
        if( sig.startsWith(methodname) )
            return true;
    }

    return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// GCF::Components::ScriptHighlighter
// This class is copied from Qt Designer
////////////////////////////////////////////////////////////////////////////////////////////////

#include <QSet>

static const QSet<QString> &qscriptKeywords() {
    static QSet<QString> keywords;
    if (keywords.empty()) {
        keywords.insert(QLatin1String("Infinity"));
        keywords.insert(QLatin1String("NaN"));
        keywords.insert(QLatin1String("abstract"));
        keywords.insert(QLatin1String("boolean"));
        keywords.insert(QLatin1String("break"));
        keywords.insert(QLatin1String("byte"));
        keywords.insert(QLatin1String("case"));
        keywords.insert(QLatin1String("catch"));
        keywords.insert(QLatin1String("char"));
        keywords.insert(QLatin1String("class"));
        keywords.insert(QLatin1String("const"));
        keywords.insert(QLatin1String("constructor"));
        keywords.insert(QLatin1String("continue"));
        keywords.insert(QLatin1String("debugger"));
        keywords.insert(QLatin1String("default"));
        keywords.insert(QLatin1String("delete"));
        keywords.insert(QLatin1String("do"));
        keywords.insert(QLatin1String("double"));
        keywords.insert(QLatin1String("else"));
        keywords.insert(QLatin1String("enum"));
        keywords.insert(QLatin1String("export"));
        keywords.insert(QLatin1String("extends"));
        keywords.insert(QLatin1String("false"));
        keywords.insert(QLatin1String("final"));
        keywords.insert(QLatin1String("finally"));
        keywords.insert(QLatin1String("float"));
        keywords.insert(QLatin1String("for"));
        keywords.insert(QLatin1String("function"));
        keywords.insert(QLatin1String("goto"));
        keywords.insert(QLatin1String("if"));
        keywords.insert(QLatin1String("implements"));
        keywords.insert(QLatin1String("import"));
        keywords.insert(QLatin1String("in"));
        keywords.insert(QLatin1String("instanceof"));
        keywords.insert(QLatin1String("int"));
        keywords.insert(QLatin1String("interface"));
        keywords.insert(QLatin1String("long"));
        keywords.insert(QLatin1String("native"));
        keywords.insert(QLatin1String("new"));
        keywords.insert(QLatin1String("package"));
        keywords.insert(QLatin1String("private"));
        keywords.insert(QLatin1String("protected"));
        keywords.insert(QLatin1String("public"));
        keywords.insert(QLatin1String("return"));
        keywords.insert(QLatin1String("short"));
        keywords.insert(QLatin1String("static"));
        keywords.insert(QLatin1String("super"));
        keywords.insert(QLatin1String("switch"));
        keywords.insert(QLatin1String("synchronized"));
        keywords.insert(QLatin1String("this"));
        keywords.insert(QLatin1String("throw"));
        keywords.insert(QLatin1String("throws"));
        keywords.insert(QLatin1String("transient"));
        keywords.insert(QLatin1String("true"));
        keywords.insert(QLatin1String("try"));
        keywords.insert(QLatin1String("typeof"));
        keywords.insert(QLatin1String("undefined"));
        keywords.insert(QLatin1String("var"));
        keywords.insert(QLatin1String("void"));
        keywords.insert(QLatin1String("volatile"));
        keywords.insert(QLatin1String("while"));
        keywords.insert(QLatin1String("with"));    // end
    }
    return keywords;
}

static QSet<QChar> alphaChars() {
    QSet<QChar> rc;
    const QString alpha = QLatin1String("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    foreach (QChar chr, alpha)
        rc.insert(chr);
    return rc;
}


GCF::Components::ScriptHighlighter::ScriptHighlighter(GCF::Components::ScriptEditor *parent)
    : QSyntaxHighlighter(parent)
{
    m_numberFormat.setForeground(Qt::blue);
    m_stringFormat.setForeground(Qt::darkGreen);
    m_typeFormat.setForeground(Qt::darkMagenta);
    m_keywordFormat.setForeground(Qt::darkYellow);
    m_labelFormat.setForeground(Qt::darkRed);
    m_commentFormat.setForeground(Qt::red);
    //m_commentFormat.setFontFamily("times");
    m_commentFormat.setFontItalic(true);
    m_preProcessorFormat.setForeground(Qt::darkBlue);
}

void GCF::Components::ScriptHighlighter::highlightBlock(const QString &text)
{
    // states
    enum {
        StateStandard,
        StateCommentStart1,
        StateCCommentStart2,
        StateCppCommentStart2,
        StateCComment,
        StateCppComment,
        StateCCommentEnd1,
        StateCCommentEnd2,
        StateStringStart,
        StateString,
        StateStringEnd,
        StateString2Start,
        StateString2,
        StateString2End,
        StateNumber,
        StatePreProcessor,
        NumStates
    };
    // tokens
    enum {
        InputAlpha,
        InputNumber,
        InputAsterix,
        InputSlash,
        InputParen,
        InputSpace,
        InputHash,
        InputQuotation,
        InputApostrophe,
        InputSep,
        NumInputs
    };

    static const uchar table[NumStates][NumInputs] = {
        { StateStandard,      StateNumber,     StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateStandard
        { StateStandard,      StateNumber,   StateCCommentStart2, StateCppCommentStart2, StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateCommentStart1
        { StateCComment,      StateCComment,   StateCCommentEnd1,   StateCComment,         StateCComment,   StateCComment,   StateCComment,     StateCComment,    StateCComment,     StateCComment }, // StateCCommentStart2
        { StateCppComment,    StateCppComment, StateCppComment,     StateCppComment,       StateCppComment, StateCppComment, StateCppComment,   StateCppComment,  StateCppComment,   StateCppComment }, // CppCommentStart2
        { StateCComment,      StateCComment,   StateCCommentEnd1,   StateCComment,         StateCComment,   StateCComment,   StateCComment,     StateCComment,    StateCComment,     StateCComment }, // StateCComment
        { StateCppComment,    StateCppComment, StateCppComment,     StateCppComment,       StateCppComment, StateCppComment, StateCppComment,   StateCppComment,  StateCppComment,   StateCppComment }, // StateCppComment
        { StateCComment,      StateCComment,   StateCCommentEnd1,   StateCCommentEnd2,     StateCComment,   StateCComment,   StateCComment,     StateCComment,    StateCComment,     StateCComment }, // StateCCommentEnd1
        { StateStandard,      StateNumber,     StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateCCommentEnd2
        { StateString,        StateString,     StateString,         StateString,           StateString,     StateString,     StateString,       StateStringEnd,   StateString,       StateString }, // StateStringStart
        { StateString,        StateString,     StateString,         StateString,           StateString,     StateString,     StateString,       StateStringEnd,   StateString,       StateString }, // StateString
        { StateStandard,      StateStandard,   StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateStringEnd
        { StateString2,       StateString2,    StateString2,        StateString2,          StateString2,    StateString2,    StateString2,      StateString2,     StateString2End,   StateString2 }, // StateString2Start
        { StateString2,       StateString2,    StateString2,        StateString2,          StateString2,    StateString2,    StateString2,      StateString2,     StateString2End,   StateString2 }, // StateString2
        { StateStandard,      StateStandard,   StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateString2End
        { StateNumber,        StateNumber,     StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard }, // StateNumber
        { StatePreProcessor,  StateStandard,   StateStandard,       StateCommentStart1,    StateStandard,   StateStandard,   StatePreProcessor, StateStringStart, StateString2Start, StateStandard } // StatePreProcessor
    };

    QString buffer;
    buffer.reserve(text.length());
    QTextCharFormat emptyFormat;

    int state = StateStandard;
    const int previousState = previousBlockState();
    if (previousState != -1)
        state = previousState;

    if (text.isEmpty()) {
        setCurrentBlockState(previousState);
        return;
    }

    int input = -1;
    int i = 0;
    bool lastWasBackSlash = false;
    bool makeLastStandard = false;

    static const QSet<QChar> alphabeth = alphaChars();
    static const QString mathChars = QString::fromLatin1("xXeE");
    static const QString numbers = QString::fromLatin1("0123456789");
    bool questionMark = false;
    QChar lastChar;
    QString firstWord;
    forever {
        const QChar c = text.at(i);

        if (lastWasBackSlash) {
            input = InputSep;
        } else {
            switch (c.toLatin1()) {
                case '*':
                    input = InputAsterix;
                    break;
                case '/':
                    input = InputSlash;
                    break;
                case '(': case '[': case '{':
                    input = InputParen;
                    if (state == StateStandard
                        || state == StateNumber
                        || state == StatePreProcessor
                        || state == StateCCommentEnd2
                        || state == StateCCommentEnd1
                        || state == StateString2End
                        || state == StateStringEnd
                       )
                        //blockData->parentheses << Parenthesis(Parenthesis::Open, c, i);
                    break;
                case ')': case ']': case '}':
                    input = InputParen;
                    if (state == StateStandard
                        || state == StateNumber
                        || state == StatePreProcessor
                        || state == StateCCommentEnd2
                        || state == StateCCommentEnd1
                        || state == StateString2End
                        || state == StateStringEnd
                       ) {
                        //blockData->parentheses << Parenthesis(Parenthesis::Closed, c, i);
                    }
                    break;
                case '#':
                    input = InputHash;
                    break;
                case '"':
                    input = InputQuotation;
                    break;
                case '\'':
                    input = InputApostrophe;
                    break;
                case ' ':
                    input = InputSpace;
                    break;
                case '1': case '2': case '3': case '4': case '5':
                case '6': case '7': case '8': case '9': case '0':
                    if (alphabeth.contains(lastChar)
                        && (!mathChars.contains(lastChar) || !numbers.contains(text.at(i - 1)))) {
                        input = InputAlpha;
                    } else {
                        if (input == InputAlpha && numbers.contains(lastChar))
                            input = InputAlpha;
                        else
                            input = InputNumber;
                    }
                    break;
                case ':': {
                              input = InputAlpha;
                              QChar nextChar = ' ';
                              if (i < text.length() - 1)
                                  nextChar = text.at(i + 1);
                              if (state == StateStandard && !questionMark &&
                                  lastChar != ':' && nextChar != ':') {
                                  for (int j = 0; j < i; ++j) {
                                      if (format(j) == emptyFormat)
                                          setFormat(j, 1, m_labelFormat);
                                  }
                              }
                              break;
                          }
                default: {
                             if (!questionMark && c == QLatin1Char('?'))
                                 questionMark = true;
                             if (c.isLetter() || c == QLatin1Char('_'))
                                 input = InputAlpha;
                             else
                                 input = InputSep;
                         } break;
            }
        }

        lastWasBackSlash = !lastWasBackSlash && c == QLatin1Char('\\');

        if (input == InputAlpha)
            buffer.append(c);

        state = table[state][input];

        switch (state) {
            case StateStandard: {
                                    setFormat(i, 1, emptyFormat);
                                    if (makeLastStandard)
                                        setFormat(i - 1, 1, emptyFormat);
                                    makeLastStandard = false;
                                    if (!buffer.isEmpty() && input != InputAlpha ) {
                                        highlightKeyword(i, buffer);
                                        buffer.clear();
                                    }
                                } break;
            case StateCommentStart1:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = true;
                                buffer.resize(0);
                                break;
            case StateCCommentStart2:
                                setFormat(i - 1, 2, m_commentFormat);
                                makeLastStandard = false;
                                buffer.resize(0);
                                break;
            case StateCppCommentStart2:
                                setFormat(i - 1, 2, m_commentFormat);
                                makeLastStandard = false;
                                buffer.resize(0);
                                break;
            case StateCComment:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_commentFormat);
                                buffer.resize(0);
                                break;
            case StateCppComment:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_commentFormat);
                                buffer.resize(0);
                                break;
            case StateCCommentEnd1:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_commentFormat);
                                buffer.resize(0);
                                break;
            case StateCCommentEnd2:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_commentFormat);
                                buffer.resize(0);
                                break;
            case StateStringStart:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, emptyFormat);
                                buffer.resize(0);
                                break;
            case StateString:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_stringFormat);
                                buffer.resize(0);
                                break;
            case StateStringEnd:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, emptyFormat);
                                buffer.resize(0);
                                break;
            case StateString2Start:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, emptyFormat);
                                buffer.resize(0);
                                break;
            case StateString2:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_stringFormat);
                                buffer.resize(0);
                                break;
            case StateString2End:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, emptyFormat);
                                buffer.resize(0);
                                break;
            case StateNumber:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat( i, 1, m_numberFormat);
                                buffer.resize(0);
                                break;
            case StatePreProcessor:
                                if (makeLastStandard)
                                    setFormat(i - 1, 1, emptyFormat);
                                makeLastStandard = false;
                                setFormat(i, 1, m_preProcessorFormat);
                                buffer.resize(0);
                                break;
        }

        lastChar = c;
        i++;
        if (i >= text.length())
            break;
    }

    highlightKeyword(text.length(), buffer);

    if (state == StateCComment
        || state == StateCCommentEnd1
        || state == StateCCommentStart2
       ) {
        state = StateCComment;
    } else if (state == StateString) {
        state = StateString;
    } else if (state == StateString2) {
        state =  StateString2;
    } else {
        state = StateStandard;
    }

    setCurrentBlockState(state);
}

void GCF::Components::ScriptHighlighter::highlightKeyword(int currentPos, const QString &buffer)
{
    if (buffer.isEmpty())
        return;

    if (buffer.at(0) == QLatin1Char('Q')) {
        setFormat(currentPos - buffer.length(), buffer.length(), m_typeFormat);
    } else {
        if (qscriptKeywords().contains(buffer)) {
            setFormat(currentPos - buffer.length(), buffer.length(), m_keywordFormat);
        }
    }
}


