// test_task.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <set>
#include <algorithm>
#include <cmath>
#define N 12


int counter = 0;
enum BRIGHTNESS {
    brigther,
    similar,
    darker
};
enum CONDITION {
    CONDITION1,
    CONDITION2,
    NONE
};

struct Point {
   
    int x, y;
    int center_x, center_y;
    Point(int x_,int y_, int center_x_, int center_y_) {
        x = x_;
        y = y_;
        center_x = center_x_;
        center_y = center_y_;
    }
    bool operator < (const Point& r) const{
        if (x >= 0 && r.x >= 0) {
            //std::cout << "Here\n";
            //std::cout << (a.second) << "\n";
            double cos_phi1 = double(y) / std::sqrt(x * x + y * y);
            double cos_phi2 = double(r.y) / std::sqrt(r.x * r.x + r.y * r.y);
            double phi1 = std::acos(cos_phi1);
            double phi2 = std::acos(cos_phi2);
           
            // std::cout << (phi1 < phi2) << "\n";
            return phi1 < phi2;

        }
        else if (x >= 0 && r.x < 0) {
            //std::cout << "Here2\n";
            return true;
        }
        else if ((x < 0 && r.x >= 0)) {
            //std::cout << "Here3\n";
            return false;
        }
        else if (x < 0 && r.x < 0) {
            //std::cout << "Here4\n";
            //std::cout << (a.second) << "\n";
            double phi1 = std::acos(double(-y) / std::sqrt(x * x + y * y));
            double phi2 = std::acos(double(-r.y) / std::sqrt(r.x * r.x + r.y * r.y));
            //std::cout << (phi1 > phi2) << "\n";
            return (phi1 < phi2);
        }
    }

};

