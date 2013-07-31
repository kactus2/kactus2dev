/* 
 *
 *  Created on: 20.6.2011
 *      Author: Antti Kamppi
 * 		filename: newbuspage.h
 */

#ifndef NEWBUSPAGE_H
#define NEWBUSPAGE_H

#include <common/dialogs/propertyPageDialog/PropertyPageView.h>
#include <common/widgets/LibraryPathSelector/librarypathselector.h>

#include <LibraryManager/libraryinterface.h>
#include <common/widgets/vlnvEditor/vlnveditor.h>

#include <QPushButton>

class NewBusPage : public PropertyPageView {
	Q_OBJECT

public:

	NewBusPage(LibraryInterface* libHandler, QWidget *parent);
	
	//! \brief The destructor
	virtual ~NewBusPage();

	/*!
     *  Pre-validates the contents of the page. This is used for enabling/disabling the OK button.
     *
     *      @return True, if the contents are valid and OK button should be enabled.
     *              False, if the contents are invalid and OK button should be disabled.
     *
     *      @remarks Must not show any message boxes.
     */
    virtual bool prevalidate() const;

    /*!
     *  Validates the contents of the page thoroughly.
     *
     *      @return True, if the contents are valid. False, if they are invalid.
     *
     *      @remarks Showing message boxes for errors is allowed.
     */
    virtual bool validate();

    /*!
     *  Applies the changes that were done in the page.
     */
    virtual void apply();

    /*!
     *  Called when the page is to be changed and this page would be hidden.
     *
     *      @return False, if the page change should be rejected. Otherwise true.
     */
    virtual bool onPageChange();

public slots:

    /*!
     *  Updates the directory based on the VLNV.
     */
    void updateDirectory();

    //! Called when the user clicks browse button.
    void onBrowse();

signals:

	//! \brief Signaled when new bus should be created.
	void createBus(VLNV const& vlnv, QString const& directory);

private:
	//! \brief No copying
	NewBusPage(const NewBusPage& other);

	//! \brief No assignment
	NewBusPage& operator=(const NewBusPage& other);	

	//! The library interface.
	LibraryInterface* libInterface_;

	//! VLNV editor.
	VLNVEditor vlnvEditor_;

	//! \brief The editor to select the directory to save to. 
	LibraryPathSelector directoryEdit_;

    //! Pushbutton for browsing the save directory.
    QPushButton* browseButton_;

    //! Flag for indicating whether the directory has been chosen using browse.
    bool directorySet_;
};

#endif // NEWBUSPAGE_H
