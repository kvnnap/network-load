//
// Created by kevin on 9/22/16.
//

#include "ConfigurationFactory.h"

#include "XML/rapidxml_utils.hpp"

#include <sstream>

using namespace std;
using namespace rapidxml;
using namespace NetworkLoad;

unique_ptr<Configuration> ConfigurationFactory::make(const string &fileName) const {

    // Open document
    unique_ptr<file<>> xmlFile(new file<>(fileName.c_str()));
    xml_document<> doc;
    doc.parse<0>(xmlFile->data());


    // Net load node
    unique_ptr<Configuration> conf ( new Configuration() );
    *conf = getNetLoad(doc.first_node());
    return conf;
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
            conf.setConfidenceInterval(getConfidenceInterval(child));
        } else if (childName == "message") {
            conf.setMessageInfo(getMessageInfo(child));
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

ConfidenceInterval ConfigurationFactory::getConfidenceInterval(rapidxml::xml_node<> *confidenceIntervalNode) {
    ConfidenceInterval confidenceInterval;
    for (xml_attribute<> *attr = confidenceIntervalNode->first_attribute(); attr; attr = attr->next_attribute()) {
        string attrName (attr->name());
        if (attrName == "stdConfidence") {
            confidenceInterval.setStdConfidence(stof(attr->value()));
        } else if (attrName == "confidenceIntervalThreshold") {
            confidenceInterval.setConfIntervalThreshold(stof(attr->value()));
        } else if (attrName == "minIterations") {
            confidenceInterval.setMinIterations(stoul(attr->value()));
        } else if (attrName == "maxIterations") {
            confidenceInterval.setMaxIterations(stoul(attr->value()));
        } else {
            throw runtime_error("Malformed XML File. Invalid XML Attribute: " + attrName);
        }
    }
    return confidenceInterval;
}
