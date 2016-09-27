//
// Created by kevin on 9/22/16.
//

#include "ConfigurationFactory.h"

#include "XML/rapidxml_utils.hpp"

#include <sstream>

using namespace std;
using namespace rapidxml;
using namespace NetworkLoad;

unique_ptr<vector<Configuration>> ConfigurationFactory::make(const string &fileName) const {

    // Open document
    unique_ptr<file<>> xmlFile(new file<>(fileName.c_str()));
    xml_document<> doc;
    doc.parse<0>(xmlFile->data());

    // Multiple netLoad nodes
    unique_ptr<vector<Configuration>> netLoadNodes ( new vector<Configuration>() );
    for (xml_node<> *child = doc.first_node(); child; child = child->next_sibling()) {
        (*netLoadNodes).push_back(getNetLoad(child));
    }
    return netLoadNodes;

    /*// Net load node
    unique_ptr<Configuration> conf ( new Configuration() );
    *conf = getNetLoad(doc.first_node());
    return conf;*/
}

Configuration ConfigurationFactory::getNetLoad(rapidxml::xml_node<> *netLoadNode) {
    Configuration conf;
    for (xml_node<> *child = netLoadNode->first_node(); child; child = child->next_sibling()) {
        string childName (child->name());
        if (childName == "messagingmethod") {
            conf.setMessagingMethod(getValue(child->value()));
        } else if (childName == "granularity") {
            conf.setGranularity(getGranularity(child));
        } else if (childName == "confidence") {
            conf.setConfidence(getConfidence(child));
        } else if (childName == "message") {
            conf.setMessageInfo(getMessageInfo(child));
        } else if (childName == "benchmark") {
            conf.setBenchmark(getBenchmark(child));
        } else if (childName == "nodes") {
            for (xml_node<> *childNode = child->first_node(); childNode; childNode = childNode->next_sibling()) {
                //
                string childNodeName (childNode->name());
                if (childNodeName == "pdsf") {
                    conf.addStepPDF(getPDSF(childNode));
                } else {
                    throw runtime_error("Malformed XML File. Invalid XML Node: " + childNodeName);
                }
            }

            // Attr -- Sanity check
            for (xml_attribute<> *attr = child->first_attribute(); attr; attr = attr->next_attribute())
            {
                string attrName (attr->name());
                if (attrName == "size") {
                    if (conf.getNodeSize() != getValue(attr->value())) {
                        throw runtime_error("Malformed XML File. Node size and the number of PDSF are different");
                    }
                } else {
                    throw runtime_error("Malformed XML File. Invalid XML Attribute: " + attrName);
                }
            }
        } else {
            throw runtime_error("Malformed XML File. Invalid XML Node: " + childName);
        }
    }
    return conf;
}

uint32_t ConfigurationFactory::getGranularity(xml_node<> *granularityNode) {
    return getValue(granularityNode->value());
}

MessageInfo ConfigurationFactory::getMessageInfo(xml_node<> *messageNode) {
    MessageInfo messageInfo;
    for (xml_attribute<> *attr = messageNode->first_attribute(); attr; attr = attr->next_attribute()) {
        string attrName (attr->name());
        if (attrName == "size") {
            messageInfo.setMessageSize(getValue(attr->value()));
        } else if (attrName == "length") {
            messageInfo.setMessageLength(getValue(attr->value()));
        } else {
            throw runtime_error("Malformed XML File. Invalid XML Attribute: " + attrName);
        }
    }
    return messageInfo;
}

Sampler::StepPDF ConfigurationFactory::getPDSF(rapidxml::xml_node<> *psdfNode) {
    vector<uint32_t> vals;
    for (xml_node<> *child = psdfNode->first_node(); child; child = child->next_sibling()) {
        string childName (child->name());
        if (childName == "val") {
            vals.push_back(getValue(child->value()));
        } else {
            throw runtime_error("Malformed XML File. Invalid XML Node: " + childName);
        }
    }
    return Sampler::StepPDF(vals);
}

uint32_t ConfigurationFactory::getValue(const std::string &value) {
    // return stoul(value);
    uint32_t val;
    istringstream strVal (value);
    if (strVal >> val) {
        return val;
    }
    throw runtime_error("Cannot convert " + value + " to uint32_t");
}

Confidence ConfigurationFactory::getConfidence(rapidxml::xml_node<> *confidenceNode) {
    Confidence confidence;
    for (xml_attribute<> *attr = confidenceNode->first_attribute(); attr; attr = attr->next_attribute()) {
        string attrName (attr->name());
        if (attrName == "stdConfidence") {
            confidence.setStdConfidence(stof(attr->value()));
        } else if (attrName == "confidenceIntervalThreshold") {
            confidence.setConfIntervalThreshold(stof(attr->value()));
        } else if (attrName == "minIterations") {
            confidence.setMinIterations(stoul(attr->value()));
        } else if (attrName == "maxIterations") {
            confidence.setMaxIterations(stoul(attr->value()));
        } else if (attrName == "maxTimeSeconds") {
            confidence.setMaxTimeSeconds(stoul(attr->value()));
        } else {
            throw runtime_error("Malformed XML File. Invalid XML Attribute: " + attrName);
        }
    }
    return confidence;
}

Benchmark ConfigurationFactory::getBenchmark(rapidxml::xml_node<> *benchmarkNode) {
    Benchmark benchmark;

    // Parse attributes
    for (xml_attribute<> *attr = benchmarkNode->first_attribute(); attr; attr = attr->next_attribute()) {
        string attrName(attr->name());
        if (attrName == "active") {
            benchmark.setActive(string(attr->value()) == "true");
        } else if (attrName == "name") {
            benchmark.setName(attr->value());
        } else if (attrName == "pointFile") {
            benchmark.setPointFile(attr->value());
        } else if (attrName == "pointInfoFile") {
            benchmark.setPointInfoFile(attr->value());
        } else if (attrName == "tikzFile") {
            benchmark.setTikzFile(attr->value());
        } else {
            throw runtime_error("Malformed XML File. Invalid XML Attribute: " + attrName);
        }
    }

    // Parse children
    for (xml_node<> *child = benchmarkNode->first_node(); child; child = child->next_sibling()) {
        string childName (child->name());
        if (childName == "messages") {
            benchmark.setMessageInfos(getMessageInfos(child));
        } else if (childName == "granularities") {
            benchmark.setGranularities(getGranularities(child));
        } else {
            throw runtime_error("Malformed XML File. Invalid XML Node: " + childName);
        }
    }

    return benchmark;
}

vector<MessageInfo> ConfigurationFactory::getMessageInfos(rapidxml::xml_node<> *messagesNode) {
    vector<MessageInfo> messages;
    for (xml_node<> *child = messagesNode->first_node(); child; child = child->next_sibling()) {
        string childName (child->name());
        if (childName == "message") {
            messages.push_back(getMessageInfo(child));
        } else {
            throw runtime_error("Malformed XML File. Invalid XML Node: " + childName);
        }
    }
    return messages;
}

vector<uint32_t> ConfigurationFactory::getGranularities(rapidxml::xml_node<> *granularityNode) {
    vector<uint32_t > granularities;
    for (xml_node<> *child = granularityNode->first_node(); child; child = child->next_sibling()) {
        string childName (child->name());
        if (childName == "granularity") {
            granularities.push_back(getGranularity(child));
        } else {
            throw runtime_error("Malformed XML File. Invalid XML Node: " + childName);
        }
    }
    return granularities;
}


