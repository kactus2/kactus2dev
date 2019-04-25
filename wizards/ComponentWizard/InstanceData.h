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
    const QString VERILOGINSTANCES("verilogInstances");
    const QString SINGLEINSTANCE("instance");

    const QString INSTANCENAME("instanceName");
    const QString MODULENAME("moduleName");
    const QString PARAMETERS("parameters");
    const QString SINGLEPARAMETER("parameter");
    const QString CONNECTIONS("connections");
    const QString SINGLECONNECTION("connection");

    const QString SUBITEMNAME("subItemName");
    const QString SUBITEMVALUE("subItemValue");
};

Q_DECLARE_METATYPE(QVector<InstanceData::instanceData>)

#endif // INSTANCEDATA_H
