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

#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QFileInfo>
#include <QList>
#include <QTextStream>

class IPluginUtility;
class ListParameterFinder;
class ConnectivityInterface;

//-----------------------------------------------------------------------------
//! The implementation for creating c-headers of global memory maps.
//-----------------------------------------------------------------------------
class GlobalMemoryMapHeaderWriter : public BaseMemoryMapHeaderWriter
{

public:

    /*!
     *  The constructor.
     *
     *    @param [in] utility                 The interface which offers core utilities for plugins.
     *    @param [in] componentDesign         The design currently in use.
     *    @param [in] componentDesignConfig   The design configuration currently in use.
     *    @param [in] parentObject            The parent object of this writer.
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
     *    @param [in] globalComponent     The top component of the design.
     *    @param [in] saveOptions         The save options for the memory map header.
     */
    virtual void writeMemoryMapHeader(QSharedPointer<Component> globalComponent,
        QList<QSharedPointer<GlobalHeaderSaveModel::SaveFileOptions> > saveOptions);

protected:
    
    /*!
     *  Display the created memory map header.
     *
     *    @param [in] filePath    The file path.
     */
    virtual void displayMemoryMapHeader(QString const& filePath) const;

private:

	//! No copying
	//! No assignment
    GlobalMemoryMapHeaderWriter(const GlobalMemoryMapHeaderWriter& other);
    GlobalMemoryMapHeaderWriter& operator=(const GlobalMemoryMapHeaderWriter& other);

    /*!
     *  Setup the CPU route containers for existing save file options.
     *
     *    @param [in] options       The existing save file options.
     *    @param [in] cpuRoutes     The located CPU route containers.
     */
    void setupRoutesForExistingOptions(
        QList<QSharedPointer<GlobalHeaderSaveModel::SaveFileOptions> > options,
        QVector<QSharedPointer<SingleCpuRoutesContainer> > cpuRoutes);

    /*!
     *  Get the matching save file option for the selected CPU container.
     *
     *    @param [in] options       List of available save file options.
     *    @param [in] singleCPU     The selected CPU container.
     *
     *    @return The matching save file option.
     */
    QSharedPointer<GlobalHeaderSaveModel::SaveFileOptions> getSaveOptionForCPU(
        QList<QSharedPointer<GlobalHeaderSaveModel::SaveFileOptions> > options,
        QSharedPointer<SingleCpuRoutesContainer> singleCPU);

    /*!
     *  Parse the CPU route.
     *
     *    @param [in] stream        Text stream to write into.
     *    @param [in] container     Container for address calculations.
     */
    void parseRoutes(QTextStream& stream, QSharedPointer<SingleCpuRoutesContainer> container);

    /*!
     *  Parse the master interface.
     *
     *    @param [in] addressContainer  Container for address calculations.
     *    @param [in] masterInterface   The selected master interface.
     */
	void parseMasterInterface(AddressContainer& addressContainer, QSharedPointer<const ConnectivityInterface> masterInterface) const;

    /*!
     *  Parse the slave interface.
     *
     *    @param [in] addressContainer  Container for address calculations.
     *    @param [in] stream            The text stream to write into.
     *    @param [in] targetInterFace   The selected target interface.
     */
	void parseTargetInterface(AddressContainer addressContainer, QTextStream& stream,
		QSharedPointer<const ConnectivityInterface> targetInterface);

    /*!
     *  Check if the selected memory item contains address blocks.
     *
     *    @param [in] memoryItem    The selected memory item.
     *
     *    @return True, if the memory item contains address blocks, false otherwise.
     */
    bool memoryItemContainsAddressBlocks(QSharedPointer<MemoryItem> memoryItem) const;

    /*!
     *  Parse the mirrored slave interface.
     *
     *    @param [in] addressContainer          Container for address calculations.
     *    @param [in] mirroredTargetInterFace   The selected mirrored target interface.
     */
	void parseMirroredTargetInterface(AddressContainer& addressContainer,
		QSharedPointer<const ConnectivityInterface> mirroredTargetInterface) const;

    /*!
     *  Creates the parameter finder using configurable element values of the instance.
     *
     *    @param [in] instanceID  The uuid of the instance of the configurable element values.
     *    @param [in] component   The component of the instance.
     *
     *    @return A list parameter finder containing the configurable element values of the instance.
     */
    QSharedPointer<ListParameterFinder> createParameterFinder(QString const& instanceID,
        QSharedPointer<Component> component);

    /*!
     *  Gets the uuid of an instance.
     *
     *    @param [in] interfaceReference  The name of the component instance.
     *
     *    @return Uuid of the instance.
     */
    QString getInstanceID(QString const& interfaceReference) const;

    /*!
     *  Get configurable elements from the selected instance.
     *
     *    @param [in] instanceId  The id of the selected instance.
     *
     *    @return Map containing the configurable elements from the selected instance as ID / value pairs.
     */
    QMap<QString, QString> getConfigurableElementsFromInstance(QString const& instanceId) const;

    /*!
     *  Get the connected interfaces.
     *
     *    @param [in] interface   The start point interface of the connection.
     *
     *    @return A list of interfaces connected to the selected interface.
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
    QList<QSharedPointer<GlobalHeaderSaveModel::SaveFileOptions> > saveOptions_;
};

#endif // GLOBALMEMORYMAPHEADERWRITER_H
