#include "controll_demo.h"
#include "cameral_demo.h"
 
cv::Mat g_latestImage;

void ImageCallback(cv::Mat& image, void* pUser) {

    if (image.empty()) return;

    // 保存当前图像到内存中变量
    g_latestImage = image.clone();  

    if (g_latestImage.empty()) {
        std::cout << "图像为空，尚未加载。" << std::endl;
    }
    else {
        std::cout << "图像已加载，尺寸为：" << g_latestImage.cols << "x" << g_latestImage.rows << std::endl;

    }

}

HImage ConvertMatToHImage(const cv::Mat& mat)
{
    HImage himage;

    if (mat.empty()) {
        std::cerr << "ConvertMatToHImage: input image is empty!" << std::endl;
        return himage;
    }

    // 设置图像头信息
    himage.imageHead.width = mat.cols;
    himage.imageHead.height = mat.rows;
    himage.imageHead.channels = mat.channels();
    himage.imageHead.length = mat.total() * mat.elemSize();

    // 分配内存并拷贝数据
    himage.data = new unsigned char[himage.imageHead.length];
    if (himage.data != nullptr) {
        std::memcpy(himage.data, mat.data, himage.imageHead.length);
    }

    return himage;
}

HImage encodeMatToHImage(const cv::Mat& mat)
{
    if (mat.empty()) {
        std::cerr << "输入图像为空！" << std::endl;
        return HImage();
    }

    // 将 Mat 压缩成 JPEG 格式
    std::vector<uchar> buf;
    std::vector<int> params = { cv::IMWRITE_JPEG_QUALITY, 90 }; // 可调整质量（0~100）
    cv::imencode(".jpg", mat, buf, params);

    // 设置头信息（注意：此时 channels 指的是压缩前的通道数）
    int width = mat.cols;
    int height = mat.rows;
    int channels = mat.channels();
    int length = static_cast<int>(buf.size());  // 压缩后的数据长度

    // 创建 HImage（data 为压缩后的数据）
    HImage hImage(width, height, channels, length, buf.data());

    return hImage;
}


HImage loadImageAsHImageRaw(const std::string& filePath) {
    // 读取图像为 BGR 格式，原始像素数据
    cv::Mat mat = cv::imread(filePath, cv::IMREAD_COLOR); // 可根据需要改为 IMREAD_UNCHANGED

    if (mat.empty()) {
        std::cerr << "读取图像失败: " << filePath << std::endl;
        return HImage();
    }

    int width = mat.cols;
    int height = mat.rows;
    int channels = mat.channels();
    int length = width * height * channels;

    return HImage(width, height, channels, length, mat.data);
}

void get_frame(HImages inputImages, HValues inputParams,
    HImages& outputImages, HValues& outputParams,
    int& errcode, std::string& errmsg, int timeout)
{
    //初始化库
    if (Mz_CameraConn::MsvInitLib() != 0) {
        std::cerr << "Failed to initialize library." << std::endl;
        return ;
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
    ret = Mz_CameraConn::MsvEnumerateDevics(deviceList);
    //std::cout << "MsvEnumerateDevics  ret=" << ret << std::endl;
    std::cout << "device num: " << deviceList.size() << std::endl;

    std::cout << "deviceList[0].m_id  " << deviceList[0].m_id << std::endl;
    std::cout << "deviceList[0].m_ip  " << deviceList[0].m_ip << std::endl;
    std::cout << "deviceList[0].configFile  " << deviceList[0].configFile << std::endl;

    // 实例化操作类
    Mz_CameraConn::COperation camOp(camera);
    ret = camOp.IsDevConnect();
    if (ret == 0) std::cout << "device is connected" << std::endl;
    else std::cout << "device is not connected  ret=" << ret << std::endl;
    //camOp.GetDeviceSN();

    // 打开设备
    ret = camOp.MsvOpenDevice();
    std::cout << "Failed to open camera device." << ret << std::endl;
    camOp.RegisterImageCallBack(ImageCallback);
    // 开始采图
    camOp.MsvStartImageCapture();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    ret = camOp.MsvStopImageCapture();



    // 停止采图
    camOp.MsvStopImageCapture();

    // 关闭设备
    camOp.MsvCloseDevice();

    // 关闭库
    Mz_CameraConn::MsvCloseLib();

  //  std::string path = "E:\\My  Download\\log\\image.jpg";

    // 创建 HImage 对象
    HImage img = encodeMatToHImage(g_latestImage);
  //  HImage img = loadImageAsHImageRaw(path);

    // 添加到输出列表
    outputImages.m_Images.push_back(img);

    // 设置返回值
    errcode = 0;
    errmsg = "图像读取成功";
    std::cout << "成功读取图片，输出图片数量: " << outputImages.m_Images.size() << std::endl;
    if (g_latestImage.empty()) {
        std::cout << "图像为空，尚未加载。" << std::endl;
    }
    else {
        std::cout << "图像已加载，尺寸为：" << g_latestImage.cols << "x" << g_latestImage.rows << std::endl;
    }


}

int init()
{

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
    ret = Mz_CameraConn::MsvEnumerateDevics(deviceList);
    //std::cout << "MsvEnumerateDevics  ret=" << ret << std::endl;
    std::cout << "device num: " << deviceList.size() << std::endl;

    std::cout << "deviceList[0].m_id  " << deviceList[0].m_id << std::endl;
    std::cout << "deviceList[0].m_ip  " << deviceList[0].m_ip << std::endl;
    std::cout << "deviceList[0].configFile  " << deviceList[0].configFile << std::endl;

    // 实例化操作类
    Mz_CameraConn::COperation camOp(camera);
    ret = camOp.IsDevConnect();
    if (ret == 0) std::cout << "device is connected" << std::endl;
    else std::cout << "device is not connected  ret=" << ret << std::endl;
    //camOp.GetDeviceSN();

    // 打开设备
    ret = camOp.MsvOpenDevice();
    std::cout << "Failed to open camera device." << ret << std::endl;
    camOp.RegisterImageCallBack(ImageCallback);
    // 开始采图
    camOp.MsvStartImageCapture();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    ret = camOp.MsvStopImageCapture();




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

    // 获取总字节数（行数 × 每行字节数）
    size_t imageSizeBytes = g_latestImage.total() * g_latestImage.elemSize();
    double imageSizeKB = imageSizeBytes / 1024.0;
    double imageSizeMB = imageSizeKB / 1024.0;

    std::cout << "Image size: " << imageSizeBytes << " bytes" << std::endl;
    std::cout << "Image size: " << imageSizeKB << " KB" << std::endl;
    std::cout << "Image size: " << imageSizeMB << " MB" << std::endl;

}

int final()
{
#if 1
    Mz_ClientControl::ClientOperation server;

    CommPorts port;
    port.isActAsServer = 1;
    port.PortName = "PIC_PORT";
    port.localhost_IP.IP = "127.0.0.1";
    port.localhost_IP.Port = 8000;

    server.InitSDK(port);

    Callbackfunc cb;
    cb.funcname = "get_frame";
    cb.func = reinterpret_cast<FunctionTableStream>(get_frame);

    cb.inputImagesnums = 0;
    cb.inputPramsnums = 0;
    cb.outputImagesnums = 1;
    cb.outputPramsnums = 0;

    server.RegsiterFunitFun(cb);
    server.StartWork();

    cout << "服务端8000已启动，等待客户端调用........" << endl;

    while (true) {
        Sleep(1000);
    }
#endif

    

    return 0;

}