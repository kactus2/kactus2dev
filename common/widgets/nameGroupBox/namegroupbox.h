//-----------------------------------------------------------------------------
// File: NameGroupBox.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 16.01.2011
//
// Description:
// This class provides 3 GUI items to set a name, displayName and description for an element in IP-Xact.
//-----------------------------------------------------------------------------

#ifndef NAMEGROUPBOX_H_
#define NAMEGROUPBOX_H_

#include <QWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QString>
#include <QValidator>

//-----------------------------------------------------------------------------
//! This class provides 3 GUI items to set a name, displayName and description for an element in IP-Xact.
//-----------------------------------------------------------------------------
class NameGroupBox: public QGroupBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of this widget.
	 */
	NameGroupBox(QWidget* parent = 0, const QString& title = QString("Names"));

	//! The destructor.
	virtual ~NameGroupBox();

	/*!
     *  Get the value of the name-field.
	 *
	 *      @return QString containing the name.
	 */
	QString name() const;

	/*!
     *  Get the value of the displayName-field.
	 *
	 *      @return QString containing the displayName.
	 */
	QString displayName() const;

	/*!
     *  Get the value of the description-field.
	 *
	 *      @return QString containing the description.
	 */
	QString description() const;

	/*!
     *  Check if the widget is in valid mode.
	 *
     *  This function checks that name-field is not empty and does not contain string with embedded whitespace 
     *  characters.
	 *
	 *      @return True if everything is ok.
	 */
	bool isValid() const;

public slots:

	/*!
     *  Set the value for the name-field.
     *
     *      @param [in] name    QString containing the name.
	 */
	void setName(const QString& name);

	/*!
     *  Set the value for the displayName-field.
	 *
	 *      @param [in] displayName     QString containing the displayName.
	 */
	void setDisplayName(const QString& displayName);

	/*!
     *  Set the value for the description-field.
     *
     *      @param [in] description     QString containing the description.
	 */
	void setDescription(const QString& description);

signals:

	//! Emitted when contents of the widget change.
	void contentChanged();

	/*!
     *  Emitted when the name-field changes.
	 */
    void nameChanged();

	/*!
     *  Emitted when the display name-field changes.
	 */
    void displayNameChanged();

	/*!
     *  Emitted when the description-field changes.
	 */
    void descriptionChanged();

private:

	//! No copying
	NameGroupBox(const NameGroupBox& other);

	//! No assignment
	NameGroupBox& operator=(const NameGroupBox& other);

	//! Line edit to set the name.
	QLineEdit* nameEdit_;

	//! Line edit to set the displayName.
	QLineEdit* displayNameEdit_;

	//! Editor to write the description.
	QPlainTextEdit* descriptionEdit_;
};

#endif /* NAMEGROUPBOX_H_ */
