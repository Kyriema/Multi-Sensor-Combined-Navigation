/*
** navcamera.cc for mscnav in /media/fwt/Data/program/mscnav/src/data
**
** Made by little fang
** Login   <fangwentao>
**
** Started on  Fri Aug 23 下午12:03:22 2019 little fang
** Last update Sat Aug 23 下午2:57:39 2019 little fang
*/

#include "data/navcamera.h"
#include "navconfig.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <chrono>

using std::chrono::milliseconds;

namespace mscnav
{

FileCameraData::~FileCameraData()
{
    if (th_collectgdata_.joinable())
    {
        th_collectgdata_.join();
    }
}
/**
 * @brief  get the latest gnss data
 * @note   
 * @param  &gd: 
 * @retval 
 */
bool FileCameraData::GetData(camera::CameraData::Ptr &cd)
{
    if (cd_datapool_.size() > 0)
    {
        std::unique_lock<std::mutex> lck(mtx_collectdata_);
        cd = cd_datapool_.front();
        cd_datapool_.pop_front();
        return true;
    }
    else if (aint_markofcollectdata_ == 0)
    {
        while (cd_datapool_.size() == 0)
        {
            std::this_thread::sleep_for(milliseconds(50));
            if (aint_markofcollectdata_ == 1)
            {
                return false;
            }
        }
        std::unique_lock<std::mutex> lck(mtx_collectdata_);
        cd = cd_datapool_.front();
        cd_datapool_.pop_front();
        return true;
    }
    else
    {
        return false;
    }
}

bool FileCameraData::StartReadCameraData()
{
    utiltool::ConfigInfo::Ptr config = utiltool::ConfigInfo::GetInstance();
    std::string camera_config_file_path = config->get<std::string>("camera_config_file_path");
    ifs_camera_data_config_.open(camera_config_file_path);
    if (!ifs_camera_data_config_.good())
    {
        LOG(ERROR) << "Read camera data config file failed" << std::endl;
        return false;
    }
    aint_markofcollectdata_ = 0;
    std::thread th(&FileCameraData::ReadingData, this);
    th_collectgdata_ = std::move(th);

    return true;
}

/**
 * @brief  读取配置文件中Camera存储的路径和时标
 * @note   默认一行格式为 gpsweek secondofweek imagefilename
 * @retval None
 */
void FileCameraData::ReadingData()
{
    utiltool::ConfigInfo::Ptr config = utiltool::ConfigInfo::GetInstance();
    std::string image_path, tmp_line;
    std::getline(ifs_camera_data_config_, image_path);
    std::vector<int> utime = config->get_array<int>("process_date");
    int start_second = config->get<int>("start_time");
    int end_second = config->get<int>("end_time");
    utiltool::NavTime start_time(utime[0], utime[1], utime[2], 0, 0, 0.0);
    utiltool::NavTime end_time(utime[0], utime[1], utime[2], 0, 0, 0.0);
    start_time += start_second % utiltool::NavTime::MAXSECONDOFDAY;
    end_time += end_second % utiltool::NavTime::MAXSECONDOFDAY;
    if (end_time <= start_time)
    {
        LOG(INFO) << "start time is " << start_time << std::endl;
        LOG(INFO) << "end time is " << end_time << std::endl;
        LOG(FATAL) << "start time is later than end time" << std::endl;
    }
    while (!ifs_camera_data_config_.eof())
    {
        std::getline(ifs_camera_data_config_, tmp_line);
        auto data = utiltool::TextSplit(tmp_line, "\\s+");
        if(data.size()<3)
            continue;
        utiltool::NavTime time(std::stoi(data[0]), std::stod(data[1]));
        if (time < start_time || time > end_time)
            continue;
        cv::Mat im = cv::imread((image_path + "/" + data[2]), CV_LOAD_IMAGE_GRAYSCALE);
        camera::CameraData::Ptr image = std::make_shared<camera::CameraData>(time, im);
        {
            std::unique_lock<std::mutex> lck(mtx_collectdata_);
            cd_datapool_.emplace_back(image);
        }
        while (cd_datapool_.size() > MAX_SIZE_CAMERAPOOL)
        {
            std::this_thread::sleep_for(milliseconds(200));
        }
    }
    ifs_camera_data_config_.close();
    aint_markofcollectdata_ = 1;
}

} // namespace mscnav