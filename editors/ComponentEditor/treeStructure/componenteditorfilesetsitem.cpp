//-----------------------------------------------------------------------------
// File: componenteditorfilesetsitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The file sets-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorfilesetsitem.h"
#include "componenteditorfilesetitem.h"

#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <editors/ComponentEditor/fileSet/filesetseditor.h>

#include <Plugins/PluginSystem/PluginManager.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>

#include <IPXACTmodels/Component/validators/FileValidator.h>
#include <IPXACTmodels/Component/validators/FileSetValidator.h>

//-----------------------------------------------------------------------------
// Function: componenteditorfilesetsitem::ComponentEditorFileSetsItem()
//-----------------------------------------------------------------------------
ComponentEditorFileSetsItem::ComponentEditorFileSetsItem(ComponentEditorTreeModel* model,
        LibraryInterface* libHandler, PluginManager& pluginMgr, QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter,
        ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
fileSets_(component->getFileSets()),
expressionParser_(expressionParser),
fileValidator_(new FileValidator(expressionParser_)),
fileSetValidator_(new FileSetValidator(fileValidator_, expressionParser_)),
pluginMgr_(pluginMgr)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

	foreach (QSharedPointer<FileSet> fileSet, *fileSets_)
    {
        QSharedPointer<ComponentEditorFileSetItem> fileSetItem(new ComponentEditorFileSetItem(
            fileSet, model, libHandler, component, referenceCounter, parameterFinder, expressionParser_,
            expressionFormatter, fileSetValidator_, fileValidator_, this));

        connect(fileSetItem.data(), SIGNAL(childRemoved(int)),
                this, SIGNAL(refreshDependencyModel()), Qt::UniqueConnection);

		childItems_.append(fileSetItem);
	}
}

//-----------------------------------------------------------------------------
// Function: componenteditorfilesetsitem::~ComponentEditorFileSetsItem()
//-----------------------------------------------------------------------------
ComponentEditorFileSetsItem::~ComponentEditorFileSetsItem()
{

}

//-----------------------------------------------------------------------------
// Function: componenteditorfilesetsitem::getFont()
//-----------------------------------------------------------------------------
QFont ComponentEditorFileSetsItem::getFont() const
{
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!fileSets_->isEmpty());
    return font;
}

//-----------------------------------------------------------------------------
// Function: componenteditorfilesetsitem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorFileSetsItem::text() const
{ 
	return tr("File sets");
}

//-----------------------------------------------------------------------------
// Function: componenteditorfilesetsitem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorFileSetsItem::editor()
{
	if (!editor_)
    {
        editor_ = new FileSetsEditor(component_, libHandler_, pluginMgr_, parameterFinder_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(fileAdded(File*)), this, SLOT(onFileAdded(File*)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(dependenciesChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connect(this, SIGNAL(refreshDependencyModel()),
            editor_, SLOT(refreshDependencyModel()), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: componenteditorfilesetsitem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorFileSetsItem::getTooltip() const
{
	return tr("Contains the file sets of the component");
}

//-----------------------------------------------------------------------------
// Function: componenteditorfilesetsitem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorFileSetsItem::createChild(int index)
{
	QSharedPointer<ComponentEditorFileSetItem> fileSetItem(new ComponentEditorFileSetItem(
        fileSets_->at(index), model_, libHandler_, component_, referenceCounter_, parameterFinder_,
        expressionParser_, expressionFormatter_, fileSetValidator_, fileValidator_, this));

    connect(fileSetItem.data(), SIGNAL(childRemoved(int)),
        this, SIGNAL(refreshDependencyModel()), Qt::UniqueConnection);

	fileSetItem->setLocked(locked_);
	childItems_.insert(index, fileSetItem);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetsItem::onFileAdded()
//-----------------------------------------------------------------------------
void ComponentEditorFileSetsItem::onFileAdded(File* file)
{
    foreach (QSharedPointer<ComponentEditorItem> item, childItems_)
    {
        QSharedPointer<ComponentEditorFileSetItem> fileSetItem = item.dynamicCast<ComponentEditorFileSetItem>();
        Q_ASSERT(fileSetItem != 0);

        if (isParentFileSet(file, fileSetItem->getFileSet()))
        {
            fileSetItem->onFileAdded(file);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: componenteditorfilesetsitem::isParentFileSet()
//-----------------------------------------------------------------------------
bool ComponentEditorFileSetsItem::isParentFileSet(File* file, const FileSet* fileset) const
{
    foreach (QSharedPointer<File> containedFile, *fileset->getFiles())
    {
        if (file == containedFile.data())
        {
            return true;
        }
    }

    return false;
}
