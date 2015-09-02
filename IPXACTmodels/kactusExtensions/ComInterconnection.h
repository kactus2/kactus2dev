//-----------------------------------------------------------------------------
// File: ComInterconnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.08.2015
//
// Description:
// Class encapsulating communication connection data.
//-----------------------------------------------------------------------------

#ifndef COMINTERCONNECTION_H
#define COMINTERCONNECTION_H

#include <IPXACTmodels/Design/Interconnection.h>

#include <IPXACTmodels/VendorExtension.h>

#include <QDomNode>

//-----------------------------------------------------------------------------
//! Class encapsulating API connection connection data.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComInterconnection : public Interconnection, public VendorExtension
{
public:
    /*!
     *  Default constructor.
     */
    ComInterconnection();

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
    ComInterconnection(QString const& name, QString const& displayName, QString const& description,
                  QSharedPointer<ActiveInterface> ref1, QSharedPointer<ActiveInterface> ref2,
                  QList<QPointF> const& route);

    /*!
     *  Copy constructor.
     */
    ComInterconnection(ComInterconnection const& rhs);

    /*!
     *  QDomNode constructor.
     *
     *      @param [in] connectionNode  The node containing the com connection.
     */
    ComInterconnection(QDomNode& connectionNode);

    /*!
     *  Destructor.
     */
    ~ComInterconnection();

    /*!
     *  Clone constructor.
     */
    virtual ComInterconnection* clone() const;

    /*!
     *  Gets the type of the extension.
     *
     *      @return The type of the extension.
     */
    virtual QString type() const;

    /*!
     *  Writes the contents to an XML stream.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    /*!
     *  Returns true if the connection is valid.
     *
     *      @param [out] errorList     The list for possible error messages.
     *      @param [in]  instanceNames The names of the HW/SW instances in the parent design.
     *      @param [in]  parentId      The identifier for the design containing the connections.
     */
    //bool isValid(QStringList& errorList, QStringList const& instanceNames,
    //             QString const& parentId) const;

	/*! \brief Check if the COM connection is in valid state.
	 *
	 * \param instanceNames The names of the HW/SW instances in the parent design.
	 *
	 * \return bool True if the COM connection is in valid state.
	*/
	//bool isValid(const QStringList& instanceNames) const;

    /*!
     *  Sets the first interface.
     *
     *      @param [in] ref A reference to a COM interface.
     */
    void setInterface1(QSharedPointer<ActiveInterface> ref);

    /*!
     *  Sets the first interface.
     *
     *      @param [in] ref A reference to a COM interface.
     */
    void setInterface2(QSharedPointer<ActiveInterface> ref);

    /*!
     *  Returns the first interface.
     *
     *      @return Pointer to the start interface.
     */
    QSharedPointer<ActiveInterface> getInterface1() const;

    /*!
     *  Returns the second interface.
     *
     *      @return Pointer to the end interface.
     */
    QSharedPointer<ActiveInterface> getInterface2() const;

    /*!
     *  Set a route for the connection.
     *
     *      @param [in] newRoute    The new route.
     */
    void setRoute(QList<QPointF> newRoute);

    /*!
     *  Get the route for the connection.
     *
     *      @return A list of positions determininig the route.
     */
    QList<QPointF> getRoute() const;
};

//-----------------------------------------------------------------------------

#endif // COMINTERCONNECTION_H
