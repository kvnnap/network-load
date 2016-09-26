//
// Created by kevin on 9/22/16.
//

#ifndef NETWORK_LOAD_CONFIGURATIONFACTORY_H
#define NETWORK_LOAD_CONFIGURATIONFACTORY_H

#include <vector>

#include "Factory/IFactory.h"
#include "Configuration.h"

#include "XML/rapidxml.hpp"

namespace NetworkLoad {

    class ConfigurationFactory
        : public IFactory<std::vector<Configuration>>
    {
    public:
        std::unique_ptr<std::vector<Configuration>> make(const std::string& fileName) const override;

    private:
        static Configuration getNetLoad(rapidxml::xml_node<>* netLoadNode);
        static uint32_t getGranularity(rapidxml::xml_node<>* granularityNode);
        static MessageInfo getMessageInfo(rapidxml::xml_node<>* messageNode);
        static ConfidenceInterval getConfidenceInterval(rapidxml::xml_node<>* confidenceIntervalNode);
        static Sampler::StepPDF getPDSF(rapidxml::xml_node<>* psdfNode);
        static uint32_t getValue (const std::string& value);

        // For benchmark
        static Benchmark getBenchmark(rapidxml::xml_node<>* benchmarkNode);
        static std::vector<MessageInfo> getMessageInfos(rapidxml::xml_node<>* messagesNode);
        static std::vector<uint32_t> getGranularities(rapidxml::xml_node<>* granularityNode);

    };

}


#endif //NETWORK_LOAD_CONFIGURATIONFACTORY_H
