//-----------------------------------------------------------------------------
// File: PowerDomainLinkEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 19.10.2023
//
// Description:
// 
//-----------------------------------------------------------------------------

#ifndef POWERDOMAINLINKEDITOR_H
#define POWERDOMAINLINKEDITOR_H

#include <common/views/EditableTableView/editabletableview.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include "PowerDomainLinkDelegate.h"
#include "PowerDomainLinkModel.h"

#include <QSharedPointer>
#include <QGroupBox>

class Parameter;

//-----------------------------------------------------------------------------
//! Container class for configurable elements in the configurable element value tables.
//-----------------------------------------------------------------------------
class PowerDomainLinkEditor : public QGroupBox
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] parent      The parent widget.
	 */
    explicit PowerDomainLinkEditor(QWidget* parent);
	
	/*!
	 *  The destructor.
	 */
    ~PowerDomainLinkEditor() final = default;

    //! No copying. No assignment.
    PowerDomainLinkEditor(const PowerDomainLinkEditor& other) = delete;
    PowerDomainLinkEditor& operator=(const PowerDomainLinkEditor& other) = delete;

	void setContent(ComponentInstance::PowerDomainLink::List powerDomains,
		QSharedPointer<Component> externalComponent, QSharedPointer<Component> internalComponent) const noexcept;

signals:

	void contentChanged();

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------


	EditableTableView* view_ = new EditableTableView(this);

	PowerDomainLinkModel* model_ = new PowerDomainLinkModel(this);

	PowerDomainLinkDelegate* delegate_ = new PowerDomainLinkDelegate(this);

	void setupLayout();
};

#endif // POWERDOMAINLINKEDITOR_H
