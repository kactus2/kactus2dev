//-----------------------------------------------------------------------------
// File: CpuDetailsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 16.6.2023
//
// Description:
// Editor for cpu properties.
//-----------------------------------------------------------------------------

#include "CpuDetailsEditor.h"

#include <IPXACTmodels/generaldeclarations.h>

#include <KactusAPI/include/ExpressionFormatter.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <QCompleter>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QLabel>

//-----------------------------------------------------------------------------
// Function: CpuDetailsEditor::CpuDetailsEditor()
//-----------------------------------------------------------------------------
CpuDetailsEditor::CpuDetailsEditor(QSharedPointer<Component> component,
    QSharedPointer<Cpu> cpu, QSharedPointer<ParameterFinder> finder,
    QSharedPointer<ExpressionParser> expressionParser,
    QWidget* parent) :
QGroupBox(tr("General"), parent),
    component_(component),
    cpu_(cpu),
    expressionParser_(expressionParser)
{
    createEditors(finder);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: CpuDetailsEditor::refresh()
//-----------------------------------------------------------------------------
void CpuDetailsEditor::refresh()
{
    if (component_->getRevision() == Document::Revision::Std14)
    {
        auto presence = cpu_->getIsPresent();
        presenceEditor_->setExpression(presence);
        presenceEditor_->setToolTip(formattedValueFor(presence));

        addressSpaceRefEditor_->clearItems();

        auto currentRefs = cpu_->getAddressSpaceRefs();
        for (QString const& name : component_->getAddressSpaceNames())
        {
            bool isSelected = currentRefs.contains(name);
            addressSpaceRefEditor_->addItem(name, false, isSelected);
        }
    }
    else if (component_->getRevision() == Document::Revision::Std22)
    {
        memoryMapRefSelector_->refresh(component_->getMemoryMapNames());
        memoryMapRefSelector_->selectItem(cpu_->getMemoryMapReference());

        rangeEditor_->blockSignals(true);
        auto range = cpu_->getRange();
        rangeEditor_->setExpression(cpu_->getRange());
        rangeEditor_->setToolTip(formattedValueFor(range));
        rangeEditor_->blockSignals(false);

        widthEditor_->blockSignals(true);
        auto width = cpu_->getWidth();
        widthEditor_->setExpression(width);
        widthEditor_->setToolTip(formattedValueFor(width));
        widthEditor_->blockSignals(false);

        aubEditor_->blockSignals(true);
        auto aub = cpu_->getAddressUnitBits();
        aubEditor_->setExpression(aub);
        aubEditor_->setToolTip(formattedValueFor(aub));
        aubEditor_->blockSignals(false);
    }
}

//-----------------------------------------------------------------------------
// Function: CpuDetailsEditor::onIsPresentChanged()
//-----------------------------------------------------------------------------
void CpuDetailsEditor::onIsPresentChanged()
{
    presenceEditor_->finishEditingCurrentWord();
    QString presence = presenceEditor_->getExpression();

    if (cpu_->getIsPresent() != presence)
    {
        cpu_->setIsPresent(presence);

        presenceEditor_->setToolTip(formattedValueFor(presence));
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: CpuDetailsEditor::onAddressSpaceRefsChanged()
//-----------------------------------------------------------------------------
void CpuDetailsEditor::onAddressSpaceRefsChanged()
{
    cpu_->setAddressSpaceRefs(addressSpaceRefEditor_->getSelectedItems());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: CpuDetailsEditor::onRangeChanged()
//-----------------------------------------------------------------------------
void CpuDetailsEditor::onRangeChanged()
{
    rangeEditor_->finishEditingCurrentWord();
    QString range = rangeEditor_->getExpression();

    if (cpu_->getRange() != range)
    {
        cpu_->setRange(range);
        rangeEditor_->setToolTip(formattedValueFor(range));

        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: CpuDetailsEditor::onWidthChanged()
//-----------------------------------------------------------------------------
void CpuDetailsEditor::onWidthChanged()
{
    widthEditor_->finishEditingCurrentWord();
    QString width = widthEditor_->getExpression();

    if (cpu_->getWidth() != width)
    {
        cpu_->setWidth(width);
        widthEditor_->setToolTip(formattedValueFor(width));

        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: CpuDetailsEditor::onAddressableUnitChanged()
//-----------------------------------------------------------------------------
void CpuDetailsEditor::onAddressUnitChanged()
{
    aubEditor_->finishEditingCurrentWord();
    QString aub = aubEditor_->getExpression();

    if (cpu_->getAddressUnitBits() != aub)
    {
        cpu_->setAddressUnitBits(aubEditor_->getExpression());
        rangeEditor_->setToolTip(formattedValueFor(aub));

        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: CpuDetailsEditor::onMemoryMapRefChanged()
//-----------------------------------------------------------------------------
void CpuDetailsEditor::onMemoryMapRefChanged(QString const& selectedMemoryMap)
{
    if (cpu_->getMemoryMapReference() != selectedMemoryMap)
    {
        cpu_->setMemoryMapReference(selectedMemoryMap);

        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: CpuDetailsEditor::formattedValueFor()
//-----------------------------------------------------------------------------
QString CpuDetailsEditor::formattedValueFor(QString const& expression) const
{
    return ExpressionFormatter::format(expression, expressionParser_);
}

//-----------------------------------------------------------------------------
// Function: CpuDetailsEditor::createEditors()
//-----------------------------------------------------------------------------
void CpuDetailsEditor::createEditors(QSharedPointer<ParameterFinder> finder)
{
    auto componentParametersModel = new ComponentParameterModel(finder, this);
    componentParametersModel->setExpressionParser(expressionParser_);

    if (component_->getRevision() == Document::Revision::Std14)
    {
        presenceEditor_ = new ExpressionEditor(finder, this);
        presenceEditor_->setFixedHeight(24);

        auto presenceCompleter = new QCompleter(this);
        presenceCompleter->setModel(componentParametersModel);
        presenceEditor_->setAppendingCompleter(presenceCompleter);

        connect(presenceEditor_, SIGNAL(increaseReference(QString const&)),
            this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
        connect(presenceEditor_, SIGNAL(decreaseReference(QString const&)),
            this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
        connect(presenceEditor_, SIGNAL(editingFinished()), this, SLOT(onIsPresentChanged()), Qt::UniqueConnection);

        addressSpaceRefEditor_ = new EnumCollectionEditor(this);

        connect(addressSpaceRefEditor_, SIGNAL(itemStateChanged(Qt::CheckState)),
            this, SLOT(onAddressSpaceRefsChanged()), Qt::UniqueConnection);

    }
    else if (component_->getRevision() == Document::Revision::Std22)
    {
        memoryMapRefSelector_ = new ReferenceSelector(this);
        memoryMapRefSelector_->setProperty("mandatoryField", true);

        connect(memoryMapRefSelector_, SIGNAL(itemSelected(QString const&)),
            this, SLOT(onMemoryMapRefChanged(QString const&)), Qt::UniqueConnection);

        rangeEditor_ = new ExpressionEditor(finder, this);
        rangeEditor_->setFixedHeight(24);
        rangeEditor_->setProperty("mandatoryField", true);

        auto rangeCompleter = new QCompleter(this);
        rangeCompleter->setModel(componentParametersModel);
        rangeEditor_->setAppendingCompleter(rangeCompleter);

        connect(rangeEditor_, SIGNAL(increaseReference(QString const&)),
            this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
        connect(rangeEditor_, SIGNAL(decreaseReference(QString const&)),
            this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
        connect(rangeEditor_, SIGNAL(editingFinished()), this, SLOT(onRangeChanged()), Qt::UniqueConnection);

        widthEditor_ = new ExpressionEditor(finder, this);
        widthEditor_->setFixedHeight(24);
        widthEditor_->setProperty("mandatoryField", true);

        auto widthCompleter = new QCompleter(this);
        widthCompleter->setModel(componentParametersModel);
        widthEditor_->setAppendingCompleter(widthCompleter);

        connect(widthEditor_, SIGNAL(increaseReference(QString const&)),
            this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
        connect(widthEditor_, SIGNAL(decreaseReference(QString const&)),
            this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
        connect(widthEditor_, SIGNAL(editingFinished()), this, SLOT(onWidthChanged()), Qt::UniqueConnection);

        aubEditor_ = new ExpressionEditor(finder, this);
        aubEditor_->setFixedHeight(24);

        auto aubCompleter = new QCompleter(this);
        aubCompleter->setModel(componentParametersModel);
        aubEditor_->setAppendingCompleter(aubCompleter);

        connect(aubEditor_, SIGNAL(increaseReference(QString const&)),
            this, SIGNAL(increaseReferences(QString const&)), Qt::UniqueConnection);
        connect(aubEditor_, SIGNAL(decreaseReference(QString const&)),
            this, SIGNAL(decreaseReferences(QString const&)), Qt::UniqueConnection);
        connect(aubEditor_, SIGNAL(editingFinished()), this, SLOT(onAddressUnitChanged()), Qt::UniqueConnection);
    }
}

//-----------------------------------------------------------------------------
// Function: CpuDetailsEditor::setupLayout()
//-----------------------------------------------------------------------------
void CpuDetailsEditor::setupLayout()
{
    setFlat(true);

    auto layout = new QGridLayout(this);

    int row = 0;
    if (component_->getRevision() == Document::Revision::Std14)
    {
        layout->addWidget(new QLabel(tr("Is present, f(x):"), this), row, 0, 1, 1);
        layout->addWidget(presenceEditor_, row, 1, 1, 1);
        ++row;

        layout->addWidget(new QLabel(tr("Address spaces:"), this), row, 0, 1, 1);
        layout->addWidget(addressSpaceRefEditor_, row, 1, 1, 1);
        ++row;
    }
    else if (component_->getRevision() == Document::Revision::Std22)
    {
        layout->addWidget(new QLabel(tr("Memory map:"), this), row, 0, 1, 1);
        layout->addWidget(memoryMapRefSelector_, row, 1, 1, 1);
        ++row;

        layout->addWidget(new QLabel(tr("Range [AUB], f(x):"), this), row, 0, 1, 1);
        layout->addWidget(rangeEditor_, row, 1, 1, 1);
        ++row;

        layout->addWidget(new QLabel(tr("Width [bits], f(x):"), this), row, 0, 1, 1);
        layout->addWidget(widthEditor_, row, 1, 1, 1);
        ++row;

        layout->addWidget(new QLabel(tr("Address unit bits (AUB), f(x):"), this), row, 0, 1, 1);
        layout->addWidget(aubEditor_, row, 1, 1, 1);
        ++row;
    }

    layout->setRowStretch(row, 1);
}
