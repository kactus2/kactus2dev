/* 
 *  	Created on: 21.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacegeneraleditor.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSSPACEGENERALEDITOR_H
#define ADDRESSSPACEGENERALEDITOR_H

#include <IPXACTmodels/addressspace.h>
#include <common/widgets/NumberLineEdit/numberlineedit.h>

#include <QGroupBox>
#include <QSharedPointer>

/*! Editor to set the general settings of an address space.
 * 
 * The editable fields are:
 * - size of the addressable unit
 * - range of the address space
 * - width of the address space
 */
class AddressSpaceGeneralEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *       @param [in] addrSpace  The address space being edited.
	 *       @param [in] parent     The owner of the editor.
	*/
	AddressSpaceGeneralEditor(QSharedPointer<AddressSpace> addrSpace, QWidget *parent);
	
	//! The destructor
	virtual ~AddressSpaceGeneralEditor();

	/*! Check if the editor is in valid state and can be saved.
	 *
	 *      @return True if everything is ok.
	*/
	bool isValid() const;

	/*! Read the settings from the address space model to the editor fields.
	 *
	*/
	void refresh();

signals:

	//! Emitted when the contents of one of the editor fields changes.
	void contentChanged();

    //! Emitted when the change in fields should be reflected in the visualization.
    void graphicsChanged();

	/*! Emitted when user changes the size of an addressable unit.
	 *
	 *       @param units The new addressable unit size.
	*/
	void addressableUnitsChanged(int units);

	/*! Emitted when user changes the width of a row in address space.
	 *
	 *       @param width The new width of a row.
	*/
	void widthChanged(int width);

	/*! Emitted when user changes the range of an address space.
	 *
	 *       @param range The new range as number of addressable units.
	*/
	void rangeChanged(QString const& range);

private slots:

	//! Handler for changes in addressable unit.
    void onAddrUnitChanged();

	//! Handler for changes in width.
    void onWidthChanged();

	//! Handler for changes in range.
    void onRangeChanged();

private:
	//! No copying
	AddressSpaceGeneralEditor(const AddressSpaceGeneralEditor& other);
	AddressSpaceGeneralEditor& operator=(const AddressSpaceGeneralEditor& other);

	//! The address space being edited.
	QSharedPointer<AddressSpace> addrSpace_;

	//! Editor to set the size of an addressable unit.
	NumberLineEdit addrUnitEditor_;

    //! Editor to set the range of the address space.
    NumberLineEdit rangeEditor_;

	//! Editor to set the width of one row in address space.
	NumberLineEdit widthEditor_;
};

#endif // ADDRESSSPACEGENERALEDITOR_H
