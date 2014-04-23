/* 
 *
 *  Created on: 13.1.2011
 *      Author: Antti Kamppi
 */

#ifndef LIBRARYREFGENERATOR_H_
#define LIBRARYREFGENERATOR_H_

#include <library/LibraryManager/vlnv.h>

#include <QGroupBox>
#include <QLineEdit>
#include <QString>
#include <QCheckBox>

/*! \brief LibraryRefGenerator is used to create a VLNV reference.
 *
 * This function is used to create references in IP-Xact that are based in
 * VLNV. This can be used to control spirit:libraryRefType in IP-Xact.
 */
class LibraryRefGenerator: public QGroupBox {

public:

	/*! \brief The constructor
	 *
	 * \param title Title for the group box
	 * \param parent Pointer to the owner of this group box
	 * \param enableBox Specifies if an enable checkBox is to be added to the
	 * widget so widget can be disabled/enabled.
	 */
	LibraryRefGenerator(const QString title, QWidget* parent = 0,
			bool enableBox = true);

	//! \brief The destructor
	virtual ~LibraryRefGenerator();

	/*! \brief Tells the caller if the dialog is activated or not.
	 *
	 * \return True if the dialog was activated and it's contents should be
	 * used. False is inactive and contents are irrelevant.
	 */
	bool isEnabled() const;

	/*! \brief Generate a vlnv that matches the user inputs
	 *
	 * If one of the vlnv elements is empty then a null pointer is returned.
	 * This class gives the ownership of the generated VLNV to the caller of
	 * this function.
	 *
	 * \param type Type of the vlnv that is to be generated
	 *
	 * \return Pointer to the VLNV instance.
	 */
	VLNV* getVLNV(VLNV::IPXactType type);

	/*! \brief Is the given VLNV information valid.
	 *
	 * If one of the fields is empty then the VLNV can not be created.
	 *
	 * \return True if all VLNV fields are ok, false if not.
	 */
	bool isValid();

	/*! \brief Set the vlnv to be displayed/edited.
	 *
	 * \param vlnv Pointer to the vlnv.
	 */
	void setVLNV(const VLNV* vlnv);

	/*! \brief Set the vlnv to be displayed/edited.
	 *
	 * \param vlnv The vlnv to be displayed/edited.
	 *
	*/
	void setVLNV(const VLNV& vlnv);

signals:

	//! \brief Emitted when contents of the widget change.
	void contentChanged();

private:

	//! No copying
	LibraryRefGenerator(const LibraryRefGenerator& other);

	//! No assignment
	LibraryRefGenerator& operator=(const LibraryRefGenerator& other);

	//! \brief Line edit for the vendor element
	QLineEdit* vendorEdit_;

	//! \brief Line edit for the library element
	QLineEdit* libraryEdit_;

	//! \brief Line edit for the name element
	QLineEdit* nameEdit_;

	//! \brief Line edit for the version element
	QLineEdit* versionEdit_;

	//! \brief Is the reference used or not
	QCheckBox* enableCheckBox_;
};

#endif /* LIBRARYREFGENERATOR_H_ */
