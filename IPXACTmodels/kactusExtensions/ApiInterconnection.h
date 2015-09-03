//-----------------------------------------------------------------------------
// File: ApiInterconnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.08.2015
//
// Description:
// Class encapsulating API connection data.
//-----------------------------------------------------------------------------

#ifndef APIINTERCONNECTION_H
#define APIINTERCONNECTION_H

#include <IPXACTmodels/Design/Interconnection.h>

#include <IPXACTmodels/VendorExtension.h>

#include <QDomNode>

//-----------------------------------------------------------------------------
//! Class encapsulating API dependency connection data.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ApiInterconnection : public Interconnection, public VendorExtension
{
public:
    /*!
     *  Default constructor.
     */
    ApiInterconnection();

    /*!
     *  Constructor which sets all the values as given.
     *
     *      @param [in] name         The name to set.
     *      @param [in] displayName  The display name to set.
     *      @param [in] description  The description to set.
     *      @param [in] ref1         The first interface reference.
     *      @param [in] ref2         The second interface reference.
     *      @param [in] route        The connection route.
     *      @param [in] imported     If true, the connection is an imported one.
     */
    ApiInterconnection(QString const& name, QString const& displayName, QString const& description,
                  QSharedPointer<ActiveInterface> ref1, QSharedPointer<ActiveInterface> ref2,
                  QList<QPointF> const& route, bool imported = false);

    /*!
     *  Copy constructor.
     */
    ApiInterconnection(ApiInterconnection const& other);

    /*!
     *  QDomNode constructor.
     *
     *      @param [in] node    The node containing the api interconnection.
     */
    ApiInterconnection(QDomNode& node);

    /*!
     *  Destructor.
     */
    ~ApiInterconnection();

    /*!
     *  Copy constructor.
     */
    virtual ApiInterconnection* clone() const;

    /*!
     *  Gets the type of the extension.
     *
     *      @return The type of the extension.
     */
    virtual QString type() const;

    /*!
     *  Writes the contents to an XML stream.
     *
     *      @param [in] writer  The selected XML stream.
     */
    virtual void write(QXmlStreamWriter& writer) const;

	/*! \brief Check if the API dependency is in valid state.
	 *
	 * \param errorList The string list to add the possible error messages to.
	 * \param instanceNames The HW/SW instance names contained in the containing design.
	 * \param parentId The identifier for the design containing the dependencies.
	 *
	 * \return True if the API dependency is in valid state.
	*/
	//bool isValid(QStringList& errorList, QStringList const& instanceNames,
	//	QString const& parentId) const;

	/*! \brief Check if the API dependency is in valid state.
	 *
	 * \param instanceNames The HW/SW instance names contained in the containing design.
	 *
	 * \return True if the API dependency is in valid state.
	*/
	//bool isValid(const QStringList& instanceNames) const;

    /*!
     *  Sets the provider reference.
     *
     *      @param [in] ref     The new interface reference.
     */
    void setInterface1(QSharedPointer<ActiveInterface> ref);

    /*!
     *  Sets the requester reference.
     *
     *      @param [in] ref     The interface reference.
     */
    void setInterface2(QSharedPointer<ActiveInterface> ref);

    /*!
     *  Sets the flag whether the connection is an imported one and should be auto-synced.
     *
     *      @param [in] imported    If true, the connection is set as imported.
     */
    void setImported(bool imported);

    /*!
     *  Returns the provider reference.
     *
     *      @return The provider interface.
     */
    QSharedPointer<ActiveInterface> getInterface1() const;

    /*!
     *  Returns the requester reference.
     *
     *      @return The requester interface.
     */
    QSharedPointer<ActiveInterface> getInterface2() const;

    /*!
     *  Returns true if the connection is an imported one.
     */
    bool isImported() const;

    /*!
     *  Assignment operator.
     */
    ApiInterconnection& operator=(ApiInterconnection const& rhs);

    /*!
     *  Set a route for the connection.
     *
     *      @param [in] newRoute    The new route.
     */
    void setRoute(QList<QPointF> newRoute);

    /*!
     *  Get the route for the connection.
     *
     *      @return A list of positions determining the route.
     */
    QList<QPointF> getRoute() const;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! If true, the connection is an imported one.
    bool imported_;
};

//-----------------------------------------------------------------------------

#endif // APIDEPENDENCY_H
