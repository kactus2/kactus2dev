//-----------------------------------------------------------------------------
// File: cpuseditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 14.06.2012
//
// Description:
// Editor to add/remove/edit the cpu-elements of a component.
//-----------------------------------------------------------------------------

#include "cpuseditor.h"
#include "cpusdelegate.h"
#include "CpuColumns.h"

#include <common/widgets/summaryLabel/summarylabel.h>
#include <KactusAPI/include/LibraryInterface.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>

#include <editors/common/ExpressionSet.h>

#include <IPXACTmodels/Component/Component.h>

#include <QCompleter>
#include <QVBoxLayout>
#include <QHeaderView>

//-----------------------------------------------------------------------------
// Function: CpusEditor::CpusEditor()
//-----------------------------------------------------------------------------
CpusEditor::CpusEditor(QSharedPointer<Component> component, LibraryInterface* handler, 
    QSharedPointer<CPUValidator> validator, 
    ExpressionSet expressions,
	QWidget* parent) :
ItemEditor(component, handler, parent),
    view_(this),
    proxy_(this),
    model_(component, validator, expressions, this)
{
    // display a label on top the table
    SummaryLabel* summaryLabel = new SummaryLabel(tr("CPUs summary"), this);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
	layout->addWidget(&view_);
	layout->setContentsMargins(0, 0, 0, 0);

	proxy_.setSourceModel(&model_);
	view_.setModel(&proxy_);

	if (component->getRevision() == Document::Revision::Std14)
    {
        view_.horizontalHeader()->hideSection(CpuColumns::SHORT_DESCRIPTION);
        view_.horizontalHeader()->hideSection(CpuColumns::MEMORY_MAP);
        view_.horizontalHeader()->hideSection(CpuColumns::RANGE);
        view_.horizontalHeader()->hideSection(CpuColumns::WIDTH);
        view_.horizontalHeader()->hideSection(CpuColumns::AUB);
	}
	else if (component->getRevision() == Document::Revision::Std22)
    {
        view_.horizontalHeader()->hideSection(CpuColumns::ADDRSPACE);
	}

	const QString compPath = handler->getDirectoryPath(component->getVlnv());
	QString defPath = QString("%1/cpusListing.csv").arg(compPath);
	view_.setDefaultImportExportPath(defPath);
	view_.setAllowImportExport(true);
	view_.setItemsDraggable(false);

    ComponentParameterModel* parameterModel = new ComponentParameterModel(expressions.finder, this);
    parameterModel->setExpressionParser(expressions.parser);

    auto parameterCompleter = new QCompleter(this);
    parameterCompleter->setModel(parameterModel);

	auto delegate = new CpusDelegate(component, parameterCompleter, expressions.finder, this);
	view_.setItemDelegate(delegate);

    connect(delegate, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(delegate, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

    connect(&model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&model_, SIGNAL(cpuAdded(int)),	this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(cpuRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
        &model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: CpusEditor::refresh()
//-----------------------------------------------------------------------------
void CpusEditor::refresh()
{
	view_.update();
}

//-----------------------------------------------------------------------------
// Function: CpusEditor::showEvent()
//-----------------------------------------------------------------------------
void CpusEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	
	if (component()->getRevision() == Document::Revision::Std22)
    {
        emit helpUrlRequested("componenteditor/cpus2022.html");
	}
	else
	{
        emit helpUrlRequested("componenteditor/cpus.html");
	}
}