std::set<Point> get_circle(int x_centre, int y_centre, int radius) {
    int x = 0;
    int y = radius;
    std::set<Point> vec;

    //std::cout << "(" << x + x_centre << ", " << y + y_centre << ") ";

    vec.insert(Point(x, y, x_centre, y_centre));
    if (radius > 0)
    {

        vec.insert(Point(x , -y , x_centre,y_centre));
        vec.insert(Point(y , x, x_centre, y_centre));
        vec.insert(Point(-y, x,x_centre,y_centre));
    }

    int P = 1 - radius;
    while (x <= y)
    {


        if (P <= 0)
            P = P + 2 * x + 3;

        else
        {

            P = P + 2 * (x - y) + 5;
            y = y - 1;
        }

        x = x + 1;



        vec.insert(Point(x, y,x_centre,y_centre));
        vec.insert(Point(x, -y, x_centre,y_centre));
        vec.insert(Point(-x, y, x_centre,y_centre));
        vec.insert(Point(-x, -y, x_centre,y_centre));




        vec.insert(Point(y , x , x_centre,y_centre));
        vec.insert(Point(y , -x , x_centre,y_centre));
        vec.insert(Point(-y , -x , x_centre, y_centre));
        vec.insert(Point(-y, x , x_centre, y_centre));
    }

  
    return vec;
}
uchar get_inensity(Point  point, const cv::Mat& img) {
    int rows = img.rows;
    int cols = img.cols;
    uchar intensity{};
    if (point.y + point.center_y <= 0 && point.x  + point.center_x<= 0) {

        intensity = img.at<uchar>(0, 0);
    }
    else if (point.y + point.center_y < 0 && point.x + point.center_x> 0)
    {
        //std::cout << "HERE\n";
        intensity = img.at<uchar>(std::min(rows-1, point.x + point.center_x), 0);
    }
    else if (point.y + point.center_y > 0 && point.x > 0)
    {
        intensity = img.at<uchar>(std::min(rows-1, point.x + point.center_x), std::min(cols - 1, point.y + point.center_y));
    }
    else if (point.y + point.center_y > 0 && point.x + point.center_x < 0)
    {
        //std::cout << std::min(img.cols - 1, pair.second) << "\n";
        intensity = img.at<uchar>(0, std::min(cols - 1, point.y + point.center_y));
        //std::cout << "Intensity " << intensity << std::endl;
    }
    return intensity;
}
bool check_is_corner(int x, int y,const cv::Mat& img,std::set<Point>& point_of_circle) {
    int counter_cond_1 = 0;
    int counter_cond_2 = 0;
    CONDITION cond = NONE;
    
  
    uchar pixel_intensity = img.at<uchar>(x, y);
  
  
    
    Point point_1 = *point_of_circle.begin();
    Point point_9 = *std::next(point_of_circle.begin(), 8);
    Point point_5 = *std::next(point_of_circle.begin(), 4);
    Point point_13 = *std::next(point_of_circle.begin(), 12);

    uchar intensity1 = get_inensity(Point(point_1.x, point_1.y,x,y), img);
    uchar intensity9 = get_inensity(Point(point_9.x, point_9.y, x, y), img);
    uchar intensity5 = get_inensity(Point(point_5.x, point_5.y, x, y), img);
    uchar intensity13 = get_inensity(Point(point_13.x, point_13.y, x, y), img);
    double threshold = 10;
    double pixel_intensity_plus_threshold = pixel_intensity + threshold;
    double pixel_intensity_minus_threshold = pixel_intensity - threshold;
    if ((std::abs(intensity1-pixel_intensity) < threshold) && (std::abs(intensity9 - pixel_intensity) < threshold)) {
        //counter++;
        return false;
    }
    else if ((intensity1 > (pixel_intensity_plus_threshold)) && (intensity9 > (pixel_intensity_plus_threshold))) {
        if ((intensity5 <  (pixel_intensity_plus_threshold)) && (intensity13 < (pixel_intensity_plus_threshold))) {
                //counter++;
                return false;
            }
    }
    else if ((intensity1 < (pixel_intensity_minus_threshold)) && (intensity9 <  (pixel_intensity_minus_threshold))) {
        if ((intensity5 > (pixel_intensity_minus_threshold)) && (intensity13 > (pixel_intensity_minus_threshold))) {
            //counter++;
            return false;
        }
    }
    for (auto& point : point_of_circle) {
        uchar intensity{};
       

        intensity = get_inensity(Point(point.x,point.y,x,y), img);
        
        if (intensity > pixel_intensity_plus_threshold) {
            counter_cond_1++;
            counter_cond_2 = 0; 
        }
        else if (intensity  < pixel_intensity_minus_threshold) {
            counter_cond_2++;
            counter_cond_1 = 0;
        }
        else {
            counter_cond_2 = 0;
            counter_cond_1 = 0;
        }
        if (counter_cond_1 > N || counter_cond_2 > N) {
            return true;
        }
    }

   
    return false;
}
void process_row(int start_row, int end_row, const cv::Mat& img, cv::Mat& img_clr, std::set<Point>& point_of_circle) {
    for (int x = start_row; x < end_row; ++x) {
        for (int y = 0; y < img.cols; ++y) {
            uchar intensity = img.at<uchar>(x, y);

            if (check_is_corner(x, y, img, point_of_circle)) {
                img_clr.at<cv::Vec3b>(x, y) = cv::Vec3b(0, 255, 0);
            }
        }
    }
}
int main()
{
    cv::Mat img;
    std::string path = "E:\\test2.jpeg";
    img = cv::imread(path, cv::IMREAD_GRAYSCALE);
    cv::Mat img_clr = cv::imread(path, cv::IMREAD_COLOR);
   
    if (img.empty())
    {
        std::cout << "Could not read the image: " << path << std::endl;
        return 1;
    }
   
   
    
    std::clock_t clock_start = std::clock();
    std::set<Point> point_of_circle = get_circle(0, 0, 3);
   const int num_threads = std::thread::hardware_concurrency();
   const int rows_per_thread = img.rows / num_threads;
   std::vector<std::thread> threads;
  
 
   for (int i = 0; i < num_threads; ++i) {
       int start_row = i * rows_per_thread;
       int end_row = (i == num_threads - 1) ? img.rows : (i + 1) * rows_per_thread;

       threads.emplace_back(process_row, start_row, end_row, std::ref(img), std::ref(img_clr),std::ref(point_of_circle));
   }

  
   for (auto& thread : threads) {
       thread.join();
   }
 
   std::cout << (std::clock() - clock_start) / 1000.0 << std::endl;
  

 
  
    cv::imwrite("E:\\labs\\test_task\\result\\output1.jpg", img_clr);
   
    int k = cv::waitKey(0);
    std::cout << img.rows * img.cols << std::endl;
    std::cout << double(counter)/ double(img.rows * img.cols) << std::endl;
}


