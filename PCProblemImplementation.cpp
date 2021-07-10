//(Producer-Consumer)
#include <iostream>
#include <cstdlib>  //rand
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
using namespace std;

const unsigned int full = 10; //Buffer Size
vector <int> buffer; //We Use This Vector As FIFO Buffer
mutex mutexc;
condition_variable sem;

void produceData() {
    int input = (rand() % 10) + 1; // 1-10
    buffer.push_back(input);
    cout << "<- Produced: " << input << " \n";
}

void consumeData() {
    int output = buffer.front();
    buffer.erase(buffer.begin());
    cout << "-> Consumed: " << output << " \n";
}

void producer() {
    while (true) {
        unique_lock<mutex> ul(mutexc);  //ul.lock();
        sem.wait(ul, []() { return buffer.size() < full; }); // if (buffer.size() < full) Same As if ({buffer.size() < full;} is true) {ul.lock and continue} else if (buffer.size() >= full) {ul.unlock and wait}
        produceData();
        ul.unlock();
        sem.notify_one(); //Notify The Other Thread
    }  //Auto ul.unlock(); (RAII)
}

void consumer() {
    while (true) {
        unique_lock<mutex> ul(mutexc);  //ul.lock();
        sem.wait(ul, []() { return buffer.size() > 0; });  //if (buffer.size > 0) Same As if (buffer.size() > 0 is true) {ul.lock and continue} else if (buffer.size() <= 0) {ul.unclock and wait}
        consumeData();
        ul.unlock();
        sem.notify_one();
    }  //Auto ul.unlock(); (RAII)
}

void producerThread() {
    producer(); 
}

void consumerThread() { 
    consumer(); 
}

int main() {
    thread t1(producerThread);
    thread t2(consumerThread);
    t1.join();
    t2.join();
    return 0;
}