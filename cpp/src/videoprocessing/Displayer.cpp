//   Written by Mateusz Rzeczyca.
//   Student - AGH University of Science and Technology
//   rzeczyca@student.agh.edu.pl
//   30.03.2019

#include "Displayer.h"

namespace mrz
{
    Displayer::Displayer() :
    image( new cv::Mat( cv::Mat::zeros(480, 640, CV_8UC1) ) ),
    raw_data( new cv::Mat ),
    image_size( new size_t( image ->total() * image ->elemSize() )),
    key( new int )
    {}

    Displayer::Displayer(int pack, char* image_metadata) :
    image( new cv::Mat ),
    raw_data( new cv::Mat(1, 4096 * pack, CV_8UC1, image_metadata) ),
    image_size( new size_t ),
    key( new int )
    {}

    Displayer::~Displayer()
    {
        delete image_size;
        delete image;
        delete key;
        delete raw_data;
    }

    void Displayer::check_if_continuous()
    {
        if( raw_data ->isContinuous() )
            *image = raw_data ->clone();
    }

    size_t* Displayer::get_image_size()
    {
        return this ->image_size;
    }

    void Displayer::decode_image()
    {
        *image = imdecode(*raw_data, 0);

        if (image ->size().width == 0)
        {
            std::cerr << "Decode failure!" << std::endl;
            exit(1);
        }
    }

    int Displayer::wait()
    {
        *key = cv::waitKey(1);
        return *key;
    }

    uchar* Displayer::get_metadata()
    {
        return image ->data;
    }

    void Displayer::show_image(const std::string protocol)
    {
        cv::imshow("Video Streaming " + protocol, *image);
    }
}