//
// Created by kevin on 9/23/16.
//

#include <iostream>
#include <chrono>
#include <algorithm>
#include <limits>
#include <fstream>
#include <sstream>

#include "Application.h"
#include "ConfigurationFactory.h"
#include "Mathematics/Statistics.h"
#include "Result.h"

using namespace std;
using namespace Mathematics;
using namespace NetworkLoad;

Application::Application(int argc, char **argv) {
    MPI::Init(argc, argv);
    //MPI::COMM_WORLD.Set_errhandler(MPI::ERRORS_THROW_EXCEPTIONS);

    int mSize = MPI::COMM_WORLD.Get_size();
    int mRank = MPI::COMM_WORLD.Get_rank();

    if (mSize < 0 || mRank < 0) {
        throw runtime_error ("Invalid size and/or rank");
    }

    // Gather information
    size = static_cast<size_t>(mSize);
    rank = static_cast<size_t>(mRank);

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
    stringstream sStr;
    if (rank == 0) {
        sStr << processorName.data();
//        vector<string> names;
//        names.push_back(processorName.data());
        // Receive
        array<char, MPI_MAX_PROCESSOR_NAME> name;
        for (size_t i = 1; i < size; ++i) {
            MPI::COMM_WORLD.Recv(name.data(), MPI_MAX_PROCESSOR_NAME, MPI::CHAR, i, 0);
            sStr << ", " << name.data();
            //names.push_back(name.data());
        }
        // Check for duplicates

    } else {
        // Send ours
        MPI::COMM_WORLD.Send(processorName.data(), MPI_MAX_PROCESSOR_NAME, MPI::CHAR, 0, 0);
    }

    hostNames = sStr.str();
    cout << hostNames << endl;
}

