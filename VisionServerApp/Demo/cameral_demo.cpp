#include "cameral_demo.h"


void MyImageCallback(cv::Mat& image, void* pUser) {
    // ��������·��
    std::string directory = "E:\\My  Download\\log\\";
    std::filesystem::create_directories(directory);  // ȷ��Ŀ¼����

    // ��ȡ��ǰʱ����Ϊ�ļ���
    std::time_t t = std::time(nullptr);
    std::tm localTime;
    localtime_s(&localTime, &t);  // �̰߳�ȫ�汾

    std::ostringstream filenameStream;
    filenameStream << std::put_time(&localTime, "%Y%m%d_%H%M%S") << ".jpg";
    std::string filepath = directory + filenameStream.str();

    // ����ͼ��Ϊ JPG
    try {
        if (!cv::imwrite(filepath, image)) {
            std::cerr << "Failed to save image to: " << filepath << std::endl;
        }
        else {
            std::cout << "Image saved to: " << filepath << std::endl;
        }
    }
    catch (const cv::Exception& e) {
        std::cerr << "OpenCV exception when saving image: " << e.what() << std::endl;
    }
}

int test100() {


    //��ʼ����
    if (Mz_CameraConn::MsvInitLib() != 0) {
        std::cerr << "Failed to initialize library." << std::endl;
        return -1;
    }

    // �����豸ID
    MsvDeviceId camera;
    camera.m_ip = "169.254.239.100";
    camera.m_type = DEVICE_TYPE_DHUA;
    camera.m_id = "BE26765AAK00052";
    camera.configFile = "";

    int ret;
    std::vector<MsvDeviceId> deviceList;
    deviceList.push_back(camera);
    ret=Mz_CameraConn::MsvEnumerateDevics(deviceList);
    //std::cout << "MsvEnumerateDevics  ret=" << ret << std::endl;
    std::cout << "device num: " << deviceList.size() << std::endl;

    std::cout << "deviceList[0].m_id  " << deviceList[0].m_id << std::endl;
    std::cout << "deviceList[0].m_ip  " << deviceList[0].m_ip << std::endl;
        std::cout << "deviceList[0].configFile  " << deviceList[0].configFile << std::endl;
    
    // ʵ����������
    Mz_CameraConn::COperation camOp(camera);
    ret = camOp.IsDevConnect();
    if (ret == 0) std::cout << "device is connected" << std::endl;
    else std::cout << "device is not connected  ret="<<ret<<std::endl;
    //camOp.GetDeviceSN();

    // ���豸
    ret = camOp.MsvOpenDevice();
        std::cout << "Failed to open camera device." << ret<<std::endl;
        camOp.RegisterImageCallBack(MyImageCallback);
        // ��ʼ��ͼ
        camOp.MsvStartImageCapture();

        getchar();

        ret = camOp.MsvStopImageCapture();
        // ��ʼ��ͼ
        camOp.MsvStartImageCapture();


#if 0
    // ���ô���ģʽΪ����
//  camOp.SetTrrigerModel(1); 
//   camOp.SetTrrigerSource(7); 
    // ������ͼ��ȡ��ͼ��
    for (int i = 0; i < 10; ++i) {
        camOp.SoftTrigger();
        cv::Mat img = camOp.GetFrame(1000); // ��ʱ1000ms
        if (!img.empty()) {
            std::cout << "Captured frame: " << img.cols << "x" << img.rows << std::endl;
            // ʾ������ʾͼ��
            cv::imshow("Captured Frame", img);
            cv::waitKey(500);
        }
        else {
            std::cout << "Failed to capture frame." << std::endl;
        }
    }
#endif


    // ֹͣ��ͼ
    camOp.MsvStopImageCapture();

    // �ر��豸
    camOp.MsvCloseDevice();

    // �رտ�
    Mz_CameraConn::MsvCloseLib();

    return 0;
}