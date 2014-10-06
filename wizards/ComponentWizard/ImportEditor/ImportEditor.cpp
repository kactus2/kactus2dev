//-----------------------------------------------------------------------------
// File: ImportEditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 10.6.2013
//
// Description:
// Used to parse source files and generating IP-XACT packages of them.
//-----------------------------------------------------------------------------

#include "ImportEditor.h"
#include "ModelParameterEditorAdapter.h"

#include <common/widgets/FileSelector/fileselector.h>

#include <editors/ComponentEditor/modelParameters/modelparametereditor.h>
#include <editors/ComponentEditor/ports/portseditor.h>

#include <library/LibraryManager/libraryinterface.h>

#include <wizards/ComponentWizard/ImportRunner.h>
#include <wizards/ComponentWizard/ImportEditor/ImportHighlighter.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>
#include <QScrollBar>
#include <QTextStream>
#include <QTextBlock>

//-----------------------------------------------------------------------------
// Function: ImportEditor::ImportEditor()
//-----------------------------------------------------------------------------
ImportEditor::ImportEditor(QSharedPointer<Component> component, 
	LibraryInterface* handler,
    PluginManager const& pluginMgr,
	QWidget *parent):
    QWidget(parent),
    splitter_(Qt::Vertical, this),
	componentXmlPath_(handler->getPath(*component->getVlnv())),
    component_(component),
    importComponent_(new Component(*component_)),
    selectedSourceFile_(),
    modelParameterEditor_(new ModelParameterEditor(importComponent_, handler, &splitter_)),
    portEditor_(new PortsEditor(importComponent_, handler, &splitter_)),
    sourceDisplayer_(new QPlainTextEdit(this)),
    fileSelector_(new FileSelector(component, this)),
    editButton_(new QPushButton(tr("Open editor"), this)),
    refreshButton_(new QPushButton(QIcon(":/icons/common/graphics/refresh.png"), "", this)),
    highlighter_(new ImportHighlighter(sourceDisplayer_, this)),
    modelParameterAdapter_(modelParameterEditor_),
    runner_(new ImportRunner(this))
{
	// CSV import/export is disabled in the wizard.
	modelParameterEditor_->setAllowImportExport(false);
	portEditor_->setAllowImportExport(false);

    runner_->setHighlighter(highlighter_);
    runner_->setModelParameterVisualizer(&modelParameterAdapter_);
    runner_->loadImportPlugins(pluginMgr);

    setSourceDisplayFormatting();

    connect(modelParameterEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(portEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(fileSelector_, SIGNAL(fileSelected(const QString&)),
        this, SLOT(onFileSelected(const QString&)), Qt::UniqueConnection);

    connect(refreshButton_, SIGNAL(clicked()),this, SLOT(onRefresh()), Qt::UniqueConnection);
    connect(editButton_, SIGNAL(clicked()), this, SLOT(onOpenEditor()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::~ImportEditor()
//-----------------------------------------------------------------------------
ImportEditor::~ImportEditor()
{

}

//-----------------------------------------------------------------------------
// Function: ImportEditor::initializeFileSelection()
//-----------------------------------------------------------------------------
void ImportEditor::initializeFileSelection()
{
    QStringList possibleFileTypes = runner_->importFileTypes();

    fileSelector_->clearFilters();

    QStringList possibleSuffixes = fileExtensionsForTypes(possibleFileTypes);
    foreach(QString possibleFileSuffix, possibleSuffixes)
    {
        fileSelector_->addFilter(possibleFileSuffix);
    }

    onRefresh();
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::checkEditorValidity()
//-----------------------------------------------------------------------------
bool ImportEditor::checkEditorValidity() const
{
    return modelParameterEditor_->isValid() && portEditor_->isValid();
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::onFileSelected()
//-----------------------------------------------------------------------------
void ImportEditor::onFileSelected(QString const& filePath)
{
    selectedSourceFile_ = filePath;

    onRefresh();
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::onOpenEditor()
//-----------------------------------------------------------------------------
void ImportEditor::onOpenEditor() 
{
    if (!selectedSourceFile_.isEmpty())
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(selectedFileAbsolutePath()));
    }
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::onRefresh()
//-----------------------------------------------------------------------------
void ImportEditor::onRefresh() 
{
    loadFileToDisplay();
    importComponent_ = runner_->parse(selectedSourceFile_, componentXmlPath_, component_);

    scrollSourceDisplayToFirstHighlight();
    portEditor_->setComponent(importComponent_);
    modelParameterEditor_->setComponent(importComponent_);

    emit componentChanged(importComponent_);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::setSourceEditorFormatting()
//-----------------------------------------------------------------------------
void ImportEditor::setSourceDisplayFormatting()
{
    QFont font("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(9);

    sourceDisplayer_->setFont(font);
    sourceDisplayer_->setTabStopWidth(4 * fontMetrics().width(' '));
    sourceDisplayer_->setReadOnly(true);
    sourceDisplayer_->setCursorWidth(0);
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::fileSuffixesForTypes()
//-----------------------------------------------------------------------------
QStringList ImportEditor::fileExtensionsForTypes(QStringList possibleFileTypes) const
{
    QStringList fileSuffixes;

    QSettings settings;
    settings.beginGroup("FileTypes");

    foreach (QString const& fileType, possibleFileTypes)
    {
        settings.beginGroup(fileType);

        QStringList fileTypeExtensions = settings.value("Extensions").toString().split(';');
        foreach (QString const& extension, fileTypeExtensions)
        {
            if (!fileSuffixes.contains(extension))
            {
                fileSuffixes.append(extension);
            }
        }
        settings.endGroup();
    }

    settings.endGroup();

    return fileSuffixes;
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::selectedFileAbsolutePath()
//-----------------------------------------------------------------------------
QString ImportEditor::selectedFileAbsolutePath() const
{
    return General::getAbsolutePath(componentXmlPath_, selectedSourceFile_);
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::loadFileToDisplay()
//-----------------------------------------------------------------------------
void ImportEditor::loadFileToDisplay()
{
    if (selectedSourceFile_.isEmpty())
    {
        sourceDisplayer_->setPlainText(QString());
        return;
    }

    QFile importFile(selectedFileAbsolutePath());
    if (!importFile.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(0, "Error ", importFile.errorString() + ": " + selectedFileAbsolutePath());
        return;
    }

    QTextStream stream(&importFile);
    sourceDisplayer_->setPlainText(stream.readAll().replace("\r\n", "\n"));       
    importFile.close();   
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::scrollToFirstHighlight()
//-----------------------------------------------------------------------------
void ImportEditor::scrollSourceDisplayToFirstHighlight() const
{
    QTextCursor cursor = sourceDisplayer_->textCursor();
    cursor.movePosition(QTextCursor::Start);
    QTextCharFormat initialFormat = cursor.charFormat();

    while (cursor.movePosition(QTextCursor::NextBlock) && cursor.charFormat() == initialFormat)
    {
    }

    if (!cursor.atEnd())
    {
        int row = cursor.block().firstLineNumber();
        sourceDisplayer_->verticalScrollBar()->setValue(row);
    }
}

//-----------------------------------------------------------------------------
// Function: ImportEditor::setupLayout()
//-----------------------------------------------------------------------------
void ImportEditor::setupLayout()
{
    // Make the splitter handles visible by creating lines. Handle 0 is always invisible.
    const int handles = 3;
    for ( int i = 1; i < handles; i++)
    {
        QSplitterHandle* handle = splitter_.handle(i);

        QVBoxLayout* layout = new QVBoxLayout(handle);
        layout->setSpacing(0);
        layout->setMargin(0);

        QFrame* line = new QFrame(handle);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        layout->addWidget(line);
    }

	QVBoxLayout* topLayout = new QVBoxLayout(this);    
    topLayout->addWidget(&splitter_);
    topLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* sourceWidget = new QWidget(this);
    QVBoxLayout* sourceLayout = new QVBoxLayout(sourceWidget);    

    sourceLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* selectorLayout = new QHBoxLayout();
    QLabel* vhdlLabel = new QLabel(tr("Top-level file to import:"), this);
    selectorLayout->addWidget(vhdlLabel);
    selectorLayout->addWidget(fileSelector_, 1);
    selectorLayout->addWidget(editButton_);    
    selectorLayout->addWidget(refreshButton_);    

    sourceLayout->addLayout(selectorLayout);
    sourceLayout->addWidget(sourceDisplayer_);

    splitter_.addWidget(sourceWidget);
    splitter_.addWidget(modelParameterEditor_);
    splitter_.addWidget(portEditor_);
    splitter_.setStretchFactor(0, 1);
}
