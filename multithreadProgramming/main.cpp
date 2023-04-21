#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <chrono>
#include <queue>
#include <cstdlib>
#include <csignal>

using namespace std;

mutex queueMutex;
mutex capacityMutex;
mutex atmStatusMutex;

queue<int> requestsQueue;
int capacity;
int atmCount;
int sizeQueue;
volatile bool stop = false;

void signalHandler(int count) {
    cout << "Received for interruption: " << count << endl;
    stop = true;
}
void launch(int numberThread)
{
    int time = rand() % 10 + 1;
    int count = time;
    this_thread::sleep_for(chrono::seconds (time));
    for (int i = 0; i < time; i++) {
        cout << "ATM " << numberThread << " finish " << count-- << " from now" << endl;

        this_thread::sleep_for(chrono::milliseconds (1000));
    }
}

void atm(int atmNumber) {
    while (!stop) {
        queueMutex.lock();
        if (!requestsQueue.empty()) {
            int requestNumber = requestsQueue.front();
            requestsQueue.pop();
            queueMutex.unlock();

            atmStatusMutex.lock();
            cout << "ATM " << atmNumber << " is busy for request " << requestNumber << endl;
            atmStatusMutex.unlock();

            launch(atmNumber);

            atmStatusMutex.lock();
            cout << "ATM " << atmNumber << " finished atm request " << requestNumber << endl;
            atmStatusMutex.unlock();

        } else {
            queueMutex.unlock();
        }

        int sleepTime = rand() % 5000 + 1;
        this_thread::sleep_for(chrono::milliseconds (sleepTime));
    }
    atmStatusMutex.lock();
    cout << "ATM " << atmNumber << " is free" << endl;
    atmStatusMutex.unlock();
}


void requestGenerator() {
    int requestNumber = 0;
    while (!stop) {
        this_thread::sleep_for(chrono::seconds (rand() % 5 + 1));
        queueMutex.lock();
        if (requestsQueue.size() < capacity) {
            requestsQueue.push(requestNumber++);
            capacityMutex.lock();
            cout << "Request " << requestNumber - 1 << " added to the queue. Current queue size: " << requestsQueue.size() << endl;
            capacityMutex.unlock();
        }
        queueMutex.unlock();
    }

    capacityMutex.lock();
    cout << "Request generator stopped" << endl;
    capacityMutex.unlock();
}

int main() {
    cout << "Enter queue size: ";
    cin >> sizeQueue;
    cout << "Enter the number of ATMs: ";
    cin >> atmCount;
    cout << "Enter the capacity of the cash dispenser: ";
    cin >> capacity;

    signal(SIGINT, signalHandler);

    vector<thread> atmThreads;
    for (int i = 1; i <= atmCount; i++) {
        atmThreads.emplace_back(atm, i);
    }

    thread requestGeneratorThread(requestGenerator);

    for (thread& t : atmThreads) {
        t.join();
    }

    requestGeneratorThread.join();

    return 0;
}
 