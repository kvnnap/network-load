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

namespace NetworkLoad {

    class Application {
    public:
        Application(int argc, char **argv);
        ~Application();

        void syncRanksAndPrint();
        void syncConfiguration();

        float startMessaging();

    private:
        Configuration configuration;
        std::array<char, MPI_MAX_PROCESSOR_NAME> processorName;
        int size, rank;
    };

}

#endif //NETWORK_LOAD_APPLICATION_H
