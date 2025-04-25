#include "controll_demo.h"
#include "cameral_demo.h"
 
cv::Mat g_latestImage;

void ImageCallback(cv::Mat& image, void* pUser) {

    if (image.empty()) return;

    // ���浱ǰͼ���ڴ��б���
    g_latestImage = image.clone();  

    if (g_latestImage.empty()) {
        std::cout << "ͼ��Ϊ�գ���δ���ء�" << std::endl;
    }
    else {
        std::cout << "ͼ���Ѽ��أ��ߴ�Ϊ��" << g_latestImage.cols << "x" << g_latestImage.rows << std::endl;

    }

}

HImage ConvertMatToHImage(const cv::Mat& mat)
{
    HImage himage;

    if (mat.empty()) {
        std::cerr << "ConvertMatToHImage: input image is empty!" << std::endl;
        return himage;
    }

    // ����ͼ��ͷ��Ϣ
    himage.imageHead.width = mat.cols;
    himage.imageHead.height = mat.rows;
    himage.imageHead.channels = mat.channels();
    himage.imageHead.length = mat.total() * mat.elemSize();

    // �����ڴ沢��������
    himage.data = new unsigned char[himage.imageHead.length];
    if (himage.data != nullptr) {
        std::memcpy(himage.data, mat.data, himage.imageHead.length);
    }

    return himage;
}

HImage encodeMatToHImage(const cv::Mat& mat)
{
    if (mat.empty()) {
        std::cerr << "����ͼ��Ϊ�գ�" << std::endl;
        return HImage();
    }

    // �� Mat ѹ���� JPEG ��ʽ
    std::vector<uchar> buf;
    std::vector<int> params = { cv::IMWRITE_JPEG_QUALITY, 90 }; // �ɵ���������0~100��
    cv::imencode(".jpg", mat, buf, params);

    // ����ͷ��Ϣ��ע�⣺��ʱ channels ָ����ѹ��ǰ��ͨ������
    int width = mat.cols;
    int height = mat.rows;
    int channels = mat.channels();
    int length = static_cast<int>(buf.size());  // ѹ��������ݳ���

    // ���� HImage��data Ϊѹ��������ݣ�
    HImage hImage(width, height, channels, length, buf.data());

    return hImage;
}


HImage loadImageAsHImageRaw(const std::string& filePath) {
    // ��ȡͼ��Ϊ BGR ��ʽ��ԭʼ��������
    cv::Mat mat = cv::imread(filePath, cv::IMREAD_COLOR); // �ɸ�����Ҫ��Ϊ IMREAD_UNCHANGED

    if (mat.empty()) {
        std::cerr << "��ȡͼ��ʧ��: " << filePath << std::endl;
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
    //��ʼ����
    if (Mz_CameraConn::MsvInitLib() != 0) {
        std::cerr << "Failed to initialize library." << std::endl;
        return ;
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
    ret = Mz_CameraConn::MsvEnumerateDevics(deviceList);
    //std::cout << "MsvEnumerateDevics  ret=" << ret << std::endl;
    std::cout << "device num: " << deviceList.size() << std::endl;

    std::cout << "deviceList[0].m_id  " << deviceList[0].m_id << std::endl;
    std::cout << "deviceList[0].m_ip  " << deviceList[0].m_ip << std::endl;
    std::cout << "deviceList[0].configFile  " << deviceList[0].configFile << std::endl;

    // ʵ����������
    Mz_CameraConn::COperation camOp(camera);
    ret = camOp.IsDevConnect();
    if (ret == 0) std::cout << "device is connected" << std::endl;
    else std::cout << "device is not connected  ret=" << ret << std::endl;
    //camOp.GetDeviceSN();

    // ���豸
    ret = camOp.MsvOpenDevice();
    std::cout << "Failed to open camera device." << ret << std::endl;
    camOp.RegisterImageCallBack(ImageCallback);
    // ��ʼ��ͼ
    camOp.MsvStartImageCapture();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    ret = camOp.MsvStopImageCapture();



    // ֹͣ��ͼ
    camOp.MsvStopImageCapture();

    // �ر��豸
    camOp.MsvCloseDevice();

    // �رտ�
    Mz_CameraConn::MsvCloseLib();

  //  std::string path = "E:\\My  Download\\log\\image.jpg";

    // ���� HImage ����
    HImage img = encodeMatToHImage(g_latestImage);
  //  HImage img = loadImageAsHImageRaw(path);

    // ��ӵ�����б�
    outputImages.m_Images.push_back(img);

    // ���÷���ֵ
    errcode = 0;
    errmsg = "ͼ���ȡ�ɹ�";
    std::cout << "�ɹ���ȡͼƬ�����ͼƬ����: " << outputImages.m_Images.size() << std::endl;
    if (g_latestImage.empty()) {
        std::cout << "ͼ��Ϊ�գ���δ���ء�" << std::endl;
    }
    else {
        std::cout << "ͼ���Ѽ��أ��ߴ�Ϊ��" << g_latestImage.cols << "x" << g_latestImage.rows << std::endl;
    }


}

int init()
{

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
    ret = Mz_CameraConn::MsvEnumerateDevics(deviceList);
    //std::cout << "MsvEnumerateDevics  ret=" << ret << std::endl;
    std::cout << "device num: " << deviceList.size() << std::endl;

    std::cout << "deviceList[0].m_id  " << deviceList[0].m_id << std::endl;
    std::cout << "deviceList[0].m_ip  " << deviceList[0].m_ip << std::endl;
    std::cout << "deviceList[0].configFile  " << deviceList[0].configFile << std::endl;

    // ʵ����������
    Mz_CameraConn::COperation camOp(camera);
    ret = camOp.IsDevConnect();
    if (ret == 0) std::cout << "device is connected" << std::endl;
    else std::cout << "device is not connected  ret=" << ret << std::endl;
    //camOp.GetDeviceSN();

    // ���豸
    ret = camOp.MsvOpenDevice();
    std::cout << "Failed to open camera device." << ret << std::endl;
    camOp.RegisterImageCallBack(ImageCallback);
    // ��ʼ��ͼ
    camOp.MsvStartImageCapture();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    ret = camOp.MsvStopImageCapture();




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

    // ��ȡ���ֽ��������� �� ÿ���ֽ�����
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

    cout << "�����8000���������ȴ��ͻ��˵���........" << endl;

    while (true) {
        Sleep(1000);
    }
#endif

    

    return 0;

}