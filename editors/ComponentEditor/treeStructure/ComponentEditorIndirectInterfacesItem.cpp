//-----------------------------------------------------------------------------
// File: ComponentEditorIndirectInterfacesItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 04.08.2017
//
// Description:
// The Indirect interfaces -item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#include "ComponentEditorIndirectInterfacesItem.h"
#include "SingleIndirectInterfaceItem.h"

#include <editors/ComponentEditor/indirectInterfaces/IndirectInterfacesEditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/IndirectInterfaceValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator.h>
#include <IPXACTmodels/Component/IndirectInterface.h>
#include <IPXACTmodels/Component/validators/CollectionValidators.h>

//-----------------------------------------------------------------------------
// Function: componenteditorIndirectInterfacesitem::ComponentEditorIndirectInterfacesItem()
//-----------------------------------------------------------------------------
ComponentEditorIndirectInterfacesItem::ComponentEditorIndirectInterfacesItem(ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QSharedPointer<ExpressionParser> expressionParser,
    BusInterfaceInterface* busInterface, ComponentEditorItem* parent, QWidget* parentWnd):
ComponentEditorItem(model, libHandler, component, parent),
indirectInterfaces_(component->getIndirectInterfaces()),
expressionParser_(expressionParser),
singleIndirectInterfaceValidator_(new IndirectInterfaceValidator(component, expressionParser,
    QSharedPointer<ParameterValidator>(new ParameterValidator(expressionParser, component->getChoices(), 
        component_->getRevision())))),
indirectInterfacesValidator_(new IndirectInterfacesValidator(singleIndirectInterfaceValidator_)),
parentWnd_(parentWnd),
busInterface_(busInterface)
{
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
    setReferenceCounter(referenceCounter);

    for (QSharedPointer<IndirectInterface> indirectInterface : *indirectInterfaces_)
    {
		QSharedPointer<SingleIndirectInterfaceItem> interfaceItem(new SingleIndirectInterfaceItem(
            indirectInterface, model, libHandler, component, referenceCounter_, parameterFinder_,
            expressionFormatter_, expressionParser_, singleIndirectInterfaceValidator_, busInterface_, this, parentWnd));

        connect(interfaceItem.data(), SIGNAL(openReferenceTree(QString const&, QString const&)),
            this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

		childItems_.append(interfaceItem);
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditorIndirectInterfacesitem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorIndirectInterfacesItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!component_->getIndirectInterfaces()->isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: componenteditorIndirectInterfacesitem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorIndirectInterfacesItem::text() const
{
	return tr("Indirect interfaces");
}

//-----------------------------------------------------------------------------
// Function: componenteditorIndirectInterfacesitem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorIndirectInterfacesItem::editor()
{
	if (!editor_)
    {
		editor_ = new IndirectInterfacesEditor(libHandler_, component_, singleIndirectInterfaceValidator_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childMoved(int, int)), this, SLOT(onMoveChild(int, int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorIndirectInterfacesitem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorIndirectInterfacesItem::getTooltip() const
{
	return tr("Contains the indirect interfaces specified for a component");
}

//-----------------------------------------------------------------------------
// Function: componenteditorIndirectInterfacesitem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorIndirectInterfacesItem::createChild(int index)
{
	QSharedPointer<SingleIndirectInterfaceItem> interfaceItem(new SingleIndirectInterfaceItem(
        indirectInterfaces_->at(index), model_, libHandler_, component_, referenceCounter_, parameterFinder_,
        expressionFormatter_, expressionParser_, singleIndirectInterfaceValidator_, busInterface_, this, parentWnd_));
	interfaceItem->setLocked(locked_);

    connect(interfaceItem.data(), SIGNAL(openReferenceTree(QString const&, QString const&)),
        this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

	childItems_.insert(index, interfaceItem);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorIndirectInterfacesItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorIndirectInterfacesItem::isValid() const
{
    auto indirectInterfacesAsNameGroup = CollectionValidators::itemListToNameGroupList(indirectInterfaces_);
    indirectInterfacesValidator_->childrenHaveUniqueNames(indirectInterfacesAsNameGroup);
    return ComponentEditorItem::isValid();
}
