//-----------------------------------------------------------------------------
// File: CSourceWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 9.5.2011
//
// Description:
// C source widget.
//-----------------------------------------------------------------------------

#include "CSourceWidget.h"

#include "CSourceTextEdit.h"
#include "CSourceContentMatcher.h"
#include "CSourceHighlighter.h"

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/kactusExtensions/ApiDefinition.h>
#include <IPXACTmodels/kactusExtensions/ApiInterface.h>
#include <IPXACTmodels/kactusExtensions/ComDefinition.h>

#include <common/TextEditProvider.h>

#include <QApplication>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QPrinter>
#include <QPrintDialog>
#include <QSettings>
#include <QSharedPointer>
#include <QTextStream>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: CSourceWidget::CSourceWidget()
//-----------------------------------------------------------------------------
CSourceWidget::CSourceWidget(QString const& sourceFile, QSharedPointer<Component> ownerComponent,
    LibraryInterface* libInterface, QWidget* mainWnd, QWidget* parent):
TabDocument(parent, DOC_PRINT_SUPPORT | DOC_EDIT_SUPPORT),
    textEdit_(new CSourceTextEdit(mainWnd, this)),
    sourceFile_(sourceFile),
    editProvider_(new TextEditProvider(*textEdit_))
{
    // Initialize content matcher and highlighter for the text editor.
    textEdit_->getMatcher().setOwner(ownerComponent);

    foreach (QSharedPointer<ApiInterface const> apiIf, ownerComponent->getApiInterfaces())
    {
        QSharedPointer<Document const> libComp = libInterface->getModelReadOnly(apiIf->getApiType());
        QSharedPointer<ApiDefinition const> apiDef = libComp.dynamicCast<ApiDefinition const>();

        if (apiDef != 0)
        {
            textEdit_->getMatcher().addSourceApiDefinition(apiDef);
            textEdit_->getHighlighter().registerAPI(apiDef);

            if (apiDef->getComDefinitionRef().isValid())
            {
                libComp = libInterface->getModelReadOnly(apiDef->getComDefinitionRef());
                QSharedPointer<ComDefinition const> comDef = libComp.dynamicCast<ComDefinition const>();

                if (comDef != 0)
                {
                    textEdit_->getMatcher().addSourceComDefinition(comDef);
                }
            }
        }
    }

    // Create the layout.
	QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(textEdit_);

    // Read and apply code editor settings.
    QSettings settings;
    applySettings(settings);
    
    readFileContentFromDisk();

    setDocumentName(QFileInfo(sourceFile).fileName());
    setDocumentType("Code");

    connect(textEdit_->document(), SIGNAL(contentsChanged()),
            this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(this, SIGNAL(contentChanged()), this, SLOT(onContentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: CSourceWidget::~CSourceWidget()
//-----------------------------------------------------------------------------
CSourceWidget::~CSourceWidget()
{
}

//-----------------------------------------------------------------------------
// Function: CSourceWidget::loadSettings()
//-----------------------------------------------------------------------------
void CSourceWidget::applySettings(QSettings& settings)
{
    // Read indentation settings.
    IndentStyle style = static_cast<IndentStyle>(settings.value("Editor/IndentStyle",
                                                                INDENT_STYLE_SPACES).toInt());
    unsigned int width = settings.value("Editor/IndentWidth", 4).toInt();    
    textEdit_->setIndentStyle(style, width);

    // Read font settings.
    QFont font = settings.value("Editor/Font", QFont("Courier New", 10)).value<QFont>();
    textEdit_->setFont(font);

    // Read highlight style settings.
    qRegisterMetaTypeStreamOperators<HighlightStyleDesc>("HighlightStyleDesc");

    for (int i = 0; i < CSourceHighlighter::STYLE_COUNT; ++i)
    {
        HighlightStyleDesc styleDesc = settings.value("Editor/Highlight/" + CSourceHighlighter::STYLE_IDS[i],
            QVariant::fromValue(CSourceHighlighter::DEFAULT_STYLES[i])).value<HighlightStyleDesc>();

        textEdit_->getHighlighter().setStyle(static_cast<CSourceHighlighter::StyleType>(i), styleDesc);
    }

    textEdit_->getHighlighter().rehighlight();
}

//-----------------------------------------------------------------------------
// Function: CSourceWidget::getSourceFile()
//-----------------------------------------------------------------------------
QString const& CSourceWidget::getSourceFile() const
{
    return sourceFile_;
}

//-----------------------------------------------------------------------------
// Function: CSourceWidget::save()
//-----------------------------------------------------------------------------
bool CSourceWidget::save()
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
// Function: CSourceWidget::print()
//-----------------------------------------------------------------------------
void CSourceWidget::print()
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
// Function: CSourceWidget::onContentChanged()
//-----------------------------------------------------------------------------
void CSourceWidget::onContentChanged()
{
    setModified(true);
}

//-----------------------------------------------------------------------------
// Function: CSourceWidget::getEditProvider()
//-----------------------------------------------------------------------------
QSharedPointer<IEditProvider> CSourceWidget::getEditProvider() const
{
    return editProvider_;
}

//-----------------------------------------------------------------------------
// Function: CSourceWidget::refresh()
//-----------------------------------------------------------------------------
void CSourceWidget::refresh()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    readFileContentFromDisk();
    
    setModified(false);
    TabDocument::refresh();

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: CSourceWidget::getIdentifyingVLNV()
//-----------------------------------------------------------------------------
VLNV CSourceWidget::getIdentifyingVLNV() const
{
	return getDocumentVLNV();
}

//-----------------------------------------------------------------------------
// Function: CSourceWidget::readFileContentFromDisk()
//-----------------------------------------------------------------------------
void CSourceWidget::readFileContentFromDisk()
{
    // Read the contents of the file to the text edit.
    QFile file(sourceFile_);

    bool success = file.open(QIODevice::ReadOnly | QIODevice::Text);

    if (!success)
    {
        // TODO: Error checking.
        return;
    }

    QTextStream stream(&file);
    textEdit_->setPlainText(stream.readAll());
}