// Tag is 1
void Application::syncConfiguration() {
    if (rank == 0) {
        // Master - assuming Configuration already loaded
        // Send pdf to other slaves
        std::array<uint32_t, 5> metaData = {
                configuration.getGranularity(),
                configuration.getMessageInfo().getMessageLength(),
                configuration.getMessageInfo().getMessageSize(),
                static_cast<uint32_t>(configuration.getNodeSize()),
                configuration.getMessagingMethod()
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
        std::array<uint32_t, 5> metaData;
        MPI::COMM_WORLD.Bcast(metaData.data(), metaData.size(), MPI::UNSIGNED, 0);
        configuration.setGranularity(metaData[0]);
        messageInfo.setMessageLength(metaData[1]);
        messageInfo.setMessageSize(metaData[2]);
        configuration.setMessageInfo(messageInfo);
        configuration.setMessagingMethod(metaData[4]);

        vector<uint32_t> vals (metaData[3], 0);
        // Receive PDF
        MPI::COMM_WORLD.Recv(vals.data(), size, MPI::UNSIGNED, 0, 1);
        configuration.addStepPDF(Sampler::StepPDF(vals));
    }
}

// Tags used are 2, 3, 4 and 5
double Application::startMessaging() {
    using namespace chrono;

    if (configuration.getMessagingMethod() == 3 || configuration.getMessagingMethod() == 4) {

        vector<uint32_t> localSourceIdList;
        vector<char> buff(configuration.getMessageInfo().getMessageLength());
        iota(buff.begin(), buff.end(), 0);

        if (rank == 0) {
            // Generate sequence of messages
            vector<pair<uint32_t , uint32_t>> messageList;

            for (uint32_t p = 0; p < size; ++p) {
                Sampler::StepPDF &pdfI = configuration.getStepPDF(p);
                for (size_t i = 0; i < configuration.getMessageInfo().getMessageSize(); ++i) {
                    ssize_t index = pdfI.getNext();
                    if (index >= 0) {
                        if (static_cast<uint32_t>(index) == p) {
                            throw runtime_error("Can't send message to self");
                        }
                        messageList.push_back({p, static_cast<uint32_t>(index)});
                    }
                }
            }

            // Shuffle messageList?
            if (configuration.getMessagingMethod() == 4) {
                random_shuffle(messageList.begin(), messageList.end());
            }

            // Send relevant messages pairs
            vector<vector<uint32_t>> sourceIDs(size);
            for (const pair<uint32_t, uint32_t> &message : messageList) {
                // The sender
                sourceIDs.at(message.first).push_back(message.first);
                sourceIDs.at(message.first).push_back(message.second);
                // The receiver
                sourceIDs.at(message.second).push_back(message.first);
                sourceIDs.at(message.second).push_back(message.second);
            }

            // Runtime Check
            if (sourceIDs.size() > numeric_limits<uint32_t>::max()) {
                throw runtime_error("Too many messages - increase data type or reduce amount");
            }

            // Send the source id's to each rank
            for (size_t i = 1; i < sourceIDs.size(); ++i) {
                uint32_t sourceIdListSize = static_cast<uint32_t>(sourceIDs[i].size());
                MPI::COMM_WORLD.Send(&sourceIdListSize, 1, MPI::UNSIGNED, i, 4);
                MPI::COMM_WORLD.Send(sourceIDs[i].data(), sourceIdListSize, MPI::UNSIGNED, i, 5);
            }

            localSourceIdList = sourceIDs[0];

        } else {
            uint32_t sourceIdListSize;
            MPI::COMM_WORLD.Recv(&sourceIdListSize, 1, MPI::UNSIGNED, 0, 4);
            localSourceIdList.resize(sourceIdListSize, 0);
            MPI::COMM_WORLD.Recv(localSourceIdList.data(), sourceIdListSize, MPI::UNSIGNED, 0, 5);
        }

        // Make sure everyone is here..
        MPI::COMM_WORLD.Barrier();

        // Setup timer
        high_resolution_clock::time_point t1 = high_resolution_clock::now();

        // Send/Receive messsages
        for (size_t i = 0; i < localSourceIdList.size(); i += 2) {
            if (localSourceIdList[i] == rank) {
                // Granularity
                for (size_t gran = 0; gran < configuration.getGranularity(); ++gran) {
                    ++buff[gran % buff.size()];
                }
                // we must send
                MPI::COMM_WORLD.Send(buff.data(), buff.size(), MPI::CHAR, localSourceIdList.at(i + 1), 3);
            } else if (localSourceIdList.at(i + 1) == rank) {
                // we must receive
                MPI::COMM_WORLD.Recv(buff.data(), buff.size(), MPI::CHAR, localSourceIdList[i], 3);
            } else {
                // Sanity check
                throw runtime_error("Mismatching Message Pair: Rank " + to_string(rank) + ": - "
                                    + to_string(localSourceIdList[i]) + " --> " + to_string(localSourceIdList[i + 1]));
            }
        }

        // Final barrier
        MPI::COMM_WORLD.Barrier();

        // Execution time
        high_resolution_clock::time_point t2 = high_resolution_clock::now();

        // calculate duration
        duration<double, milli> fMilliSec = t2 - t1;

        return fMilliSec.count();

    } else {

        // Warning
        if (rank == 0) {
            cout << "Using old messaging method. This is not a good model!" << endl;
        }
        // Generate sequence of destinations
        vector<size_t> destinations;
        for (size_t i = 0; i < configuration.getMessageInfo().getMessageSize(); ++i) {
            ssize_t index = configuration.getStepPDF().getNext();
            if (index >= 0) {
                if (static_cast<size_t>(index) == rank) {
                    throw runtime_error("Can't send message to self");
                }
                destinations.push_back(static_cast<size_t>(index));
            }
        }

        // Generate Counts so that other nodes know how many messages to expect
        // From this rank
        vector<size_t> sendCounts(size, 0);
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
        vector<size_t> receiveCounts(size, 0);
        for (size_t i = 0; i < receiveCounts.size(); ++i) {
            if (i != rank) {
                MPI::COMM_WORLD.Recv(&receiveCounts[i], 1, MPI::UNSIGNED, i, 2);
                totalCounts += receiveCounts[i];
            }
        }

        //
        uint32_t method = configuration.getMessagingMethod();

        vector<char> buff(configuration.getMessageInfo().getMessageLength());
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
            throw runtime_error("Unknown Method: " + to_string(method));
        }

        // Final barrier
        MPI::COMM_WORLD.Barrier();

        // Execution time
        high_resolution_clock::time_point t2 = high_resolution_clock::now();

        // calculate duration
        duration<double, milli> fMilliSec = t2 - t1;

        return fMilliSec.count();
    }
}

// No Tags - using broadcast
Result Application::gatherConfidentMessage() {
    Result result;

    if (rank == 0) {
        vector<double> executionTimes;

        size_t minMeasurements = configuration.getConfidenceInterval().getMinIterations();
        size_t maxTimeSeconds = configuration.getConfidenceInterval().getMaxTimeSeconds();

        result.setT1();

        do {
            // Send confidence flag
            MPI::COMM_WORLD.Bcast(&result.confident, 1, MPI::BOOL, 0);

            // Compute
            executionTimes.push_back(startMessaging());
            result.setT2(static_cast<double>(maxTimeSeconds));

            if ((executionTimes.size() == minMeasurements) || result.timeout) {
                // Do confidence check
                // Do twice more measurements if not confident enough
                minMeasurements *= 2;

                // Check if we're confident enough this time round
                result.computeConfidenceMetrics(configuration, executionTimes);
            }
        } while (!result.confident && !result.timeout &&
                 (configuration.getConfidenceInterval().getMaxIterations() == 0 ||
                  executionTimes.size() < configuration.getConfidenceInterval().getMaxIterations()));

        result.messageInfo = configuration.getMessageInfo();
        result.granularity = configuration.getGranularity();
        result.computeMean(executionTimes);

        // Fake confidence flag and send it
        bool confident = true;
        MPI::COMM_WORLD.Bcast(&confident, 1, MPI::BOOL, 0);

        MPI::COMM_WORLD.Barrier();
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
    }

    return result;
}

