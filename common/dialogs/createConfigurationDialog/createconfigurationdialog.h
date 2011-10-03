/* 
 *  	Created on: 30.8.2011
 *      Author: Antti Kamppi
 * 		filename: createconfigurationdialog.h
 *		Project: Kactus 2
 */

#ifndef CREATECONFIGURATIONDIALOG_H
#define CREATECONFIGURATIONDIALOG_H

#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <models/component.h>

#include <QDialog>
#include <QPushButton>
#include <QRadioButton>
#include <QLineEdit>
#include <QComboBox>
#include <QStringList>
#include <QSharedPointer>

class LibraryInterface;

/*! \brief Dialog for user to input the info needed to create a new configuration for a component.
 *
 */
class CreateConfigurationDialog : public QDialog {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param handler Pointer to the instance that manages the library.
	 * \param component Pointer to the component to create the configuration for.
	 * \param parent Pointer to the owner of this dialog
	 * \param f Specifies the appearance of the dialog.
	 *
	*/
	CreateConfigurationDialog(LibraryInterface* handler,
		QSharedPointer<Component> component,
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

	/*! \brief True if user wanted to use an existing design.
	 * 
	 * False if user wanted to create a new design.
	 *
	 * \return bool True if user wanted to use an existing design.
	*/
	bool useExistingDesign() const;

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

private slots:

	//! \brief Handler for ok-button clicks.
	void onOk();

	//! \brief Handler for configNameEdit's textEdited signal.
	void onConfNameChanged(const QString& newName);

private:
	//! \brief No copying
	CreateConfigurationDialog(const CreateConfigurationDialog& other);

	//! \brief No assignment
	CreateConfigurationDialog& operator=(const CreateConfigurationDialog& other);

	//! \brief Set up the layout of this dialog.
	void setupLayout();

	//! \brief Set up the connections for the editors.
	void setupConnections();

	/*! \brief Validate the inputs in the dialog
	 *
	 * \return QStringList contains the error messages for errors found in the dialog inputs.
	*/
	QStringList validate() const;

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! \brief Pointer to the component to create the configuration for.
	QSharedPointer<Component> component_;

	//! \brief Radio button that selects that current design is to be used.
	QRadioButton useExistingRadio_;

	//! \brief Radio button that selects that a new design is to be created 
	QRadioButton createNewRadio_;

	//! \brief Line editor to set the name for the new view
	QLineEdit configNameEdit_;

	//! \brief Editor to set the vlnv for the new configuration (and possibly design)
	VLNVEditor vlnvEdit_;

	//! \brief Select the top-level implementation for this configuration.
	QComboBox implementationCombo_;

	//! \brief Accept the dialog
	QPushButton okButton_;

	//! \brief Reject the dialog
	QPushButton cancelButton_;
};

#endif // CREATECONFIGURATIONDIALOG_H
