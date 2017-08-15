//-----------------------------------------------------------------------------
// File: ComponentEditorIndirectInterfacesItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.08.2017
//
// Description:
// The Indirect interfaces -item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorIndirectInterfacesitem.h"
#include "SingleIndirectInterfaceItem.h"

#include <editors/ComponentEditor/IndirectInterfaces/IndirectInterfaceseditor.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/validators/IndirectInterfaceValidator.h>
#include <IPXACTmodels/common/validators/ParameterValidator2014.h>

//-----------------------------------------------------------------------------
// Function: componenteditorIndirectInterfacesitem::ComponentEditorIndirectInterfacesItem()
//-----------------------------------------------------------------------------
ComponentEditorIndirectInterfacesItem::ComponentEditorIndirectInterfacesItem(ComponentEditorTreeModel* model,
    LibraryInterface* libHandler, QSharedPointer<Component> component,
    QSharedPointer<ReferenceCounter> referenceCounter,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<ExpressionParser> expressionParser,
    ComponentEditorItem* parent, QWidget* parentWnd):
ComponentEditorItem(model, libHandler, component, parent),
    indirectInterfaces_(component->getIndirectInterfaces()),
    validator_(new IndirectInterfaceValidator(component, expressionParser, 
        QSharedPointer<ParameterValidator2014>(new ParameterValidator2014(expressionParser, component->getChoices())))),
    parentWnd_(parentWnd),
    expressionParser_(expressionParser)
{
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);
    setReferenceCounter(referenceCounter);

	foreach (QSharedPointer<IndirectInterface> indirectInterface, *indirectInterfaces_)
    {
		QSharedPointer<SingleIndirectInterfaceItem> interfaceItem(
            new SingleIndirectInterfaceItem(indirectInterface, model, libHandler, component, referenceCounter_,
            parameterFinder_, expressionFormatter_, expressionParser_, validator_, this, parentWnd));

        connect(interfaceItem.data(), SIGNAL(openReferenceTree(QString)),
            this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

		childItems_.append(interfaceItem);
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditorIndirectInterfacesitem::~ComponentEditorIndirectInterfacesItem()
//-----------------------------------------------------------------------------
ComponentEditorIndirectInterfacesItem::~ComponentEditorIndirectInterfacesItem() 
{
}

//-----------------------------------------------------------------------------
// Function: componenteditorIndirectInterfacesitem::createIndirectInterfaceValidator()
//-----------------------------------------------------------------------------
/*void ComponentEditorIndirectInterfacesItem::createIndirectInterfaceValidator()
{
    QSharedPointer<PortMapValidator> portMapValidator(
        new PortMapValidator(expressionParser_, component_->getPorts(), libHandler_));

    QSharedPointer<ParameterValidator2014> parameterValidator(
        new ParameterValidator2014(expressionParser_, component_->getChoices()));

    validator_ = QSharedPointer<IndirectInterfaceValidator>(new IndirectInterfaceValidator(expressionParser_,
        component_->getChoices(), component_->getViews(), component_->getPorts(), component_->getAddressSpaces(),
        component_->getMemoryMaps(), component_->getIndirectInterfaces(), component_->getFileSets(),
        component_->getRemapStates(), portMapValidator, parameterValidator, libHandler_));
}
*/
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
		editor_ = new IndirectInterfacesEditor(libHandler_, component_, parameterFinder_);
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
	QSharedPointer<SingleIndirectInterfaceItem> interfaceItem(
		new SingleIndirectInterfaceItem(indirectInterfaces_->at(index), model_, libHandler_, component_, 
        referenceCounter_, parameterFinder_, expressionFormatter_, expressionParser_, validator_, this, parentWnd_));
	interfaceItem->setLocked(locked_);

    connect(interfaceItem.data(), SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

	childItems_.insert(index, interfaceItem);
}
