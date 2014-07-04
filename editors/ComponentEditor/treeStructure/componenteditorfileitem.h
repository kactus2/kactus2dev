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
#include <IPXACTmodels/file.h>

#include <QRegExpValidator>


class ComponentEditorFileItem : public ComponentEditorItem {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param file Pointer to the file being edited.
	 * \param model Pointer to the model that owns the items.
	 * \param libHandler Pointer to the instance that manages the library.
	 * \param component Pointer to the component being edited.
	 * \param parent Pointer to the parent item.
	 *
	*/
	ComponentEditorFileItem(QSharedPointer<File> file,
		ComponentEditorTreeModel* model,
		LibraryInterface* libHandler,
		QSharedPointer<Component> component,
		ComponentEditorItem* parent);

	//! \brief The destructor
	virtual ~ComponentEditorFileItem();

	/*! \brief Get the tool tip for the item.
	 * 
	 * \return The text for the tool tip to print to user.
	*/
	virtual QString getTooltip() const;

	/*! \brief Get the text to be displayed to user in the tree for this item.
	 *
	 * \return QString Contains the text to display.
	*/
	virtual QString text() const;

	/*! \brief Check the validity of this item and sub items.
	 *
	 * \return bool True if item is in valid state.
	*/
	virtual bool isValid() const;

	/*! \brief Get pointer to the editor of this item.
	 *
	 * \return Pointer to the editor to use for this item.
	*/
	virtual ItemEditor* editor();

	/*! \brief Tells if the item can be opened or not.
	 * 
	 * Files can always be opened and this function returns true.
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

	/*! \brief Handler for editor's contentChanged signal.
	 *
	*/
	virtual void onEditorChanged();

private:
	//! \brief No copying
	ComponentEditorFileItem(const ComponentEditorFileItem& other);

	//! \brief No assignment
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
     *  Returns true if the item should be opened in Kactus2 CSource editor.
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

    //! \brief Pointer to the file being edited.
	QSharedPointer<File> file_;

	//! \brief Checks if the given string is a URL (invalids are allowed) or not.
	QRegExpValidator* urlTester_;

	//! \brief Validates the file path to check if they are valid URLs.
	QRegExpValidator* urlValidator_;

    //! Action to open the file for editing with default editor.
    QAction* editAction_;

    //! Action to open the file for editing with a chosen editor.
    QAction* editWithAction_;

    //! Action to run the file for editing with an application.
    QAction* runAction_;
};

#endif // COMPONENTEDITORFILEITEM_H
