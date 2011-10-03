/* 
 *
 *  Created on: 20.12.2010
 *      Author: Antti Kamppi
 */

#ifndef VLNVBOX_H_
#define VLNVBOX_H_

#include <QGroupBox>
#include <QWidget>
#include <QObject>
#include <QCheckBox>

/*! \brief The widget for user to select which vlnv tags to include in search.
 *
 */
class VLNVBox: public QGroupBox {

	Q_OBJECT

public:

	//! \brief struct that contains the info on which vlnv tags to include in search.
	struct VLNVSearchSettings {

		//! \brief include vendor-field.
		bool vendor_;

		//! \brief include library-field.
		bool library_;

		//! \brief include name-field.
		bool name_;

		//! \brief include version-field.
		bool version_;

		/*! \brief The struct constructor
		 *
		 * \param vendor setting for vendors
		 * \param library setting for libraries
		 * \param name setting for names
		 * \param version setting for versions
		 *
		*/
		VLNVSearchSettings(bool vendor, bool library, bool name, bool version);

		/*! \brief The default constructor.
		 *
		 * Constructs struct with all fields set to TRUE.
		*/
		VLNVSearchSettings();
	};

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	VLNVBox(QWidget* parent);

	//! \brief The destructor
	virtual ~VLNVBox();

public slots:

	//! \brief Handler when the state of one of the check boxes changes state.
	void onCheckStateChanged();

signals:

	//! \brief Inform that the state of the filter should change
	void stateChanged(const VLNVBox::VLNVSearchSettings* settings);

private:

	//! \brief No Copying
	VLNVBox(const VLNVBox& other);

	//! \brief No assignment
	VLNVBox& operator=(const VLNVBox& other);

	//! \brief CheckBox to select/deselect vendors within search
	QCheckBox searchVendors_;

	//! \brief CheckBox to select/deselect libraries within search
	QCheckBox searchLibraries_;

	//! \brief CheckBox to select/deselect names within search
	QCheckBox searchNames_;

	//! \brief CheckBox to select/deselect versions within search
	QCheckBox searchVersions_;

	//! \brief Contains the search settings for this group
	VLNVSearchSettings settings_;
};

#endif /* VLNVBOX_H_ */
