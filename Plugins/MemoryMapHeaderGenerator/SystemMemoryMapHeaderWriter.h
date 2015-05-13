//-----------------------------------------------------------------------------
// File: SystemMemoryMapHeaderWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.05.2015
//
// Description:
// The implementation for creating c-headers of a system, including other generated code for the HW platform.
//-----------------------------------------------------------------------------

#ifndef SYSTEMMEMORYMAPHEADERWRITER_H
#define SYSTEMMEMORYMAPHEADERWRITER_H

#include "BaseMemoryMapHeaderWriter.h"

#include "systemheadersavemodel.h"

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/SystemView.h>

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QFileInfo>
#include <QList>
#include <QTextStream>

class IPluginUtility;

//-----------------------------------------------------------------------------
//! The base implementation for memory map header writer.
//-----------------------------------------------------------------------------
class SystemMemoryMapHeaderWriter : public BaseMemoryMapHeaderWriter
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] utility         The interface which offers core utilites for plugins.
     *      @param [in] parentObject    The parent object of this writer.
     */
    SystemMemoryMapHeaderWriter(IPluginUtility* utility, QObject* parentObject);

    /*!
     *  The destructor.
     */
    virtual ~SystemMemoryMapHeaderWriter();

    /*!
     *  Write the memory map header file.
     *
     *      @param [in] component               The component whose header file is being written.
     *      @param [in] designConfiguration     The design configuration for the component.
     *      @param [in] design                  The designg for the component.
     */
    virtual void writeMemoryMapHeader(QSharedPointer<Component> component,
        QSharedPointer<DesignConfiguration> designConfiguration, QSharedPointer<Design> design);

protected:

private:

	//! No copying
    SystemMemoryMapHeaderWriter(const SystemMemoryMapHeaderWriter& other);

	//! No assignment
    SystemMemoryMapHeaderWriter& operator=(const SystemMemoryMapHeaderWriter& other);

	/*!
	 *  Search the CPU instances and their containing component to find the generated global headers.
	 *
	 *      @param [in] component   Pointer to the hierarchical component to search.
	 *      @param [in] hwViewName  The name of the hierarchical view on the component.
	 */
	void searchInstanceFiles(QSharedPointer<const Component> component, const QString& hwViewName);

    /*!
     *  Find the system view currently used in the system design.
     *
     *      @param [in] component               The component whose system view is in use.
     *      @param [in] designConfiguration     The design configuration of the component.
     */
    QSharedPointer<SystemView> findSystemView(QSharedPointer<Component> component,
        QSharedPointer<DesignConfiguration> designConfiguration) const;

    //! Used to save info on instances and their active views to be parsed.
    struct HierParsingInfo {

        //! Identifies the component.
        QSharedPointer<const Component> comp_;

        //! The view which is set for the component.
        QString activeView_;

        //! The constructor.
        HierParsingInfo(QSharedPointer<const Component> comp, const QString& viewName);

        //! Copy constructor. No deep copy needed for the pointer.
        HierParsingInfo(const HierParsingInfo& other);

        //! Assignment operator. No deep copy needed for the pointer.
        HierParsingInfo& operator=(const HierParsingInfo& other);
    };

    //! The parent object of this writer.
    QObject* parentObject_;

    //! The system header options.
    QList<SystemHeaderSaveModel::SysHeaderOptions> systemGeneratorSettings_;
};

#endif // SYSTEMMEMORYMAPHEADERWRITER_H
