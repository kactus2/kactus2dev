//-----------------------------------------------------------------------------
// File: ApplicationItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.9.2011
//
// Description:
// Graphics item for representing SW application components.
//-----------------------------------------------------------------------------

#ifndef APPLICATIONITEM_H
#define APPLICATIONITEM_H

#include "SWComponentItem.h"

class ProgramEntityItem;

//-----------------------------------------------------------------------------
//! ApplicationItem class.
//-----------------------------------------------------------------------------
class ApplicationItem : public SWComponentItem
{
    Q_OBJECT 

public:
    enum { Type = UserType + 17 };

    /*!
     *  Constructor.
     *
     *      @param [in] instanceName              The name of the component instance.
     *      @param [in] displayName               The component instance's display name.
     *      @param [in] description               The component instance's description.
     *      @param [in] configurableElementValue  The component instance's configurable element values.
     *      @param [in] parent                    The parent graphics item.
     */
    ApplicationItem(QSharedPointer<Component> component,
                          QString const& instanceName = QString("unnamed"),
                          QString const& displayName = QString(),
                          QString const& description = QString(),
                          QMap<QString, QString> const& configurableElementValues = QMap<QString, QString>(),
                          QGraphicsItem *parent = 0);

	/*!
     *  Destructor.
     */
	virtual ~ApplicationItem();

    /*!
     *  Updates the diagram component to reflect the current state of the component model.
     */
    virtual void updateComponent();

    int type() const { return Type; }

public slots:
    /*!
     *  Opens the application source file.
     */
    void openSource();

signals:
    //! Emitted when the application source file should be opened in editor.
    void openSource(ProgramEntityItem* progEntity);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);

private:
    // Disable copying.
    ApplicationItem(ApplicationItem const& rhs);
    ApplicationItem& operator=(ApplicationItem const& rhs);
};

//-----------------------------------------------------------------------------

#endif // APPLICATIONITEM_H
