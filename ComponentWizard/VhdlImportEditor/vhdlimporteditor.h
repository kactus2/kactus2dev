/* 
 *	Created on:	10.6.2013
 *	Author:		Antti Kamppi
 *	File name:	vhdlimporteditor.h
 *	Project:		Kactus 2
*/

#ifndef VHDLIMPORTEDITOR_H
#define VHDLIMPORTEDITOR_H

#include <models/component.h>
#include <IPXactWrapper/ComponentEditor/modelParameters/modelparametereditor.h>
#include <IPXactWrapper/ComponentEditor/ports/portseditor.h>
#include <Vhdl2IPXact/VhdlParser.h>

#include <QWidget>
#include <QSharedPointer>
#include <QString>

#include <QTextEdit>

class LibraryInterface;
class FileSelector;

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
	 * \param basePath Path to the component's xml file, used as base when operating on relative file paths.
	 * \param component Pointer to the component being edited.
	 * \param handler Pointer to the instance which manages the library.
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	VhdlImportEditor(const QString& basePath,
		QSharedPointer<Component> component,
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

	/*! \brief Handler for changes in the file selector.
	 *
	 * Method: 		onFileSelected
	 * Full name:	VhdlImportEditor::onFileSelected
	 * Access:		private 
	 *
	 * \param filePath The relative path of the selected file.
	 *
	*/
	void onFileSelected(const QString& filePath);

private:
	
	//! \brief No copying
	VhdlImportEditor(const VhdlImportEditor& other);

	//! \brief No assignment
	VhdlImportEditor& operator=(const VhdlImportEditor& other);

	//! \brief Path to the containing xml
	QString basePath_;

	//! \brief Used to select the top-level vhdl file.
	FileSelector* fileSelector_;

	//! \brief Displays and parses the vhdl code
	// TODO: replace this with actual call in final.
	VhdlParser* vhdlParser_;

	//! \brief Editor to set the generics.
	ModelParameterEditor* modelParams_;

	//! \brief Editor to set the ports.
	PortsEditor* ports_;
};

#endif // VHDLIMPORTEDITOR_H
