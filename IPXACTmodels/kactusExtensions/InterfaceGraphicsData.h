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
     *  Default constructor.
     */
    InterfaceGraphicsData(QString const& name, QPointF position = QPointF(), QVector2D direction = QVector2D());

    /*!
     *  Copy constructor.
     */
    InterfaceGraphicsData(const InterfaceGraphicsData& other);

    /*!
     *  Constructor which reads the API interface from an XML node.
     *
     *      @param [in] node The source XML node.
     */
    InterfaceGraphicsData(QDomNode& node);

    /*!
     *  Destructor.
     */
    ~InterfaceGraphicsData();

    /*!
     *  Clones the api interface.
     *
     *      @return A cloned api interface.
     */
    virtual InterfaceGraphicsData* clone() const;
    
    /*!
     *  Assignment operator.
     */
    InterfaceGraphicsData& operator=(InterfaceGraphicsData const& rhs);

    /*!
     *  Gets the api interface type.
     *
     *      @return The type of the api interface.
     */
    virtual QString type() const;

    /*!
     *  Writes the contents of the API interface to an XML stream.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    /*!
     *  Returns true if the contents of the API interface are valid.
     *
     *      @param [out] errorList  Error list which is appended with errors if found while validating.
     *      @param [in]  parentId   The identifier of the containing component.
     */
//     bool isValid(QStringList& errorList, QString const& parentId) const;

    /*!
     *  Returns true if the contents of the API interface are valid.
     */
//     bool isValid() const;

    void setName(QString const& newName);

    QString getName() const;

    void setPosition(QPointF const& newPosition);

    QPointF getPosition() const;

    bool hasPosition() const;

    void setDirection(QVector2D const& newDirection);

    QVector2D getDirection() const;

    bool hasDirection() const;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The API type (i.e. a VLNV reference to an API definition).
//     VLNV apiType_;
    QString name_;

    //! Dependency direction.
//     DependencyDirection dependencyDir_;
//     QPointF position_;
    QSharedPointer<Kactus2Position> positionExtension_;

    //! The default position in the parent component's graphical representation (optional).
//     QPointF defaultPos_;
    QVector2D direction_;
};

//-----------------------------------------------------------------------------

#endif // INTERFACEGRAPHICSDATA_H
