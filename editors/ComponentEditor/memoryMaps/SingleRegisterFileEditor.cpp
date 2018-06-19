#include "SingleRegisterFileEditor.h"

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <QFormLayout>
#include <QScrollArea>
#include <QSplitter>

SingleRegisterFileEditor::SingleRegisterFileEditor(
        QSharedPointer<RegisterFile> registerFile,
        QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<RegisterFileValidator> registerFileValidator,
        QWidget* parent /* = 0 */):
    ItemEditor(component, handler, parent),
    registerFile_(registerFile),
    nameEditor_(registerFile, this, tr("Register File name and description")),
    registerFileEditor_(new RegisterFileEditor(registerFile_, component, handler, parameterFinder, expressionFormatter, registerFileValidator, this)),
    offsetEditor_(new ExpressionEditor(parameterFinder, this)),
    rangeEditor_(new ExpressionEditor(parameterFinder, this)),
    dimensionEditor_(new ExpressionEditor(parameterFinder, this)),
    isPresentEditor_(new ExpressionEditor(parameterFinder, this)),
    expressionParser_(expressionParser),
    registerFileValidator_(registerFileValidator)
{

    offsetEditor_->setFixedHeight(20);
    rangeEditor_->setFixedHeight(20);
    dimensionEditor_->setFixedHeight(20);
    isPresentEditor_->setFixedHeight(20);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser_);

    ParameterCompleter* offsetParameterCompleter = new ParameterCompleter(this);
    offsetParameterCompleter->setModel(componentParametersModel);

    ParameterCompleter* rangeParameterCompleter = new ParameterCompleter(this);
    rangeParameterCompleter->setModel(componentParametersModel);

    ParameterCompleter* dimensionParameterCompleter = new ParameterCompleter(this);
    dimensionParameterCompleter->setModel(componentParametersModel);

    ParameterCompleter* isPresentCompleter = new ParameterCompleter(this);
    isPresentCompleter->setModel(componentParametersModel);

    offsetEditor_->setAppendingCompleter(offsetParameterCompleter);
    rangeEditor_->setAppendingCompleter(rangeParameterCompleter);
    dimensionEditor_->setAppendingCompleter(dimensionParameterCompleter);
    isPresentEditor_->setAppendingCompleter(isPresentCompleter);

    setupLayout();

    connectSignals();

    offsetEditor_->setProperty("mandatoryField", true);
    rangeEditor_->setProperty("mandatoryField", true);
}




//-----------------------------------------------------------------------------
// Function: SingleRegisterFileEditor::~SingleRegisterFileEditor()
//-----------------------------------------------------------------------------
SingleRegisterFileEditor::~SingleRegisterFileEditor()
{

}

//-----------------------------------------------------------------------------
// Function: SingleRegisterFileEditor::showEvent()
//-----------------------------------------------------------------------------
void SingleRegisterFileEditor::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/addressblock.html");
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterFileEditor::refresh()
//-----------------------------------------------------------------------------
void SingleRegisterFileEditor::refresh()
{
    nameEditor_.refresh();
    registerFileEditor_->refresh();

    changeExpressionEditorsSignalBlockStatus(true);

    offsetEditor_->setExpression(registerFile_->getAddressOffset());
    offsetEditor_->setToolTip(formattedValueFor(registerFile_->getAddressOffset()));

    rangeEditor_->setExpression(registerFile_->getRange());
    rangeEditor_->setToolTip(formattedValueFor(registerFile_->getRange()));

    dimensionEditor_->setExpression(registerFile_->getDimension());
    dimensionEditor_->setToolTip(formattedValueFor(registerFile_->getDimension()));

    isPresentEditor_->setExpression(registerFile_->getIsPresent());
    isPresentEditor_->setToolTip(formattedValueFor(registerFile_->getIsPresent()));

    changeExpressionEditorsSignalBlockStatus(false);
}


//-----------------------------------------------------------------------------
// Function: SingleRegisterFileEditor::onRangeChanged()
//-----------------------------------------------------------------------------
void SingleRegisterFileEditor::onRangeEdited()
{
    rangeEditor_->finishEditingCurrentWord();
    registerFile_->setRange(rangeEditor_->getExpression());

    rangeEditor_->setToolTip(formattedValueFor(registerFile_->getRange()));
}

void SingleRegisterFileEditor::onDimensionEdited()
{
    dimensionEditor_->finishEditingCurrentWord();

    QString newDimension = dimensionEditor_->getExpression();
    registerFile_->setDimension(newDimension);

    dimensionEditor_->setToolTip(formattedValueFor(newDimension));
}

