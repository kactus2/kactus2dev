/* 
 *  	Created on: 11.7.2011
 *      Author: Antti Kamppi
 * 		filename: firmnessgroup.h
 *		Project: Kactus 2
 */

#ifndef FIRMNESSGROUP_H
#define FIRMNESSGROUP_H

#include <common/utils.h>

#include <QGroupBox>
#include <QCheckBox>

/*! Contains the check boxes to set the re-usability options for VLNVDialer.
 *
 */
class FirmnessGroup : public QGroupBox {
	Q_OBJECT

public:

	/*! The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	FirmnessGroup(QWidget *parent);
	
	//! The destructor
	virtual ~FirmnessGroup();

    /*!
     *  Sets new firmness options.
     *
     *      @param [in] options   The options to set.
     */
    void setFirmness(Utils::FirmnessOptions options);

    /*!
     *  Gets the selected firmness options.
     *
     *      @return Selected firmness options.
     */
    Utils::FirmnessOptions getFirmness() const;

signals:

	//! Emitted when one of the check boxes changes state.
	void optionsChanged(const Utils::FirmnessOptions& options);

private slots:

	//! Handler for template check box state changes.
	void onTemplateChanged(bool checked);

	//! Handler for mutable check box state changes.
	void onMutableChanged(bool checked);

	//! Handler for configuration check box state changes.
	void onConfigurationChanged(bool checked);

    //! Handler for definitions check box state changes.
    void onDefinitionChanged(bool checked);

private:
	//! No copying
	FirmnessGroup(const FirmnessGroup& other);

	//! No assignment
	FirmnessGroup& operator=(const FirmnessGroup& other);

	//! Check box to select templates in/out of search results.
	QCheckBox templateBox_;

	//! Check box to select mutable in/out of search results.
	QCheckBox mutableBox_;

	//! Check box to select fixed in/out of search results.
	QCheckBox fixedBox_;

    //! Check box to select definitions in/out of search results.
    QCheckBox definitionsBox_;

	//! Contains the current re-usability settings.
	Utils::FirmnessOptions options_;
};

#endif // FIRMNESSGROUP_H