Results Application::startBenchmark() {
    Results results;
    uint32_t rounds;

    if (rank == 0) {
        // Set configuration -- assuming already loaded
        results.setConfiguration(configuration);
        results.setHostName(hostNames);

        // Get reference to benchmark
        const Benchmark& benchmark = configuration.getBenchmark();

        // Standard run
        if (!benchmark.isActive()) {
            rounds = 1;
            MPI::COMM_WORLD.Bcast(&rounds, 1, MPI::UNSIGNED, 0);
            syncConfiguration();
            results.addResult(gatherConfidentMessage());
            return results;
        }

        // Benchmark
        rounds = static_cast<uint32_t>(benchmark.getGranularities().size() * benchmark.getMessageInfos().size());
        MPI::COMM_WORLD.Bcast(&rounds, 1, MPI::UNSIGNED, 0);

        for (uint32_t granularity : benchmark.getGranularities()) {
            configuration.setGranularity(granularity);
            for (const MessageInfo& messageInfo : benchmark.getMessageInfos()) {
                configuration.setMessageInfo(messageInfo);
                // Do a run
                syncConfiguration();
                results.addResult(gatherConfidentMessage());
            }
        }

        // Export to File
        results.exportTikzPlot(configuration.getBenchmark().getName(), configuration.getBenchmark().getTikzFile(),
                               configuration.getBenchmark().getPointFile());
        results.exportPointsInfo(configuration.getBenchmark().getName(), configuration.getBenchmark().getPointInfoFile());
    } else {
        MPI::COMM_WORLD.Bcast(&rounds, 1, MPI::UNSIGNED, 0);
        for (uint32_t i = 0; i < rounds; ++i) {
            syncConfiguration();
            gatherConfidentMessage();
        }
    }

    return results;
}

size_t Application::getRank() const {
    return rank;
}

void Application::loadConfiguration(const string &configurationFileName) {
    if (configurationFileName.size() == 0) {
        throw runtime_error ("Source XML file for configuration not set");
    }
    vector<Configuration> localConfigurations = *ConfigurationFactory().make(configurationFileName);
    for (const Configuration& conf : localConfigurations) {
        if (conf.getNodeSize() != size) {
            throw runtime_error("Mismatch with number of nodes in XML and on the cluster");
        }
    }

    // Append to our own
    configurations.insert(configurations.end(), localConfigurations.begin(), localConfigurations.end());
}

void Application::resetConfiguration() {
    configuration = Configuration();
}

std::vector<Results> Application::startMultipleBenchmarks() {
    // read XML file locator file
    vector<Results> results;

    if (rank == 0) {
        vector<string> xmlFileNames;
        // Read file
        {
            ifstream input("XmlFileLocations.txt");
            for (std::string line; getline(input, line);) {
                if (line.size() > 4) {
                    xmlFileNames.push_back(line);
                }
            }
        }

        // Load number of benchmarks
        for (const string &xmlFileName : xmlFileNames) {
            loadConfiguration(xmlFileName);
        }

        uint32_t numberOfBenchmarks = configurations.size();
        MPI::COMM_WORLD.Bcast(&numberOfBenchmarks, 1, MPI::UNSIGNED, 0);

        // Start
        size_t i = 0;
        for (const Configuration & conf : configurations) {

            // Print progress
            cout << "Benchmark: " << ++i << " out of " << configurations.size() << endl;

            // reset app - nothing much
            resetConfiguration();

            // start!
            configuration = conf;
            results.push_back(startBenchmark());
        }
    } else {
        uint32_t numberOfBenchmarks;
        MPI::COMM_WORLD.Bcast(&numberOfBenchmarks, 1, MPI::UNSIGNED, 0);
        for (uint32_t benchmarkNumber = 0; benchmarkNumber < numberOfBenchmarks; ++benchmarkNumber) {
            // Slaves do not need resetting of configuration variables, but let's simulate a clean Application object
            resetConfiguration();
            results.push_back(startBenchmark());
        }
    }

    return results;
}
