//
// Created by kevin on 9/22/16.
//

#ifndef NETWORK_LOAD_CONFIGURATION_H
#define NETWORK_LOAD_CONFIGURATION_H

#include <cstdint>

#include "Sampler/StepPDF.h"

namespace NetworkLoad {

    class MessageInfo {
    public:
        MessageInfo();

        void setMessageSize(uint32_t p_messageSize);
        void setMessageLength(uint32_t p_messageLength);

        uint32_t getMessageSize() const;
        uint32_t getMessageLength() const;

    private:
        uint32_t size;
        uint32_t length;
    };

    std::ostream& operator<< (std::ostream& strm, const MessageInfo& messageInfo);

    class Configuration {
    public:
        Configuration();

        void setGranularity(uint32_t p_granularity);
        void setMessageInfo(const MessageInfo& p_messageInfo);
        void addStepPDF(const Sampler::StepPDF& p_stepPDF);

        uint32_t getGranularity() const;
        const MessageInfo& getMessageInfo() const;
        const std::vector<Sampler::StepPDF>& getStepPDFs() const;
        Sampler::StepPDF& getStepPDF();
        size_t getNodeSize() const;

    private:
        uint32_t granularity;
        MessageInfo messageInfo;
        std::vector<Sampler::StepPDF> stepPDFs;
    };

    std::ostream& operator<< (std::ostream& strm, const Configuration& conf);
}


#endif //NETWORK_LOAD_CONFIGURATION_H
