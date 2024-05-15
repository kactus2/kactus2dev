#include "ConfigJsonParser.h"
#include <QtXml>

ConfigJsonParser::ConfigJsonParser(){}

ConfigJsonParser::ConfigStruct* ConfigJsonParser::readFile() {

    QFile configFile(path_);

    if(configFile.open(QIODevice::ReadOnly)){

        QByteArray configBytes = configFile.readAll();
        configFile.close();

        QJsonParseError jsonError;
        QJsonDocument document = QJsonDocument::fromJson( configBytes, &jsonError );

        if(jsonError.error == QJsonParseError::NoError) {

            if(document.isObject()) {
                QJsonObject jsonObj = document.object();

                config_.InterconVLNV = jsonObj.value("intercon").toString();
                config_.DesignVLNV = jsonObj.value("top").toString();
                config_.BusVLNV = jsonObj.value("bus").toString();
                config_.ClkVLNV = jsonObj.value("clk").toString();
                config_.RstVLNV = jsonObj.value("rst").toString();

                config_.AddressWidth = jsonObj.value("Address width").toInt();
                config_.IDWidth = jsonObj.value("ID width").toInt();
                config_.UserWidth = jsonObj.value("User width").toInt();

                QJsonArray compListArr = jsonObj.value("Targets").toArray();

                for(int i=0; i<compListArr.size(); i++){
                    QJsonObject targetObj = compListArr.at(i).toObject();
                    TargetStruct target;

                    target.Index = targetObj.value("Index").toInt();
                    target.Name = targetObj.value("Name").toString();
                    target.DataWidth = targetObj.value("Data width").toInt();

                    QJsonArray addrRegionArr = targetObj.value("Address regions").toArray();

                    for(int k=0; k<addrRegionArr.size(); ++k){
                        QJsonObject addrObj = addrRegionArr.at(k).toObject();
                        AddressPair addrPair;
                        addrPair.Start = addrObj.value("Start").toString();
                        addrPair.End = addrObj.value("End").toString();
                        target.AddressRegions.append(addrPair);
                    }
                    config_.TargetList.append(target);
                }
            }
        }
    }
    return &config_;
}
