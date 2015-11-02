//-----------------------------------------------------------------------------
// File: createconfigurationdialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 30.08.2011
//
// Description:
// Dialog for user to input the info needed to create a new configuration for a component.
//-----------------------------------------------------------------------------

#ifndef CREATECONFIGURATIONDIALOG_H
#define CREATECONFIGURATIONDIALOG_H

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/assistedLineEdit/BasicLineContentMatcher.h>

#include <QDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QComboBox>
#include <QSharedPointer>

class LibraryInterface;
class LineEditEx;
class Component;

//-----------------------------------------------------------------------------
//! Dialog for user to input the info needed to create a new configuration for a component.
//-----------------------------------------------------------------------------
class CreateConfigurationDialog : public QDialog
{
	Q_OBJECT

public:

	//! Enum to get info on what kind of operation user wanted for new configuration.
	enum DesignSelection
    {
		USE_EXISTING = 0,	// use the same design as the previous configuration
		CREATE_EMPTY = 1,	// create new empty design for the new configuration
		CREATE_COPY = 2		// create new design by copying the previous design
	};

	/*!
     *  The constructor.
	 *
	 *      @param [in] handler     Pointer to the instance that manages the library.
	 *      @param [in] component   Pointer to the component to create the configuration for.
     *      @param [in] configType  What kind of configuration is to be created.
	 *      @param [in] parent      Pointer to the owner of this dialog
	 *      @param [in] f           Specifies the appearance of the dialog.
	 */
	CreateConfigurationDialog(LibraryInterface* handler,
		QSharedPointer<Component> component,
        KactusAttribute::Implementation configType,
		QWidget *parent, 
		Qt::WindowFlags f = Qt::Dialog);
	
	//! The destructor.
	virtual ~CreateConfigurationDialog();

	/*!
     *  Get the VLNV for the design configuration to create.
	 *
	 *      @return VLNV the vlnv for the design configuration.
	 */
	VLNV getConfigurationVLNV() const;

	/*!
     *  Get the VLNV for the design to create.
	 *
	 *      @return VLNV the vlnv for the design.
	 */
	VLNV getDesignVLNV() const;

	/*!
     *  Get the name for the view to be saved to the component.
	 *
	 *      @return QString Contains the name for the view.
	 */
	QString getConfigurationName() const;

	/*!
     *  Get the name of the view that contains the implementation for the component.
	 *
	 *      @return QString contains the name for the implementation view.
	 */
	QString getImplementationViewName() const;

	/*!
     *  Find out if user wanted to use existing design or create a new one.
	 *
	 *      @return CreateConfigurationDialog::DesignSelection specifies what kind of operation was selected.
	 */
	DesignSelection designSelection() const;

private slots:

	//! Handler for ok-button clicks.
	void onOk();

	//! Handler for configNameEdit's textEdited signal.
	void onConfNameChanged(const QString& newName);

    //! Prevalidates the dialog input and either enables or disables the OK button.
    void prevalidate();

private:
	//! No copying.
	CreateConfigurationDialog(const CreateConfigurationDialog& other);

	//! No assignment.
	CreateConfigurationDialog& operator=(const CreateConfigurationDialog& other);

	//! Set up the layout of this dialog.
	void setupLayout();

	//! Set up the connections for the editors.
	void setupConnections();

	//! Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! Pointer to the component to create the configuration for.
	QSharedPointer<Component> component_;

    //! Specifies what kind of configuration is created.
    KactusAttribute::Implementation configType_;

    //! Line editor to set the name for the new view.
    LineEditEx* configNameEdit_;

    //! Content matcher for the configuration name editor.
    BasicLineContentMatcher configNameMatcher_;

	//! Radio button that selects that current design is to be used.
	QRadioButton* useExistingRadio_;

    //! Description label for use existing radio button.
    QLabel* useExistingDescLabel_;

	//! Radio button that selects that a new design is to be created.
	QRadioButton* createNewRadio_;

    //! Description label for create new radio button.
    QLabel* createNewDescLabel_;

	//! Radio button that selects a new design to be copied from the old one.
	QRadioButton* createCopyRadio_;

    //! Description label for create copy radio button.
    QLabel* createCopyDescLabel_;

	//! Editor to set the vlnv for the new configuration (and possibly design).
	VLNVEditor* vlnvEdit_;

	//! Select the top-level implementation for this configuration.
	QComboBox* implementationCombo_;

	//! Accept the dialog.
	QPushButton* okButton_;

	//! Reject the dialog.
	QPushButton* cancelButton_;
};

#endif // CREATECONFIGURATIONDIALOG_H
