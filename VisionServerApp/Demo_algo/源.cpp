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
    // 1. ʹ�� OpenCV ��ȡͼ��
    cv::Mat img = cv::imread(filePath, cv::IMREAD_UNCHANGED); // ����ԭʼͨ����

    // ���ͼ���Ƿ���سɹ�
    if (img.empty()) {
        return HImage(); // ���ؿն���
    }

    // 2. ׼��ͼ�����
    int width = img.cols;
    int height = img.rows;
    int channels = img.channels();
    int length = width * height * channels; // ���������ܳ���

    // 3. ���� HImage ����
    HImage hImage;
    hImage.imageHead.width = width;
    hImage.imageHead.height = height;
    hImage.imageHead.channels = channels;
    hImage.imageHead.length = length;

    // 4. �����ڴ沢��������
    hImage.data = new unsigned char[length];
    if (hImage.data != nullptr) {
        // OpenCV Ĭ���� BGR ���У������Ҫ RGB ����������ת��
        if (channels == 3) {
            cv::cvtColor(img, img, cv::COLOR_BGR2RGB); // BGR -> RGB
        }

        // ����ͼ������
        memcpy(hImage.data, img.data, length * sizeof(unsigned char));
    }
    else {
        // �ڴ����ʧ��
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
    // ���÷���ֵ
    outputImages.m_Images.push_back(inputImages.m_Images[0]);

    errcode = 0;
    errmsg = "ͼ���ȡ�ɹ�";
    std::cout << "�ɹ���ȡͼƬ�����ͼƬ����: " << outputImages.m_Images.size() << std::endl;
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

    cout << "�����8001���������ȴ��ͻ��˵���........" << endl;


    while (true) {
        Sleep(1000);
    }

    return 0;
}


int main()
{
    ClientOperation client;

    CommPorts port;
    port.isActAsServer = 0;  // �ͻ���
    port.PortName = "COM1";
   // port.localhost_IP.IP = "192.168.2.225";
   // port.localhost_IP.Port = 12345;

    port.remote_IP.IP = "192.168.2.225";
    port.remote_IP.Port = 12345;

    client.InitSDK(port);
    client.StartWork();


    // ���÷���˵� pic_display ����
    HValues val1;
    HValues val2;
    HImages img;  // ���ڽ��շ��ص�ͼ������
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
        cout << "ͼ���ȡ�ɹ������ص�ͼ������: " << img.m_Images.size() << endl;
    }
    else {
        cout << "����: " << errmsg << endl;
    }

    return 0;

}