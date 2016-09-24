//
// Created by kevin on 9/23/16.
//

#include <iostream>
#include <chrono>

#include "Application.h"
#include "ConfigurationFactory.h"
#include "Mathematics/Statistics.h"

using namespace std;
using namespace Mathematics;
using namespace NetworkLoad;

Application::Application(int argc, char **argv) {
    MPI::Init(argc, argv);
    //MPI::COMM_WORLD.Set_errhandler(MPI::ERRORS_THROW_EXCEPTIONS);

    // Gather information
    size = MPI::COMM_WORLD.Get_size();
    rank = MPI::COMM_WORLD.Get_rank();

    // Get Our Name
    int len;
    MPI::Get_processor_name(processorName.data(), len);
    if (len < MPI_MAX_PROCESSOR_NAME) {
        processorName[len] = '\0';
    }
}

Application::~Application() {
    MPI::Finalize();
}

void Application::syncRanksAndPrint() {
    // Gather host names and print them - tag is 0
    if (rank == 0) {
        cout << "Ranks: " << processorName.data();
//        vector<string> names;
//        names.push_back(processorName.data());
        // Receive
        array<char, MPI_MAX_PROCESSOR_NAME> name;
        for (size_t i = 1; i < size; ++i) {
            MPI::COMM_WORLD.Recv(name.data(), MPI_MAX_PROCESSOR_NAME, MPI::CHAR, i, 0);
            cout << ", " << name.data();
            //names.push_back(name.data());
        }
        cout << endl;
        // Check for duplicates

    } else {
        // Send ours
        MPI::COMM_WORLD.Send(processorName.data(), MPI_MAX_PROCESSOR_NAME, MPI::CHAR, 0, 0);
    }
}

// Tag is 1
void Application::syncConfiguration() {
    if (rank == 0) {
        // Master
        // Read Xml
        configuration = *ConfigurationFactory().make("configuration.xml");
        if (configuration.getNodeSize() != size) {
            throw runtime_error ("Mismatch with number of nodes in XML and on the cluster");
        }
        // Send pdf to other slaves
        std::array<uint32_t, 4> metaData = {
                configuration.getGranularity(),
                configuration.getMessageInfo().getMessageLength(),
                configuration.getMessageInfo().getMessageSize(),
                static_cast<uint32_t>(configuration.getNodeSize())
        };
        // Send Meta data
        MPI::COMM_WORLD.Bcast(metaData.data(), metaData.size(), MPI::UNSIGNED, 0);
        // Send PDSFs
        for (size_t i = 1; i < size; ++i) {
            MPI::COMM_WORLD.Send(configuration.getStepPDFs().at(i).getPDSF().data(), size, MPI::UNSIGNED, i, 1);
        }
    } else {
        MessageInfo messageInfo;
        // Others
        std::array<uint32_t, 4> metaData;
        MPI::COMM_WORLD.Bcast(metaData.data(), metaData.size(), MPI::UNSIGNED, 0);
        configuration.setGranularity(metaData[0]);
        messageInfo.setMessageLength(metaData[1]);
        messageInfo.setMessageSize(metaData[2]);
        configuration.setMessageInfo(messageInfo);

        vector<uint32_t> vals (metaData[3], 0);
        // Receive PDF
        MPI::COMM_WORLD.Recv(vals.data(), size, MPI::UNSIGNED, 0, 1);
        configuration.addStepPDF(Sampler::StepPDF(vals));
    }
//    if (rank == 1) {
//        cout << configuration << endl;
//    }
}

