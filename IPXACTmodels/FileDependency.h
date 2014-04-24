//-----------------------------------------------------------------------------
// File: FileDependency.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.01.2013
//
// Description:
// File dependency class.
//-----------------------------------------------------------------------------

#ifndef FILEDEPENDENCY_H
#define FILEDEPENDENCY_H

#include <common/Global.h>

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QObject>

class FileDependencyItem;

//-----------------------------------------------------------------------------
//! File dependency class.
//-----------------------------------------------------------------------------
class KACTUS2_API FileDependency : public QObject
{
public:
    //-----------------------------------------------------------------------------
    //! Status enumeration.
    //-----------------------------------------------------------------------------
    enum Status
    {
        STATUS_UNCHANGED = 0,
        STATUS_ADDED,
        STATUS_REMOVED
    };

    /*!
     *  Constructor.
     */
    FileDependency();

    /*!
     *  Copy constructor.
     */
    FileDependency(FileDependency const& rhs);

    /*!
     *  Constructor which reads the file dependency from an XML node.
     *
     *      @param [in] node The source XML node.
     */
    FileDependency(QDomNode& node);
    
    /*!
     *  Destructor.
     */
    ~FileDependency();

    /*!
     *  Writes the contents of the file dependency to an XML stream.
     *
     *      @param [in] write The XML stream writer.
     */
    void write(QXmlStreamWriter& writer);

    /*!
     *  Reverses the dependency.
     */
    void reverse();

    /*!
     *  Sets the 'from' file.
     *
     *      @param [in] filename The name of the file.
     */
    void setFile1(QString const& filename);

    /*!
     *  Sets the 'to' file.
     *
     *      @param [in] filename The name of the file.
     */
    void setFile2(QString const& filename);

    /*!
     *  Sets the description.
     *
     *      @param [in] desc The description to set.
     */
    void setDescription(QString const& desc);

    /*!
     *  Sets the dependency locked/unlocked.
     *
     *      @param [in] locked If true, the dependency is set locked; false for unlocked.
     */
    void setLocked(bool locked);

    /*!
     *  Sets the dependency bidirectional.
     *
     *      @param [in] bidiretional If true, the dependency is set as bidirectional (two-way);
     *                               false for one-way dependency.
     */
    void setBidirectional(bool bidirectional);

    /*!
     *  Sets the dependency manual.
     *
     *      @param [in] manual If true, the dependency is treated as a manual dependency;
     *                         otherwise it is treated as an automatic (generated) one.
     */
    void setManual(bool manual);

    /*!
     *  Sets the status of the dependency.
     *
     *      @param [in] status The status to set.
     */
    void setStatus(Status status);

    /*!
     *  Returns the name of the 'from' file.
     */
    QString const& getFile1() const;

    /*!
     *  Returns the name of the 'to' file.
     */
    QString const& getFile2() const;

    /*!
     *  Returns the description.
     */
    QString const& getDescription() const;

    /*!
     *  Returns true if the dependency is locked.
     */
    bool isLocked() const;

    /*!
     *  Returns true if the dependency is bidirectional.
     */
    bool isBidirectional() const;

    /*!
     *  Returns true if the dependency is manual.
     */
    bool isManual() const;

    /*!
     *  Returns the status.
     */
    Status getStatus() const;

    /*!
     *  Assignment operator.
     */
    FileDependency& operator=(FileDependency const& rhs);

    //-----------------------------------------------------------------------------
    // Internal methods.
    //-----------------------------------------------------------------------------

    /*!
     *  Sets the file item pointers.
     *
     *      @param [in] fileItem1 The file item corresponding to file1.
     *      @param [in] fileItem2 The file item corresponding to file2.
     */
    void setItemPointers(FileDependencyItem* fileItem1, FileDependencyItem* fileItem2);

    /*!
     *  Returns the file item pointer for file1.
     */
    FileDependencyItem* getFileItem1() const;

    /*!
     *  Returns the file item pointer for file2.
     */
    FileDependencyItem* getFileItem2() const;

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the first (from) file.
    QString file1_;

    //! The name of the second (to) file.
    QString file2_;

    //! The dependency description.
    QString desc_;

    //! If true, the dependency is locked and cannot be modified.
    bool locked_;

    //! If true, the dependency is bidirectional.
    bool bidirectional_;

    //! If true, the dependency is a manually created one.
    bool manual_;

    //! The status of the dependency.
    Status status_;

    //! The file item pointers.
    FileDependencyItem* fileItem1_;
    FileDependencyItem* fileItem2_;
};

#endif // FILEDEPENDENCY_H
