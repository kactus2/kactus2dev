/* 
 *  	Created on: 28.5.2012
 *      Author: Antti Kamppi
 * 		filename: namegroupeditor.h
 *		Project: Kactus 2
 */

#ifndef NAMEGROUPEDITOR_H
#define NAMEGROUPEDITOR_H

#include <IPXACTmodels/NameGroup.h>

#include <QGroupBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QString>

/*! \brief Editor to edit the details of a NameGroup-struct.
 *
 */
class NameGroupEditor : public QGroupBox {
	Q_OBJECT

public:

	//! \brief The maximum height for the editor
	static const int MAX_EDITOR_HEIGHT = 130;

	/*! \brief The constructor
	 *
	 * \param nameGroup Reference to the name group to edit.
	 * \param parent Pointer to the owner of this widget.
	 * \param title The title for the group box.
	 *
	*/
	NameGroupEditor(NameGroup& nameGroup,
		QWidget *parent,
		const QString& title = QString("Names"));

	//! \brief The destructor
	~NameGroupEditor();

	/*! \brief Get the name of the name group.
	 *
	 * \return The contents of the name field.
	*/
	QString getName() const;

	/*! \brief Get the display name of the name group.
	 *
	 * \return The contents of the display name field.
	*/
	QString getDisplayName() const;

	/*! \brief Get the description of the name group.
	 *
	 * \return The contents of the description field.
	*/
	QString getDescription() const;

	/*! \brief Check if the editor is in valid state.
	 *
	 * \return True if the name-field is not empty.
	*/
	bool isValid() const;

public slots:

	/*! \brief Refresh the edit fields.
	 *
	*/
	void refresh();

signals:

	//! \brief Emitted when user edits one of the fields.
	void contentChanged();

    /*!
     *  Informs of change in name.
     */
    void nameChanged();

private slots:

	/*! \brief Handler for editing of the name field.
	 *
	 * \param newName The new name for the name group.
	 *
	*/
	void onNameChanged(const QString& newName);

	/*! \brief Handler for editing of the display name field.
	 *
	 * \param newName The new display name for the name group.
	 *
	*/
	void onDisplayNameChanged(const QString& newName);

	/*! \brief Handler for editing of the description field.
	 *
	*/
	void onDescriptionChanged();

private:
	//! \brief No copying
	NameGroupEditor(const NameGroupEditor& other);

	//! \brief No assignment
	NameGroupEditor& operator=(const NameGroupEditor& other);

	//! \brief The name group to edit.
	NameGroup& nameGroup_;

	//! \brief Line edit to set the name.
	QLineEdit nameEdit_;

	//! \brief Line edit to set the displayName.
	QLineEdit displayNameEdit_;

	//! \brief Editor to write the description.
	QPlainTextEdit descriptionEdit_;
};

#endif // NAMEGROUPEDITOR_H
