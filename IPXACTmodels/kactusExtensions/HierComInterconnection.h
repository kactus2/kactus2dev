//-----------------------------------------------------------------------------
// File: HierComInterconnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.8.2015
//
// Description:
// Hierarchical COM connection.
//-----------------------------------------------------------------------------

#ifndef HIERCOMINTERCONNECTION_H
#define HIERCOMINTERCONNECTION_H

#include <IPXACTmodels/Design/Interconnection.h>

#include <IPXACTmodels/common/VendorExtension.h>

#include <QVector2D>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Class encapsulating COM dependency connection data.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT HierComInterconnection: public Interconnection, public VendorExtension
{
public:
    /*!
     *  Default constructor.
     */
    HierComInterconnection();

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
    HierComInterconnection(QString const& name, QString const& displayName, QString const& description,
                      QString const& interfaceRef,
                      QSharedPointer<ActiveInterface> ref,
                      QPointF const& position, QVector2D const& direction,
                      QList<QPointF> const& route);

    /*!
     *  Copy constructor.
     */
    HierComInterconnection(HierComInterconnection const& rhs);

    /*!
     *  The QDomNode constructor.
     *
     *      @param [in] connectionNode  The node containing the hierarchical com connection.
     */
    HierComInterconnection(QDomNode& connectionNode);

    /*!
     *  Destructor.
     */
    ~HierComInterconnection();

    /*!
     *  Clone constructor.
     */
    virtual HierComInterconnection* clone() const;

    /*!
     *  Get the type of the extension.
     *
     *      @return The type of the extension.
     */
    virtual QString type() const;

    /*!
     *  Writes the contents to an XML stream.
     */
    virtual void write(QXmlStreamWriter& writer) const;

	/*! \brief Check if the hierarchical COM connection is in valid state.
	 *
	 * \param errorList The string list to contain the possible error messages.
	 * \param instanceNames The names of the HW/SW instances in the parent design.
	 * \param parentId The identifier of the containing design.
	 *
	 * \return True if the hierarchical COM connection is in valid state.
	*/
	//bool isValid(QStringList& errorList, QStringList const& instanceNames,
    //             QString const& parentId) const;

	/*! \brief Check if the hierarchical COM connection is in valid state.
	 *
	 * \param instanceNames The names of the HW/SW instances in the parent design.
	 *
	 * \return True if the hierarchical COM connection is in valid state.
	*/
	//bool isValid(const QStringList& instanceNames) const;

    /*!
     *  Sets the top-level COM interface reference.
     *
     *      @param [in] interfaceRef Name reference to an COM interface in the top-level component.
     */
    void setTopInterfaceRef(QString const& interfaceRef);

    /*!
     *  Sets the interface reference to an COM interface in a contained SW component instance.
     *
     *      @param [in] ref The interface reference.
     */
    void setInterface(QSharedPointer<ActiveInterface> ref);

    /*!
     *  Returns the name reference to an COM interface in the top-level component.
     */
    QString const& getTopInterfaceRef() const;

    /*!
     *  Returns the interface reference to an COM interface in a contained SW component instance.
     */
    QSharedPointer<ActiveInterface> getInterface() const;

    /*!
     *  Returns the position of the interface in the top-level design diagram.
     */
    QPointF const& getPosition() const;

    /*!
     *  Returns the direction of the interface in the top-level design diagram.
     */
    QVector2D const& getDirection() const;

    /*!
     *  Set the position of the interface.
     *
     *      @param [in] newPosition     The new position for the interface.
     */
    void setPosition(QPointF const& newPosition);

    /*!
     *  Set the direction of the interface.
     *
     *      @param [in] newDirection    The new direction for the interface.
     */
    void setDirection(QVector2D const& newDirection);

    /*!
     *  Assignment operator.
     */
    HierComInterconnection& operator=(HierComInterconnection const& other);

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

    /*!
     *  Write the position of the hierarchical com interface.
     *
     *      @param [in] xmlWriter   The used XML writer.
     */
    void writePosition(QXmlStreamWriter& xmlWriter) const;

    /*!
     *  Write the direction of the hierarchical com interface.
     *
     *      @param [in] xmlWriter   The used XML writer.
     */
    void writeVectorDirection(QXmlStreamWriter& xmlWriter) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The interface reference in the top-level component.
    QString topInterfaceRef_;

    //! The position of the top-level interface in the design diagram.
    QPointF position_;

    //! The direction of the top-level interface in the design diagram.
    QVector2D direction_;
};

//-----------------------------------------------------------------------------

#endif // HIERCOMINTERCONNECTION_H
