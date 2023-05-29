//-----------------------------------------------------------------------------
// File: InstanceData.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 10.04.2019
//
// Description:
// Model containing the parsed instance data.
//-----------------------------------------------------------------------------

#ifndef INSTANCEDATA_H
#define INSTANCEDATA_H

#include <QString>
#include <QVector>
#include <QMetaType>

//-----------------------------------------------------------------------------
//! Model containing the parsed instance data.
//-----------------------------------------------------------------------------
namespace InstanceData
{
    //! The parsed instance data.
    struct instanceData
    {
        //! Name.
        QString instanceName_;

        //! Module name.
        QString moduleName_;

        //! Contained parameters.
        QVector<QPair<QString, QString> > parameters_;

        //! Contained port connections.
        QVector<QPair<QString, QString> > portConnections_;
    };

    //! Reserved vocabulary for instance data.
    const std::string VERILOGINSTANCES("verilogInstances");
    const std::string SINGLEINSTANCE("instance");

    const std::string INSTANCENAME("instanceName");
    const std::string MODULENAME("moduleName");
    const std::string PARAMETERS("parameters");
    const std::string SINGLEPARAMETER("parameter");
    const std::string CONNECTIONS("connections");
    const std::string SINGLECONNECTION("connection");

    const std::string SUBITEMNAME("subItemName");
    const std::string SUBITEMVALUE("subItemValue");
};

Q_DECLARE_METATYPE(QVector<InstanceData::instanceData>)

#endif // INSTANCEDATA_H
