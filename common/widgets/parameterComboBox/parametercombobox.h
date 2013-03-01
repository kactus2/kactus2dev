/* 
 *	Created on: 1.3.2013
 *	Author:		Antti Kamppi
 * 	File name:	parametercombobox.h
 * 	Project:	Kactus 2
*/

#ifndef PARAMETERCOMBOBOX_H
#define PARAMETERCOMBOBOX_H

#include <models/component.h>

#include <QComboBox>
#include <QSharedPointer>

/*! \brief The combo box to select parameter or model parameter of a component.
 *
 */
class ParameterComboBox : public QComboBox {
	Q_OBJECT

public:

	/*! \brief The constructor.
	 *
	 * Method: 		ParameterComboBox
	 * Full name:	ParameterComboBox::ParameterComboBox
	 * Access:		private 
	 *
	 * \param component Pointer to the component which contains the parameters/model parameters.
	 * \param parent Pointer to the owner of the combo box.
	 * \param editable If true then the combo box is editable and user may input custom text.
	 *
	*/
	ParameterComboBox(QSharedPointer<Component> component, QWidget *parent, bool editable = true);
	
	//! \brief The destructor
	virtual ~ParameterComboBox();

	//! \brief Set the text for the combo box.
	void setRemapText(const QString& text);

public slots:

	//! \brief Update the contents of the combo box.
	void refresh();

signals:

	//! \brief Emitted when the text in the combo box changes.
	void textChanged(const QString& text);

private slots:

	//! \brief Handler for changes in the combo box.
	void onEditChanged(const QString& text);

private:
	
	//! \brief No copying
	ParameterComboBox(const ParameterComboBox& other);

	//! \brief No assignment
	ParameterComboBox& operator=(const ParameterComboBox& other);

	//! \brief The component which contains the parameters and model parameters.
	QSharedPointer<Component> component_;
};

#endif // PARAMETERCOMBOBOX_H
