/* 
 *
 *  Created on: 13.1.2011
 *      Author: Antti Kamppi
 */

#ifndef VLNVGENERATOR_H_
#define VLNVGENERATOR_H_

#include <library/LibraryManager/vlnv.h>

#include <QGroupBox>
#include <QLineEdit>

/*! \brief VLNVGenerator is used to create/edit a VLNV for IP-Xact document.
 *
 */
class VLNVGenerator: public QGroupBox {

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget
	 */
	VLNVGenerator(QWidget* parent = 0);

	//! \brief The destructor
	virtual ~VLNVGenerator();

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

	/*! \brief Set the VLNV to be displayed in the GUI.
	 *
	 * \param vlnv Pointer to the vlnv to be displayed/edited.
	 */
	void setVLNV(const VLNV* vlnv);

	/*! \brief Set the VLNV to be displayed in the GUI.
	 *
	 * \param vlnv Reference to the vlnv to be displayed/edited.
	 */
	void setVLNV(const VLNV& vlnv);

signals:

	//! \brief Emitted when contents of the widget change
	void contentChanged();

private:

	//! No copying
	VLNVGenerator(const VLNVGenerator& other);

	//! No assignment
	VLNVGenerator& operator=(const VLNVGenerator& other);

	//! \brief Line edit for the vendor element
	QLineEdit* vendorEdit_;

	//! \brief Line edit for the library element
	QLineEdit* libraryEdit_;

	//! \brief Line edit for the name element
	QLineEdit* nameEdit_;

	//! \brief Line edit for the version element
	QLineEdit* versionEdit_;
};

#endif /* VLNVGENERATOR_H_ */
