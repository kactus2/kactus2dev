//-----------------------------------------------------------------------------
// File: componenteditoritem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 07.05.2012
//
// Description:
// ComponentEditorItem is one item in the navigation tree in component editor.
//-----------------------------------------------------------------------------

#include "componenteditoritem.h"
#include "componenteditortreemodel.h"

#include <editors/ComponentEditor/itemeditor.h>
#include <editors/ComponentEditor/itemvisualizer.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <QApplication>
#include <QIcon>

//-----------------------------------------------------------------------------
// Function: componenteditoritem::ComponentEditorItem()
//-----------------------------------------------------------------------------
ComponentEditorItem::ComponentEditorItem(ComponentEditorTreeModel* model,
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent) :
	QObject(parent),
	libHandler_(libHandler),
	component_(component),
	model_(model),
	parent_(parent)
{

	connect(this, SIGNAL(contentChanged(ComponentEditorItem*)),
		model, SLOT(onContentChanged(ComponentEditorItem*)), Qt::UniqueConnection);
	connect(this, SIGNAL(contentChanged()),
		model, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(this, SIGNAL(createChild(ComponentEditorItem*, int)),
		model, SLOT(addItem(ComponentEditorItem*, int)), Qt::UniqueConnection);
	connect(this, SIGNAL(removeChild(ComponentEditorItem*, int)),
		model, SLOT(removeItem(ComponentEditorItem*, int)), Qt::UniqueConnection);
	connect(this, SIGNAL(moveChild(ComponentEditorItem*, int, int)),
		model, SLOT(moveItem(ComponentEditorItem*, int, int)), Qt::UniqueConnection);

	connect(this, SIGNAL(selectItem(ComponentEditorItem*)),
		model, SLOT(onSelectItem(ComponentEditorItem*)), Qt::UniqueConnection);

	connect(this, SIGNAL(helpUrlRequested(QString const&)),
		model, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);
	connect(this, SIGNAL(errorMessage(const QString&)),
		model, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);

	connect(this, SIGNAL(openDesign(const VLNV&, const QString&)),
		model, SIGNAL(openDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(openSWDesign(const VLNV&, const QString&)),
		model, SIGNAL(openSWDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(openSystemDesign(const VLNV&, const QString&)),
		model, SIGNAL(openSystemDesign(const VLNV&, const QString&)), Qt::UniqueConnection);
	connect(this, SIGNAL(openBus(const VLNV&)),
		model, SIGNAL(openBus(const VLNV&)), Qt::UniqueConnection);
	connect(this, SIGNAL(openAbsDef(const VLNV&)),
		model, SIGNAL(openAbsDef(const VLNV&)), Qt::UniqueConnection);
	connect(this, SIGNAL(openComDefinition(const VLNV&)),
		model, SIGNAL(openComDefinition(const VLNV&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::ComponentEditorItem()
//-----------------------------------------------------------------------------
ComponentEditorItem::ComponentEditorItem(LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorTreeModel* parent) :
	QObject(parent),
	libHandler_(libHandler),
	component_(component),
	model_(parent)
{
	connect(this, SIGNAL(contentChanged(ComponentEditorItem*)),
		parent, SLOT(onContentChanged(ComponentEditorItem*)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::~ComponentEditorItem()
//-----------------------------------------------------------------------------
ComponentEditorItem::~ComponentEditorItem()
{
	if (editor_)
	{
		editor_->deleteLater();
		editor_ = nullptr;
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::row()
//-----------------------------------------------------------------------------
int ComponentEditorItem::row() const
{
	if (parent_)
	{
		return parent_->getIndexOf(this);
	}
	return -1;
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::getIndexOf()
//-----------------------------------------------------------------------------
int ComponentEditorItem::getIndexOf(const ComponentEditorItem* child) const
{
	for (int i = 0; i < childItems_.size(); ++i)
	{
		if (childItems_.at(i).data() == child)
		{
			return i;
		}
	}
	return -1;
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::rowCount()
//-----------------------------------------------------------------------------
int ComponentEditorItem::rowCount() const
{
	return childItems_.count();
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::hasChildren()
//-----------------------------------------------------------------------------
bool ComponentEditorItem::hasChildren() const
{
	return !childItems_.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::child()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentEditorItem> ComponentEditorItem::child(const int index)
{
	if (index < 0 || childItems_.count() < index)
	{
		return QSharedPointer<ComponentEditorItem>();
	}

	return childItems_.at(index);
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::parent()
//-----------------------------------------------------------------------------
ComponentEditorItem* ComponentEditorItem::parent()
{
	return parent_;
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorItem::getFont() const
{
	return QApplication::font();
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorItem::isValid() const
{
	// if at least one child is not valid then this is not valid
	return std::all_of(childItems_.cbegin(), childItems_.cend(),
		[](auto const& childItem) { return childItem->isValid(); });
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorItem::isModified()
//-----------------------------------------------------------------------------
bool ComponentEditorItem::isModified() const
{
	return modified_;
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::clearModified()
//-----------------------------------------------------------------------------
void ComponentEditorItem::clearModified()
{
	bool changed = modified_;

	modified_ = false;

	for (auto const& child : childItems_)
	{
		child->clearModified();
	}

	if (changed)
	{
		emit contentChanged(this);
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::setModified()
//-----------------------------------------------------------------------------
void ComponentEditorItem::setModified()
{
	modified_ = true;

	if (parent_)
	{
		parent_->setModified();
	}
}
//-----------------------------------------------------------------------------
// Function: componenteditoritem::visualizer()
//-----------------------------------------------------------------------------
ItemVisualizer* ComponentEditorItem::visualizer()
{
	return nullptr;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorItem::onEditorChanged()
//-----------------------------------------------------------------------------
void ComponentEditorItem::onEditorChanged()
{
	setModified();

	// if there is a valid parent then update it also
	if (parent_)
	{
		emit contentChanged(parent_);
	}

	// update this item
	emit contentChanged(this);

	for (auto const& childItem : childItems_)
	{
		emit contentChanged(childItem.data());
	}
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorItem::onGraphicsChanged()
//-----------------------------------------------------------------------------
void ComponentEditorItem::onGraphicsChanged()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
	for (auto const& childItem : childItems_)
	{
		childItem->updateGraphics();
	}

	updateGraphics();
	QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorItem::setLocked()
//-----------------------------------------------------------------------------
void ComponentEditorItem::setLocked(bool locked)
{
	// if this item contains an editor
	if (editor_)
	{
		editor_->setProtection(locked);
	}

	// also tell child items
	for (auto const& childItem : childItems_)
	{
		childItem->setLocked(locked);
	}

	locked_ = locked;
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::refreshEditor()
//-----------------------------------------------------------------------------
void ComponentEditorItem::refreshEditor()
{
	if (editor_)
	{
		editor_->refresh();
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::onAddChild()
//-----------------------------------------------------------------------------
void ComponentEditorItem::onAddChild(int index)
{
	emit createChild(this, index);
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::onRemoveChild()
//-----------------------------------------------------------------------------
void ComponentEditorItem::onRemoveChild(int index)
{
	emit removeChild(this, index);
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::onMoveChild()
//-----------------------------------------------------------------------------
void ComponentEditorItem::onMoveChild(int source, int target)
{
	emit moveChild(this, source, target);
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::moveChild()
//-----------------------------------------------------------------------------
void ComponentEditorItem::moveChild(const int sourceIndex, int targetIndex)
{
	// if the source index can not be used to identify an item
	if (sourceIndex < 0 || sourceIndex >= childItems_.count())
	{
		return;
	}

	// Take the item from the list
	QSharedPointer<ComponentEditorItem> itemToMove = childItems_.takeAt(sourceIndex);

	childItems_.insert(targetIndex, itemToMove);
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorItem::createChild(int /*index*/)
{
	// This must be implemented in sub classes to create the correct type of child.
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::removeChild()
//-----------------------------------------------------------------------------
void ComponentEditorItem::removeChild(int index)
{
	Q_ASSERT(index >= 0);
	Q_ASSERT(index < childItems_.size());

	// first tell the child to remove the graphics item
	childItems_[index]->removeGraphicsItem();

	// then the child can be removed
	childItems_.removeAt(index);
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::canBeOpened()
//-----------------------------------------------------------------------------
bool ComponentEditorItem::canBeOpened() const
{
	// Normally items can't be opened
	return false;
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::openItem()
//-----------------------------------------------------------------------------
void ComponentEditorItem::openItem()
{
	// Normally items can't be opened
	return;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorItem::actions()
//-----------------------------------------------------------------------------
QList<QAction* > ComponentEditorItem::actions()
{
	return QList<QAction*>();
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::getGraphicsItem()
//-----------------------------------------------------------------------------
QGraphicsItem* ComponentEditorItem::getGraphicsItem()
{
	return nullptr;
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::updateGraphics()
//-----------------------------------------------------------------------------
void ComponentEditorItem::updateGraphics()
{
	return;
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::removeGraphicsItem()
//-----------------------------------------------------------------------------
void ComponentEditorItem::removeGraphicsItem()
{
	return;
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::hasIcon()
//-----------------------------------------------------------------------------
bool ComponentEditorItem::hasIcon() const
{
	// Normally items have no icon.
	return false;
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::getIcon()
//-----------------------------------------------------------------------------
QIcon ComponentEditorItem::getIcon() const
{
	// Normally items have no default icon.
	return QIcon();
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::onSelectRequest()
//-----------------------------------------------------------------------------
void ComponentEditorItem::onSelectRequest()
{
	emit selectItem(this);
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::setReferenceCounter()
//-----------------------------------------------------------------------------
void ComponentEditorItem::setReferenceCounter(QSharedPointer<ReferenceCounter> newReferenceCounter)
{
	referenceCounter_ = newReferenceCounter;
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::setParameterFinder()
//-----------------------------------------------------------------------------
void ComponentEditorItem::setParameterFinder(QSharedPointer<ParameterFinder> parameterFinder)
{
	parameterFinder_ = parameterFinder;
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::setExpressionFormatter()
//-----------------------------------------------------------------------------
void ComponentEditorItem::setExpressionFormatter(QSharedPointer<ExpressionFormatter> expressionFormatter)
{
	expressionFormatter_ = expressionFormatter;
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::connectItemEditorToReferenceCounter()
//-----------------------------------------------------------------------------
void ComponentEditorItem::connectItemEditorToReferenceCounter()
{
	connect(editor_, SIGNAL(increaseReferences(QString)),
		referenceCounter_.data(), SLOT(increaseReferenceCount(QString)), Qt::UniqueConnection);
	connect(editor_, SIGNAL(decreaseReferences(QString)),
		referenceCounter_.data(), SLOT(decreaseReferenceCount(QString)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: componenteditoritem::connectItemEditorToVendorExtensionsEditor()
//-----------------------------------------------------------------------------
void ComponentEditorItem::connectItemEditorToVendorExtensionsEditor()
{
	connect(editor_, SIGNAL(changeVendorExtensions(QString const&, QSharedPointer<Extendable>)),
		this, SIGNAL(changeVendorExtensions(QString const&, QSharedPointer<Extendable>)), Qt::UniqueConnection);
}
