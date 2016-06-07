//-----------------------------------------------------------------------------
// File: ImportEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 10.6.2013
//
// Description:
// Used to parse source files and generating IP-XACT packages of them.
//-----------------------------------------------------------------------------

#ifndef VHDLIMPORTEDITOR_H
#define VHDLIMPORTEDITOR_H

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <QPushButton>
#include <QSharedPointer>
#include <QSplitter>
#include <QString>
#include <QTabWidget>
#include <QWidget>

class FileSet;
class Component;
class FileSelector;
class ImportRunner;
class LibraryInterface;
class PluginManager;
class PortsEditor;
class View;

//-----------------------------------------------------------------------------
//! Used to parse source files and generating IP-XACT packages of them.
//-----------------------------------------------------------------------------
class ImportEditor : public QWidget
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component               The component being edited.
	 *      @param [in] handler                 The instance which manages the library.
	 *      @param [in] pluginMgr               The plugin manager.
	 *      @param [in] parameterFinder         The parameter finder.
	 *      @param [in] expressionFormatter     The expression formatter.
	 *      @param [in] parent                  The owner of this widget.
	 */
	ImportEditor(QSharedPointer<Component> component,
        LibraryInterface* handler,
        PluginManager const& pluginMgr,
        QSharedPointer <ComponentParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QWidget *parent);
	
	//! The destructor.
	~ImportEditor();

	/*!
     *  Initialize the combo box to select the top-level vhdl vfile.
	 * 
	 *  Display all available VHDL files and if one was previously selected and still available, keep it as
     *  selected.
	 */
	void initializeFileSelection();

	/*!
	 *  Checks the validity of model parameter and port editors.
	 *
	 *      @return False if contents of either editor is not valid.
	 */
	bool checkEditorValidity() const;

signals:

	//! Emitted when the contents of the editors change.
	void contentChanged();

    //! Emitted when a new component is imported.
    void componentChanged(QSharedPointer<Component>);

    /*!
     *  Increase the number of references to the selected parameter.
     *
     *      @param [in] id      The selected parameter.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Decrease the number of references to the selected parameter.
     *
     *      @param [in] id      The selected parameter.
     */
    void decreaseReferences(QString const& id);

private slots:

    //! Called when a source file has been selected.
    void onFileSelected(QString const& filePath, QSharedPointer<FileSet> fileSet);

    //! Called when the selected source file should be opened in an editor.
    void onOpenEditor();

    //! Called when the source file should be re-parsed.
    void onRefresh();

private:
	
	//! No copying.
	ImportEditor(const ImportEditor& other);    

	//! No assignment.
	ImportEditor& operator=(const ImportEditor& other);

    /*!
     *  Finds the file extensions for given file types.
     *
     *      @param [in] possibleFileTypes   The file types whose extensions to find.
     *
     *      @return The file extensions for the file types.
     */
    QStringList fileExtensionsForTypes(QStringList possibleFileTypes) const;

    /*!
     *  Gets the absolute path to the selected source file.
     *
     *      @return The absolute path to the selected source file.
     */
    QString selectedFileAbsolutePath() const;

    /*!
     *  Scrolls the source displayer to the first highlighted line, if any.
     */
    void scrollSourceDisplayToFirstHighlight() const;

    //! Creates the layout for the editor.            
    void setupLayout();

    //-----------------------------------------------------------------------------
    //! Data.
    //-----------------------------------------------------------------------------

    //! Splitter for source file view and editors.
    QSplitter splitter_;

	//! Path to the XML file containing the target component.
	QString componentXmlPath_;

    //! The original component.
    QSharedPointer<Component> component_;

    //! The imported component.
    QSharedPointer<Component> importComponent_ ;

    //! Top-level source file relative path from component XML file.
    QString selectedSourceFile_;

	//! The file set where the selectedSourceFile_ belongs to.
	QSharedPointer<FileSet> selectedFileSet_;

	//! Editor for the imported ports.
	PortsEditor* portEditor_;

    //! Used to select the top-level source file.
    FileSelector* fileSelector_;

    //! Button for opening an editor for the selected source file.
    QPushButton* editButton_;

    //! Button for refreshing ports and model parameters from the selected file.
    QPushButton* refreshButton_;

    //! Tabs for displaying imported source files.
    QTabWidget* sourceDisplayTabs_;

    //! Runner for all available import parsers.
    ImportRunner* runner_;

    //! Widget for showing notifications from import plugins.
    QWidget* messageBox_;

    //! The available component views.
    QSharedPointer<QList<QSharedPointer<View> > > componentViews_;
};

#endif // VHDLIMPORTEDITOR_H
