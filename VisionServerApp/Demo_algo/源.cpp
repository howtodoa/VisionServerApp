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



int main()
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