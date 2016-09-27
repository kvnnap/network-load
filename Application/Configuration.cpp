//
// Created by kevin on 9/22/16.
//

#include <ostream>

#include "Configuration.h"

using namespace std;
using namespace NetworkLoad;

MessageInfo::MessageInfo()
    : size (), length ()
{}

Configuration::Configuration()
    : messagingMethod(), granularity ()
{}

void MessageInfo::setMessageSize(uint32_t p_messageSize) {
    size = p_messageSize;
}

void MessageInfo::setMessageLength(uint32_t p_messageLength) {
    length = p_messageLength;
}

uint32_t MessageInfo::getMessageSize() const {
    return size;
}

uint32_t MessageInfo::getMessageLength() const {
    return length;
}

size_t Configuration::getNodeSize() const {
    return stepPDFs.size();
}

void Configuration::setGranularity(uint32_t p_granularity) {
    granularity = p_granularity;
}

void Configuration::addStepPDF(const Sampler::StepPDF &p_stepPDF) {
    stepPDFs.push_back(p_stepPDF);
}

uint32_t Configuration::getGranularity() const {
    return granularity;
}

const std::vector<Sampler::StepPDF> &Configuration::getStepPDFs() const {
    return stepPDFs;
}

void Configuration::setMessageInfo(const MessageInfo &p_messageInfo) {
    messageInfo = p_messageInfo;
}

const MessageInfo &Configuration::getMessageInfo() const {
    return messageInfo;
}

/*Sampler::StepPDF &Configuration::getStepPDF() {
    return stepPDFs.at(0);
}*/

Sampler::StepPDF &Configuration::getStepPDF(size_t index) {
    return stepPDFs.at(index);
}

void Configuration::setConfidence(const Confidence &p_confidence) {
    confidence = p_confidence;
}

const Confidence &Configuration::getConfidence() const {
    return confidence;
}

void Configuration::setMessagingMethod(uint32_t p_messagingMethod) {
    messagingMethod = p_messagingMethod;
}

uint32_t Configuration::getMessagingMethod() const {
    return messagingMethod;
}

void Configuration::setBenchmark(const Benchmark &p_benchmark) {
    benchmark = p_benchmark;
}

const Benchmark &Configuration::getBenchmark() const {
    return benchmark;
}

ostream& NetworkLoad::operator<<(ostream &strm, const Configuration &conf) {
    strm << "Granularity: " << conf.getGranularity() << endl
         << "Message: " << endl << conf.getMessageInfo() << endl
         << "Confidence: " << endl << conf.getConfidence() << endl
         << "Number of Nodes: " << conf.getNodeSize() << endl
         << "PDFs: " << endl;
    for (const Sampler::StepPDF& stepPDF : conf.getStepPDFs()) {
        strm << stepPDF << endl;
    }
    strm << "Benchmark: " << endl << conf.getBenchmark() << endl;
    return strm;
}

ostream& NetworkLoad::operator<<(ostream &strm, const MessageInfo &messageInfo) {
    strm << "Message Size/Count: " << messageInfo.getMessageSize() << endl
         << "Message Length: " << messageInfo.getMessageLength();
    return strm;
}

ostream &::NetworkLoad::operator<<(ostream &strm, const Benchmark &benchmark) {
    strm << "Active: " << (benchmark.isActive() ? "Yes" : "No") << endl
         << "MessageInfos: " << endl;
    for (const MessageInfo& messageInfo : benchmark.getMessageInfos()) {
        strm << messageInfo << endl;
    }
    strm << "Granularities: " << endl;
    for (uint32_t gran : benchmark.getGranularities()) {
        strm << gran << endl;
    }
    return strm;
}

Confidence::Confidence()
    : stdConfidence (), confidenceIntervalThreshold(), minIterations (), maxIterations (), maxTimeSeconds ()
{}

void Confidence::setStdConfidence(float p_stdConfidence) {
    stdConfidence = p_stdConfidence;
}

void Confidence::setConfIntervalThreshold(float p_cIT) {
    confidenceIntervalThreshold = p_cIT;
}

void Confidence::setMinIterations(size_t p_minIterations) {
    minIterations = p_minIterations;
}

void Confidence::setMaxIterations(size_t p_maxIterations) {
    maxIterations = p_maxIterations;
}

float Confidence::getStdConfidence() const {
    return stdConfidence;
}

float Confidence::getConfIntervalThreshold() const {
    return confidenceIntervalThreshold;
}

size_t Confidence::getMinIterations() const {
    return minIterations;
}

size_t Confidence::getMaxIterations() const {
    return maxIterations;
}

void Confidence::setMaxTimeSeconds(size_t p_maxTimeSeconds) {
    maxTimeSeconds = p_maxTimeSeconds;
}

size_t Confidence::getMaxTimeSeconds() const {
    return maxTimeSeconds;
}

ostream& NetworkLoad::operator<<(ostream &strm, const Confidence &confidence) {
    strm << "Std Confidence: " << confidence.getStdConfidence() << endl
         << "Confidence Threshold: " << confidence.getConfIntervalThreshold() << endl
         << "Min Iterations: " << confidence.getMinIterations() << endl
         << "Max Iterations: " << confidence.getMaxIterations() << endl
         << "Timeout (s): " << confidence.getMaxTimeSeconds();
    return strm;
}

void Benchmark::addMessageInfo(const MessageInfo &messageInfo) {
    messages.push_back(messageInfo);

}

void Benchmark::addGranularity(uint32_t granularity) {
    granularities.push_back(granularity);
}

const vector<MessageInfo> &Benchmark::getMessageInfos() const {
    return messages;
}

const vector<uint32_t> &Benchmark::getGranularities() const {
    return granularities;
}

void Benchmark::setActive(bool p_active) {
    active = p_active;
}

bool Benchmark::isActive() const {
    return active;
}

void Benchmark::setMessageInfos(const std::vector<MessageInfo> &p_messageInfos) {
    messages = p_messageInfos;
}

void Benchmark::setGranularities(const std::vector<uint32_t> &p_granularities) {
    granularities = p_granularities;
}

Benchmark::Benchmark()
    : active ()
{}

void Benchmark::setName(const std::string &p_name) {
    name = p_name;
}

void Benchmark::setPointFile(const std::string &p_pointFile) {
    pointFile = p_pointFile;
}

void Benchmark::setTikzFile(const std::string &p_tikzFile) {
    tikzFile = p_tikzFile;
}

const std::string &Benchmark::getName() const {
    return name;
}

const std::string &Benchmark::getPointFile() const {
    return pointFile;
}

const std::string &Benchmark::getTikzFile() const {
    return tikzFile;
}

const std::string &Benchmark::getPointInfoFile() const {
    return pointInfoFile;
}

void Benchmark::setPointInfoFile(const std::string &p_pointInfoFile) {
    pointInfoFile = p_pointInfoFile;
}
