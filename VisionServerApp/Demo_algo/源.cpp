#pragma once
#include <iostream>
#include <windows.h>
#include "../MZ_ClientControl/PacketHead.h"
#include "../MZ_ClientControl/MZ_ClientControl.h"
#include <windows.h>
#include <fstream>
#include <chrono>
#include <thread>
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace Mz_ClientControl;


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

int test_algo()
{
    Mz_ClientControl::ClientOperation server;

    CommPorts port;
    port.isActAsServer = 1;
    port.PortName = "PIC_PORT";
    port.localhost_IP.IP = "127.0.0.1";
    port.localhost_IP.Port = 8001;

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

    cout << "服务端8001已启动，等待客户端调用........" << endl;


    while (true) {
        Sleep(1000);
    }

    return 0;
}


int main()
{
    ClientOperation client;

    CommPorts port;
    port.isActAsServer = 0;  // 客户端
    port.PortName = "COM1";
   // port.localhost_IP.IP = "192.168.2.225";
   // port.localhost_IP.Port = 12345;

    port.remote_IP.IP = "192.168.2.225";
    port.remote_IP.Port = 12345;

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

    const std::string path = "E:\\My  Download\\log\\test.jpg";
    HImages inputimgs;
    HImage testimg = LoadImageToHImage(path);
    inputimgs.m_Images.push_back(testimg);
    std::cout << "This line is in file: " << __FILE__ << ", line: " << __LINE__ << std::endl;
    client.DoActionFun(port, "add", val1, HImages(), &val2, &img, &errcode, &errmsg, 10000);
    std::cout << "val2.m_Values[0].I():    " << val2.m_Values[0].I() << std::endl;

    if (errcode == 0) {
        cout << "图像读取成功，返回的图像数量: " << img.m_Images.size() << endl;
    }
    else {
        cout << "错误: " << errmsg << endl;
    }

    return 0;

}