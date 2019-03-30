//   Written by Mateusz Rzeczyca.
//   Student - AGH University of Science and Technology
//   info@mateuszrzeczyca.pl
//   30.03.2019

#ifndef VIDEOSTREAMING_VIDEOSERVER_H
#define VIDEOSTREAMING_VIDEOSERVER_H

#include "../inc_def.h"

namespace mrz
{
    class VIDEOserver;
}

class mrz::VIDEOserver{
public:
    VIDEOserver();

    void check_if_continuous();
    size_t& get_image_size();
    void create_window();
    int get_key();
    int wait();
    uchar* get_metadata();
    void show_image();

private:
    cv::Mat image;
    size_t image_size;
    int key;
};


#endif //VIDEOSTREAMING_VIDEOSERVER_H
