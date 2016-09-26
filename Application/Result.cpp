//
// Created by kevin on 9/26/16.
//

#include <sstream>

#include "Mathematics/Statistics.h"
#include "Result.h"


using namespace std;
using namespace std::chrono;
using namespace NetworkLoad;
using namespace Mathematics;

NetworkLoad::Result::Result()
    : granularity (),
      mean (),
      sampleStdDeviation(),
      stdDeviationOfTheMean(),
      confidenceInterval (),
      runDuration (),
      iterations (),
      confident (),
      timeout ()
{}

void Result::computeConfidenceMetrics(const Configuration& configuration, const vector<double> &executionTimes) {
    sampleStdDeviation = Statistics::SampleStdDeviation(executionTimes);
    stdDeviationOfTheMean = Statistics::StdDeviationOfTheMean(sampleStdDeviation, executionTimes.size());
    confidenceInterval = stdDeviationOfTheMean * configuration.getConfidenceInterval().getStdConfidence();
    confident = confidenceInterval <= configuration.getConfidenceInterval().getConfIntervalThreshold();
}

void Result::computeMean(const std::vector<double> &executionTimes) {
    mean = Statistics::Mean(executionTimes);
    iterations = executionTimes.size();
}

void Result::setT2(double maxDuration) {
    t2 = high_resolution_clock::now();
    duration<double> fMilliSec = t2 - t1;
    runDuration = fMilliSec.count();
    timeout = runDuration >= maxDuration;
}

void Result::setT1() {
    t1 = high_resolution_clock::now();
}

string Result::exportDatum() const {
    string points;
    stringstream sPoints;
    sPoints << granularity << " " << messageInfo.getMessageLength() << " " << mean /*<< " vs " << to_string(mean)*/ << endl;
    return sPoints.str();
    /*points += to_string(granularity);
    points += ' ';
    points += to_string(messageInfo.getMessageLength());
    points += ' ';
    points += to_string(mean);
    points += '\n';
    return points;*/
}

ostream &::NetworkLoad::operator<<(ostream &strm, const NetworkLoad::Result &result) {
    strm << "Mean Time (ms): " << result.mean << endl
         << "Granularity: " << result.granularity << endl
         << "Message Length (bytes): " << result.messageInfo.getMessageLength() << endl
         << "Message Count: " << result.messageInfo.getMessageSize() << endl
         << "Type: " << (result.confident ? "Confident Mean" : "Cut-Off Mean") << endl
         << "Timed Out: " << (result.timeout ? "Yes" : "No") << endl
         << "Confidence Interval: " << result.confidenceInterval << endl
         << "Mean Variance: " << (result.stdDeviationOfTheMean * result.stdDeviationOfTheMean) << endl
         << "Data Variance: " << (result.sampleStdDeviation * result.sampleStdDeviation) << endl
         << "Iterations: " << result.iterations << endl
         << "Run Duration (s): " << result.runDuration << endl;
    return strm;
}
