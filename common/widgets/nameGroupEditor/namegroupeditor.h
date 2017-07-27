/* 
 *  	Created on: 28.5.2012
 *      Author: Antti Kamppi
 * 		filename: namegroupeditor.h
 *		Project: Kactus 2
 */

#ifndef NAMEGROUPEDITOR_H
#define NAMEGROUPEDITOR_H

#include <IPXACTmodels/common/NameGroup.h>

#include <QGroupBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QString>

/*! Editor to edit the details of a NameGroup-struct.
 *
 */
class NameGroupEditor : public QGroupBox {
	Q_OBJECT

public:

	//! The maximum height for the editor
	static const int MAX_EDITOR_HEIGHT = 130;

    NameGroupEditor(QSharedPointer<NameGroup> nameGroup,
        QWidget *parent,
        const QString& title = QString("Names"));

	//! The destructor
	virtual ~NameGroupEditor();

	/*! Get the name of the name group.
	 *
	 * \return The contents of the name field.
	*/
	QString name() const;

	/*! Get the display name of the name group.
	 *
	 * \return The contents of the display name field.
	*/
	QString displayName() const;

	/*! Get the description of the name group.
	 *
	 * \return The contents of the description field.
	*/
	QString description() const;

	/*! Check if the editor is in valid state.
	 *
	 * \return True if the name-field is not empty.
	*/
	bool isValid() const;

public slots:

	/*! Refresh the edit fields.
	 *
	*/
	void refresh();

signals:

	//! Emitted when user edits one of the fields.
	void contentChanged();

    /*!
     *  Informs of change in name.
     */
    void nameChanged();

private slots:

	/*! Handler for editing of the name field.
	 *
	 * \param newName The new name for the name group.
	 *
	*/
	void onNameChanged(const QString& newName);

	/*! Handler for editing of the display name field.
	 *
	 * \param newName The new display name for the name group.
	 *
	*/
	void onDisplayNameChanged(const QString& newName);

	/*! Handler for editing of the description field.
	 *
	*/
	void onDescriptionChanged();

private:
	//! No copying
	NameGroupEditor(const NameGroupEditor& other);
    void setupLayout();

	//! No assignment
	NameGroupEditor& operator=(const NameGroupEditor& other);

	//! The name group to edit.
	QSharedPointer<NameGroup> nameGroup_;

	//! Line edit to set the name.
	QLineEdit nameEdit_;

	//! Line edit to set the displayName.
	QLineEdit displayNameEdit_;

	//! Editor to write the description.
	QPlainTextEdit descriptionEdit_;
};

#endif // NAMEGROUPEDITOR_H
