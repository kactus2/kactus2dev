//-----------------------------------------------------------------------------
// File: MCAPISourceWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.5.2011
//
// Description:
// MCAPI source widget.
//-----------------------------------------------------------------------------

#include "MCAPISourceWidget.h"

#include "MCAPITextEdit.h"
#include "MCAPIContentMatcher.h"
#include "MCAPIHighlighter.h"

#include <common/TextEditProvider.h>

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QCoreApplication>
#include <QSettings>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: MCAPISourceWidget()
//-----------------------------------------------------------------------------
MCAPISourceWidget::MCAPISourceWidget(QString const& sourceFile,
                                     QSharedPointer<MCAPIContentMatcher> mcapiMatcher,
                                     QWidget* mainWnd, QWidget* parent) : TabDocument(parent, DOC_PRINT_SUPPORT | DOC_EDIT_SUPPORT),
                                                                          sourceFile_(sourceFile),
                                                                          editProvider_()
{
    textEdit_ = new MCAPITextEdit(mcapiMatcher, mainWnd, this);
    editProvider_ = QSharedPointer<TextEditProvider>(new TextEditProvider(*textEdit_));

	QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(textEdit_);

    QSettings settings;
    applySettings(settings);
    
    // Read the contents of the file to the text edit.
    QFile file(sourceFile);

    bool success = file.open(QIODevice::ReadOnly | QIODevice::Text);
    Q_ASSERT(success);

    QTextStream stream(&file);
    textEdit_->setPlainText(stream.readAll());

    setDocumentName(QFileInfo(sourceFile).fileName());
    setDocumentType("Code");

    connect(textEdit_->document(), SIGNAL(contentsChanged()),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(this, SIGNAL(contentChanged()),
        this, SLOT(onContentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ~MCAPISourceWidget()
//-----------------------------------------------------------------------------
MCAPISourceWidget::~MCAPISourceWidget()
{
}

//-----------------------------------------------------------------------------
// Function: loadSettings()
//-----------------------------------------------------------------------------
void MCAPISourceWidget::applySettings(QSettings const& settings)
{
    // Read indentation settings.
    MCAPITextEdit::IndentStyle style =
        static_cast<MCAPITextEdit::IndentStyle>(settings.value("editor/indentStyle",
                                                               MCAPITextEdit::INDENT_STYLE_SPACES).toInt());
    unsigned int width = settings.value("editor/indentWidth", 4).toInt();    
    textEdit_->setIndentStyle(style, width);

    // Read font settings.
    QFont font = settings.value("editor/font", QFont("Courier New", 10)).value<QFont>();
    textEdit_->setFont(font);

    // Read highlight style settings.
    qRegisterMetaTypeStreamOperators<HighlightStyleDesc>("HighlightStyleDesc");

    for (int i = 0; i < MCAPIHighlighter::STYLE_COUNT; ++i)
    {
        HighlightStyleDesc styleDesc = settings.value("editor/highlight/" + MCAPIHighlighter::STYLE_IDS[i],
            QVariant::fromValue(MCAPIHighlighter::DEFAULT_STYLES[i])).value<HighlightStyleDesc>();

        textEdit_->getHighlighter().setStyle(static_cast<MCAPIHighlighter::StyleType>(i), styleDesc);
    }

    textEdit_->getHighlighter().applyStyles();
}

//-----------------------------------------------------------------------------
// Function: getSourceFile()
//-----------------------------------------------------------------------------
QString const& MCAPISourceWidget::getSourceFile() const
{
    return sourceFile_;
}

//-----------------------------------------------------------------------------
// Function: save()
//-----------------------------------------------------------------------------
bool MCAPISourceWidget::save()
{
    QFile file(sourceFile_);

    // Check if the file could not be opened.
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
                           tr("Unable to save source file ") + sourceFile_ +
                           tr("! The file may be read only."), QMessageBox::Ok, this);

        msgBox.exec();
        return false;
    }

    file.write(textEdit_->toPlainText().toStdString().c_str());

    return TabDocument::save();
}

//-----------------------------------------------------------------------------
// Function: print()
//-----------------------------------------------------------------------------
void MCAPISourceWidget::print()
{
    QPrinter printer;

    // Query the user for printing settings.
    QPrintDialog dlg(&printer, this);
    dlg.setWindowTitle(tr("Print Source Document"));

    if (textEdit_->textCursor().hasSelection())
    {
        dlg.addEnabledOption(QAbstractPrintDialog::PrintSelection);
    }

    if (dlg.exec() != QDialog::Accepted)
    {
        return;
    }
    
    // Print the document.
    textEdit_->print(&printer);
}

//-----------------------------------------------------------------------------
// Function: onContentChanged()
//-----------------------------------------------------------------------------
void MCAPISourceWidget::onContentChanged()
{
    setModified(true);
}

//-----------------------------------------------------------------------------
// Function: getEditProvider()
//-----------------------------------------------------------------------------
IEditProvider* MCAPISourceWidget::getEditProvider()
{
    return editProvider_.data();
}
