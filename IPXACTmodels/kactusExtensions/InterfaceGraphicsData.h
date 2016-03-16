//-----------------------------------------------------------------------------
// File: InterfaceGraphicsData.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.02.2016
//
// Description:
// Interface graphics class for defining properties of graphical interfaces in designs.
//-----------------------------------------------------------------------------

#ifndef INTERFACEGRAPHICSDATA_H
#define INTERFACEGRAPHICSDATA_H

#include <IPXACTmodels/common/VendorExtension.h>

// #include <IPXACTmodels/kactusExtensions/Kactus2Position.h>

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QPointF>
#include <QVector2D>
#include <QSharedPointer>

class Kactus2Position;

//-----------------------------------------------------------------------------
//! Interface graphics class for defining properties of graphical interfaces in designs.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT InterfaceGraphicsData : public VendorExtension
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] name        Name of the interface.
     *      @param [in] position    Position of the interface.
     *      @param [in] direction   Direction of the interface.
     */
    InterfaceGraphicsData(QString const& name, QPointF position = QPointF(), QVector2D direction = QVector2D());

    /*!
     *  Copy constructor.
     */
    InterfaceGraphicsData(const InterfaceGraphicsData& other);

    /*!
     *  Constructor which reads the interface graphics data from an XML node.
     *
     *      @param [in] node    The source XML node.
     */
    InterfaceGraphicsData(QDomNode& node);

    /*!
     *  Destructor.
     */
    virtual ~InterfaceGraphicsData();

    /*!
     *  Clones the interface graphics data.
     *
     *      @return Cloned interface graphics data.
     */
    virtual InterfaceGraphicsData* clone() const;
    
    /*!
     *  Assignment operator.
     */
    InterfaceGraphicsData& operator=(InterfaceGraphicsData const& rhs);

    /*!
     *  Gets the interface graphics data type.
     *
     *      @return The type of the interface graphics data.
     */
    virtual QString type() const;

    /*!
     *  Writes the contents of the interface graphics data to an XML stream.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    /*!
     *  Set a name.
     *
     *      @param [in] newName     The new name.
     */
    void setName(QString const& newName);

    /*!
     *  Get the name.
     *
     *      @return The name of the interface.
     */
    QString getName() const;

    /*!
     *  Set the position for the interface.
     *
     *      @param [in] newPosition     The new position.
     */
    void setPosition(QPointF const& newPosition);

    /*!
     *  Get the position.
     *
     *      @return The position of the interface.
     */
    QPointF getPosition() const;

    /*!
     *  Check if the interface has been given a position.
     *
     *      @return True, if the interface has a position, false otherwise.
     */
    bool hasPosition() const;

    /*!
     *  Set a direction for the interface.
     *
     *      @param [in] newDirection    The new interface direction.
     */
    void setDirection(QVector2D const& newDirection);

    /*!
     *  Get the direction of the interface.
     *
     *      @return The direction of the interface.
     */
    QVector2D getDirection() const;

    /*!
     *  Check if the interface has been given a direction.
     *
     *      @return True, if the interface has been given a direction, otherwise false.
     */
    bool hasDirection() const;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the interface graphics data.
    QString name_;

    //! Extension containing the position of the interface.
    QSharedPointer<Kactus2Position> positionExtension_;

    //! The direction of the interface.
    QVector2D direction_;
};

//-----------------------------------------------------------------------------

#endif // INTERFACEGRAPHICSDATA_H