void SingleRegisterFileEditor::onOffsetEdited()
{
    offsetEditor_->finishEditingCurrentWord();
    registerFile_->setAddressOffset(offsetEditor_->getExpression());

    QString formattedAddressOffset = formattedValueFor(registerFile_->getAddressOffset());
    offsetEditor_->setToolTip(formattedAddressOffset);
}
//-----------------------------------------------------------------------------
// Function: SingleRegisterEditor::onIsPresentEdited()
//-----------------------------------------------------------------------------
void SingleRegisterFileEditor::onIsPresentEdited()
{
    isPresentEditor_->finishEditingCurrentWord();

    QString newIsPresent = isPresentEditor_->getExpression();
    registerFile_->setIsPresent(newIsPresent);

    isPresentEditor_->setToolTip(formattedValueFor(newIsPresent));
}

//-----------------------------------------------------------------------------
// Function: SingleRegisterFileEditor::formattedValueFor()
//-----------------------------------------------------------------------------
QString SingleRegisterFileEditor::formattedValueFor(QString const& expression) const
{
    return ExpressionFormatter::format(expression, expressionParser_);
}


//-----------------------------------------------------------------------------
// Function: SingleRegisterFileEditor::setupLayout()
//-----------------------------------------------------------------------------
void SingleRegisterFileEditor::setupLayout()
{
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox* registerDefinitionGroup = new QGroupBox(tr("Register definition"));

    QFormLayout* registerDefinitionLayout = new QFormLayout(registerDefinitionGroup);
    registerDefinitionLayout->addRow(tr("Offset [AUB], f(x):"), offsetEditor_);
    registerDefinitionLayout->addRow(tr("Range [AUB], f(x):"), rangeEditor_);
    registerDefinitionLayout->addRow(tr("Dimension, f(x):"), dimensionEditor_);
    registerDefinitionLayout->addRow(tr("Is present, f(x):"), isPresentEditor_);


    QHBoxLayout* topOfPageLayout = new QHBoxLayout();
    topOfPageLayout->addWidget(&nameEditor_, 0);
    topOfPageLayout->addWidget(registerDefinitionGroup, 0);

    QWidget* topOfPageWidget = new QWidget();
    topOfPageWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    topOfPageWidget->setLayout(topOfPageLayout);
    topOfPageWidget->setContentsMargins(0, 0, 0, 0);

    QSplitter* verticalSplitter = new QSplitter(Qt::Vertical, scrollArea);
    verticalSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    verticalSplitter->addWidget(topOfPageWidget);
    verticalSplitter->addWidget(registerFileEditor_);
    verticalSplitter->setStretchFactor(1, 1);

    QSplitterHandle* handle = verticalSplitter->handle(1);
    QVBoxLayout* handleLayout = new QVBoxLayout(handle);
    handleLayout->setSpacing(0);
    handleLayout->setMargin(0);

    QFrame* line = new QFrame(handle);
    line->setLineWidth(2);
    line->setMidLineWidth(2);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    handleLayout->addWidget(line);

    verticalSplitter->setHandleWidth(10);

    scrollArea->setWidget(verticalSplitter);
}
//-----------------------------------------------------------------------------
// Function: SingleRegisterFileEditor::connectSignals()
//-----------------------------------------------------------------------------
void SingleRegisterFileEditor::connectSignals()
{
    connect(registerFileEditor_, SIGNAL(childAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
    connect(registerFileEditor_, SIGNAL(childRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);
    connect(registerFileEditor_, SIGNAL(increaseReferences(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(registerFileEditor_, SIGNAL(decreaseReferences(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);

    connect(offsetEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(dimensionEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(dimensionEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(increaseReference(QString const&)),
        this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(decreaseReference(QString const&)),
        this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);

    connect(offsetEditor_, SIGNAL(editingFinished()), this, SLOT(onOffsetEdited()), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(editingFinished()), this, SLOT(onRangeEdited()), Qt::UniqueConnection);
    connect(dimensionEditor_, SIGNAL(editingFinished()), this, SLOT(onDimensionEdited()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SLOT(onIsPresentEdited()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(dimensionEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(registerFileEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&nameEditor_, SIGNAL(nameChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(offsetEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(dimensionEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(registerFileEditor_, SIGNAL(graphicsChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
}


//-----------------------------------------------------------------------------
// Function: SingleRegisterFileEditor::changeExpressionEditorSignalBlocks()
//-----------------------------------------------------------------------------
void SingleRegisterFileEditor::changeExpressionEditorsSignalBlockStatus(bool blockStatus) const
{
    offsetEditor_->blockSignals(blockStatus);
    rangeEditor_->blockSignals(blockStatus);
    dimensionEditor_->blockSignals(blockStatus);
    isPresentEditor_->blockSignals(blockStatus);
}
