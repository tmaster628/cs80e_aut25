// A very short program intended for students learning about multithreading, race conditions, and false sharing.
// StructFiller spins up two threads to increment members
// NUM1 and NUM2 to the K_NUM_FULL threshold.

#include <iostream>
#include <thread>
#include <string>
#include <mutex>
#include <ctime>
#include <pthread.h>
#include <sched.h>

using namespace std;

static const int K_NUM_FULL = 200000;

struct Data
{
    int num1 = 0;
    int num2 = 0;
};

void fillStruct(struct Data &d, const int id)
{
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(id, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    if (id == 0)
    {
        while (d.num1 < K_NUM_FULL)
        {
            d.num1++;
        }
    }

    else if (id == 1)
    {
        while (d.num2 < K_NUM_FULL)
        {
            d.num2++;
        }
    }
}

/**
 * MAIN() spins up 2 threads. These threads must work together
 * to fill the numbers in DATA to K_NUM_FULL.
 */
int main()
{

    // Timer setup.
    clock_t start = clock();

    thread arr[2]; // Unused!
    Data usr_data;

    for (int thread_id = 0; thread_id < 2; thread_id++)
    {
        arr[thread_id] = thread(fillStruct, ref(usr_data), thread_id);
    }
    for (int thread_id = 0; thread_id < 2; thread_id++)
    {
        arr[thread_id].join();
    }

    clock_t end = clock();

    cout << "All done! Here is the state of usr_data now..." << endl;
    cout << "{ " << usr_data.num1 << ", " << usr_data.num2 << " }" << endl;

    cout << "This operation took " << end - start << " cycles." << endl;

    return 0;
}
