/* 
 *  	Created on: 11.6.2012
 *      Author: Antti Kamppi
 * 		filename: addressspaceseditor.cpp
 *		Project: Kactus 2
 */

#include "addressspaceseditor.h"
#include "addressspacesdelegate.h"

#include "AddressSpaceColumns.h"

#include <common/widgets/summaryLabel/summarylabel.h>

#include <editors/ComponentEditor/parameters/ComponentParameterModel.h>
#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/memoryMaps/ExpressionProxyModel.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: AddressSpacesEditor::AddressSpacesEditor()
//-----------------------------------------------------------------------------
AddressSpacesEditor::AddressSpacesEditor(QSharedPointer<Component> component,
    LibraryInterface* handler,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser):
ItemEditor(component, handler),
    view_(this),
    model_(component, parameterFinder, expressionFormatter, this)
{
    model_.setExpressionParser(expressionParser);

    ExpressionProxyModel* proxy = new ExpressionProxyModel(expressionParser, this);
    proxy->setColumnToAcceptExpressions(AddressSpaceColumns::RANGE);
    proxy->setColumnToAcceptExpressions(AddressSpaceColumns::WIDTH);

	proxy->setSourceModel(&model_);

	view_.setModel(proxy);

	const QString compPath = ItemEditor::handler()->getDirectoryPath(*ItemEditor::component()->getVlnv());
	QString defPath = QString("%1/addrSpaceList.csv").arg(compPath);
	view_.setDefaultImportExportPath(defPath);
	view_.setAllowImportExport(true);

	view_.setItemsDraggable(false);
    view_.setSortingEnabled(true);

    ComponentParameterModel* completionModel = new ComponentParameterModel(parameterFinder, this);
    completionModel->setExpressionParser(expressionParser);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(completionModel);

    AddressSpacesDelegate* delegate = new AddressSpacesDelegate(parameterCompleter, parameterFinder, this);
 	view_.setItemDelegate(delegate);
    
    // display a label on top the table
    SummaryLabel* summaryLabel = new SummaryLabel(tr("Address spaces summary"), this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(summaryLabel, 0, Qt::AlignCenter);
    layout->addWidget(&view_);
    layout->setContentsMargins(0, 0, 0, 0);

	connect(&model_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	
	connect(&model_, SIGNAL(addrSpaceAdded(int)), this, SIGNAL(childAdded(int)), Qt::UniqueConnection);
	connect(&model_, SIGNAL(addrSpaceRemoved(int)),	this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);

	connect(&view_, SIGNAL(doubleClicked(const QModelIndex&)),
		this, SLOT(onDoubleClick(const QModelIndex&)), Qt::UniqueConnection);

    connect(delegate, SIGNAL(increaseReferences(QString)), 
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(delegate, SIGNAL(decreaseReferences(QString)), 
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);;

    connect(&model_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesEditor::~AddressSpacesEditor()
//-----------------------------------------------------------------------------
AddressSpacesEditor::~AddressSpacesEditor()
{
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesEditor::isValid()
//-----------------------------------------------------------------------------
bool AddressSpacesEditor::isValid() const
{
	return model_.isValid();
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesEditor::refresh()
//-----------------------------------------------------------------------------
void AddressSpacesEditor::refresh()
{
	view_.update();
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesEditor::showEvent()
//-----------------------------------------------------------------------------
void AddressSpacesEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/addressspaces.html");
}

//-----------------------------------------------------------------------------
// Function: AddressSpacesEditor::onDoubleClick()
//-----------------------------------------------------------------------------
void AddressSpacesEditor::onDoubleClick(QModelIndex const& index)
{
	// if the column is for interface references
	if (index.isValid() && index.column() == AddressSpaceColumns::INTERFACE_BINDING)
    {
		// get the names of the interface that refer to selected memory map
		QStringList busIfNames = index.data(Qt::UserRole).toStringList();

		// if there is exactly one bus interface
        // inform component editor that bus interface editor should be selected
        if (busIfNames.size() == 1)
        {
		    emit selectBusInterface(busIfNames.first());
		}
	}
}
