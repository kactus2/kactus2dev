//-----------------------------------------------------------------------------
// File: SegmentComboBox.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.02.2022
//
// Description:
// The combo box to select one of the defined segments.
//-----------------------------------------------------------------------------

#ifndef SEGMENTCOMBOBOX_H
#define SEGMENTCOMBOBOX_H

#include <QComboBox>

class AddressSpace;
class Component;

//-----------------------------------------------------------------------------
//! The combo box to select one of the defined segments.
//-----------------------------------------------------------------------------
class SegmentComboBox : public QComboBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      param [in] parent   Pointer to the owner of the combo box.
	 */
    SegmentComboBox(QWidget *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~SegmentComboBox() = default;

    /*!
     *  Setup the combo box.
     *
     *      @param [in] containingSpace     Address space containing the available segments.
     *      @param [in] segmentReference    Currently selected segment.
     */
    void setupSelection(QSharedPointer<AddressSpace> containingSpace, QString const& segmentReference = "");

private:
	
    //! No copying.	No assignment.
    SegmentComboBox(const SegmentComboBox& other);
    SegmentComboBox& operator=(const SegmentComboBox& other);
};

#endif // SEGMENTCOMBOBOX_H
