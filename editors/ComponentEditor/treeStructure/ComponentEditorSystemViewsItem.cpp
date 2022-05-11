//-----------------------------------------------------------------------------
// File: ComponentEditorSystemViewsItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 13.7.2012
//
// Description:
// The System Views-item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "ComponentEditorSystemViewsItem.h"

#include "ComponentEditorSystemViewItem.h"

#include <KactusAPI/include/FileInterface.h>
#include <KactusAPI/include/FileBuilderInterface.h>
#include <KactusAPI/include/FileSetInterface.h>
#include <editors/ComponentEditor/software/systemView/SystemViewsEditor.h>

#include <IPXACTmodels/kactusExtensions/SystemView.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/FileValidator.h>
#include <IPXACTmodels/Component/validators/FileSetValidator.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewsItem::ComponentEditorSystemViewsItem()
//-----------------------------------------------------------------------------
ComponentEditorSystemViewsItem::ComponentEditorSystemViewsItem(ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ExpressionFormatter> expressionFormatter, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
fileSetInterface_()
{
    constructFileSetInterface(parameterFinder, expressionParser, expressionFormatter);

	foreach (QSharedPointer<SystemView> systemView, component->getSystemViews())
    {
		QSharedPointer<ComponentEditorSystemViewItem> systemViewItem(
			new ComponentEditorSystemViewItem(systemView, model, libHandler, component, fileSetInterface_, this)); 
		childItems_.append(systemViewItem);
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewsItem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorSystemViewsItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());        
    font.setBold(component_->hasSystemViews());    
    return font;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewsItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorSystemViewsItem::getTooltip() const
{
	return tr("Contains the system views of the component");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewsItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorSystemViewsItem::text() const
{
	return tr("System views");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewsItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorSystemViewsItem::editor()
{
	if (!editor_)
    {
		editor_ = new SystemViewsEditor(component_, libHandler_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
	}

	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorSystemViewsItem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorSystemViewsItem::createChild(int index)
{
	QSharedPointer<ComponentEditorSystemViewItem> systemViewItem(
		new ComponentEditorSystemViewItem(
            component_->getSystemViews().at(index), model_, libHandler_, component_, fileSetInterface_, this));
	systemViewItem->setLocked(locked_);
	childItems_.insert(index, systemViewItem);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetsItem::constructFileSetInterface()
//-----------------------------------------------------------------------------
void ComponentEditorSystemViewsItem::constructFileSetInterface(QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter)
{
    QSharedPointer<FileValidator> fileValidator(new FileValidator(expressionParser));

    FileInterface* fileInterface = new FileInterface(fileValidator, expressionParser, expressionFormatter);
    FileBuilderInterface* fileBuilderInterface = new FileBuilderInterface(expressionParser, expressionFormatter);

    QSharedPointer<FileSetValidator> fileSetValidator(new FileSetValidator(fileValidator, expressionParser));

    fileSetInterface_ = new FileSetInterface(
        fileSetValidator, expressionParser, expressionFormatter, fileInterface, fileBuilderInterface);

    fileSetInterface_->setFileSets(component_->getFileSets());
}
