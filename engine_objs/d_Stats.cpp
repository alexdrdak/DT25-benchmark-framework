#include <iostream>
#include <algorithm>
#include <numeric>
#include "d_Stats.h"

d_Stats::d_Stats(const std::filesystem::path& _outfilePath, int _framesPerScene, int _seriesNum) : d_path(_outfilePath) {
    d_Logger::info("d_Stats: Writing stats into '" + _outfilePath.string() + "'.");

    d_clearAndResizeInternalBuffer(_framesPerScene, _seriesNum);

    d_worker = std::thread(&d_Stats::d_process, this);
}

d_Stats::~d_Stats() {
    {
        std::lock_guard<std::mutex> d_lock(d_mtx);
        d_stop_flag = true;
    }
    d_cv.notify_all();
    d_worker.join();

    d_computeAndWrite();
}

void d_Stats::d_writeMessage(std::string _data) {
    {
        std::unique_lock<std::mutex> d_lock(d_mtx);
        d_queue.push(std::move(_data));
    }

    d_cv.notify_one();
}

void d_Stats::d_process() {
    d_entry d_received_data {};

    while(true) {        
        std::unique_lock<std::mutex> d_lock(d_mtx);
        d_cv.wait(d_lock, [this]() { return !d_message_queue.empty() || d_stop_flag; });

        if(d_stop_flag) {
            return;
        }

        d_received_data = std::move(d_message_queue.front());
        d_message_queue.pop();

        d_msData.push_back(d_received_data);
    }
}

void d_Stats::d_writeStat(d_entry _entry) {
    {
        std::unique_lock<std::mutex> d_lock(d_mtx);
        d_message_queue.push(std::move(_entry));
    }

    d_cv.notify_one();
}

void d_Stats::d_restart(const std::filesystem::path& _outfilePath, int _framesPerScene, int _seriesNum) {
    {
        std::lock_guard<std::mutex> d_lock(d_mtx);
        d_stop_flag = true;
    }
    d_cv.notify_all();
    d_worker.join();

    d_computeAndWrite();
    d_path = _outfilePath;
    d_clearAndResizeInternalBuffer(_framesPerScene, _seriesNum);

    d_stop_flag = false;
    d_worker = std::thread(&d_Stats::d_process, this);
}

void d_Stats::d_sequentialDump(std::filesystem::path _path, std::vector<unsigned int>& _data, unsigned int _dFrames)
{
    assert(_data.size() > _dFrames, "plano daco");

    std::ofstream d_temp(_path.string());    
    if (d_temp.bad())
        d_Logger::err("Error opening '" + _path.string() + "' for writing sample count logs.");

    d_temp << "sampleCount" << std::endl;
    for (int it = 0; it < _dFrames; it++)
        d_temp << std::to_string(_data[it]) << std::endl;

    d_temp.close();
}

void d_Stats::d_setSamples(std::vector<unsigned int>& _samples)
{
    d_sampleData = _samples;
    d_sampleData.erase(d_sampleData.begin());
}

void d_Stats::d_computeAndWrite() {
    if (d_sampleData.size() < d_msData.size())
        d_sampleData.resize(d_msData.size());

    d_out.open(d_path);
    d_out << "processTime,samples,objectsRendered\n";
    for (size_t i = 0; i < d_msData.size(); i++)
    {
        d_out << d_msData[i].d_processTime << "," << d_sampleData[i] << "," << d_msData[i].renderedObjects << "\n";        
    }

    d_out.close();
                  
    d_path.replace_filename(d_path.stem().string() + "-mean.csv");

    d_Logger::debug("Current directory: "+std::filesystem::current_path().string());
    d_out.open(d_path.string(), std::ios::in | std::ios::trunc);
    if(d_out.bad()) {
        d_Logger::err("Error opening '" + d_path.string() + "' file in d_Stats::d_computeAndWrite().");
        return;
    }

    d_Logger::info("Computing and writing means...");

    float d_cAvg_pTime = 0.0f;
    float d_cAvgPrev_pTime = 0.0f;
    float d_cAvg_fInvocs = 0.0f;
    float d_cAvgPrev_fInvocs = 0.0f;

    d_out << "processTime,samples,objectsRendered" << std::endl;
    for(size_t i = 0; i < d_msData.size(); i++) {
        d_cAvg_pTime = (d_msData[i].d_processTime + i * d_cAvgPrev_pTime) / (i + 1);
        d_cAvg_fInvocs = (d_sampleData[i] + i * d_cAvgPrev_fInvocs) / (i + 1);

        d_out << d_cAvg_pTime << "," << d_cAvg_fInvocs << "," << d_msData[i].renderedObjects << std::endl;

        d_cAvgPrev_pTime = d_cAvg_pTime;
        d_cAvgPrev_fInvocs= d_cAvg_fInvocs;
    }

    d_Logger::info("Stats written to '" + d_path.string() + "'.");
    d_out.close();
}

void d_Stats::d_clearAndResizeInternalBuffer(int _framesPerScene, int _seriesNum) {
    d_msData.clear();
    d_msData.reserve(_framesPerScene);    

    d_sampleData.clear();
    d_sampleData.reserve(_framesPerScene);
}
