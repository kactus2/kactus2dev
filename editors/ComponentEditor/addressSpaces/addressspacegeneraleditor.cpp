//-----------------------------------------------------------------------------
// File: addressspacegeneraleditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.02.2012
//
// Description:
// Editor to set the general settings of an address space.
//-----------------------------------------------------------------------------

#include "addressspacegeneraleditor.h"

#include <KactusAPI/include/utils.h>

#include <editors/ComponentEditor/common/ExpressionEditor.h>
#include <KactusAPI/include/ExpressionParser.h>
#include <KactusAPI/include/ExpressionFormatter.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>


#include <IPXACTmodels/Component/AddressSpace.h>

#include <QCompleter>
#include <QIntValidator>
#include <QFormLayout>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::AddressSpaceGeneralEditor()
//-----------------------------------------------------------------------------
AddressSpaceGeneralEditor::AddressSpaceGeneralEditor(QSharedPointer<AddressSpace> addrSpace,
    QStringList busInterfaceNames,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionParser> expressionParser,
    Document::Revision docRevision,
    QWidget *parent):
QGroupBox(tr("General"), parent),
addrSpace_(addrSpace),
expressionParser_(expressionParser),
addrUnitEditor_(this),
rangeEditor_(new ExpressionEditor(parameterFinder, this)),
widthEditor_(new ExpressionEditor(parameterFinder, this)),
isPresentEditor_(new ExpressionEditor(parameterFinder, this)),
initiatorInterfaceBindingLabel_(new QLabel(this))
{
	Q_ASSERT(addrSpace_);

    addrUnitEditor_.setValidator(new QIntValidator(this));
    addrUnitEditor_.setPlaceholderText("8");

    rangeEditor_->setFixedHeight(ExpressionEditor::DEFAULT_HEIGHT);
    widthEditor_->setFixedHeight(ExpressionEditor::DEFAULT_HEIGHT);
    isPresentEditor_->setFixedHeight(ExpressionEditor::DEFAULT_HEIGHT);

	// Set the back ground colors for mandatory fields.
	widthEditor_->setProperty("mandatoryField", true);
	rangeEditor_->setProperty("mandatoryField", true);

    ComponentParameterModel* parameterCompletionModel = new ComponentParameterModel(parameterFinder, this);
    parameterCompletionModel->setExpressionParser(expressionParser);

    auto rangeEditorCompleter = new QCompleter(this);
    rangeEditorCompleter->setModel(parameterCompletionModel);

    auto widthEditorCompleter = new QCompleter(this);
    widthEditorCompleter->setModel(parameterCompletionModel);

    auto isPresentEditorCompleter = new QCompleter(this);
    isPresentEditorCompleter->setModel(parameterCompletionModel);

    rangeEditor_->setAppendingCompleter(rangeEditorCompleter);
    widthEditor_->setAppendingCompleter(widthEditorCompleter);
    isPresentEditor_->setAppendingCompleter(isPresentEditorCompleter);

    setupLayout(docRevision);

	refresh(busInterfaceNames);

    connect(&addrUnitEditor_, SIGNAL(editingFinished()), this, SLOT(onAddrUnitChanged()), Qt::UniqueConnection);
    connect(&addrUnitEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);

	connect(widthEditor_, SIGNAL(editingFinished()), this, SLOT(onWidthChanged()), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    
    connect(widthEditor_, SIGNAL(increaseReference(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(widthEditor_, SIGNAL(decreaseReference(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

	connect(rangeEditor_, SIGNAL(editingFinished()), this, SLOT(onRangeChanged()), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(editingFinished()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);

    connect(rangeEditor_, SIGNAL(increaseReference(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(rangeEditor_, SIGNAL(decreaseReference(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(isPresentEditor_, SIGNAL(editingFinished()), this, SLOT(onIsPresentChanged()), Qt::UniqueConnection);

    connect(isPresentEditor_, SIGNAL(increaseReference(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(isPresentEditor_, SIGNAL(decreaseReference(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::~AddressSpaceGeneralEditor()
//-----------------------------------------------------------------------------
AddressSpaceGeneralEditor::~AddressSpaceGeneralEditor()
{
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::isValid()
//-----------------------------------------------------------------------------
bool AddressSpaceGeneralEditor::isValid() const
{
	return !rangeEditor_->getExpression().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::refresh()
//-----------------------------------------------------------------------------
void AddressSpaceGeneralEditor::refresh(QStringList masterInterfaceNames)
{
    widthEditor_->blockSignals(true);
    rangeEditor_->blockSignals(true);
    isPresentEditor_->blockSignals(true);

    addrUnitEditor_.setText(addrSpace_->getAddressUnitBits());

    widthEditor_->setExpression(addrSpace_->getWidth());
    widthEditor_->setToolTip(format(addrSpace_->getWidth()));

	rangeEditor_->setExpression(addrSpace_->getRange());
    rangeEditor_->setToolTip(format(addrSpace_->getRange()));

    isPresentEditor_->setExpression(addrSpace_->getIsPresent());
    isPresentEditor_->setToolTip(format(addrSpace_->getIsPresent()));

    widthEditor_->blockSignals(false);
    rangeEditor_->blockSignals(false);
    isPresentEditor_->blockSignals(false);

    if (masterInterfaceNames.isEmpty())
    {
        initiatorInterfaceBindingLabel_->setText(tr("No binding"));
    }
    else
    {
        initiatorInterfaceBindingLabel_->setText(masterInterfaceNames.join(", "));
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::onAddrUnitChanged()
//-----------------------------------------------------------------------------
void AddressSpaceGeneralEditor::onAddrUnitChanged()
{
    QString newAddressUnitBits = addrUnitEditor_.text();
    addrSpace_->setAddressUnitBits(newAddressUnitBits);

    addrUnitEditor_.setToolTip(format(newAddressUnitBits));

    emit assignNewAddressUnitBits(newAddressUnitBits);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::onWidthChanged()
//-----------------------------------------------------------------------------
void AddressSpaceGeneralEditor::onWidthChanged()
{
    widthEditor_->finishEditingCurrentWord();

    addrSpace_->setWidth(widthEditor_->getExpression());   
    widthEditor_->setToolTip(format(widthEditor_->getExpression()));

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::onRangeChanged()
//-----------------------------------------------------------------------------
void AddressSpaceGeneralEditor::onRangeChanged()
{
    rangeEditor_->finishEditingCurrentWord();
    addrSpace_->setRange(rangeEditor_->getExpression());
    rangeEditor_->setToolTip(format(rangeEditor_->getExpression()));

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::onIsPresentChanged()
//-----------------------------------------------------------------------------
void AddressSpaceGeneralEditor::onIsPresentChanged()
{
    isPresentEditor_->finishEditingCurrentWord();
    addrSpace_->setIsPresent(isPresentEditor_->getExpression());
    isPresentEditor_->setToolTip(format(isPresentEditor_->getExpression()));

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::setupLayout()
//-----------------------------------------------------------------------------
void AddressSpaceGeneralEditor::setupLayout(Document::Revision docRevision)
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);

    QFormLayout* layout = new QFormLayout();
    layout->addRow(tr("Addressable unit bits (AUB):"), &addrUnitEditor_);
    layout->addRow(tr("Range (=size) [AUB], f(x):"), rangeEditor_);
    layout->addRow(tr("Width [bits], f(x):"), widthEditor_);

    if (docRevision == Document::Revision::Std22)
    {
        layout->addRow(tr("Initiator interface binding(s):"), initiatorInterfaceBindingLabel_);
        isPresentEditor_->setVisible(false);
    }
    else
    {
        layout->addRow(tr("Is present, f(x):"), isPresentEditor_);
        layout->addRow(tr("Master interface binding(s):"), initiatorInterfaceBindingLabel_);
    }

    topLayout->addLayout(layout);
    topLayout->addStretch(1); // For initiator/master interface binding label alignment
}

//-----------------------------------------------------------------------------
// Function: AddressSpaceGeneralEditor::format()
//-----------------------------------------------------------------------------
QString AddressSpaceGeneralEditor::format(QString const& expression) const
{
    return ExpressionFormatter::format(expression, expressionParser_);
}
