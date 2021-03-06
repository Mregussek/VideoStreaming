#include <iostream>
#include "opencv2/opencv.hpp"
#include <chrono>

/*
We are going to use the weighed method to convert RGB image to 
gray scale. The equation to do that is:
gray_image = ( (0.3 * R) + (0.59 * G) + (0.11 * B) ).
*/

class ImageOperator
{
public:
    ImageOperator() = default;
    ~ImageOperator() = default;

    static void cvtCol(const cv::Mat& input, cv::Mat& output);
    static void to_gray_m1(const cv::Mat& input, cv::Mat& output);
    static void to_gray_m2(const cv::Mat& input, cv::Mat& output);
    static void to_gray_m3(const cv::Mat& input, cv::Mat& output);
    static void to_gray(const unsigned char* input,
                        const int width,
                        const int height,
                        const int channel,
                        unsigned char* output);
};

void ImageOperator::cvtCol(const cv::Mat& input, cv::Mat& output)
{
    cv::cvtColor(input, output, cv::COLOR_BGR2GRAY);
}

/*
We used cv::Vec3b because each element consists of three 
channels bgr (blue, green and red) and each one of 
them is 1-Byte so we need 3-Bytes.

The output Mat consists of just one channel and has 
the same size of the input Mat (number of rows and columns) 
so we can access its raw unsigned char pointer data and modify it.
*/

void ImageOperator::to_gray_m1(const cv::Mat &input, cv::Mat &output)
{
  
    unsigned char *data_out = (unsigned char*)(output.data);
    int ind = 0;
    auto end = input.end<cv::Vec3b>();
    cv::MatConstIterator_<cv::Vec3b> it = input.begin<cv::Vec3b>();
    for (; it != end; ++it)
    {
        const unsigned char& r = (*it)[2];
        const unsigned char& g = (*it)[1];
        const unsigned char& b = (*it)[0];
        data_out[ind] = 0.3 * r + 0.59 * g + 0.11 * b;
        ind++;
    }

}

/*
The total number of bytes for an image can be calculated by:
img_size_in_byte = number_of_channels * img_width*img_height

So if we have 40*40 bgr image, the total size for it 3*40*40 = 4800 and 
the corresponding gray scale image size is 1*40*40 = 1600.
*/

void ImageOperator::to_gray_m2(const cv::Mat &input, cv::Mat &output)
{
    unsigned char* data_in = (unsigned char*)(input.data);
    unsigned char* data_out = (unsigned char*)(output.data);

    int index = 0;
    int byte_size = input.channels() * input.rows * input.cols;

    while(index != byte_size)
    {
        data_out[index / input.channels()] = unsigned(
            0.11 * data_in[index] + 0.59 * data_in[index + 1] + 0.3 * data_in[index + 2]
            );

        index += 3;
    }
}

/*
The image data is stored in consecutive bytes in memory. 
In this method, you can access the pixel using its row and column 
coordinates but from 1D char pointer.

input.step gives the total number of bytes in one row. So, if you 
multiply that with the row index, you can get the pointer to a 
specific row. After accessing a specific row, you can use col index to 
access a certain column.

In other words: to access one pixel at x,y location in 2D array, you can 
write img[x][y]. To get the same result in 1D array, you have to 
use img[y*step+x]. 
*/

void ImageOperator::to_gray_m3(const cv::Mat &input, cv::Mat &output)
{
    unsigned char* data_in = (unsigned char*)(input.data);
    unsigned char* data_out = (unsigned char*)(output.data);

    int index = 0;
    int cols_limit = input.cols * input.channels();

    for (int row = 0; row < input.rows; ++row)
    {
        for (int col = 0; col < cols_limit; col += input.channels())
        {
            int new_index = row * input.step + col;
            *data_out = 0.11 * data_in[new_index] +
                             0.59 * data_in[new_index + 1] +
                             0.3 * data_in[new_index + 2];
            data_out++;
        }
    }

}

// NO OPENCV
// This function generates the least assembly code!

void ImageOperator::to_gray(const unsigned char* bgr_input,
                            const int width,
                            const int height,
                            const int channel,
                            unsigned char* gray_output)
{
    int index = 0;
    int step = channel * width;

    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < step; col += channel)
        {
            int new_index = row * step + col;

            *gray_output = 0.11 * *(bgr_input + new_index) +
                           0.59 * *(bgr_input + new_index + 1) +
                           0.3 * *(bgr_input + new_index + 2);

            gray_output++;
        }
    }

}

int main()
{
    cv::VideoCapture cam(0);

    if (! cam.isOpened() )
        throw std::runtime_error("Error");

    cv::Mat output(480, 640, CV_8UC1);
    cv::Mat rgb_output(480, 640, CV_8UC3);
    cv::Mat frame;
    
    while(true)
    {
        cam >> frame;
        cv::resize(frame, frame, cv::Size(640, 480));

        cv::imshow("bgr_frame", frame);

        auto t1 = std::chrono::high_resolution_clock::now();
        ImageOperator::cvtCol(frame, output);
        auto t2 = std::chrono::high_resolution_clock::now();

        std::cout << "cvtColor() took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
              << " milliseconds\n";

        cv::imshow("opencv_func", output);

        t1 = std::chrono::high_resolution_clock::now();
        ImageOperator::to_gray_m1(frame, output);
        t2 = std::chrono::high_resolution_clock::now();
        cv::imshow("to_gray_m1",output);

        std::cout << "to_gray_m1() took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
              << " milliseconds\n";
        
        t1 = std::chrono::high_resolution_clock::now();
        ImageOperator::to_gray_m2(frame, output);
        t2 = std::chrono::high_resolution_clock::now();
        cv::imshow("to_gray_m2",output);

        std::cout << "to_gray_m2() took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
              << " milliseconds\n";

        t1 = std::chrono::high_resolution_clock::now();
        ImageOperator::to_gray_m3(frame, output);
        t2 = std::chrono::high_resolution_clock::now();
        cv::imshow("to_gray_m3",output);

        std::cout << "to_gray_m3() took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
              << " milliseconds\n";

        t1 = std::chrono::high_resolution_clock::now();
        ///No OpenCV
        const unsigned char* bgr_input = (unsigned char*)frame.data;
        unsigned char* gray_output = new unsigned char[frame.rows * frame.cols];
        ImageOperator::to_gray(bgr_input, frame.cols, frame.rows, frame.channels(), gray_output);
        cv::Mat output_gray(frame.rows, frame.cols, CV_8UC1, gray_output);
        t2 = std::chrono::high_resolution_clock::now();
        cv::imshow("to_gray_no_opencv", output_gray);

        std::cout << "no_opencv() took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()
              << " milliseconds\n";

        if(cv::waitKey(30) >= 0)
            break;
    }

    return 0;
}
