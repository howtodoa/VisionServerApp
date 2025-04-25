#include "cameral_demo.h"


void MyImageCallback(cv::Mat& image, void* pUser) {
    // 创建保存路径
    std::string directory = "E:\\My  Download\\log\\";
    std::filesystem::create_directories(directory);  // 确保目录存在

    // 获取当前时间作为文件名
    std::time_t t = std::time(nullptr);
    std::tm localTime;
    localtime_s(&localTime, &t);  // 线程安全版本

    std::ostringstream filenameStream;
    filenameStream << std::put_time(&localTime, "%Y%m%d_%H%M%S") << ".jpg";
    std::string filepath = directory + filenameStream.str();

    // 保存图像为 JPG
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


    //初始化库
    if (Mz_CameraConn::MsvInitLib() != 0) {
        std::cerr << "Failed to initialize library." << std::endl;
        return -1;
    }

    // 构造设备ID
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
    
    // 实例化操作类
    Mz_CameraConn::COperation camOp(camera);
    ret = camOp.IsDevConnect();
    if (ret == 0) std::cout << "device is connected" << std::endl;
    else std::cout << "device is not connected  ret="<<ret<<std::endl;
    //camOp.GetDeviceSN();

    // 打开设备
    ret = camOp.MsvOpenDevice();
        std::cout << "Failed to open camera device." << ret<<std::endl;
        camOp.RegisterImageCallBack(MyImageCallback);
        // 开始采图
        camOp.MsvStartImageCapture();

        getchar();

        ret = camOp.MsvStopImageCapture();
        // 开始采图
        camOp.MsvStartImageCapture();


#if 0
    // 设置触发模式为软触发
//  camOp.SetTrrigerModel(1); 
//   camOp.SetTrrigerSource(7); 
    // 触发采图并取回图像
    for (int i = 0; i < 10; ++i) {
        camOp.SoftTrigger();
        cv::Mat img = camOp.GetFrame(1000); // 超时1000ms
        if (!img.empty()) {
            std::cout << "Captured frame: " << img.cols << "x" << img.rows << std::endl;
            // 示例：显示图像
            cv::imshow("Captured Frame", img);
            cv::waitKey(500);
        }
        else {
            std::cout << "Failed to capture frame." << std::endl;
        }
    }
#endif


    // 停止采图
    camOp.MsvStopImageCapture();

    // 关闭设备
    camOp.MsvCloseDevice();

    // 关闭库
    Mz_CameraConn::MsvCloseLib();

    return 0;
}