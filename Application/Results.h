//
// Created by kevin on 9/26/16.
//

#ifndef NETWORK_LOAD_RESULTS_H
#define NETWORK_LOAD_RESULTS_H

#include "Result.h"

namespace NetworkLoad {

    class Results {
    public:

        // Export Logic
        void
        exportTikzPlot(const std::string &name, const std::string &fileName, const std::string &pointFileName) const;
        void exportPoints(const std::string& fileName) const;
        void exportPointsInfo(const std::string &name, const std::string &pointInfoFileName) const;

        void addResult(const Result& result);
        void setConfiguration(const Configuration& p_configuration);
        void setHostName(const std::string& p_hostNames);

        const Configuration& getConfiguration() const;
        const std::vector<Result>& getResults() const;
        const std::string& getHostNames() const;

    private:
        Configuration configuration;
        std::vector<Result> results;
        std::string hostNames;
    };

    std::ostream& operator<< (std::ostream& strm, const Results& results);
}

#endif //NETWORK_LOAD_RESULTS_H
