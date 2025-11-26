#include "ConfigJsonParser.h"
#include <QtXml>

using namespace InterconnectGeneration;

ConfigJsonParser::ConfigJsonParser(){}

ConfigStruct* ConfigJsonParser::readFile() {

    QFile configFile(path_);

    if(configFile.open(QIODevice::ReadOnly)){

        QByteArray configBytes = configFile.readAll();
        configFile.close();

        QJsonParseError jsonError;
        QJsonDocument document = QJsonDocument::fromJson( configBytes, &jsonError );

        if(jsonError.error == QJsonParseError::NoError) {

            if(document.isObject()) {
                QJsonObject jsonObj = document.object();

                config_.interconVLNV = jsonObj.value("intercon").toString();
                config_.designVLNV = jsonObj.value("top").toString();
                config_.busVLNV = jsonObj.value("bus").toString();
                config_.clkVLNV = jsonObj.value("clk").toString();
                config_.rstVLNV = jsonObj.value("rst").toString();

                config_.busType = strToBusType(jsonObj.value("busType").toString());
                config_.addressWidth = jsonObj.value("addressWidth").toString(); // corresponds to parsedAddressWidth in config struct
                config_.dataWidth = jsonObj.value("dataWidth").toString();
                config_.idWidth = jsonObj.value("idWidth").toInt();
                config_.userWidth = jsonObj.value("userWidth").toInt();

                QJsonArray initListArr = jsonObj.value("initiators").toArray();
                config_.initList.resize(initListArr.size());

                for(int n=0; n<initListArr.size(); n++){
                    QJsonObject initObj = initListArr.at(n).toObject();
                    InitStruct init;

                    init.index = initObj.value("index").toInt();
                    init.name = initObj.value("name").toString();
                    //init.dataWidth = initObj.value("Data width").toInt();

                    config_.initList[init.index] = init;
               }

                QJsonArray targetListArr = jsonObj.value("targets").toArray();
                config_.targetList.resize(targetListArr.size());

                for(int i=0; i<targetListArr.size(); i++){
                    QJsonObject targetObj = targetListArr.at(i).toObject();
                    TargetStruct target;

                    target.index = targetObj.value("index").toInt();
                    target.name = targetObj.value("name").toString();
                    //target.dataWidth = targetObj.value("Data width").toInt();

                    QJsonArray addrRegionArr = targetObj.value("addressRegions").toArray();

                    for(int k=0; k<addrRegionArr.size(); ++k){
                        QJsonObject addrObj = addrRegionArr.at(k).toObject();
                        AddressPair addrPair;
                        addrPair.start = addrObj.value("start").toString();
                        addrPair.end = addrObj.value("end").toString();
                        target.addressRegions.append(addrPair);
                    }
                    config_.targetList[target.index] = target;
                }
            }
        }
    }
    return &config_;
}

//-----------------------------------------------------------------------------
// Function: ConfigJsonParser::strToBusType()
//-----------------------------------------------------------------------------
InterconnectGeneration::BusType InterconnectGeneration::ConfigJsonParser::strToBusType(QString const& busTypeStr)
{
    auto busTypeStrUpper = busTypeStr.toUpper();
    if (busTypeStrUpper.compare("AXI4") == 0)
    {
        return BusType::AXI4;
    }
    else if (busTypeStrUpper.compare("AXI4LITE") == 0)
    {
        return BusType::AXI4LITE;
    }
    else if (busTypeStrUpper.compare("OBI") == 0)
    {
        return BusType::OBI;
    }

    return BusType::UNKNOWN;
}

//-----------------------------------------------------------------------------
// Function: ConfigJsonParser::busTypeToStr()
//-----------------------------------------------------------------------------
QString ConfigJsonParser::busTypeToStr(BusType busType)
{
    switch (busType)
    {
    case BusType::AXI4:
        return QString("AXI4");
    case BusType::AXI4LITE:
        return QString("AXI4LITE");
    case BusType::OBI:
        return QString("OBI");
    default:
        return QString("Unknown");
    }
}
