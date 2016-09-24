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

    class ConfidenceInterval {
    public:
        ConfidenceInterval();

        // Setters
        void setStdConfidence(float p_stdConfidence);
        void setConfIntervalThreshold(float p_cIT);
        void setMinIterations(size_t p_minIterations);
        void setMaxIterations(size_t p_maxIterations);

        // Getters
        float getStdConfidence() const;
        float getConfIntervalThreshold() const;
        size_t getMinIterations() const;
        size_t getMaxIterations() const;
    private:
        float stdConfidence;
        float confidenceIntervalThreshold;
        size_t minIterations;
        size_t maxIterations;
    };

    std::ostream& operator<< (std::ostream& strm, const ConfidenceInterval& ConfidenceInterval);

    class Configuration {
    public:
        Configuration();

        // Setters
        void setMessagingMethod(uint32_t p_messagingMethod);
        void setGranularity(uint32_t p_granularity);
        void setMessageInfo(const MessageInfo& p_messageInfo);
        void addStepPDF(const Sampler::StepPDF& p_stepPDF);
        void setConfidenceInterval(const ConfidenceInterval& p_confidenceInterval);

        // Getters
        uint32_t getGranularity() const;
        uint32_t getMessagingMethod() const;
        const MessageInfo& getMessageInfo() const;
        const std::vector<Sampler::StepPDF>& getStepPDFs() const;
        size_t getNodeSize() const;
        const ConfidenceInterval& getConfidenceInterval() const;

        // Non-const getters
        Sampler::StepPDF& getStepPDF();

    private:
        uint32_t messagingMethod;
        uint32_t granularity;
        MessageInfo messageInfo;
        ConfidenceInterval confidenceInterval;
        std::vector<Sampler::StepPDF> stepPDFs;
    };

    std::ostream& operator<< (std::ostream& strm, const Configuration& conf);
}


#endif //NETWORK_LOAD_CONFIGURATION_H
