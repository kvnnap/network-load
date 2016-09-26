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
        void setMaxTimeSeconds(size_t p_maxTimeSeconds);

        // Getters
        float getStdConfidence() const;
        float getConfIntervalThreshold() const;
        size_t getMinIterations() const;
        size_t getMaxIterations() const;
        size_t getMaxTimeSeconds() const;
    private:
        float stdConfidence;
        float confidenceIntervalThreshold;
        size_t minIterations;
        size_t maxIterations;
        size_t maxTimeSeconds;
    };

    std::ostream& operator<< (std::ostream& strm, const ConfidenceInterval& ConfidenceInterval);

    class Benchmark {
    public:
        Benchmark();

        void addMessageInfo(const MessageInfo& messageInfo);
        void addGranularity(uint32_t granularity);
        void setMessageInfos(const std::vector<MessageInfo>& p_messageInfos);
        void setGranularities(const std::vector<uint32_t>& p_granularities);
        void setActive(bool p_active);
        void setName(const std::string& p_name);
        void setPointFile(const std::string& p_pointFile);
        void setPointInfoFile(const std::string& p_pointInfoFile);
        void setTikzFile(const std::string& p_tikzFile);

        const std::vector<MessageInfo>& getMessageInfos() const;
        const std::vector<uint32_t>& getGranularities() const;
        bool isActive() const;
        const std::string& getName() const;
        const std::string& getPointFile() const;
        const std::string& getPointInfoFile() const;
        const std::string& getTikzFile() const;
    private:
        std::vector<MessageInfo> messages;
        std::vector<uint32_t> granularities;
        std::string name;
        std::string pointFile;
        std::string pointInfoFile;
        std::string tikzFile;
        bool active;
    };

    std::ostream& operator<< (std::ostream& strm, const Benchmark& benchmark);

    class Configuration {
    public:
        Configuration();

        // Setters
        void setMessagingMethod(uint32_t p_messagingMethod);
        void setGranularity(uint32_t p_granularity);
        void setMessageInfo(const MessageInfo& p_messageInfo);
        void addStepPDF(const Sampler::StepPDF& p_stepPDF);
        void setConfidenceInterval(const ConfidenceInterval& p_confidenceInterval);
        void setBenchmark(const Benchmark& p_benchmark);

        // Getters
        uint32_t getGranularity() const;
        uint32_t getMessagingMethod() const;
        const MessageInfo& getMessageInfo() const;
        const std::vector<Sampler::StepPDF>& getStepPDFs() const;
        size_t getNodeSize() const;
        const ConfidenceInterval& getConfidenceInterval() const;
        const Benchmark& getBenchmark() const;

        // Non-const getters
        //Sampler::StepPDF& getStepPDF();
        Sampler::StepPDF& getStepPDF(size_t index = 0);

    private:
        uint32_t messagingMethod;
        uint32_t granularity;
        MessageInfo messageInfo;
        ConfidenceInterval confidenceInterval;
        std::vector<Sampler::StepPDF> stepPDFs;
        Benchmark benchmark;
    };

    std::ostream& operator<< (std::ostream& strm, const Configuration& conf);
}


#endif //NETWORK_LOAD_CONFIGURATION_H
