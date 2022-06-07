//-----------------------------------------------------------------------------
// File: memorymapseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 11.04.2012
//
// Description:
// Editor for editing the memory maps of a component.
//-----------------------------------------------------------------------------

#include "memorymapseditor.h"
#include "memorymapsmodel.h"
#include "MemoryMapsColumns.h"
#include "MemoryMapsView.h"

#include <common/widgets/summaryLabel/summarylabel.h>
#include <common/views/EditableTreeView/EditableTreeSortFilter.h>

#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <KactusAPI/include/MemoryMapInterface.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/RemapState.h>

#include <QVBoxLayout>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: memorymapseditor::MemoryMapsEditor()
//-----------------------------------------------------------------------------
MemoryMapsEditor::MemoryMapsEditor(MemoryMapInterface* mapInterface, QSharedPointer<Component> component,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
    LibraryInterface* handler, QWidget *parent):
ItemEditor(component, handler, parent),
view_(new MemoryMapsView(this)),
proxy_(new EditableTreeSortFilter(this)),
model_(new MemoryMapsModel(parameterFinder, expressionParser, mapInterface, this)),
delegate_(),
interface_(mapInterface),
component_(component)
{
    mapInterface->setMemoryMaps(component);

    // display a label on top the table
    SummaryLabel* summaryLabel = new SummaryLabel(tr("Memory maps summary"), this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_->setSourceModel(model_);
	view_->setModel(proxy_);

    // Set case-insensitive sorting.
    proxy_->setSortCaseSensitivity(Qt::CaseInsensitive);

	const QString compPath = ItemEditor::handler()->getDirectoryPath(ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/memMapsList.csv").arg(compPath);
	view_->setDefaultImportExportPath(defPath);
	view_->setAllowImportExport(true);
	view_->setSortingEnabled(true);

    ComponentParameterModel* componentParametersModel = new ComponentParameterModel(parameterFinder, this);
    componentParametersModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(componentParametersModel);

    delegate_ = new MemoryMapsDelegate(parameterCompleter, parameterFinder, getRemapStateNames(), this);
    view_->setItemDelegate(delegate_);

    QHeaderView* header = new QHeaderView(Qt::Horizontal, this);
    header->setStretchLastSection(true);
    view_->setHeader(header);

    connectSignals();
}

//-----------------------------------------------------------------------------
// Function: memorymapseditor::~MemoryMapsEditor()
//-----------------------------------------------------------------------------
MemoryMapsEditor::~MemoryMapsEditor()
{

}

//-----------------------------------------------------------------------------
// Function: memorymapseditor::connectSignals()
//-----------------------------------------------------------------------------
void MemoryMapsEditor::connectSignals()
{
    connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(graphicsChanged()), this, SIGNAL(graphicsChanged()), Qt::UniqueConnection);
    connect(model_, SIGNAL(memoryMapAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
    connect(model_, SIGNAL(memoryMapRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

    connect(model_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_->itemDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(memoryRemapAdded(int, QString const&)),
        this, SIGNAL(memoryRemapAdded(int, QString const&)), Qt::UniqueConnection);
    connect(model_, SIGNAL(memoryRemapRemoved(int, QString const&)),
        this, SIGNAL(memoryRemapRemoved(int, QString const&)), Qt::UniqueConnection);

    connect(model_, SIGNAL(aubChangedOnRow(int)), 
        this, SIGNAL(changeInAddressUnitBitsOnRow(int)), Qt::UniqueConnection);

    connect(view_, SIGNAL(addItems(QModelIndexList const&)),
        proxy_, SLOT(onAddItem(QModelIndexList const&)), Qt::UniqueConnection);
    connect(proxy_, SIGNAL(addItem(QModelIndex const&)),
        model_, SLOT(onAddItem(QModelIndex const&)), Qt::UniqueConnection);

    connect(view_, SIGNAL(addSubItem(QModelIndexList const&)), 
        proxy_, SLOT(onAddSubItem(QModelIndexList const&)), Qt::UniqueConnection);
    connect(proxy_, SIGNAL(addSubItem(QModelIndex const&)),
        model_, SLOT(onAddMemoryRemap(const QModelIndex&)), Qt::UniqueConnection);

    connect(view_, SIGNAL(removeSelectedItems(QModelIndexList const&)),
        proxy_, SLOT(onRemoveSelectedRows(QModelIndexList const&)), Qt::UniqueConnection);
    connect(proxy_, SIGNAL(removeItem(QModelIndex const&)),
        model_, SLOT(onRemoveItem(QModelIndex const&)), Qt::UniqueConnection);

    connect(view_, SIGNAL(removeAllSubItems(QModelIndexList const&)),
        proxy_, SLOT(onRemoveAllSubItemsFromIndexes(QModelIndexList const&)), Qt::UniqueConnection);
    connect(proxy_, SIGNAL(removeAllSubItemsFromIndex(QModelIndex const&)),
        model_, SLOT(onRemoveAllChildItemsFrom(QModelIndex const&)), Qt::UniqueConnection);

    connect(view_, SIGNAL(doubleClicked(const QModelIndex&)),
        this, SLOT(onDoubleClick(const QModelIndex&)), Qt::UniqueConnection);

    connect(view_, SIGNAL(copyRows(QModelIndexList)),
        model_, SLOT(onCopyRows(QModelIndexList)), Qt::UniqueConnection);
    connect(view_, SIGNAL(pasteRows(QModelIndex)), model_, SLOT(onPasteRows(QModelIndex)), Qt::UniqueConnection);

    connect(model_, SIGNAL(memoryMapNameChanged(QString const&, QString const&)),
        this, SIGNAL(memoryMapNameChanged(QString const&, QString const&)), Qt::UniqueConnection);
    connect(model_, SIGNAL(memoryRemapNameChanged(QString const&, QString const&, QString const&)), this,
        SIGNAL(memoryRemapNameChanged(QString const&, QString const&, QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: memorymapseditor::refresh()
//-----------------------------------------------------------------------------
void MemoryMapsEditor::refresh()
{
    delegate_->updateRemapStateNames(getRemapStateNames());
	view_->update();
    view_->expandAll();

    interface_->setMemoryMaps(component_);
}

//-----------------------------------------------------------------------------
// Function: memorymapseditor::getRemapStateNames()
//-----------------------------------------------------------------------------
QStringList MemoryMapsEditor::getRemapStateNames() const
{
    QStringList remapStateNames;
    foreach (QSharedPointer<RemapState> remapState, *component()->getRemapStates())
    {
        remapStateNames.append(remapState->name());
    }

    return remapStateNames;
}

//-----------------------------------------------------------------------------
// Function: memorymapseditor::showEvent()
//-----------------------------------------------------------------------------
void MemoryMapsEditor::showEvent( QShowEvent* event )
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/memorymaps.html");
}

//-----------------------------------------------------------------------------
// Function: memorymapseditor::onDoubleClick()
//-----------------------------------------------------------------------------
void MemoryMapsEditor::onDoubleClick( const QModelIndex& index )
{
	QModelIndex origIndex = proxy_->mapToSource(index);
	
	// index must be valid
	if (!origIndex.isValid())
    {
		return;
	}

	// if the column is for interface references
    if (origIndex.column() == MemoryMapsColumns::INTERFACE_COLUMN)
    {
		// get the names of the interface that refer to selected memory map
		QStringList busIfNames = model_->data(origIndex, Qt::UserRole).toStringList();

		// if there are no bus interfaces or there are many
		if (busIfNames.size() != 1)
        {
			return;
		}
		
		// inform component editor that bus interface editor should be selected
		emit selectBusInterface(busIfNames.first());
	}
}

//-----------------------------------------------------------------------------
// Function: memorymapseditor::sizeHint()
//-----------------------------------------------------------------------------
QSize MemoryMapsEditor::sizeHint() const
{
	return QSize(MemoryMapsEditor::WIDTH, MemoryMapsEditor::HEIGHT);
}
