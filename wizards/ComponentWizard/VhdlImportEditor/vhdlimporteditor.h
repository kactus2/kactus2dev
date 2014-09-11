//-----------------------------------------------------------------------------
// File: vhdlimporteditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 10.6.2013
//
// Description:
// Used to parse VHDL files and generating IP-XACT packages of them.
//-----------------------------------------------------------------------------

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
class SourceFileDisplayer;

//-----------------------------------------------------------------------------
//! Used to parse VHDL files and generating IP-XACT packages of them.
//-----------------------------------------------------------------------------
class VhdlImportEditor : public QWidget {
	Q_OBJECT

public:

	/*! 
	 *  The constructor.
	 *
	 *  @param [in]   component The component being edited.
	 *  @param [in]   handler   The instance which manages the library.
	 *  @param [in]   parent    The owner of this widget.
	 *
	 */
	VhdlImportEditor(QSharedPointer<Component> component, LibraryInterface* handler, QWidget *parent);
	
	//! The destructor.
	~VhdlImportEditor();

	/*! 
     * Initialize the combo box to select the top-level vhdl vfile.
	 * 
	 * Display all available VHDL files and if one was previously selected and still
	 * available, keep it as selected.
	 *
	 */
	void initializeFileSelection();

	/*!
	 *  Checks the validity of model parameter and port editors.
	 *
	 * @return False if contents of either editor is not valid.
	 */
	bool checkEditorValidity() const;

signals:

	//! Emitted when the contents of the editors change.
	void contentChanged();

private slots:

    //! Called when a source file has been selected.
    void onFileSelected(QString const& filePath);

    //! Called when the selected source file should be opened in an editor.
    void onOpenEditor();

    //! Called when the source file should be re-parsed.
    void onRefresh();

private:
	
	//! No copying
	VhdlImportEditor(const VhdlImportEditor& other);

	//! No assignment
	VhdlImportEditor& operator=(const VhdlImportEditor& other);

    //! Creates the layout for the editor.            
    void setupLayout();

    //! Splitter for source file view and editors.
    QSplitter splitter_;

	//! Path to the xml file containing the target component.
	QString componentXmlPath_;

    //! Top-level vhdl file absolute path.
    QString selectedSourceFile_;

	//! Editor for the imported generics.
	ModelParameterEditor* modelParameterEditor_;

	//! Editor for the imported ports.
	PortsEditor* portEditor_;

    //! Display widget for selected source file content.
    SourceFileDisplayer* sourceDisplayer_;

    //! Used to select the top-level vhdl file.
    FileSelector* fileSelector_;

    //! Button for opening an editor for vhdl file.
    QPushButton* editButton_;

    //! Button for refreshing ports and model parameters from the selected file.
    QPushButton* refreshButton_;

    //! Parser for the vhdl code file.
    VhdlParser* vhdlParser_;
};

#endif // VHDLIMPORTEDITOR_H
