//-----------------------------------------------------------------------------
// File: PlatformComponentItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 26.9.2011
//
// Description:
// Graphics item for representing SW platform components.
//-----------------------------------------------------------------------------

#ifndef PLATFORMCOMPONENTITEM_H
#define PLATFORMCOMPONENTITEM_H

#include <common/graphicsItems/ComponentItem.h>

//-----------------------------------------------------------------------------
//! PlatformComponentItem class.
//-----------------------------------------------------------------------------
class PlatformComponentItem : public ComponentItem
{
    Q_OBJECT 

public:
    enum { Type = UserType + 14 };

    /*!
     *  Constructor.
     */
    PlatformComponentItem(QSharedPointer<Component> component,
                          QString const& instanceName = QString("unnamed"),
                          QString const& displayName = QString(),
                          QString const& description = QString(),
                          QMap<QString, QString> const& configurableElementValues = QMap<QString, QString>(),
                          QGraphicsItem *parent = 0);

	/*!
     *  Destructor.
     */
	virtual ~PlatformComponentItem();

    /*!
     *  Updates the diagram component to reflect the current state of the component model.
     */
    virtual void updateComponent();

    int type() const { return Type; }

private:
    // Disable copying.
    PlatformComponentItem(PlatformComponentItem const& rhs);
    PlatformComponentItem& operator=(PlatformComponentItem const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Icon for hierarchical platform components.
    QGraphicsPixmapItem* hierIcon_;
};

//-----------------------------------------------------------------------------

#endif // PLATFORMCOMPONENTITEM_H
