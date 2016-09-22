//
// Created by kevin on 9/22/16.
//

#include "Configuration.h"

using namespace NetworkLoad;

MessageInfo::MessageInfo()
    : size (), length ()
{}

Configuration::Configuration()
    : granularity ()
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


