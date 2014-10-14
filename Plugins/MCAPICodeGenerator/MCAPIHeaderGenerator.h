//-----------------------------------------------------------------------------
// File: MCAPIHeaderGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 13.10.2014
//
// Description:
// MCAPI generator for instance specific headers.
//-----------------------------------------------------------------------------

#ifndef MCAPIHeaderGenerator_H
#define MCAPIHeaderGenerator_H

#include <IPXACTmodels/ComDefinition.h>
#include <IPXACTmodels/ComInterface.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <Plugins/PluginSystem/IPluginUtility.h>

#include <MCAPIParser.h>

class Component;
class CSourceWriter;

//-----------------------------------------------------------------------------
//! MCAPI code generator.
//-----------------------------------------------------------------------------
class MCAPIHeaderGenerator
{
public:
    /*!
     *  The constructor.
     *
     *      @param [in] parser   A parser that has already parsed the design for MCAPI generation.
     *      @param [in] utility   The same IPluginUtility, which is fed to the plugin class as parameter.
     */
     MCAPIHeaderGenerator( MCAPIParser& parser, IPluginUtility* utility );
    ~MCAPIHeaderGenerator();

    /*!
     *  Generates source files associated with the top level component of the design.
     *
     *      @param [in] design   The design subject to generation.
     *      @param [in] topComponent   The top level component associated with the design.
     *      @param [in] desgConf   The design configuration associated with the design.
     *      @param [in] topDir   The location of the top level component in the file system.
     */
     void generateTopLevel(QSharedPointer<Design> design, QSharedPointer<Component> topComponent,
        QSharedPointer<DesignConfiguration const> desgConf, QString topDir);

private:

    /*!
     *  Creates the indent string based on the application settings.
     */
    QString createIndentString();

    /*!
     *  Generates a header file containing data specific to a software instance.
     *
     *      @param [in] filename   The name of the source file to write.
     *      @param [in] nodeData   MCAPI node data associated with the instance.
     */
    void generateInstanceHeader(QString& directory, MCAPIParser::NodeData& nodeData);

    /*!
     *  Add generated MCAPI code files to the fileSet of the associated component.
     *
     *      @param [in] directory   The directory, where the files are inserted on the file system.
     *      @param [in] topComponent   The top level component associated with the design.
     *      @param [in] instance   The software instance, which instance header was generated.
     *      @param [in] desgConf   The design configuration associated with the design.
     */
     void addGeneratedMCAPIToFileset(QString directory, QSharedPointer<Component> topComponent,
        SWInstance& instance, QSharedPointer<DesignConfiguration const> desgConf);

    /*!
     *  Find connections of given software instance and returns a list of pairs, where the first is from the
     *  end of the given instance, and second the other end.
     *
     *      @param [in] design   The design where the software instance belongs to.
     *      @param [in] ourInstance   The software instance, which connections are listed.
     *      @param [in] component   The software component of ourInstance.
     *      @return List of "our" interfaces paired with their connected interfaces.
     */
     QList<QPair<QSharedPointer<ComInterface>, ComInterfaceRef> > findConnectedComInterfaces(
        QSharedPointer<const Design> design, SWInstance &ourInstance, QSharedPointer<Component> component );

     /*!
     *  Writes remote endpoint of 'ourInterface' and endpoint definitions of both given interfaces.
     *  The definitions are appended to the list 'endpointDefs'.
      *
      *      @param [in] writer   The source writer.
      *      @param [in] ourEnd   Data associated with end point in "our" end of connection.
      *      @param [in] theirEnd   Data associated with end point in "their" end of connection.
      *      @param [in] endpointDefs   The list of endpoint definition names.
      */
      void writeRemoteEndpoint(CSourceWriter &writer, MCAPIParser::EndPointData ourEnd,
          MCAPIParser::EndPointData theirEnd, QStringList& endpointDefs);

    /*!
     *  Writes the given endpoint definition names to array.
     *
     *      @param [in] writer   The source writer.
     *      @param [in] endpointDefs   The list of endpoint definition names.
     */
     void writeEndpointDefList(CSourceWriter &writer, QStringList &endpointDefs);

    /*!
     *  Converts direction string of Kactus2 to enum used by PMQ-MCAPI.
     *
     *      @param [in] direction   Direction string to be converted.
     *      @return The created enum.
     */
     QString transferDirectionToEString(General::Direction direction);

    /*!
     *  Converts transfer type string of Kactus2 to enum used by PMQ-MCAPI.
     *
     *      @param [in] transferType   Transfer type string to be converted.
     *      @return The created enum.
     */
     QString transferTypeToEString(QString transferType);

    /*!
     *  Makes an endpoint identifier understood by PMQ-MCAPI.
     *
     *      @param [in] domainID   The domain of the endpoint.
     *      @param [in] nodeID   The node of the endpoint.
     *      @param [in] portID   The port of the endpoint.
     *      @return The created string.
     */
     QString makeIdString(QString domainID, QString nodeID, QString portID);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Endpoints parsed from component
    QList<MCAPIParser::EndPointData> componentEndpoints_;
    //! Nodes parsed from design.
    QList<MCAPIParser::NodeData> designNodes_;
    //! The plugin utility.
    IPluginUtility* utility_;
};

#endif // MCAPIHeaderGenerator_H
