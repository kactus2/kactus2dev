//-----------------------------------------------------------------------------
// File: vlnvdisplayer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.03.2011
//
// Description:
// VLNVDisplayer is a widget to display a vlnv tag in a GUI item.
//-----------------------------------------------------------------------------

#ifndef VLNVDISPLAYER_H
#define VLNVDISPLAYER_H

#include <IPXACTmodels/common/VLNV.h>

#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>

//-----------------------------------------------------------------------------
//! VLNVDisplayer is a widget to display a vlnv tag in a GUI item.
//-----------------------------------------------------------------------------
class VLNVDisplayer : public QGroupBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of this widget.
	 *      @param [in] vlnv    Reference to the vlnv to be displayed
	 */
	VLNVDisplayer(QWidget *parent, const VLNV& vlnv, bool compact = false);

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of this widget.
	 *      @param [in] vlnv    Pointer to the vlnv to be displayed
	 */
	VLNVDisplayer(QWidget *parent, const VLNV* vlnv, bool compact = false);

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  Pointer to the owner of this widget.
	 */
	VLNVDisplayer(QWidget* parent);

	//! The destructor.
	virtual ~VLNVDisplayer();

	/*!
     *  Set the vlnv to be displayed in the widget.
	 *
	 *      @param [in] vlnv    Reference to the VLNV to be displayed
	 */
	void setVLNV(const VLNV& vlnv, bool compact = false);

	/*!
     *  Also display path along with the vlnv in the widget.
	 *
	 *      @param [in] path    The path to display
	 */
	void setPath(const QString& path);

private:

	//! No copying.
	VLNVDisplayer(const VLNVDisplayer& other);

	//! No assignment.
	VLNVDisplayer& operator=(const VLNVDisplayer& other);

    //! Title labels for each field.
    QLabel vendorLabel_;
    QLabel libraryLabel_;
    QLabel nameLabel_;
    QLabel versionLabel_;

	//! The label to display the vendor.
	QLabel vendor_;

	//! The label to display the library.
	QLabel library_;

	//! The label to display the name.
	QLabel name_;

	//! The label to display the version.
	QLabel version_;
	
	//! The layout of the widget (non-compact).
	QGridLayout* layout_;

    //! Compact layout.
    QHBoxLayout* compactLayout_;
};

#endif // VLNVDISPLAYER_H
