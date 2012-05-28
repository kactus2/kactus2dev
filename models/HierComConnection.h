//-----------------------------------------------------------------------------
// File: HierComConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.5.2012
//
// Description:
// Hierarchical COM dependency.
//-----------------------------------------------------------------------------

#ifndef HIERCOMCONNECTION_H
#define HIERCOMCONNECTION_H

#include "ComConnection.h"

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QPointF>
#include <QVector2D>

//-----------------------------------------------------------------------------
//! Class encapsulating COM dependency connection data.
//-----------------------------------------------------------------------------
class HierComConnection
{
public:
    /*!
     *  Default constructor.
     */
    HierComConnection();

    /*!
     *  Constructor which sets all the values as given.
     *
     *      @param [in] name          The name to set.
     *      @param [in] displayName   The display name to set.
     *      @param [in] description   The description to set.
     *      @param [in] interfaceRef  Name reference to an COM interface in the top-level component.
     *      @param [in] ref           The interface reference to a contained SW component instance.
     *      @param [in] position      The position of the top-level interface in the design diagram.
     *      @param [in] direction     The direction of the top-level interface in the design diagram.
     *      @param [in] route         The connection route.
     */
    HierComConnection(QString const& name, QString const& displayName, QString const& description,
                      QString const& interfaceRef, ComInterfaceRef const& ref,
                      QPointF const& position, QVector2D const& direction,
                      QList<QPointF> const& route);

    /*!
     *  Copy constructor.
     */
    HierComConnection(HierComConnection const& rhs);

    /*!
     *  Constructor which reads the COM dependency from an XML node.
     *
     *      @param [in] node The source XML node.
     */
    HierComConnection(QDomNode& node);

    /*!
     *  Destructor.
     */
    ~HierComConnection();

    /*!
     *  Writes the contents to an XML stream.
     */
    void write(QXmlStreamWriter& writer) const;

    /*!
     *  Sets the name of the dependency.
     *
     *      @param [in] name The name to set.
     */
    void setName(QString const& name);

    /*!
     *  Sets the display name of the dependency.
     *
     *      @param [in] displayName The display name to set.
     */
    void setDisplayName(QString const& displayName);

    /*!
     *  Sets the description of the dependency.
     *
     *      @param [in] description The description to set.
     */
    void setDescription(QString const& description);

    /*!
     *  Sets the top-level COM interface reference.
     *
     *      @param [in] interfaceRef Name reference to an COM interface in the top-level component.
     */
    void setInterfaceRef(QString const& interfaceRef);

    /*!
     *  Sets the interface reference to an COM interface in a contained SW component instance.
     *
     *      @param [in] ref The interface reference.
     */
    void setInterface(ComInterfaceRef const& ref);

    /*!
     *  Returns the name of the dependency.
     */
    QString const& getName() const;

    /*!
     *  Returns the display name of the SW instance.
     */
    QString const& getDisplayName() const;

    /*!
     *  Returns the description of the SW instance.
     */
    QString const& getDescription() const;

    /*!
     *  Returns the name reference to an COM interface in the top-level component.
     */
    QString const& getInterfaceRef() const;

    /*!
     *  Returns the interface reference to an COM interface in a contained SW component instance.
     */
    ComInterfaceRef const& getInterface() const;

    /*!
     *  Returns the position of the interface in the top-level design diagram.
     */
    QPointF const& getPosition() const;

    /*!
     *  Returns the direction of the interface in the top-level design diagram.
     */
    QVector2D const& getDirection() const;

    /*!
     *  Returns the connection route.
     */
    QList<QPointF> const& getRoute() const;

    /*!
     *  Assignment operator.
     */
    HierComConnection& operator=(HierComConnection const& rhs);

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the dependency.
    QString name_;

    //! The display name of the dependency.
    QString displayName_;

    //! The description of the dependency.
    QString desc_;

    //! The interface reference in the top-level component.
    QString interfaceRef_;

    //! The other reference to an SW component instance.
    ComInterfaceRef interface_;

    //! The position of the top-level interface in the design diagram.
    QPointF position_;

    //! The direction of the top-level interface in the design diagram.
    QVector2D direction_;

    //! The connection route.
    QList<QPointF> route_;
};

//-----------------------------------------------------------------------------

#endif // HIERCOMCONNECTION_H
