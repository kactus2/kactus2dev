/* 
 *  	Created on: 30.8.2011
 *      Author: Antti Kamppi
 * 		filename: createconfigurationdialog.h
 *		Project: Kactus 2
 */

#ifndef CREATECONFIGURATIONDIALOG_H
#define CREATECONFIGURATIONDIALOG_H

#include <common/widgets/vlnvEditor/vlnveditor.h>
#include <common/widgets/assistedLineEdit/BasicLineContentMatcher.h>

#include <models/component.h>

#include <QDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QLineEdit>
#include <QComboBox>
#include <QStringList>
#include <QSharedPointer>

class LibraryInterface;
class LineEditEx;

/*! \brief Dialog for user to input the info needed to create a new configuration for a component.
 *
 */
class CreateConfigurationDialog : public QDialog {
	Q_OBJECT

public:

	//! \brief Enum to get info on what kind of operation user wanted for new configuration.
	enum DesignSelection {
		USE_EXISTING = 0,	// use the same design as the previous configuration
		CREATE_EMPTY = 1,	// create new empty design for the new configuration
		CREATE_COPY = 2		// create new design by copying the previous design
	};

	/*! \brief The constructor
	 *
	 * \param handler Pointer to the instance that manages the library.
	 * \param component Pointer to the component to create the configuration for.
     * \param configType What kind of configuration is to be created.
	 * \param parent Pointer to the owner of this dialog
	 * \param f Specifies the appearance of the dialog.
	 *
	*/
	CreateConfigurationDialog(LibraryInterface* handler,
		QSharedPointer<Component> component,
        KactusAttribute::Implementation configType,
		QWidget *parent, 
		Qt::WindowFlags f = Qt::Dialog);
	
	//! \brief The destructor
	virtual ~CreateConfigurationDialog();

	/*! \brief Get the VLNV for the design configuration to create.
	 *
	 *
	 * \return VLNV the vlnv for the design configuration.
	*/
	VLNV getConfigurationVLNV() const;

	/*! \brief Get the VLNV for the design to create.
	 *
	 *
	 * \return VLNV the vlnv for the design.
	*/
	VLNV getDesignVLNV() const;

	/*! \brief Get the name for the view to be saved to the component.
	 *
	 *
	 * \return QString Contains the name for the view.
	*/
	QString getConfigurationName() const;

	/*! \brief Get the name of the view that contains the implementation for the component.
	 *
	 *
	 * \return QString contains the name for the implementation view.
	*/
	QString getImplementationViewName() const;

	/*! \brief Find out if user wanted to use existing design or create a new one.
	 *
	 *
	 * \return CreateConfigurationDialog::DesignSelection 
	 * specifies what kind of operation was selected.
	*/
	DesignSelection designSelection() const;

private slots:

	//! \brief Handler for ok-button clicks.
	void onOk();

	//! \brief Handler for configNameEdit's textEdited signal.
	void onConfNameChanged(const QString& newName);

    //! Prevalidates the dialog input and either enables or disables the OK button.
    void prevalidate();

private:
	//! \brief No copying
	CreateConfigurationDialog(const CreateConfigurationDialog& other);

	//! \brief No assignment
	CreateConfigurationDialog& operator=(const CreateConfigurationDialog& other);

	//! \brief Set up the layout of this dialog.
	void setupLayout();

	//! \brief Set up the connections for the editors.
	void setupConnections();

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! \brief Pointer to the component to create the configuration for.
	QSharedPointer<Component> component_;

    //! Specifies what kind of configuration is created.
    KactusAttribute::Implementation configType_;

    //! \brief Line editor to set the name for the new view
    LineEditEx* configNameEdit_;

    //! Content matcher for the configuration name editor.
    BasicLineContentMatcher configNameMatcher_;

	//! \brief Radio button that selects that current design is to be used.
	QRadioButton* useExistingRadio_;

    //! Description label for use existing radio button.
    QLabel* useExistingDescLabel_;

	//! \brief Radio button that selects that a new design is to be created 
	QRadioButton* createNewRadio_;

    //! Description label for create new radio button.
    QLabel* createNewDescLabel_;

	//! \brief Radio button that selects a new design to be copied from the old one
	QRadioButton* createCopyRadio_;

    //! Description label for create copy radio button.
    QLabel* createCopyDescLabel_;

	//! \brief Editor to set the vlnv for the new configuration (and possibly design)
	VLNVEditor* vlnvEdit_;

	//! \brief Select the top-level implementation for this configuration.
	QComboBox* implementationCombo_;

	//! \brief Accept the dialog
	QPushButton* okButton_;

	//! \brief Reject the dialog
	QPushButton* cancelButton_;
};

#endif // CREATECONFIGURATIONDIALOG_H
