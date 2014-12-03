/* 
 *
 *  Created on: 16.1.2011
 *      Author: Antti Kamppi
 */

#ifndef NAMEGROUPBOX_H_
#define NAMEGROUPBOX_H_

#include <QWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QString>
#include <QValidator>

/*! \brief NameGroupBox is widget to edit a spirit:nameGroup element in IP-Xact.
 *
 * This class provides 3 GUI items to set a name, displayName and description
 * for an element in IP-Xact.
 */
class NameGroupBox: public QGroupBox {

	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 */
	NameGroupBox(QWidget* parent = 0, const QString& title = QString("Names"));

	//! \brief The destructor
	virtual ~NameGroupBox();

	/*! \brief Get the value of the name-field.
	 *
	 * \return QString containing the name.
	 */
	QString getName() const;

	/*! \brief Get the value of the displayName-field.
	 *
	 * \return QString containing the displayName.
	 */
	QString getDisplayName() const;

	/*! \brief Get the value of the description-field.
	 *
	 * \return QString containing the description.
	 */
	QString getDescription() const;

	/*! \brief Check if the widget is in valid mode.
	 *
	 * This function checks that name-field is not empty and does not contain
	 * string with embedded whitespace characters.
	 *
	 * \return True if everything is ok.
	 */
	bool isValid() const;

public slots:

	/*! \brief Set the value for the name-field.
	*
	* \param name QString containing the name.
	*/
	void setName(const QString& name);

	/*! \brief Set the value for the displayName-field.
	*
	* \param displayName QString containing the displayName.
	*/
	void setDisplayName(const QString& displayName);

	/*! \brief Set the value for the description-field.
	*
	* \param description QString containing the description.
	*/
	void setDescription(const QString& description);

signals:

	//! \brief Emitted when contents of the widget change.
	void contentChanged();

	/*! \brief Emitted when the name-field changes
	 * 
	 * \param newName The new name.
	 */
	void nameChanged(const QString& newName);

	/*! \brief Emitted when the display name-field changes.
	 *
	 * \param newDispName The new display name
	 *
	*/
	void displayNameChanged(const QString& newDispName);

	/*! \brief Emitted when the description-field changes.
	 *
	 * \param newDescription The new description.
	 *
	*/
	void descriptionChanged(const QString& newDescription);

private slots:

	//! \brief Handler when user edits the description.
	void onDescriptionChanged();

private:

	//! No copying
	NameGroupBox(const NameGroupBox& other);

	//! No assignment
	NameGroupBox& operator=(const NameGroupBox& other);

	//! \brief Line edit to set the name.
	QLineEdit* nameEdit_;

	//! \brief Line edit to set the displayName.
	QLineEdit* displayNameEdit_;

	//! \brief Editor to write the description.
	QPlainTextEdit* descriptionEdit_;
};

#endif /* NAMEGROUPBOX_H_ */
