/* 
 *	Created on:	10.6.2013
 *	Author:		Antti Kamppi
 *	File name:	vhdlimporteditor.h
 *	Project:		Kactus 2
*/

#ifndef VHDLIMPORTEDITOR_H
#define VHDLIMPORTEDITOR_H

#include <IPXACTmodels/component.h>

#include <QWidget>
#include <QSharedPointer>
#include <QString>

#include <QTextEdit>
#include <QSplitter>
#include <QPushButton>

class ModelParameterEditor;
class PortsEditor;
class LibraryInterface;
class FileSelector;
class VhdlParser;
/*! \brief Used to parse VHDL files and generating IP-XACT packages of them.
 *
 */
class VhdlImportEditor : public QWidget {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * Method: 		VhdlImportEditor
	 * Full name:	VhdlImportEditor::VhdlImportEditor
	 * Access:		private 
	 *
	 * \param component Pointer to the component being edited.
	 * \param handler Pointer to the instance which manages the library.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	VhdlImportEditor(QSharedPointer<Component> component,
		LibraryInterface* handler,
		QWidget *parent);
	
	//! \brief The destructor.
	~VhdlImportEditor();

	/*! \brief Initialize the combo box to select the top-level vhdl.
	 * 
	 * Display all available VHDL files and if one was previously selected and still
	 * available, keep it as selected.
	 * 
	 * Method: 		initializeFileSelection
	 * Full name:	VhdlImportEditor::initializeFileSelection
	 * Access:		public 
	 *
	 *
	*/
	void initializeFileSelection();

    void setVhdlFileSelectionEnabled(bool enabled);


	/*! \brief Check the validity of model parameter and port editors.
	 *
	 * Method: 		checkEditorValidity
	 * Full name:	VhdlImportEditor::checkEditorValidity
	 * Access:		public 
	 *
	 *
	 * \return False if contents of either editor is not valid.
	*/
	bool checkEditorValidity() const;

signals:

	//! \brief Emitted when the contents of the editors change.
	void contentChanged();

private slots:

    void onFileSelected( const QString& filePath );
    void onOpenEditor();
    void onRefresh();

private:
	
	//! \brief No copying
	VhdlImportEditor(const VhdlImportEditor& other);

	//! \brief No assignment
	VhdlImportEditor& operator=(const VhdlImportEditor& other);

    /*!
     *  Creates the layout for the page.
     */
    void setupLayout();

    //! Splitter for vhdl view and editors.
    QSplitter splitter_;

	//! \brief Path to the containing xml
	QString componentXmlPath_;

    //! \brief Top-level vhdl file absolute path.
    QString selectedSourceFile_;

	//! \brief Editor to set the generics.
	ModelParameterEditor* modelParameterEditor_;

	//! \brief Editor to set the ports.
	PortsEditor* portEditor_;

    //! \brief Used to select the top-level vhdl file.
    FileSelector* fileSelector_;

    //! \brief Button for opening an editor for vhdl file.
    QPushButton* editButton_;

    //! \brief Button for refreshing ports and model parameters from file.
    QPushButton* refreshButton_;

    //! \brief Displays and parses the vhdl code
    VhdlParser* vhdlParser_;

};

#endif // VHDLIMPORTEDITOR_H
