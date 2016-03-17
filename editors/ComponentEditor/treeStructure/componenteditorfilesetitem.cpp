//-----------------------------------------------------------------------------
// File: fileseteditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 10.05.2012
//
// Description:
// The item for a single file set in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorfilesetitem.h"
#include "componenteditorfileitem.h"
#include "componenteditortreemodel.h"

#include <editors/ComponentEditor/fileSet/fileseteditor.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>

#include <IPXACTmodels/Component/validators/FileSetValidator.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::ComponentEditorFileSetItem()
//-----------------------------------------------------------------------------
ComponentEditorFileSetItem::ComponentEditorFileSetItem(QSharedPointer<FileSet> fileSet,
        ComponentEditorTreeModel* model, LibraryInterface* libHandler, QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<FileSetValidator> validator, QSharedPointer<FileValidator> fileValidator,
        ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
fileSet_(fileSet),
files_(fileSet->getFiles()),
filesetValidator_(validator),
fileValidator_(fileValidator),
expressionParser_(expressionParser)
{
    setReferenceCounter(referenceCounter);
    setParameterFinder(parameterFinder);
    setExpressionFormatter(expressionFormatter);

    int childCount = files_->size();
    for (int i = 0; i < childCount; i++)
    {
        createChild(i);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::~ComponentEditorFileSetItem()
//-----------------------------------------------------------------------------
ComponentEditorFileSetItem::~ComponentEditorFileSetItem()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorFileSetItem::text() const
{
	return fileSet_->name();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorFileSetItem::isValid() const
{
 	if (!filesetValidator_->validate(fileSet_))
    {
 		return false;
 	}

	// check that the dependent directories exist
	QString xmlPath = libHandler_->getPath(component_->getVlnv());

	foreach (QString const& relDirPath, *fileSet_->getDependencies())
    {
		QString absPath = General::getAbsolutePath(xmlPath, relDirPath);

		if (!QFileInfo(absPath).exists())
        {
			return false;
		}
	}

	return ComponentEditorItem::isValid();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorFileSetItem::editor()
{
	if (!editor_)
    {
        editor_ = new FileSetEditor(
            libHandler_, component_, fileSet_, parameterFinder_, expressionParser_, expressionFormatter_, NULL);
        editor_->setProtection(locked_);
        connect(editor_, SIGNAL(contentChanged()), this, SLOT(onEditorChanged()), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childAdded(int)), this, SLOT(onAddChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childRemoved(int)), this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(childMoved(int, int)), this, SLOT(onMoveChild(int, int)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(helpUrlRequested(QString const&)), this, SIGNAL(helpUrlRequested(QString const&)));

        connect(editor_, SIGNAL(childRemoved(int)), this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);

        connectItemEditorToReferenceCounter();
	}

	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorFileSetItem::getTooltip() const
{
	return tr("Contains a list of files and their build commands");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorFileSetItem::createChild(int index)
{
	QSharedPointer<ComponentEditorFileItem> fileItem (new ComponentEditorFileItem(files_->at(index), model_,
        libHandler_, component_, fileValidator_, parameterFinder_, expressionParser_, referenceCounter_, this));

    connect(fileItem.data(), SIGNAL(openCSource(QString const&, QSharedPointer<Component>)),
            model_, SIGNAL(openCSource(QString const&, QSharedPointer<Component>)), Qt::UniqueConnection);

	fileItem->setLocked(locked_);
	childItems_.insert(index, fileItem);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::onFileAdded()
//-----------------------------------------------------------------------------
void ComponentEditorFileSetItem::onFileAdded(File* file)
{
    if (files_->at(files_->size() - 1) != file)
    {
        Q_ASSERT(false);
    }

    onAddChild(files_->size() - 1);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::getFileSet()
//-----------------------------------------------------------------------------
FileSet const* ComponentEditorFileSetItem::getFileSet() const
{
    return fileSet_.data();
}
