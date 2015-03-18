/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: configurableelementeditor.cpp
 *		Project: Kactus 2
 */

#include "configurableelementeditor.h"

#include "configurableelementdelegate.h"

#include <IPXACTmodels/component.h>

#include <designEditors/HWDesign/HWComponentItem.h>

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <QIcon>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSharedPointer>
#include <QAbstractItemDelegate>

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::ConfigurableElementEditor()
//-----------------------------------------------------------------------------
ConfigurableElementEditor::ConfigurableElementEditor(QSharedPointer<ParameterFinder> parameterFinder, 
                                                     QSharedPointer<ExpressionFormatter> expressionFormatter,
                                                     QWidget *parent,
													 const QString& title):
QGroupBox(title, parent),
view_(this),
filter_(this),
componentParameterModel_(new ComponentParameterModel(this, parameterFinder)),
model_(parameterFinder, expressionFormatter, this)
{
	filter_.setSourceModel(&model_);
	view_.setModel(&filter_);

    ParameterCompleter* parameterCompleter = new ParameterCompleter(this);
    parameterCompleter->setModel(componentParameterModel_);

    QSharedPointer <IPXactSystemVerilogParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));
    componentParameterModel_->setExpressionParser(expressionParser);
    model_.setExpressionParser(expressionParser);

	// set options for the view
	view_.setSortingEnabled(true);
	view_.setItemsDraggable(false);

	view_.setItemDelegate(new ConfigurableElementDelegate(parameterCompleter, parameterFinder, this));

    view_.setAlternatingRowColors(false);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(&view_);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::~ConfigurableElementEditor()
//-----------------------------------------------------------------------------
ConfigurableElementEditor::~ConfigurableElementEditor() 
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::setComponent()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::setComponent(ComponentItem* component) 
{
	model_.setComponent(component);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::clear()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::clear() 
{
	model_.clear();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementEditor::onRemoveClick()
//-----------------------------------------------------------------------------
void ConfigurableElementEditor::onRemoveClick() 
{
	QModelIndex index = view_.currentIndex();
	QModelIndex sourceIndex = filter_.mapToSource(index);
	if (sourceIndex.isValid())
		model_.onRemove(sourceIndex);
}