// Tags used are 2 and 3
float Application::startMessaging() {
    using namespace chrono;
    // Generate sequence of destinations
    vector<size_t> destinations;
    for (size_t i = 0; i < configuration.getMessageInfo().getMessageSize(); ++i) {
        ssize_t index = configuration.getStepPDF().getNext();
        if (index == rank) {
            throw runtime_error ("Can't send message to self");
        }
        if (index >= 0) {
            destinations.push_back(static_cast<size_t>(index));
        }
    }

    // Generate Counts so that other nodes know how many messages to expect
    // From this rank
    vector<size_t> sendCounts (size, 0);
    for (size_t i : destinations) {
        ++sendCounts.at(i);
    }

    // Communicate sendCounts
    for (size_t i = 0; i < sendCounts.size(); ++i) {
        if (i != rank) {
            MPI::COMM_WORLD.Send(&sendCounts[i], 1, MPI::UNSIGNED, i, 2);
        }
    }

    // Receive sendCounts from other ranks
    size_t totalCounts = 0;
    vector<size_t> receiveCounts (size, 0);
    for (size_t i = 0; i < receiveCounts.size(); ++i) {
        if (i != rank) {
            MPI::COMM_WORLD.Recv(&receiveCounts[i], 1, MPI::UNSIGNED, i, 2);
            totalCounts += receiveCounts[i];
        }
    }

    // Communicate method
    uint32_t method = configuration.getMessagingMethod();
    MPI::COMM_WORLD.Bcast(&method, 1, MPI::UNSIGNED, 0);

    vector<char> buff (configuration.getMessageInfo().getMessageLength());
    iota(buff.begin(), buff.end(), 0);

    // Make sure everyone is here..
    MPI::COMM_WORLD.Barrier();

    // Setup timer
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    // Start messaging - Tag 3
    if (method == 0) {
        for (size_t destination : destinations) {
            MPI::COMM_WORLD.Send(buff.data(), buff.size(), MPI::CHAR, destination, 3);
            // Granularity
            for (size_t gran = 0; gran < configuration.getGranularity(); ++gran) {
                ++buff[gran % buff.size()];
            }
        }
        // Receive
        for (size_t i = 0; i < totalCounts; ++i) {
            MPI::COMM_WORLD.Recv(buff.data(), buff.size(), MPI::CHAR, MPI::ANY_SOURCE, 3);
        }
    } else if (method == 1 || method == 2) {
        // These method make sure messages are sent by a single node at any point in time
        for (size_t i = 0; i < receiveCounts.size(); ++i) {
            if (i != rank) {
                // receive receiveCounts[i] messages from rank i - receiveCounts[rank] is 0
                for (size_t r = 0; r < receiveCounts[i]; ++r) {
                    MPI::COMM_WORLD.Recv(buff.data(), buff.size(), MPI::CHAR, i, 3);
                }
            } else {
                // Our turn to send
                if (method == 1) { // Shuffled send (normal)
                    for (size_t destination : destinations) {
                        MPI::COMM_WORLD.Send(buff.data(), buff.size(), MPI::CHAR, destination, 3);
                        // Granularity
                        for (size_t gran = 0; gran < configuration.getGranularity(); ++gran) {
                            ++buff[gran % buff.size()];
                        }
                    }
                } else if (method == 2) { // Directed send
                    for (size_t j = 0; j < sendCounts.size(); ++j) {
                        // send sendCounts[j] messages to rank j - sendCounts[rank] is 0, no need to check
                        if (j != rank) {
                            for (size_t s = 0; s < sendCounts[j]; ++s) {
                                MPI::COMM_WORLD.Send(buff.data(), buff.size(), MPI::CHAR, j, 3);
                                // Granularity
                                for (size_t gran = 0; gran < configuration.getGranularity(); ++gran) {
                                    ++buff[gran % buff.size()];
                                }
                            }
                        }
                    }
                }
            }
        }
    } else {
        throw runtime_error ("Unknown Method: " + to_string(method));
    }

    // Final barrier
    MPI::COMM_WORLD.Barrier();

    // Execution time
    high_resolution_clock::time_point t2 = high_resolution_clock::now();

    // calculate duration
    duration<float, milli> fMilliSec = t2 - t1;

    return fMilliSec.count();
}

// No Tags - using broadcast
float Application::gatherConfidentMessage() {
    if (rank == 0) {
        vector<float> executionTimes;
        bool confident = false;
        size_t minMeasurements = configuration.getConfidenceInterval().getMinIterations();
        do {
            // Send confidence flag
            MPI::COMM_WORLD.Bcast(&confident, 1, MPI::BOOL, 0);

            // Compute
            executionTimes.push_back(startMessaging());

            if (executionTimes.size() == minMeasurements) {
                // Do confidence check
                // Do twice more measurements if not confident enough
                minMeasurements *= 2;

                // Check if we're confident enough this time round
                const float sampleStdDeviation = Statistics::SampleStdDeviation(executionTimes);
                const float stdDeviationOfTheMean = Statistics::StdDeviationOfTheMean(sampleStdDeviation,
                                                                                      executionTimes.size());
                const float confidenceInterval =
                        stdDeviationOfTheMean * configuration.getConfidenceInterval().getStdConfidence();
                confident = confidenceInterval <= configuration.getConfidenceInterval().getConfIntervalThreshold();
            }
        } while (!confident &&
                 (configuration.getConfidenceInterval().getMaxIterations() == 0 ||
                  executionTimes.size() < configuration.getConfidenceInterval().getMaxIterations()));

        const float confidentMean = Statistics::Mean(executionTimes);
        if (confident) {
            cout << "Confident Mean: " << confidentMean << endl;
        } else {
            cout << "Cut-Off Mean: " << confidentMean << endl;
        }

        // Reuse confidence flag and send it
        confident = true;
        MPI::COMM_WORLD.Bcast(&confident, 1, MPI::BOOL, 0);

        MPI::COMM_WORLD.Barrier();

        return confidentMean;
    } else {
        bool confident;
        do {
            // Read resume flag
            MPI::COMM_WORLD.Bcast(&confident, 1, MPI::BOOL, 0);
            if (!confident) {
                startMessaging();
            }
        } while (!confident);
        MPI::COMM_WORLD.Barrier();
        return 0.f;
    }
}
