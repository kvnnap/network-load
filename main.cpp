#include <iostream>
#include <string>
#include <mpi/mpi.h>

using namespace std;

int main(int argc, char **argv) {
    int size, rank, len;
    char name[MPI_MAX_PROCESSOR_NAME];
    MPI::Init(argc, argv);

    size = MPI::COMM_WORLD.Get_size();
    rank = MPI::COMM_WORLD.Get_rank();
    MPI::Get_processor_name(name, len);

    //printf("Size: %d, Rank: %d, ProcName: %s", size, rank, std::string(name, len).c_str());
    cout << "Size: " << size << " Rank: " << rank << " Proc Name: " << string(name, len) << endl;

    MPI::Finalize();
    return 0;
}
