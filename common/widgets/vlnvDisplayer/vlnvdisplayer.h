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
#include <QFormLayout>
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
	 *      @param [in] parent      Pointer to the owner of this widget.
	 *      @param [in] vlnv        Reference to the vlnv to be displayed
     *      @param [in] compact     A compact VLNV display takes less space.
	 */
    VLNVDisplayer(QWidget *parent, VLNV const& vlnv, bool compact = false);

	/*!
     *  The destructor.
     */
	virtual ~VLNVDisplayer();

	/*!
     *  Set the vlnv to be displayed in the widget.
	 *
	 *      @param [in] vlnv    Reference to the VLNV to be displayed
	 */
    void setVLNV(const VLNV& vlnv);

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

    /*!
     *  Setup the VLNV layout.
     */
    void setupLayout();

    /*!
     *  Setup a compact VLNV layout.
     */
    void setupCompactLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

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
	QFormLayout* layout_;

    //! Compact layout.
    QHBoxLayout* compactLayout_;

    //! Label containing the folder path of the displayed VLNV item.
    QLabel* pathLabel_;

    //! Compact VLNV display takes less screen space.
    bool compact_;
};

#endif // VLNVDISPLAYER_H
