//-----------------------------------------------------------------------------
// File: LanguageTools.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 15.6.2023
//
// Description:
// Implementation of the ipxact:languageTools element.
//-----------------------------------------------------------------------------

#ifndef LANGUAGE_TOOLS_H
#define LANGUAGE_TOOLS_H

#include <IPXACTmodels/common/FileBuilder.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Implementation of the ipxact:languageTools element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT LanguageTools : public NameGroup, public Extendable
{
public:

    //! Implementation of the ipxact:linkerCommandFile element.
    struct LinkerCommandFile : public Extendable
    {
        QString name_;

        QString commandLineSwitch_;

        QString enable_;

        QStringList generatorRefs_;
    };

    /*!
     *  The constructor.
     */
    LanguageTools();

    /*!
     *  The copy constructor.
     */
    LanguageTools(const LanguageTools& other);

    /*!
     *  The assignment operator.
     */
    LanguageTools& operator=(const LanguageTools& other);

    /*!
     *  The destructor.
     */
    virtual ~LanguageTools();

    /*!
     *  Create a copy of the language tools.
     *
     *      @return A copy of the language tools object.
     */
    QSharedPointer<LanguageTools> clone() const;

    /*!
     *  Get the file builders.
     *
     *      @return A pointer to a list containing the file builders.
     */
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > getFileBuilders() const;

    /*!
     *  Set the file builders.
     *
     *      @param [in] newFileBuilders  The new file builders.
     */
    void setFileBuilders(QSharedPointer<QList<QSharedPointer<FileBuilder> > > newDefaultFileBuilders);

    /*!
     *  Get the linker defined in the language tools.
     *
     *      @return The linker in the language tools.
     */
    QString getLinker() const;

    /*!
     *  Set the linker in the language tools.
     *
     *      @param [in] linker The linker to set.
     */
    void setLinker(QString const& linker);

    /*!
     *  Get the linker flags defined in the language tools.
     *
     *      @return The linker glags in the language tools.
     */
    QString getLinkerFlags() const;

    /*!
     *  Set the linker flags in the language tools.
     *
     *      @param [in] linker The linker flags to set.
     */
    void setLinkerFlags(QString const& flags);

    /*!
     *  Get the linker command file defined in the language tools.
     *
     *      @return The linker command file.
     */
    QSharedPointer<LanguageTools::LinkerCommandFile> getLinkerCommandFile() const;


private:

    /*!
     *  Copy file builders.
     */
    void copyFileBuilders(const LanguageTools& other) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------


    //! A list of default file builders.
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > fileBuilders_ =
        QSharedPointer<QList<QSharedPointer<FileBuilder> > >(new QList<QSharedPointer<FileBuilder> >);

    //! The used linker.
    QString linker_;

    //! The used linker flags.
    QString linkerFlags_;

    //! The linker command file.
    QSharedPointer<LinkerCommandFile> linkerCommandFile_ = QSharedPointer<LinkerCommandFile>(new LinkerCommandFile);

};

#endif // LANGUAGE_TOOLS_H