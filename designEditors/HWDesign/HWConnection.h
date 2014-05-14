/* 
 *
 * 		filename: HWConnection.h
 */

#ifndef HWCONNECTION_H
#define HWCONNECTION_H

#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QUndoCommand>

#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsItemTypes.h>

/*! \brief HWConnection represents graphically an IP-XACT interconnection
 *
 */
class HWConnection : public GraphicsConnection
{
    Q_OBJECT

public:
    enum { Type = GFX_TYPE_DIAGRAM_INTERCONNECTION };

    /*!
     *  Constructor.
     */
    HWConnection(ConnectionEndpoint *endpoint1, ConnectionEndpoint *endpoint2,
                           bool autoConnect, QString const& name, QString const& displayName,
                           QString const& description, DesignDiagram* parent);

    /*!
     *  Constructor which creates an open-ended diagram interconnection.
     */
    HWConnection(QPointF const& p1, QVector2D const& dir1,
                           QPointF const& p2, QVector2D const& dir2,
                           QString const& displayName,
                           QString const& description,
                           DesignDiagram* parent);

    virtual ~HWConnection();

    /*!
     *  Sets the bus width label visible/invisible.
     *
     *      @param [in] visible If true, the label is set visible. Otherwise false.
     */
    void setBusWidthVisible(bool visible);

    /*! 
     *  Connects the ends of the interconnection.
     *
     *      @remarks There must be valid endpoints below the start and end positions
     *               of the interconnection.
     */
    virtual bool connectEnds();

    /*! \brief Set the routing of the interconnection
     *
     */
    virtual void setRoute(QList<QPointF> path);

    /*! \brief Update the end positions when connected endpoints are moved
     *
     */
    virtual void updatePosition();

    /*!
     *  Sets the left bound of the ad-hoc endpoint involved in the connection.
     *
     *      @param [in] endpointIndex Which endpoint is concerned. Either 1 or 2.
     *      @param [in] leftBound     The left bound.
     */
    void setAdHocLeftBound(int endpointIndex, int leftBound);

    /*!
     *  Sets the right bound of the ad-hoc endpoint involved in the connection.
     *
     *      @param [in] endpointIndex Which endpoint is concerned. Either 1 or 2.
     *      @param [in] rightBound    The right bound.
     */
    void setAdHocRightBound(int endpointIndex, int rightBound);

    /*!
     *  Returns the left bound of the ad-hoc endpoint involved in the connection.
     *
     *      @param [in] endpointIndex Which endpoint is concerned. Either 1 or 2.
     *
     *      @return The left bound of the endpoint.
     */
    int getAdHocLeftBound(int endpointIndex) const;

    /*!
     *  Returns the left bound of the ad-hoc endpoint involved in the connection.
     *
     *      @param [in] endpointIndex Which endpoint data is concerned. Either 1 or 2.
     *
     *      @return The right bound of the endpoint.
     */
    int getAdHocRightBound(int endpointIndex) const;

    /*!
     *  Returns true if the connection is a bus connection. Returns false if it isn't (i.e. it is an ad-hoc
     *  connection).
     */
    bool isBus() const;

    int type() const { return Type; }

    /*!
     *  Sets the vendor extensions for the connection.
     *
     *      @param [in] vendorExtensions   The vendor extensions to set.
     *
     *      @remark     Should be used only for hierConnections.
     */
    void setVendorExtensions(QList<QSharedPointer<VendorExtension> > const& vendorExtensions);

    /*!
     *  Returns the vendor extensions of the connection.
     *
     *      @return     The vendor extensions of the connection.
     *
     *      @remark     Should be used only for hierConnections.
     */
    QList<QSharedPointer<VendorExtension> > getVendorExtensions() const;

private:

    //-----------------------------------------------------------------------------
    //! Structure for storing ad-hoc port bounds.
    //-----------------------------------------------------------------------------
    struct AdHocPortBound
    {
        int left_;
        int right_;

        /*!
         *  Default constructor.
         */
        AdHocPortBound() : left_(-1), right_(-1)
        {
        }
    };

    /*!
     *  Updates the width label based on the endpoints.
     */
    void updateWidthLabel();

    /*!
     *  Calculates the total bus width.
     *
     *      @return The bus width.
     */
    int calculateBusWidth() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The connection width label.
    QGraphicsTextItem* widthLabel_;

    //! The ad-hoc port bounds.
    AdHocPortBound portBounds_[2];

    /*!
     * HierConnection vendor extensions.
     */
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;
};

#endif // HWCONNECTION_H
