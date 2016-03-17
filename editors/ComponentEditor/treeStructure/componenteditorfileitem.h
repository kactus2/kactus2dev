//-----------------------------------------------------------------------------
// File: componenteditorfileitem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 16.05.2012
//
// Description:
// The item for a single file in component editor's navigation tree.
//-----------------------------------------------------------------------------

#ifndef COMPONENTEDITORFILEITEM_H
#define COMPONENTEDITORFILEITEM_H

#include "componenteditoritem.h"

#include <QRegExpValidator>


class Component;
class ExpressionParser;
class File;
class FileValidator;
class LibraryInterface;
class ParameterFinder;

//-----------------------------------------------------------------------------
//! The item for a single file in component editor's navigation tree.
//-----------------------------------------------------------------------------
class ComponentEditorFileItem : public ComponentEditorItem
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] file                The file being edited.
	 *      @param [in] model               The model that owns the items.
	 *      @param [in] libHandler          The instance that manages the library.
	 *      @param [in] component           The component being edited.
     *      @param [in] validator           The validator for checking file validity.
     *      @param [in] parameterFinder     The used parameter finder.
     *      @param [in] expressionParser    Parser for calculating expressions.
     *      @param [in] referenceCounter    The counter for parameter references.
	 *      @param [in] parent              The parent item.
	 */
    ComponentEditorFileItem(QSharedPointer<File> file, ComponentEditorTreeModel* model,
        LibraryInterface* libHandler, QSharedPointer<Component> component, QSharedPointer<FileValidator> validator, 
        QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ReferenceCounter> referenceCounter, ComponentEditorItem* parent);

	//! The destructor.
	virtual ~ComponentEditorFileItem();

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
     *  Tells if the item can be opened or not.
	 * 
	 *      @return Files can always be opened and this function returns true.
	 */
	virtual bool canBeOpened() const;

    /*!
     *  Returns the possible actions for opening the item.
     *
     *      @return The actions to open the item.
     */
    virtual QList<QAction*> actions() const;

public slots:
	
	//! Called to open the item in a default editor.
	virtual void openItem();

    //! Called to let the user choose an editor to open the item.
    virtual void openWith();

    //! Called to run the file with the executable set for the file type.
    virtual void run();

protected slots:

	/*!
     *  Handler for editor's contentChanged signal.
	 */
	virtual void onEditorChanged();

	//! Opens the folder that contains the file.
	void onOpenContainingFolder();

private:
	
    //! No copying. No assignment.
	ComponentEditorFileItem(const ComponentEditorFileItem& other);
	ComponentEditorFileItem& operator=(const ComponentEditorFileItem& other);
   
    //! Finds the absolute path of the file represented by the item.
    QString fileAbsolutePath() const;

    /*!
     *  Starts the selected application and runs the file with it.
     *
     *      @param [in] applicationPath   Absolute path to the application executable.     
     */
    void runInApplication(QString const& applicationPath);

    /*!
     *  Checks if the item should be opened in Katus2 CSource editor.
     *
     *      @return True if the item should be opened, false otherwise.
     */
    virtual bool useKactusCSourceEditor() const;

    /*!
     *  Checks if an executable has been set for the file type.
     *
     *      @return True, if valid executable has been set, otherwise false.
     */
    bool runExecutableSet() const;

    /*!
     *  Gets the absolute path to the executable set for the file type.
     *
     *      @return The path to executable.
     */
    QString executablePath() const;

    /*!
     *  Finds all the environmental variables in a text and replaces them with the variable value.
     *
	 *      @param [in] text    The text to search through.
     *
     *      @return A text where variables have been replaced with their values.
     */
	QString resolveEnvironmentVariables(QString const& text) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The file being edited.
	QSharedPointer<File> file_;

    //! The validator for checking file validity.
    QSharedPointer<FileValidator> validator_;

    //! Action to open the file for editing with default editor.
    QAction* editAction_;

    //! Action to open the file for editing with a chosen editor.
    QAction* editWithAction_;

    //! Action to run the file for editing with an application.
	QAction* runAction_;

	//! Action to open the containing folder.
	QAction* openContainingFolderAction_;

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // COMPONENTEDITORFILEITEM_H
