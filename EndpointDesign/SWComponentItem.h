//-----------------------------------------------------------------------------
// File: SWComponentItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.11.2011
//
// Description:
// Base class for all SW components visualized in a system design.
//-----------------------------------------------------------------------------

#ifndef SWCOMPONENTITEM_H
#define SWCOMPONENTITEM_H

#include <common/graphicsItems/ComponentItem.h>

//-----------------------------------------------------------------------------
//! SWComponentItem class.
//-----------------------------------------------------------------------------
class SWComponentItem : public ComponentItem
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] size                      The initial rectangle size.
     *      @param [in] instanceName              The name of the component instance.
     *      @param [in] displayName               The component instance's display name.
     *      @param [in] description               The component instance's description.
     *      @param [in] configurableElementValue  The component instance's configurable element values.
     *      @param [in] parent                    The parent graphics item.
     */
    SWComponentItem(QRectF const& size,
                    QSharedPointer<Component> component,
                    QString const& instanceName = QString("unnamed"),
                    QString const& displayName = QString(),
                    QString const& description = QString(),
                    QMap<QString, QString> const& configurableElementValues = QMap<QString, QString>(),
                    QGraphicsItem *parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~SWComponentItem();

    /*!
     *  Sets the flag whether the component has been imported or not.
     *
     *      @param [in] imported If true, the component is marked as imported.
     */
    void setImported(bool imported);

    /*!
     *  Returns true if the component has been marked as imported.
     */
    bool isImported() const;

private:
    // Disable copying.
    SWComponentItem(SWComponentItem const& rhs);
    SWComponentItem& operator=(SWComponentItem const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The boolean flag for imported property.
    bool imported_;
};

//-----------------------------------------------------------------------------

#endif // SWCOMPONENTITEM_H
