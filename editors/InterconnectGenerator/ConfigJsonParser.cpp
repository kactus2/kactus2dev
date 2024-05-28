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

                config_.BusType = jsonObj.value("Bus type").toString();
                config_.AddressWidth = jsonObj.value("Address width").toInt();
                config_.IDWidth = jsonObj.value("ID width").toInt();
                config_.UserWidth = jsonObj.value("User width").toInt();

                QJsonArray initListArr = jsonObj.value("Initiators").toArray();
                config_.InitList.resize(initListArr.size());

                for(int n=0; n<initListArr.size(); n++){
                    QJsonObject initObj = initListArr.at(n).toObject();
                    InitStruct init;

                    init.Index = initObj.value("Index").toInt();
                    init.Name = initObj.value("Name").toString();
                    init.DataWidth = initObj.value("Data width").toInt();

                    config_.InitList[init.Index] = init;
               }

                QJsonArray targetListArr = jsonObj.value("Targets").toArray();
                config_.TargetList.resize(targetListArr.size());

                for(int i=0; i<targetListArr.size(); i++){
                    QJsonObject targetObj = targetListArr.at(i).toObject();
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
                    config_.TargetList[target.Index] = target;
                }
            }
        }
    }
    return &config_;
}
