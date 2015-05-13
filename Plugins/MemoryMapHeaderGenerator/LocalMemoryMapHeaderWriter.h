//-----------------------------------------------------------------------------
// File: LocalMemoryMapHeaderWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 07.05.2015
//
// Description:
// The implementation for creating c-headers of local memory maps.
//-----------------------------------------------------------------------------

#ifndef LOCALMEMORYMAPHEADERWRITER_H
#define LOCALMEMORYMAPHEADERWRITER_H

#include "BaseMemoryMapHeaderWriter.h"

#include "localheadersavemodel.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QFileInfo>
#include <QList>
#include <QTextStream>

class IPluginUtility;

//-----------------------------------------------------------------------------
//! The implementation for creating c-headers of local memory maps.
//-----------------------------------------------------------------------------
class LocalMemoryMapHeaderWriter : public BaseMemoryMapHeaderWriter
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] utility         The interface which offers core utilities for plugins.
     *      @param [in] localComponent  The selected component.
     *      @param [in] parentObject    The parent object of this writer.
     */
    LocalMemoryMapHeaderWriter(IPluginUtility* utility, QSharedPointer<Component> localComponent,
        QObject* parentObject);

    /*!
     *  The destructor.
     */
    virtual ~LocalMemoryMapHeaderWriter();

    /*!
     *  Write the memory map header file.
     *
     *      @param [in] saveOptions     The save options for the memory map header.
     */
    virtual void writeMemoryMapHeader(QList<LocalHeaderSaveModel::SaveFileOptions*> saveOptions);

protected:

    /*!
     *  Display the created memory map header.
     *
     *      @param [in] filePath    The file path to the memory map header.
     */
    virtual void displayMemoryMapHeader(QString const& filePath) const;

private:

	//! No copying
    LocalMemoryMapHeaderWriter(const LocalMemoryMapHeaderWriter& other);

	//! No assignment
    LocalMemoryMapHeaderWriter& operator=(const LocalMemoryMapHeaderWriter& other);

    //! The selected component.
    QSharedPointer<Component> component_;

    //! The parent object of this writer.
    QObject* parentObject_;

    //! The save options for the memory map header.
    QList<LocalHeaderSaveModel::SaveFileOptions*> saveOptions_;
};

#endif // LOCALMEMORYMAPHEADERWRITER_H
