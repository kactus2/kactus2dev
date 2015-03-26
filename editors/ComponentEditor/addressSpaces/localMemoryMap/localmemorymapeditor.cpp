/* 
 *  	Created on: 13.10.2012
 *      Author: Antti Kamppi
 * 		filename: localmemorymapeditor.cpp
 *		Project: Kactus 2
 */

#include "localmemorymapeditor.h"

#include <common/widgets/summaryLabel/summarylabel.h>
#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/views/EditableTableView/editabletableview.h>

#include <editors/ComponentEditor/memoryMaps/MemoryMapColumns.h>
#include <editors/ComponentEditor/memoryMaps/memorymapdelegate.h>
#include <editors/ComponentEditor/memoryMaps/memorymapmodel.h>
#include <editors/ComponentEditor/memoryMaps/memorymapproxy.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapEditor::LocalMemoryMapEditor()
//-----------------------------------------------------------------------------
LocalMemoryMapEditor::LocalMemoryMapEditor(QSharedPointer<MemoryMap> memoryMap,
                                           QSharedPointer<Component> component,
                                           LibraryInterface* handler,
                                           QSharedPointer<ParameterFinder> parameterFinder,
                                           QSharedPointer<ExpressionFormatter> expressionFormatter,
                                           QWidget *parent)
    : QGroupBox(tr("Local memory map"), parent),
      localMemoryMap_(memoryMap),
      nameEditor_(new NameGroupEditor(memoryMap->getNameGroup(), this)),
      view_(new EditableTableView(this)),
      proxy_(new MemoryMapProxy(this)),
      model_(0),
      component_(component),
      handler_(handler)
{
    setCheckable(true);
    setChecked(!localMemoryMap_->isEmpty());

    QSharedPointer<IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));

    model_ = new MemoryMapModel(memoryMap, component->getChoices(), expressionParser, parameterFinder,
        expressionFormatter, this);

    ComponentParameterModel* componentParameterModel = new ComponentParameterModel(parameterFinder, this);
    componentParameterModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(componentParameterModel);

	proxy_->setSourceModel(model_);
	view_->setModel(proxy_);

	// display a label on top the table
	SummaryLabel* summaryLabel = new SummaryLabel(tr("Address blocks"), this);

	proxy_->setSourceModel(model_);
	view_->setModel(proxy_);

	const QString compPath = handler_->getDirectoryPath(*component_->getVlnv());
	QString defPath = QString("%1/localAddrBlockList.csv").arg(compPath);
	view_->setDefaultImportExportPath(defPath);
	view_->setAllowImportExport(true);

	// items can not be dragged
	view_->setItemsDraggable(false);
    view_->setItemDelegate(new MemoryMapDelegate(parameterCompleter, parameterFinder, this));
	view_->setSortingEnabled(true);

	view_->sortByColumn(MemoryMapColumns::BASE_COLUMN, Qt::AscendingOrder);

    connect(this, SIGNAL(toggled(bool)), this, SLOT(onCheckStateChanged()), Qt::UniqueConnection);

	connect(nameEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(model_, SIGNAL(itemAdded(int)), this, SIGNAL(itemAdded(int)), Qt::UniqueConnection);
	connect(model_, SIGNAL(itemRemoved(int)), this, SIGNAL(itemRemoved(int)), Qt::UniqueConnection);

	// connect view to model
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

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(nameEditor_);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(view_);
}

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapEditor::~LocalMemoryMapEditor()
//-----------------------------------------------------------------------------
LocalMemoryMapEditor::~LocalMemoryMapEditor()
{
}

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapEditor::isValid()
//-----------------------------------------------------------------------------
bool LocalMemoryMapEditor::isValid() const
{
	return nameEditor_->isValid() && model_->isValid();
}

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapEditor::refresh()
//-----------------------------------------------------------------------------
void LocalMemoryMapEditor::refresh()
{
	nameEditor_->refresh();
	view_->update();
    
    onCheckStateChanged();
}

//-----------------------------------------------------------------------------
// Function: LocalMemoryMapEditor::onEnableChanged()
//-----------------------------------------------------------------------------
void LocalMemoryMapEditor::onCheckStateChanged()
{
    if (!isChecked() && !localMemoryMap_->isEmpty())
    {
        setChecked(true);
        emit errorMessage("Cannot unselect local memory map since all fields are not empty.");        
    }
}
