//-----------------------------------------------------------------------------
// File: ViewReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.09.2015
//
// Description:
// Reader class for IP-XACT view element.
//-----------------------------------------------------------------------------

#ifndef VIEWREADER_H
#define VIEWREADER_H

#include "View.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT view element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ViewReader : public QObject
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    ViewReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~ViewReader();

    /*!
     *  Creates a new view from a given view node.
     *
     *      @param [in] viewNode    XML description of the view.
     *
     *      @return The created view.
     */
    QSharedPointer<View> createViewFrom(QDomNode const& viewNode) const;

private:

    //! No copying allowed.
    ViewReader(ViewReader const& rhs);
    ViewReader& operator=(ViewReader const& rhs);

    /*!
     *  Reads the name group.
     *
     *      @param [in] viewNode    XML description of the view.
     *      @param [in] newView     The new view item.
     */
    void parseNameGroup(QDomNode const& viewNode, QSharedPointer<View> newView) const;

    /*!
     *  Reads the presence.
     *
     *      @param [in] viewNode    XML description of the view.
     *      @param [in] newView     The new view item.
     */
    void parseIsPresent(QDomNode const& viewNode, QSharedPointer<View> newView) const;

    /*!
     *  Reads the envIdentifiers.
     *
     *      @param [in] viewNode    XML description of the view.
     *      @param [in] newView     The new view item.
     */
    void parseEnvIdentifiers(QDomNode const& viewNode, QSharedPointer<View> newView) const;

    /*!
     *  Reads the instantiation references.
     *
     *      @param [in] viewNode    XML description of the view.
     *      @param [in] newView     The new view item.
     */
    void parseInstantiationRefs(QDomNode const& viewNode, QSharedPointer<View> newView) const;
};

#endif // VIEWREADER_H