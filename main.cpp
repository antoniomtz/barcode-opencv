#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect/barcode.hpp>
#include <iomanip>

int randColor() {
  // Generate a random number between 0 and 255
  int r = rand() % 256;
  int g = rand() % 256;
  int b = rand() % 256;

  // Return the random color
  return (r << 16) | (g << 8) | b;
}

int main(int argc, char** argv)
{   
    if(argc <= 1)
    {
        std::cout<<"Add the input source as argument. (rtsp://127.0.0.1:8554/camera_0)"<<std::endl;
        return 1;       
    }

    cv::barcode::BarcodeDetector barcodeDetector;        
    std::vector< cv::Point > corners;
    std::vector< std::string > decoded_type;
    std::vector< std::string > decoded_info;

    cv::Mat frame;    
    cv::VideoCapture cap(argv[1]);

    while(1)    
    {    
       // Record the starting time
        auto startTime = std::chrono::high_resolution_clock::now();
        cap >> frame;        

        barcodeDetector.detectAndDecodeWithType(frame,decoded_info,decoded_type,corners);

        if(!corners.empty())
        {           
            for (size_t i = 0; i < corners.size(); i += 4)
            {
                const size_t idx = i / 4;
                const bool isDecodable = idx < decoded_info.size()
                    && idx < decoded_type.size()
                    && !decoded_type[idx].empty();
                const cv::Scalar lineColor = isDecodable ? cv::Scalar(0, 255, 0, 128 ) : cv::Scalar(0,255,255);
                // draw barcode rectangle
                std::vector<cv::Point> contour(corners.begin() + i, corners.begin() + i + 4);
                const std::vector<std::vector<cv::Point>> contours {contour};
                cv::drawContours(frame, contours, 0, lineColor, 1);
                // draw vertices
                for (size_t j = 0; j < 4; j++)
                    circle(frame, contour[j], 2, randColor(), -1);
                // write decoded text
                if (isDecodable)
                {
                    std::ostringstream buf;
                    buf << "[" << decoded_type[idx] << "] " << decoded_info[idx];
                    putText(frame, buf.str(), contour[1], cv::FONT_HERSHEY_COMPLEX, 0.8, cv::Scalar(0,0,255), 1);                         
                    std::cout<<"{type:\""<<decoded_type[i/4]<<"\",value:\""<<decoded_info[i/4];
                    cv::Rect rect = boundingRect(contour);
                    std::cout<<"\",xmin:"<<rect.x<<",ymin:"<<rect.y<<",xmax:"<<rect.x + rect.width<<",ymax:"<<rect.y + rect.height<<"}"<<std::endl;
                }
            }
        }              
        cv::imshow("FrameOut",frame); 
        cv::waitKey(1);                        
    }

    return 0;
}


