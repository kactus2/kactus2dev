//-----------------------------------------------------------------------------
// File: typegroup.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.07.2011
//
// Description:
// Contains the check boxes to set the type options for VLNVDialer.
//-----------------------------------------------------------------------------

#ifndef TYPEGROUP_H
#define TYPEGROUP_H

#include <KactusAPI/include/utils.h>

#include <QGroupBox>
#include <QCheckBox>

//-----------------------------------------------------------------------------
//! Contains the check boxes to set the type options for VLNVDialer.
//-----------------------------------------------------------------------------
class TypeGroup : public QGroupBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *      @param [in] parent The owner of this widget.
	 */
	TypeGroup(QWidget *parent);
	
	/*!
     *  The destructor
     */
	virtual ~TypeGroup();

    /*!
     *  Sets new type options.
     *
     *      @param [in] options   The options to set.
     */
    void setTypes(Utils::TypeOptions options);

    /*!
     *  Gets the selected type options.
     *
     *      @return Selected type options.
     */
    Utils::TypeOptions getTypes() const;

signals:

	/*!
     *  Emitted when type settings change.
     */
	void optionsChanged(const Utils::TypeOptions& options);

private slots:

	/*!
     *  Handler for componentBox state changes.
     */
    void onComponentChange(bool checked);

	/*!
     *  Handler for busApiBox state changes.
     */
    void onBusChange(bool checked);

    /*!
     *  Handler for catalog state changes.
     */
    void onCatalogChange(bool checked);

    /*!
     *  Handler for Api/Com state changes.
     */
    void onApiComChange(bool checked);

    /*!
     *  Handler for advancedBox state changes.
     */
    void onAdvancedChange(bool checked);

private:
	//! No copying. No assignment.
	TypeGroup(const TypeGroup& other);
	TypeGroup& operator=(const TypeGroup& other);

	//! Check box to select components in/out of search results.
	QCheckBox componentBox_;

	//! Check box to select buses/APIs in/out of search results.
	QCheckBox busBox_;

    //! Check box to select buses/APIs in/out of search results.
    QCheckBox catalogBox_;

    //! Check box to select buses/APIs in/out of search results.
    QCheckBox apiComBox_;

	//! Check box to select other IP-Xact types in/out of search results.
	QCheckBox advancedBox_;

	//! Contains the current search settings for type.
	Utils::TypeOptions options_;
};

#endif // TYPEGROUP_H
