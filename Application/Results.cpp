//
// Created by kevin on 9/26/16.
//

#include <fstream>
#include "Results.h"

using namespace std;
using namespace NetworkLoad;

ostream& ::NetworkLoad::operator<<(ostream &strm, const Results &results) {
    strm << "Host Names (Ranks): " << results.getHostNames() << endl
         << "Configuration: " << endl << results.getConfiguration() << endl
         << "Result[s]: " << endl;
    size_t i = 0;
    for (const Result& result : results.getResults()) {
        strm << "Result/Datum #" << ++i << endl << result << endl;
    }
    return strm;
}

void Results::addResult(const Result& result) {
    results.push_back(result);
}

void Results::setConfiguration(const Configuration &p_configuration) {
    configuration = p_configuration;
}

const Configuration &Results::getConfiguration() const {
    return configuration;
}

const std::vector<Result> &Results::getResults() const {
    return results;
}

void Results::exportTikzPlot(const string &name, const string &fileName, const string &pointFileName) const {
    exportPoints(pointFileName);
    if (fileName.size() == 0) {
        return;
    }
    string plot;
    plot += "% Benchmark Name: " + name + "\n";
    plot += "\\begin{tikzpicture}\n";
    plot += "\t\\begin{axis}[view={-20}{20}, grid=both]\n";
    plot += "\t\t\\addplot3[surf] file {" + pointFileName + "};\n";
    plot += "\t\t\\addplot3[surf, point meta=explicit] table [z expr=0.5, meta index=2] {filename.txt};\n";
    plot += "\t\\end{axis}\n";
    plot += "\\end{tikzpicture}\n";
    ofstream (fileName) << plot;
}

void Results::exportPoints(const std::string &fileName) const {
    if (fileName.size() == 0) {
        return;
    }
    ofstream file (fileName);
    for (const Result& result : results) {
        file << result.exportDatum();
    }
}

void Results::exportPointsInfo(const string &name, const string &pointInfoFileName) const {
    if (pointInfoFileName.size() == 0) {
        return;
    }
    ofstream (pointInfoFileName) << "Benchmark Name: " << name << endl << *this;
}

void Results::setHostName(const std::string &p_hostNames) {
    hostNames = p_hostNames;
}

const std::string &Results::getHostNames() const {
    return hostNames;
}
