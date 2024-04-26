#include <iostream>
#include <mutex>
#include <thread>
#include <atomic>

class SharedData {
public:
    SharedData() : value(0), stopFlag(false) {}

    void setValue(int newValue) {
        std::lock_guard<std::mutex> lock(data_mutex);
        value = newValue;
    }

    int getValue() const {
        std::lock_guard<std::mutex> lock(data_mutex);
        return value;
    }

    void setStopFlag() {
        stopFlag.store(true);
    }

    bool shouldStop() const {
        return stopFlag.load();
    }

private:
    int value;
    mutable std::mutex data_mutex;
    std::atomic<bool> stopFlag;
};

void writer_thread(SharedData &sharedData) {
    for (;;) {
        if (sharedData.shouldStop()) {
            break;
        }
        sharedData.setValue(rand() % 100);
    }
}

void reader_thread(const SharedData &sharedData) {
    for (;;) {
        if (sharedData.shouldStop()) {
            break;
        }
        int value = sharedData.getValue();
        std::cout << "Read: " << value << std::endl;
    }
}

int main() {
    SharedData sharedData;

    std::thread writer(writer_thread, std::ref(sharedData));
    std::thread reader(reader_thread, std::cref(sharedData));

    std::this_thread::sleep_for(std::chrono::seconds(1));

    sharedData.setStopFlag();

    writer.join();
    reader.join();

    return 0;
}
