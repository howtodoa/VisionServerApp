#include "controll_demo.h"
#include <opencv2/opencv.hpp>
using namespace std;
using namespace Mz_ClientControl;

// 自定义函数：读取本地图像并返回 HImage
void pic_display_produce(HImages inputImages, HValues inputParams,
    HImages& outputImages, HValues& outputParams,
    int& errcode, std::string& errmsg, int timeout)
{

    int width = 640;
    int height = 480;
    int channels = 3;  // 3 通道（RGB）
    int imageDataLength = width * height * channels;  // 数据长度

    // 分配内存
    unsigned char* imageData = new unsigned char[imageDataLength];

    std::srand(std::time(0));

    // 填充图像数据（生成彩色图像）
    for (int i = 0; i < imageDataLength; i += 3) {
        imageData[i] = std::rand() % 256;       // R 通道（0-255）
        imageData[i + 1] = std::rand() % 256;   // G 通道（0-255）
        imageData[i + 2] = std::rand() % 256;   // B 通道（0-255）
    }
    // 创建 HImage 对象
    HImage img(width, height, channels, imageDataLength, imageData);

     delete[] imageData;

    // 深拷贝文件数据到 img.data
   // img.data = new unsigned char[fileSize];
    //memcpy(img.data, buffer.data(), fileSize);

    // 添加到输出列表
    outputImages.m_Images.push_back(img);

    // 设置返回值
    errcode = 0;
    errmsg = "图像读取成功";
    std::cout << "成功读取图片，输出图片数量: " << outputImages.m_Images.size() << std::endl;
}


void pic_display_handle(HImages inputImages, HValues inputParams,
    HImages& outputImages, HValues& outputParams,
    int& errcode, std::string& errmsg, int timeout)
{
    std::cout << "enter" << std::endl;
    std::cout << "inputImages.m_Images[0].imageHead.channels: " << inputImages.m_Images[0].imageHead.channels << std::endl;
    // 设置返回值
    outputImages.m_Images.push_back(inputImages.m_Images[0]);
   
    errcode = 0;
    errmsg = "图像读取成功";
    std::cout << "成功读取图片，输出图片数量: " << outputImages.m_Images.size() << std::endl;
}



HImage LoadImageToHImage(const std::string& filePath) {
    // 1. 使用 OpenCV 读取图像
    cv::Mat img = cv::imread(filePath, cv::IMREAD_UNCHANGED); // 保留原始通道数

    // 检查图像是否加载成功
    if (img.empty()) {
        return HImage(); // 返回空对象
    }

    // 2. 准备图像参数
    int width = img.cols;
    int height = img.rows;
    int channels = img.channels();
    int length = width * height * channels; // 计算数据总长度

    // 3. 创建 HImage 对象
    HImage hImage;
    hImage.imageHead.width = width;
    hImage.imageHead.height = height;
    hImage.imageHead.channels = channels;
    hImage.imageHead.length = length;

    // 4. 分配内存并复制数据
    hImage.data = new unsigned char[length];
    if (hImage.data != nullptr) {
        // OpenCV 默认是 BGR 排列，如果需要 RGB 可以在这里转换
        if (channels == 3) {
            cv::cvtColor(img, img, cv::COLOR_BGR2RGB); // BGR -> RGB
        }

        // 复制图像数据
        memcpy(hImage.data, img.data, length * sizeof(unsigned char));
    }
    else {
        // 内存分配失败
        hImage.Rlease();
    }

    return hImage;
}



// 启动客户端
int main()
{
    ClientOperation client;

    CommPorts port;
    port.isActAsServer = 0;  // 客户端
    port.PortName = "PIC_PORT";
    port.localhost_IP.IP = "127.0.0.1";
    port.localhost_IP.Port = 8000;

    client.InitSDK(port);
    client.StartWork();

    // 调用服务端的 pic_display 函数
    HValues val1;
    HValues val2;
    HImages img;  // 用于接收返回的图像数据
    int errcode = 0;
    string errmsg = "";

    val1.m_Values.push_back(HValue(1));
    val1.m_Values.push_back(HValue(2));
    std::cout << "This line is in file: " << __FILE__ << ", line: " << __LINE__ << std::endl;
    client.DoActionFun(port, "pic_display", val1, HImages(), &val2, &img, &errcode, &errmsg,1000);

    
    if (errcode == 0) {
        cout << "图像读取成功，返回的图像数量: " << img.m_Images.size() << endl;
    }
    else {
        cout << "错误: " << errmsg << endl;
    }
    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // 停止客户端
    client.StopWork();
    client.FreeSDK();

    return 0;
}



// 启动服务端
int test_Service()
{
    Mz_ClientControl::ClientOperation server;

    CommPorts port;
    port.isActAsServer = 1;
    port.PortName = "PIC_PORT";
    port.localhost_IP.IP = "127.0.0.1";
    port.localhost_IP.Port = 8000;

    server.InitSDK(port);

    Callbackfunc cb;
    cb.funcname = "pic_display_produce";
    cb.func = reinterpret_cast<FunctionTableStream>(pic_display_produce);
  
    cb.inputImagesnums = 0;
    cb.inputPramsnums = 0;
    cb.outputImagesnums = 1;
    cb.outputPramsnums = 0;

    server.RegsiterFunitFun(cb);
    server.StartWork();

    cout << "服务端8000已启动，等待客户端调用........" << endl;

    CommPorts port1;
    port.isActAsServer = 1;
    port.PortName = "PIC_PORT";
    port.localhost_IP.IP = "127.0.0.1";
    port.localhost_IP.Port = 8001;

    server.InitSDK(port);

    Callbackfunc cb1;
    cb1.funcname = "pic_display_produce";
    cb1.func = reinterpret_cast<FunctionTableStream>(pic_display_produce);

    cb1.inputImagesnums = 0;
    cb1.inputPramsnums = 0;
    cb1.outputImagesnums = 1;
    cb1.outputPramsnums = 0;

    server.RegsiterFunitFun(cb1);
    server.StartWork();

    cout << "服务端8001已启动，等待客户端调用........" << endl;

    while (true) {
        Sleep(1000);
    }

    return 0;
}



int test_service_toQT()//test_service_toQT
{
    Mz_ClientControl::ClientOperation server;

    CommPorts port;
    port.isActAsServer = 1;
    port.PortName = "PIC_PORT";
    port.localhost_IP.IP = "127.0.0.1";
    port.localhost_IP.Port = 8000;

    server.InitSDK(port);

    Callbackfunc cb;
    cb.funcname = "pic_display_handle";
    cb.func = reinterpret_cast<FunctionTableStream>(pic_display_handle);

    cb.inputImagesnums = 1;
    cb.inputPramsnums = 0;
    cb.outputImagesnums = 1;
    cb.outputPramsnums = 0;

    server.RegsiterFunitFun(cb);
    server.StartWork();

    cout << "服务端8000已启动，等待客户端调用........" << endl;

  
    while (true) {
        Sleep(1000);
    }

    return 0;
}
