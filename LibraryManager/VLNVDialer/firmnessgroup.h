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

/*! \brief Contains the check boxes to set the re-usability options for VLNVDialer.
 *
 */
class FirmnessGroup : public QGroupBox {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	FirmnessGroup(QWidget *parent);
	
	//! \brief The destructor
	virtual ~FirmnessGroup();

signals:

	//! \brief Emitted when one of the check boxes changes state.
	void optionsChanged(const Utils::FirmnessOptions& options);

private slots:

	//! \brief Handler for template check box state changes.
	void onTemplateChanged(int state);

	//! \brief Handler for block check box state changes.
	void onBlockChanged(int state);

	//! \brief Handler for integration check box state changes.
	void onIntegrationChanged(int state);

	//! \brief Handler for configuration check box state changes.
	void onConfigurationChanged(int state);

private:
	//! \brief No copying
	FirmnessGroup(const FirmnessGroup& other);

	//! \brief No assignment
	FirmnessGroup& operator=(const FirmnessGroup& other);
	
	//! \brief Check box to select templates in/out of search results.
	QCheckBox templateBox_;

	//! \brief Check box to select mutable in/out of search results.
	QCheckBox mutableBox_;

	//! \brief Check box to select parameterizable in/out of search results.
	//QCheckBox parameterizableBox_;

	//! \brief Check box to select fixed in/out of search results.
	QCheckBox fixedBox_;

	//! \brief Contains the current re-usability settings.
	Utils::FirmnessOptions options_;
};

#endif // FIRMNESSGROUP_H
