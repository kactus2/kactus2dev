/* 
 *
 *  Created on: 11.3.2011
 *      Author: Antti Kamppi
 * 		filename: vlnvdisplayer.h
 */

#ifndef VLNVDISPLAYER_H
#define VLNVDISPLAYER_H

#include <library/LibraryManager/vlnv.h>

#include <QGroupBox>
#include <QLabel>
#include <QSizePolicy>
#include <QGridLayout>
#include <QHBoxLayout>

/*! \brief VLNVDisplayer is a widget to display a vlnv tag in a GUI item.
 *
 */
class VLNVDisplayer : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 * \param vlnv Reference to the vlnv to be displayed
	 *
	*/
	VLNVDisplayer(QWidget *parent, const VLNV& vlnv, bool compact = false);

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 * \param vlnv Pointer to the vlnv to be displayed
	 *
	*/
	VLNVDisplayer(QWidget *parent, const VLNV* vlnv, bool compact = false);

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	VLNVDisplayer(QWidget* parent);

	//! \brief The destructor
	virtual ~VLNVDisplayer();

	/*! \brief Set the vlnv to be displayed in the widget
	 *
	 * \param vlnv Reference to the VLNV to be displayed
	 *
	*/
	void setVLNV(const VLNV& vlnv, bool compact = false);

	/*! \brief Also display path along with the vlnv in the widget.
	 *
	 * \param path The path to display
	 *
	*/
	void setPath(const QString& path);

private:

	//! \brief No copying
	VLNVDisplayer(const VLNVDisplayer& other);

	//! No assignment
	VLNVDisplayer& operator=(const VLNVDisplayer& other);

    //! Title labels for each field.
    QLabel vendorLabel_;
    QLabel libraryLabel_;
    QLabel nameLabel_;
    QLabel versionLabel_;

	//! \brief The label to display the vendor
	QLabel vendor_;

	//! \brief The label to display the library
	QLabel library_;

	//! \brief The label to display the name
	QLabel name_;

	//! \brief The label to display the version
	QLabel version_;
	
	//! \brief The layout of the widget (non-compact)
	QGridLayout* layout_;

    //! Compact layout.
    QHBoxLayout* compactLayout_;
	
};

#endif // VLNVDISPLAYER_H
