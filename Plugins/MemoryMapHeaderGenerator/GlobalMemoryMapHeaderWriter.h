//-----------------------------------------------------------------------------
// File: GlobalMemoryMapHeaderWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.05.2015
//
// Description:
// The implementation for creating c-headers of global memory maps.
//-----------------------------------------------------------------------------

#ifndef GLOBALMEMORYMAPHEADERWRITER_H
#define GLOBALMEMORYMAPHEADERWRITER_H

#include "BaseMemoryMapHeaderWriter.h"

#include "globalheadersavemodel.h"

#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QFileInfo>
#include <QList>
#include <QTextStream>

class IPluginUtility;
class ListParameterFinder;

//-----------------------------------------------------------------------------
//! The implementation for creating c-headers of global memory maps.
//-----------------------------------------------------------------------------
class GlobalMemoryMapHeaderWriter : public BaseMemoryMapHeaderWriter
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] utility                 The interface which offers core utilities for plugins.
     *      @param [in] componentDesign         The design currently in use.
     *      @param [in] componentDesignConfig   The design configuration currently in use.
     *      @param [in] parentObject            The parent object of this writer.
     */
    GlobalMemoryMapHeaderWriter(IPluginUtility* utility, QSharedPointer<Design> componentDesign,
        QSharedPointer<DesignConfiguration> componentDesignConfig, QObject* parentObject);

    /*!
     *  The destructor.
     */
    virtual ~GlobalMemoryMapHeaderWriter();

    /*!
     *  Write the memory map header into a file.
     *
     *      @param [in] globalComponent     The top component of the design.
     *      @param [in] saveOptions         The save options for the memory map header.
     */
    virtual void writeMemoryMapHeader(QSharedPointer<Component> globalComponent,
        QList<GlobalHeaderSaveModel::SaveFileOptions*> saveOptions);

protected:
    
    /*!
     *  Display the created memory map header.
     *
     *      @param [in] filePath    The file path.
     */
    virtual void displayMemoryMapHeader(QString const& filePath) const;

private:

	//! No copying
    GlobalMemoryMapHeaderWriter(const GlobalMemoryMapHeaderWriter& other);

	//! No assignment
    GlobalMemoryMapHeaderWriter& operator=(const GlobalMemoryMapHeaderWriter& other);

    /*!
	 *  Parse the interface and its connections.
	 *
	 *      @param [in] offset      The current offset.
	 *      @param [in] stream      The text stream to write into.
	 *      @param [in] interface   Identifies the current interface to parse.
	 */
	void parseInterface(qint64 offset, QTextStream& stream, QSharedPointer<ActiveInterface> interface);

    /*!
     *  Parse the master interface.
     *
     *      @param [in] offset      The current offset.
     *      @param [in] component   The component in use.
     *      @param [in] stream      The text stream to write into.
     *      @param [in] interFace   Identifies the current interface to parse.
     */
    void parseMasterInterface(qint64 offset, QSharedPointer<Component> component, QTextStream& stream,
        QSharedPointer<ActiveInterface> interface);

    /*!
     *  Parse the slave interface.
     *
     *      @param [in] offset      The current offset.
     *      @param [in] component   The component in use.
     *      @param [in] stream      The text stream to write into.
     *      @param [in] interFace   Identifies the current interface to parse.
     */
    void parseSlaveInterface(qint64 offset, QSharedPointer<Component> component, QTextStream& stream,
        QSharedPointer<ActiveInterface> interface);

    /*!
     *  Parse the mirrored slave interface.
     *
     *      @param [in] offset      The current offset.
     *      @param [in] component   The component in use.
     *      @param [in] stream      The text stream to write into.
     *      @param [in] interFace   Identifies the current interface to parse.
     */
    void parseMirroredSlaveInterface(qint64 offset, QSharedPointer<Component> component, QTextStream& stream,
        QSharedPointer<ActiveInterface> interface);

    /*!
     *  Parse the mirrored master interface.
     *
     *      @param [in] offset      The current offset.
     *      @param [in] component   The component in use.
     *      @param [in] stream      The text stream to write into.
     *      @param [in] interFace   Identifies the current interface to parse.
     */
    void parseMirroredMasterInterface(qint64 offset, QSharedPointer<Component> component, QTextStream& stream,
        QSharedPointer<ActiveInterface> interface);

    /*!
     *  Creates the parameter finder using configurable element values of the instance.
     *
     *      @param [in] instanceID  The uuid of the instance of the configurable element values.
     *      @param [in] component   The component of the instance.
     *
     *      @return A list parameter finder containing the configurable element values of the instance.
     */
    QSharedPointer<ListParameterFinder> createParameterFinder(QString const& instanceID,
        QSharedPointer<Component> component);

    /*!
     *  Gets the uuid of an instance.
     *
     *      @param [in] interfaceReference  The name of the component instance.
     *
     *      @return Uuid of the instance.
     */
    QString getInstanceID(QString const& interfaceReference) const;

    /*!
     *  Get configurable elements from the selected instance.
     *
     *      @param [in] instanceId  The id of the selected instance.
     *
     *      @return Map containing the configurable elements from the selected instance as ID / value pairs.
     */
    QMap<QString, QString> getConfigurableElementsFromInstance(QString const& instanceId) const;

    /*!
     *  Get the connected interfaces.
     *
     *      @param [in] interface   The start point interface of the connection.
     *
     *      @return A list of interfaces connected to the selected interface.
     */
    QList<QSharedPointer<ActiveInterface> > getConnectedInterfaces(QSharedPointer<ActiveInterface> interface);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent object of this writer.
    QObject* parentObject_;

    //! The hw design of the component.
    QSharedPointer<Design> componentDesign_;

    //! The hw design configuration of the component.
    QSharedPointer<DesignConfiguration> componentDesignConfig_;

    //! A list of interfaces that have been checked.
    QList<QSharedPointer<ActiveInterface> > operatedInterfaces_;

    //! A list of save options for the writer.
    QList<GlobalHeaderSaveModel::SaveFileOptions*> saveOptions_;
};

#endif // GLOBALMEMORYMAPHEADERWRITER_H
