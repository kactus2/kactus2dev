/* 
 *	Created on:	3.4.2013
 *	Author:		Antti Kamppi
 *	File name:	bspbuildeditor.h
 *	Project:		Kactus 2
*/

#ifndef BSPBUILDEDITOR_H
#define BSPBUILDEDITOR_H

#include <IPXACTmodels/bspbuildcommand.h>
#include <IPXACTmodels/component.h>

#include <QGroupBox>
#include <QSharedPointer>
#include <QComboBox>
#include <QLineEdit>

class FileTypeSelector;
class CpuSelector;

/*! \brief The editor to set the BSP build options for an SW view of a HW component.
 *
 */
class BSPBuildEditor : public QGroupBox {
	Q_OBJECT

public:

	//! \brief Define the minimum sizes for the editors.
	enum Sizes {
		LINE_WIDTH = 200
	};

	/*! \brief The constructor.
	 *
	 * Method: 		BSPBuildEditor
	 * Full name:	BSPBuildEditor::BSPBuildEditor
	 * Access:		private 
	 *
	 * \param BSPCommand Pointer to the command being edited.
	 * \param parent Pointer to the owner of the editor.
	 *
	*/
	BSPBuildEditor(QSharedPointer<BSPBuildCommand> BSPCommand,
		QSharedPointer<Component> component,
		QWidget *parent);
	
	//! \brief The destructor.
	virtual ~BSPBuildEditor();

	/*! \brief Check for the validity of the edited BSP build command.
	*
	* \return True if all build commands are in valid state.
	*/
	virtual bool isValid() const;

public slots:

	//! \brief Refresh the contents to display.
	void refresh();

signals:

	//! \brief Inform that contents of this editor have changed.
	void contentChanged();

private slots:

	//! \brief Handler for changes in file type.
	void onFileTypeChange(const QString& fileType);

	//! \brief Handler for changes in command.
	void onCommandChange(const QString& newCom);

	//! \brief Handler for changes in arguments.
	void onArgumentChange(const QString& newArg);

	//! \brief Handler for changes in cpu name.
	void onCpuChange(const QString& cpuName);

private:
	
	//! \brief No copying
	BSPBuildEditor(const BSPBuildEditor& other);

	//! \brief No assignment
	BSPBuildEditor& operator=(const BSPBuildEditor& other);

	//! \brief Pointer to the command being edited.
	QSharedPointer<BSPBuildCommand> bspCommand_;

	//! \brief The file type for the BSP file.
	FileTypeSelector* fileType_;

	//! \brief Command to build the board support package.
	QLineEdit* command_;

	//! \brief Arguments for the command.
	QLineEdit* arguments_;

	//! \brief The cpu name for the BSP generator.
	CpuSelector* cpuName_;
};

#endif // BSPBUILDEDITOR_H
