#include "controll_demo.h"
#include <opencv2/opencv.hpp>
using namespace std;
using namespace Mz_ClientControl;

// �Զ��庯������ȡ����ͼ�񲢷��� HImage
void pic_display_produce(HImages inputImages, HValues inputParams,
    HImages& outputImages, HValues& outputParams,
    int& errcode, std::string& errmsg, int timeout)
{

    int width = 640;
    int height = 480;
    int channels = 3;  // 3 ͨ����RGB��
    int imageDataLength = width * height * channels;  // ���ݳ���

    // �����ڴ�
    unsigned char* imageData = new unsigned char[imageDataLength];

    std::srand(std::time(0));

    // ���ͼ�����ݣ����ɲ�ɫͼ��
    for (int i = 0; i < imageDataLength; i += 3) {
        imageData[i] = std::rand() % 256;       // R ͨ����0-255��
        imageData[i + 1] = std::rand() % 256;   // G ͨ����0-255��
        imageData[i + 2] = std::rand() % 256;   // B ͨ����0-255��
    }
    // ���� HImage ����
    HImage img(width, height, channels, imageDataLength, imageData);

     delete[] imageData;

    // ����ļ����ݵ� img.data
   // img.data = new unsigned char[fileSize];
    //memcpy(img.data, buffer.data(), fileSize);

    // ��ӵ�����б�
    outputImages.m_Images.push_back(img);

    // ���÷���ֵ
    errcode = 0;
    errmsg = "ͼ���ȡ�ɹ�";
    std::cout << "�ɹ���ȡͼƬ�����ͼƬ����: " << outputImages.m_Images.size() << std::endl;
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



// �����ͻ���
int main()
{
    ClientOperation client;

    CommPorts port;
    port.isActAsServer = 0;  // �ͻ���
    port.PortName = "PIC_PORT";
    port.localhost_IP.IP = "127.0.0.1";
    port.localhost_IP.Port = 8000;

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
    std::cout << "This line is in file: " << __FILE__ << ", line: " << __LINE__ << std::endl;
    client.DoActionFun(port, "pic_display", val1, HImages(), &val2, &img, &errcode, &errmsg,1000);

    
    if (errcode == 0) {
        cout << "ͼ���ȡ�ɹ������ص�ͼ������: " << img.m_Images.size() << endl;
    }
    else {
        cout << "����: " << errmsg << endl;
    }
    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    // ֹͣ�ͻ���
    client.StopWork();
    client.FreeSDK();

    return 0;
}



// ���������
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

    cout << "�����8000���������ȴ��ͻ��˵���........" << endl;

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

    cout << "�����8001���������ȴ��ͻ��˵���........" << endl;

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

    cout << "�����8000���������ȴ��ͻ��˵���........" << endl;

  
    while (true) {
        Sleep(1000);
    }

    return 0;
}
