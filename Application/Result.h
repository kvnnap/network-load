//
// Created by kevin on 9/26/16.
//

#ifndef NETWORK_LOAD_RESULT_H
#define NETWORK_LOAD_RESULT_H

#include <ostream>
#include <chrono>
#include <vector>
#include "Configuration.h"

namespace NetworkLoad {

    class Result {
    public:

        Result();

        // Export logic
        std::string exportDatum() const;

        // Should business logic be here?
        void computeMean(const std::vector<double>& executionTimes);
        void computeConfidenceMetrics(const Configuration& configuration, const std::vector<double>& executionTimes);
        void setT1();
        void setT2(double maxDuration);

        // Data
        std::chrono::high_resolution_clock::time_point t1;
        std::chrono::high_resolution_clock::time_point t2;

        // Point Data
        MessageInfo messageInfo;
        uint32_t granularity;
        double mean;

        double sampleStdDeviation;
        double stdDeviationOfTheMean;
        double confidenceInterval;
        double runDuration;
        size_t iterations;
        bool confident;
        bool timeout;
    };

    std::ostream& operator<< (std::ostream& strm, const Result& result);
}


#endif //NETWORK_LOAD_RESULT_H
