//-----------------------------------------------------------------------------
// File: componenteditorfilesetsitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 09.05.2012
//
// Description:
// The file sets-item in the component editor navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORFILESETSITEM_H
#define COMPONENTEDITORFILESETSITEM_H

#include "componenteditoritem.h"

#include <QSharedPointer>

class Component;
class ExpressionParser;
class FileSet;
class File;
class FileValidator;
class FileSetValidator;
class ParameterFinder;
class PluginManager;
class ReferenceCounter;
class ExpressionParser;
class ParameterFinder;
class ExpressionFormatter;

//-----------------------------------------------------------------------------
//! The file sets-item in the component editor navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorFileSetsItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] model                   The model that owns the items.
	 *      @param [in] libHandler              The instance that manages the library.
  	 *      @param [in] pluginMgr               The plugin manager.
	 *      @param [in] component               The component being edited.
     *      @param [in] referenceCounter        The counter for parameter references.
     *      @param [in] parameterFinder         Finder used to identify parameters.
     *      @param [in] expressionParser        Parser used to calculate expressions.
     *      @param [in] expressionFormatter     Formatter used to format expressions.
     *      @param [in] parameterFinder         The finder for component parameters.
	 *      @param [in] parent                  The parent item.
	 */
    ComponentEditorFileSetsItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
        PluginManager& pluginMgr, QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter,
        ComponentEditorItem* parent);

	//! The destructor.
	virtual ~ComponentEditorFileSetsItem();

    /*!
     *  Get the font to be used for text of this item.
	 *
	 *      @return QFont instance that defines the font to be used.
	 */
	virtual QFont getFont() const;

	/*!
     *  Get the tool tip for the item.
	 * 
	 *      @return The text for the tool tip to print to user.
	 */
	virtual QString getTooltip() const;

	/*!
     *  Get the text to be displayed to user in the tree for this item.
	 *
	 *      @return QString Contains the text to display.
	 */
	virtual QString text() const;

	/*!
     *  Get The editor of this item.
	 *
	 *      @return The editor to use for this item.
	 */
	virtual ItemEditor* editor();

	/*!
     *  Add a new child to the item.
	 *
	 *      @param [in] index   The index to add the child into.
	 */
	virtual void createChild(int index);

signals:

    //! Emitted when the dependency model should be refreshed.
    void refreshDependencyModel();

private slots:
    /*!
     *  Handles the addition of a file outside the file set editor.
     */
    void onFileAdded(File* file);

private:

    bool isParentFileSet(File* file, const FileSet* fileset) const;

	//! No copying. No assignment.
	ComponentEditorFileSetsItem(const ComponentEditorFileSetsItem& other);
	ComponentEditorFileSetsItem& operator=(const ComponentEditorFileSetsItem& other);

	//! The file sets to edit.
    QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets_;

    //! Expression parser for filesets and files.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! Validator for file items.
    QSharedPointer<FileValidator> fileValidator_;

    //! Validator for file set items.
    QSharedPointer<FileSetValidator> fileSetValidator_;

	//! The plugin manager.
	PluginManager& pluginMgr_;
};

#endif // COMPONENTEDITORFILESETSITEM_H
