//
// Created by kevin on 9/23/16.
//

#ifndef NETWORK_LOAD_APPLICATION_H
#define NETWORK_LOAD_APPLICATION_H

#include <cstdint>
#include <cstddef>
#include <array>
#include <mpi.h>
#include "Configuration.h"
#include "Result.h"
#include "Results.h"

namespace NetworkLoad {

    class Application {
    public:
        Application(int argc, char **argv);
        ~Application();

        // Synchronisation
        void syncRanksAndPrint();

        // Benchmark
        std::vector<Results> startMultipleBenchmarks();

        // Get
        size_t getRank() const;

    private:
        // reset
        void resetConfiguration();

        // Configuration
        void loadConfiguration(const std::string &configurationFileName);
        void syncConfiguration();

        // Messaging and confidence
        double startMessaging();
        Result gatherConfidentMessage();

        // Benchmark
        Results startBenchmark();

        // Data
        std::vector<Configuration> configurations;
        Configuration configuration;
        std::array<char, MPI_MAX_PROCESSOR_NAME> processorName;
        size_t size, rank;
        std::string hostNames;
    };

}

#endif //NETWORK_LOAD_APPLICATION_H
