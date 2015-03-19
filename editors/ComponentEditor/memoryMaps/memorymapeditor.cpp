/* 
 *  	Created on: 22.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapeditor.cpp
 *		Project: Kactus 2
 */

#include "memorymapeditor.h"

#include "memorymapmodel.h"
#include "memorymapdelegate.h"
#include "MemoryMapColumns.h"
#include "memorymapproxy.h"

#include <common/views/EditableTableView/editabletableview.h>
#include <common/widgets/summaryLabel/summarylabel.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: MemoryMapEditor::MemoryMapEditor()
//-----------------------------------------------------------------------------
MemoryMapEditor::MemoryMapEditor(QSharedPointer<Component> component, LibraryInterface* handler,
    QSharedPointer<MemoryMap> memoryMap, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget* parent /* = 0 */):
ItemEditor(component, handler, parent),
view_(new EditableTableView(this)),
proxy_(new MemoryMapProxy(this)),
model_(0)
{
    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));

    model_ = new MemoryMapModel(memoryMap, component->getChoices(), expressionParser, parameterFinder,
        expressionFormatter, this);

    ComponentParameterModel* componentParameterModel = new ComponentParameterModel(this, parameterFinder);
    componentParameterModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(componentParameterModel);

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Address blocks summary"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_->setSourceModel(model_);
	view_->setModel(proxy_);

	//! \brief Enable import/export csv file
	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/addrBlockList.csv").arg(compPath);
	view_->setDefaultImportExportPath(defPath);
	view_->setAllowImportExport(true);

	// items can not be dragged
	view_->setItemsDraggable(false);

	view_->setSortingEnabled(true);

    view_->setItemDelegate(new MemoryMapDelegate(parameterCompleter, parameterFinder, this));

	view_->sortByColumn(MemoryMapColumns::BASE_COLUMN, Qt::AscendingOrder);

	connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(itemAdded(int)),	this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(model_, SIGNAL(itemRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(view_, SIGNAL(addItem(const QModelIndex&)),
		model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(view_, SIGNAL(removeItem(const QModelIndex&)),
		model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

    connect(view_->itemDelegate(), SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(view_->itemDelegate(), SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapEditor::~MemoryMapEditor()
//-----------------------------------------------------------------------------
MemoryMapEditor::~MemoryMapEditor()
{
}

//-----------------------------------------------------------------------------
// Function: MemoryMapEditor::isValid()
//-----------------------------------------------------------------------------
bool MemoryMapEditor::isValid() const
{
	return model_->isValid();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapEditor::refresh()
//-----------------------------------------------------------------------------
void MemoryMapEditor::refresh()
{
	view_->update();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapEditor::showEvent()
//-----------------------------------------------------------------------------
void MemoryMapEditor::showEvent( QShowEvent* event )
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/memorymap.html");
}

//-----------------------------------------------------------------------------
// Function: MemoryMapEditor::sizeHint()
//-----------------------------------------------------------------------------
QSize MemoryMapEditor::sizeHint() const
{
	return QSize(MemoryMapEditor::WIDTH, MemoryMapEditor::HEIGHT);
}
