//-----------------------------------------------------------------------------
// File: ComConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 11.4.2012
//
// Description:
// Class encapsulating communication connection data.
//-----------------------------------------------------------------------------

#ifndef COMCONNECTION_H
#define COMCONNECTION_H

#include <common/Global.h>

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QPointF>

//-----------------------------------------------------------------------------
//! ComInterfaceRef structure.
//-----------------------------------------------------------------------------
struct ComInterfaceRef
{
    QString componentRef;   //!< Name reference to an SW component instance.
    QString comRef;         //!< Name reference to a COM interface that is contained by the SW component instance.

    /*!
     *  Default constructor.
     */
    ComInterfaceRef() : componentRef(), comRef()
    {
    }

    /*!
     *  Constructor.
     */
    ComInterfaceRef(QString const& componentRef_, QString const& comRef_)
        : componentRef(componentRef_),
          comRef(comRef_)
    {
    }
};

//-----------------------------------------------------------------------------
//! Class encapsulating API connection connection data.
//-----------------------------------------------------------------------------
class KACTUS2_API ComConnection
{
public:
    /*!
     *  Default constructor.
     */
    ComConnection();

    /*!
     *  Constructor which sets all the values as given.
     *
     *      @param [in] name         The name to set.
     *      @param [in] displayName  The display name to set.
     *      @param [in] description  The description to set.
     *      @param [in] ref1         A reference to the first COM interface.
     *      @param [in] ref2         A reference to the second COM interface.
     *      @param [in] route        The connection route.
     */
    ComConnection(QString const& name, QString const& displayName, QString const& description,
                  ComInterfaceRef const& ref1, ComInterfaceRef const& ref2,
                  QList<QPointF> const& route);

    /*!
     *  Copy constructor.
     */
    ComConnection(ComConnection const& rhs);

    /*!
     *  Constructor which reads the API connection from an XML node.
     *
     *      @param [in] node The source XML node.
     */
    ComConnection(QDomNode& node);

    /*!
     *  Destructor.
     */
    ~ComConnection();

    /*!
     *  Writes the contents to an XML stream.
     */
    void write(QXmlStreamWriter& writer) const;

    /*!
     *  Returns true if the connection is valid.
     *
     *      @param [out] errorList     The list for possible error messages.
     *      @param [in]  instanceNames The names of the HW/SW instances in the parent design.
     *      @param [in]  parentId      The identifier for the design containing the connections.
     */
    bool isValid(QStringList& errorList, QStringList const& instanceNames,
                 QString const& parentId) const;

    /*!
     *  Sets the name of the connection.
     *
     *      @param [in] name The name to set.
     */
    void setName(QString const& name);

    /*!
     *  Sets the display name of the connection.
     *
     *      @param [in] displayName The display name to set.
     */
    void setDisplayName(QString const& displayName);

    /*!
     *  Sets the description of the connection.
     *
     *      @param [in] description The description to set.
     */
    void setDescription(QString const& description);

    /*!
     *  Sets the first interface.
     *
     *      @param [in] ref A reference to a COM interface.
     */
    void setInterface1(ComInterfaceRef const& ref);

    /*!
     *  Sets the first interface.
     *
     *      @param [in] ref A reference to a COM interface.
     */
    void setInterface2(ComInterfaceRef const& ref);

    /*!
     *  Sets the flag whether the connection is off-page or not.
     *
     *      @param [in] offPage If true, the connection is set off-page.
     */
    void setOffPage(bool offPage);

    /*!
     *  Returns the name of the connection.
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
     *  Returns the first interface.
     */
    ComInterfaceRef const& getInterface1() const;

    /*!
     *  Returns the second interface.
     */
    ComInterfaceRef const& getInterface2() const;

    /*!
     *  Returns the connection route.
     */
    QList<QPointF> const& getRoute() const;

    /*!
     *  Returns true if the connection is off-page.
     */
    bool isOffPage() const;

    /*!
     *  Assignment operator.
     */
    ComConnection& operator=(ComConnection const& rhs);

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the connection.
    QString name_;

    //! The display name of the connection.
    QString displayName_;

    //! The description of the connection.
    QString desc_;

    //! Provider reference.
    ComInterfaceRef interface1_;

    //! Requester reference.
    ComInterfaceRef interface2_;

    //! The route of the connection.
    QList<QPointF> route_;

    //! If true, the connection is off-page.
    bool offPage_;
};

//-----------------------------------------------------------------------------

#endif // COMCONNECTION_H
