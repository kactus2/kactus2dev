/* 
 *  	Created on: 21.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacegeneraleditor.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSSPACEGENERALEDITOR_H
#define ADDRESSSPACEGENERALEDITOR_H

#include <models/addressspace.h>
#include <common/widgets/NumberLineEdit/numberlineedit.h>

#include <QSpinBox>
#include <QGroupBox>

/*! \brief Editor to set the general settings of an address space.
 * 
 * The editable fields are:
 * - size of the addressable unit
 * - range of the address space
 * - width of the address space
 */
class AddressSpaceGeneralEditor : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param addrSpace Pointer to the address space being edited.
	 * \param parent Pointer to the owner of the editor.
	 *
	*/
	AddressSpaceGeneralEditor(AddressSpace* addrSpace, QWidget *parent);
	
	//! \brief The destructor
	virtual ~AddressSpaceGeneralEditor();

	/*! \brief Check if the editor is in valid state and can be saved.
	 *
	 * \return True if everything is ok.
	*/
	bool isValid() const;

	/*! \brief Save the settings from the editor fields to the address space model
	 *
	*/
	void makeChanges();

	/*! \brief Read the settings from the address space model to the editor fields.
	 *
	*/
	void restoreChanges();

signals:

	//! \brief Emitted when the contents of one of the editor fields changes.
	void contentChanged();

	/*! \brief Emitted when user changes the size of an addressable unit.
	 *
	 * \param units The new addressable unit size.
	 *
	*/
	void addressableUnitsChanged(int units);

	/*! \brief Emitted when user changes the width of a row in address space.
	 *
	 * \param width The new width of a row.
	 *
	*/
	void widthChanged(int width);

	/*! \brief Emitted when user changes the range of an address space.
	 *
	 * \param range The new range as number of addressable units.
	 *
	*/
	void rangeChanged(const QString& range);

private:
	//! \brief No copying
	AddressSpaceGeneralEditor(const AddressSpaceGeneralEditor& other);

	//! \brief No assignment
	AddressSpaceGeneralEditor& operator=(const AddressSpaceGeneralEditor& other);

	//! \brief Pointer to the address space being edited.
	AddressSpace* addrSpace_;

	//! \brief Editor to set the size of an addressable unit.
	QSpinBox addrUnit_;

	//! \brief Editor to set the width of one row in address space.
	QSpinBox width_;

	//! \brief Editor to set the range of the address space.
	NumberLineEdit range_;
};

#endif // ADDRESSSPACEGENERALEDITOR_H
