//-----------------------------------------------------------------------------
// File: bspbuildeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 03.04.2013
//
// Description:
// The editor to set the BSP build options for an SW view of a HW component.
//-----------------------------------------------------------------------------

#ifndef BSPBUILDEDITOR_H
#define BSPBUILDEDITOR_H

#include <QGroupBox>
#include <QSharedPointer>
#include <QComboBox>
#include <QLineEdit>

class BSPBuildCommand;
class Component;
class FileTypeSelector;
class CpuSelector;

//-----------------------------------------------------------------------------
//! The editor to set the BSP build options for an SW view of a HW component.
//-----------------------------------------------------------------------------
class BSPBuildEditor : public QGroupBox
{
	Q_OBJECT

public:

	//! Define the minimum sizes for the editors.
	enum Sizes
    {
		LINE_WIDTH = 200
	};

	/*! The constructor.
	 *
	 *      @param [in] BSPCommand Pointer to the command being edited.
	 *      @param [in] parent Pointer to the owner of the editor.
	*/
	BSPBuildEditor(QSharedPointer<BSPBuildCommand> BSPCommand, QSharedPointer<Component> component,
		QWidget* parent);
	
	//! The destructor.
	virtual ~BSPBuildEditor();

	/*! Check for the validity of the edited BSP build command.
	*
	*      @return True if all build commands are in valid state.
	*/
	virtual bool isValid() const;

public slots:

	//! Refresh the contents to display.
	void refresh();

signals:

	//! Inform that contents of this editor have changed.
	void contentChanged();

private slots:

	//! Handler for changes in file type.
	void onFileTypeChange(const QString& fileType);

	//! Handler for changes in command.
	void onCommandChange(const QString& newCom);

	//! Handler for changes in arguments.
	void onArgumentChange(const QString& newArg);

	//! Handler for changes in cpu name.
	void onCpuChange(const QString& cpuName);

private:
	
	//! No copying
	BSPBuildEditor(const BSPBuildEditor& other);

	//! No assignment
	BSPBuildEditor& operator=(const BSPBuildEditor& other);

	//! Pointer to the command being edited.
	QSharedPointer<BSPBuildCommand> bspCommand_;

	//! The file type for the BSP file.
	FileTypeSelector* fileTypeSelector_;

	//! Command to build the board support package.
	QLineEdit* commandEditor_;

	//! Arguments for the command.
	QLineEdit* argumentsEditor_;

	//! The cpu name for the BSP generator.
	CpuSelector* cpuSelector_;
};

#endif // BSPBUILDEDITOR_H
