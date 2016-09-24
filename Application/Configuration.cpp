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

Sampler::StepPDF &Configuration::getStepPDF() {
    return stepPDFs.at(0);
}

void Configuration::setConfidenceInterval(const ConfidenceInterval &p_confidenceInterval) {
    confidenceInterval = p_confidenceInterval;
}

const ConfidenceInterval &Configuration::getConfidenceInterval() const {
    return confidenceInterval;
}

void Configuration::setMessagingMethod(uint32_t p_messagingMethod) {
    messagingMethod = p_messagingMethod;
}

uint32_t Configuration::getMessagingMethod() const {
    return messagingMethod;
}

ostream& NetworkLoad::operator<<(ostream &strm, const Configuration &conf) {
    strm << "Granularity: " << conf.getGranularity() << endl
         << "Message: " << conf.getMessageInfo() << endl
         << "ConfidenceInterval: " << conf.getConfidenceInterval() << endl;
    for (const Sampler::StepPDF& stepPDF : conf.getStepPDFs()) {
        strm << stepPDF << endl;
    }
    return strm;
}

ostream& NetworkLoad::operator<<(ostream &strm, const MessageInfo &messageInfo) {
    strm << "Message Size: " << messageInfo.getMessageSize() << endl
         << "Message Length: " << messageInfo.getMessageLength();
    return strm;
}

ConfidenceInterval::ConfidenceInterval()
    : stdConfidence (), confidenceIntervalThreshold(), minIterations (), maxIterations ()
{}

void ConfidenceInterval::setStdConfidence(float p_stdConfidence) {
    stdConfidence = p_stdConfidence;
}

void ConfidenceInterval::setConfIntervalThreshold(float p_cIT) {
    confidenceIntervalThreshold = p_cIT;
}

void ConfidenceInterval::setMinIterations(size_t p_minIterations) {
    minIterations = p_minIterations;
}

void ConfidenceInterval::setMaxIterations(size_t p_maxIterations) {
    maxIterations = p_maxIterations;
}

float ConfidenceInterval::getStdConfidence() const {
    return stdConfidence;
}

float ConfidenceInterval::getConfIntervalThreshold() const {
    return confidenceIntervalThreshold;
}

size_t ConfidenceInterval::getMinIterations() const {
    return minIterations;
}

size_t ConfidenceInterval::getMaxIterations() const {
    return maxIterations;
}

ostream& NetworkLoad::operator<<(ostream &strm, const ConfidenceInterval &confidenceInterval) {
    strm << "Std Confidence: " << confidenceInterval.getStdConfidence() << endl
         << "Confidence Threshold: " << confidenceInterval.getConfIntervalThreshold() << endl
         << "Min Iterations: " << confidenceInterval.getMinIterations() << endl
         << "Max Iterations: " << confidenceInterval.getMaxIterations() << endl;
    return strm;
}