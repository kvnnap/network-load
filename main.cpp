#include <iostream>
#include <string>
#include <mpi.h>
#include <cstddef>
#include <Application/Application.h>

#include "Application/ConfigurationFactory.h"

using namespace std;
using namespace NetworkLoad;

int main(int argc, char **argv) {

    Application application (argc, argv);

    try {
        application.syncRanksAndPrint();
        application.syncConfiguration();
        application.gatherConfidentMessage();
    } catch (const exception& exception) {
        cout << "Exception: " << exception.what() << endl;
        MPI::COMM_WORLD.Abort(-1);
    }

    return 0;
}
