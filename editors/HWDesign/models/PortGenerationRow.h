//-----------------------------------------------------------------------------
// File: PortGenerationRow.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 3.6.2011
//
// Description:
// One row in port generation table.
//-----------------------------------------------------------------------------

#include <QObject>

#include "models/generaldeclarations.h"

//-----------------------------------------------------------------------------
//! PortGenerationRow class.
//-----------------------------------------------------------------------------
class PortGenerationRow : public QObject
{
    Q_OBJECT

public:

    //! \brief The number of columns the row represents.
    static const int COLUMNS = 7;

    /*! \brief The constructor.
    *
    * @param name Name of the port in opposing interface.
    * @param direction Direction of the port in opposing interface.
    * @param description The port description in opposing interface.
    * @param size The size of the port. Size must be the same in both interfaces.
    */
    PortGenerationRow(QString name = QString(), General::Direction direction = General::DIRECTION_INVALID,
        QString description = QString(), int size = 0);

    //! \brief The destructor.
    virtual ~PortGenerationRow();

    /*! \brief Get the port description in opposing interface.
    *
    * @return The port description.
    */
    QString getSourceDescription() const;    

  /*! \brief Get the port direction in opposing interface.
    *
    * @return The port direction.
    */
    General::Direction getSourceDirection() const;

  /*! \brief Get the port name in opposing interface.
    *
    * @return The port name.
    */
    QString getSourceName() const;    

  /*! \brief Get the port size.
    *
    * @return The port size.
    */
    int getSize() const;

    /*! \brief Set the size of both ports.
    *
    * @param [in] size The size to set.
    */
    void setSize(int const size);

    /*! \brief Get the port name in draft component.
    *
    * @return The port name.
    */
    QString getDraftName() const;

    /*! \brief Set the name of draft component.
    *
    * @param [in] name The name to set.
    */
    void setDraftName(QString const name);

  /*! \brief Get the port direction in draft component.
    *
    * @return The port direction in draft component.
    */
    General::Direction getDraftDirection() const;

    /*! \brief Set the port direction in draft component.
    *
    * @param [in] direction The direction to set.
    */
    void setDraftDirection(General::Direction const direction);

    /*! \brief Get the port description in draft component.
    *
    * @return The port description.
    */
    QString getDraftDescription() const;

    /*! \brief Set the port description in draft component.
    *
    * @param [in] description The description to set.
    */
    void setDraftDescription(QString const description);

    /*! \brief Checks if the row is valid.
    *
    * @return True if the row is valid.
    */
    bool isValid() const;

private:
    //! \brief No copying.
    PortGenerationRow(const PortGenerationRow& other);

    //! No assignment.
    PortGenerationRow& operator=(const PortGenerationRow& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Port name in opposing interface.
    QString sourceName_;

    //! Port direction in opposing interface.
    General::Direction sourceDirection_;

    //! Port description in opposing interface.
    QString sourceDescription_;

    //! Port size in both interfaces.
    int size_;

    //! Port name in draft component.
    QString draftName_;

    //! Port direction in draft component.
    General::Direction draftDirection_;

    //! Port description in draft component.
    QString draftDescription_;
};

