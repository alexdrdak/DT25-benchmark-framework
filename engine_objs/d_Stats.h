#ifndef DT25_D_STATS_H
#define DT25_D_STATS_H


#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>
#include <fstream>
#include <filesystem>
#include "d_Logger.h"
#include <cassert>

struct d_entry {
    int d_serie;
    int64_t d_processTime;
    unsigned int renderedObjects;
};

class d_Stats {
protected:
    std::mutex d_mtx;
    std::condition_variable d_cv;
    std::thread d_worker;
    std::queue<std::string> d_queue;
    std::queue<d_entry> d_message_queue;
    bool d_stop_flag = false;
    std::vector<d_entry> d_msData = {};
    std::vector<unsigned int> d_sampleData;

    std::ofstream d_out;
    std::filesystem::path d_path;

    void d_computeAndWrite();
    void d_clearAndResizeInternalBuffer(int _framesPerScene, int _seriesNum);
    void d_process();

public:
    d_Stats(const std::filesystem::path& _outfilePath, int _framesPerScene, int _seriesNum = 1);
    ~d_Stats();

    void d_writeMessage(std::string data);
    void d_writeStat(d_entry _entry);

    void d_restart(const std::filesystem::path& _outfilePath, int _framesPerScene, int _seriesNum = 1);

    void d_sequentialDump(std::filesystem::path _path, std::vector<unsigned int>& _data, unsigned int _dFrames);

    void d_setSamples(std::vector<unsigned int>& _samples);
};


#endif