//-----------------------------------------------------------------------------
// File: fileseteditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 10.05.2012
//
// Description:
// The item for a single file set in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORFILESETITEM_H
#define COMPONENTEDITORFILESETITEM_H

#include "componenteditoritem.h"

#include <QSharedPointer>

class File;
class FileValidator;
class FileSet;
class FileSetValidator;
class LibraryInterface;
class ParameterFinder;
class ExpressionParser;
class ExpressionFormatter;
class ReferenceCounter;

//-----------------------------------------------------------------------------
//! The item for a single file set in the component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorFileSetItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] fileSet                 The file set being edited.
	 *      @param [in] model                   The model that owns the items.
	 *      @param [in] libHandler              The instance that manages the library.
	 *      @param [in] component               The component being edited.
     *      @param [in] referenceCounter        The counter for parameter references.
     *      @param [in] parameterFinder         Finder used to identify parameters.
     *      @param [in] expressionParser        Parser used to calculate expressions.
     *      @param [in] expressionFormatter     Formatter used to format expressions.
     *      @param [in] validator               The validator for checking file set validity.
     *      @param [in] fileValidator           The validator for checking child item (file) validity.
	 *      @param [in] parent                  The parent item.
     */
    ComponentEditorFileSetItem(QSharedPointer<FileSet> fileSet, ComponentEditorTreeModel* model,
        LibraryInterface* libHandler, QSharedPointer<Component> component,
        QSharedPointer<ReferenceCounter> referenceCounter, QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<FileSetValidator> validator, QSharedPointer<FileValidator> fileValidator,
        ComponentEditorItem* parent);

    //! The destructor.
	virtual ~ComponentEditorFileSetItem();

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
     *  Check the validity of this item and sub items.
	 *
	 *      @return bool True if item is in valid state.
	 */
	virtual bool isValid() const;

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

    /*!
     *  Handles the addition of a file outside the editor.
     */
    void onFileAdded(File* file);

    /*!
     *  Returns the file set corresponding to this item.
     */
    FileSet const* getFileSet() const;

signals:
    
    /*!
     *  Indicates the removal of an item.
     *
     *      @param [in] index   The index of the removed item.
     */
    void childRemoved(int index);

private:

    //! No copying. No assignment.
	ComponentEditorFileSetItem(const ComponentEditorFileSetItem& other);
	ComponentEditorFileSetItem& operator=(const ComponentEditorFileSetItem& other);
    
	//! The file set being edited.
	QSharedPointer<FileSet> fileSet_;

	//! Contains the files of the file set.
    QSharedPointer<QList<QSharedPointer<File> > > files_;

    //! The validator for checking file set validity.
    QSharedPointer<FileSetValidator> filesetValidator_;

    //! The validator for checking child validity.
    QSharedPointer<FileValidator> fileValidator_;

    //! Formatter used to change parameter ids to parameter names.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // COMPONENTEDITORFILESETITEM_H
