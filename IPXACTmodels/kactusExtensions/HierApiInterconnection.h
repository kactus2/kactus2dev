//-----------------------------------------------------------------------------
// File: HierApiInterconnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.08.2015
//
// Description:
// Hierarchical API connection.
//-----------------------------------------------------------------------------

#ifndef HIERAPIINTERCONNECTION_H
#define HIERAPIINTERCONNECTION_H

#include <QPointF>
#include <QVector2D>

#include <IPXACTmodels/Design/Interconnection.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <QDomNode>

//-----------------------------------------------------------------------------
//! Class encapsulating API dependency connection data.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT HierApiInterconnection : public Interconnection, public VendorExtension
{
public:
    /*!
     *  Default constructor.
     */
    HierApiInterconnection();

    /*!
     *  Constructor which sets all the values as given.
     *
     *      @param [in] name          The name to set.
     *      @param [in] displayName   The display name to set.
     *      @param [in] description   The description to set.
     *      @param [in] interfaceRef  Name reference to an API interface in the top-level component.
     *      @param [in] ref           The interface reference to a contained SW component instance.
     *      @param [in] position      The position of the top-level interface in the design diagram.
     *      @param [in] direction     The direction of the top-level interface in the design diagram.
     *      @param [in] route         The connection route.
     */
    HierApiInterconnection(QString const& name, QString const& displayName, QString const& description,
                      QString const& interfaceRef,
                      QSharedPointer<ActiveInterface> ref,
                      QPointF const& position, QVector2D const& direction,
                      QList<QPointF> const& route);

    /*!
     *  Copy constructor.
     */
    HierApiInterconnection(HierApiInterconnection const& rhs);

    /*!
     *  QDomNode constructor.
     *
     *      @param [in] connectionNode  The node containing the connection.
     */
    HierApiInterconnection(QDomNode& connectionNode);

    /*!
     *  Destructor.
     */
    ~HierApiInterconnection();

    /*!
     *  The clone constructor.
     */
    virtual HierApiInterconnection* clone() const;

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

	/*! \brief Check if the hierarchical API dependency is in valid state.
	 *
	 * \param errorList The string list to add the possible error messages to.
	 * \param instanceNames The HW/SW instance names contained in the containing design.
	 * \param parentId The identifier for the design containing the dependencies.
	 *
	 * \return True if the API dependency is in valid state.
	*/
	//bool isValid(QStringList& errorList, QStringList const& instanceNames,
	//	QString const& parentId) const;

	/*! \brief Check if the hierarchical API dependency is in valid state.
	 *
	 * \param instanceNames The HW/SW instance names contained in the containing design.
	 *
	 * \return True if the API dependency is in valid state.
	*/
	//bool isValid(const QStringList& instanceNames) const;

    /*!
     *  Sets the top-level API interface reference.
     *
     *      @param [in] interfaceRef    Name reference to an API interface in the top-level component.
     */
    void setTopInterfaceRef(QString const& interfaceRef);

    /*!
     *  Sets the interface reference to an API interface in a contained SW component instance.
     *
     *      @param [in] ref     The interface reference.
     */
    void setInterface(QSharedPointer<ActiveInterface> ref);

    /*!
     *  Returns the name reference to an API interface in the top-level component.
     *
     *      @return The name of the referenced API interface in the top-level component.
     */
    QString const& getTopInterfaceRef() const;

    /*!
     *  Returns the interface reference to an API interface in a contained SW component instance.
     *
     *      @return The used interface.
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
     *      @param [in] newDirection    The new direction of the interface.
     */
    void setDirection(QVector2D const& newDirection);

    /*!
     *  Assignment operator.
     */
    HierApiInterconnection& operator=(HierApiInterconnection const& rhs);

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
     *  Writes the position of the hierarchical interface.
     *
     *      @param [in] writer  The selected XML writer.
     */
    void writePosition(QXmlStreamWriter& writer) const;

    /*!
     *  Writes the direction of the hierarchical interface.
     *
     *      @param [in] writer   [Description].
     */
    void writeVectorDirection(QXmlStreamWriter& writer) const;

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

#endif // HIERAPIINTERCONNECTION_H
