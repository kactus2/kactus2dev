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

#include "PortEditorAdapter.h"
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
// Function: VhdlImportEditor::VhdlImportEditor()
//-----------------------------------------------------------------------------
ImportEditor::ImportEditor(QSharedPointer<Component> component, 
	LibraryInterface* handler,
    PluginManager const& pluginMgr,
	QWidget *parent):
    QWidget(parent),
    splitter_(Qt::Vertical, this),
	componentXmlPath_(handler->getPath(*component->getVlnv())),
    component_(component),
    selectedSourceFile_(),
    modelParameterEditor_(new ModelParameterEditor(component, handler, &splitter_)),
    portEditor_(new PortsEditor(component, handler, &splitter_)),
    sourceDisplayer_(new QPlainTextEdit(this)),
    fileSelector_(new FileSelector(component, this)),
    editButton_(new QPushButton(tr("Open editor"), this)),
    refreshButton_(new QPushButton(QIcon(":/icons/common/graphics/refresh.png"), "", this)),
    highlighter_(new ImportHighlighter(sourceDisplayer_, this)),
    portAdapter_(portEditor_),
    modelParameterAdapter_(modelParameterEditor_),
    runner_(new ImportRunner(this))
{
	Q_ASSERT(component);

	// CSV import/export is disabled in the wizard.
	modelParameterEditor_->setAllowImportExport(false);
	portEditor_->setAllowImportExport(false);

    runner_->setHighlighter(highlighter_);
    runner_->setPortVisualizer(&portAdapter_);
    runner_->setModelParameterVisualizer(&modelParameterAdapter_);
    runner_->loadImportPlugins(pluginMgr);

    setSourceDisplayFormatting();

    connect(modelParameterEditor_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(portEditor_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(fileSelector_, SIGNAL(fileSelected(const QString&)),
        this, SLOT(onFileSelected(const QString&)), Qt::UniqueConnection);

    connect(refreshButton_, SIGNAL(clicked()),this, SLOT(onRefresh()), Qt::UniqueConnection);
    connect(editButton_, SIGNAL(clicked()), this, SLOT(onOpenEditor()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor::~VhdlImportEditor()
//-----------------------------------------------------------------------------
ImportEditor::~ImportEditor()
{

}

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor::initializeFileSelection()
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
}

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor::checkEditorValidity()
//-----------------------------------------------------------------------------
bool ImportEditor::checkEditorValidity() const
{
    return modelParameterEditor_->isValid() && portEditor_->isValid();
}

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor::onFileSelected()
//-----------------------------------------------------------------------------
void ImportEditor::onFileSelected(QString const& filePath)
{
    if (filePath.isEmpty())
    {
        return;
    }

    selectedSourceFile_ = filePath;

    onRefresh();
}

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor::onOpenEditor()
//-----------------------------------------------------------------------------
void ImportEditor::onOpenEditor() 
{
    if (!selectedSourceFile_.isEmpty())
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(selectedFileAbsolutePath()));
    }
}

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor::onRefresh()
//-----------------------------------------------------------------------------
void ImportEditor::onRefresh() 
{
    if (!selectedSourceFile_.isEmpty())
    {
        loadFileToDisplay();
        runner_->parse(selectedSourceFile_, componentXmlPath_, component_);

        scrollSourceDisplayToFirstHighlight();
    }
}

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor::setSourceEditorFormatting()
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
// Function: VhdlImportEditor::fileSuffixesForTypes()
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
// Function: VhdlImportEditor::selectedFileAbsolutePath()
//-----------------------------------------------------------------------------
QString ImportEditor::selectedFileAbsolutePath() const
{
    return General::getAbsolutePath(componentXmlPath_, selectedSourceFile_);
}

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor::loadFileToDisplay()
//-----------------------------------------------------------------------------
void ImportEditor::loadFileToDisplay()
{
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
// Function: VhdlImportEditor::scrollToFirstHighlight()
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
// Function: VhdlImportEditor::setupLayout()
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
    QLabel* vhdlLabel = new QLabel(tr("Top-level VHDL file:"), this);
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
