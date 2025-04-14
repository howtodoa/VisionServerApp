#include <iostream>
#include <tchar.h>
#include <windows.h> 
#include "MZ_ADOConn.h"

//��������޸�
int test1() {
    Mz_DB::DBOperation db;
    wchar_t connStr[] = L"Driver={MySQL ODBC 8.0 Unicode Driver};Server=localhost;Database=mz_test;Uid=root;Pwd=123456;";

    // 1. �������ݿ�
    int connectResult = db.Mz_ConnectDB(connStr);
    if (connectResult == 0) {
        // 3. �򿪼�¼��
        wchar_t recordStr[] = L"SELECT * FROM camera_parameters where camera_number=4"; 
        int openResult = db.Mz_OpenRecord(recordStr);

        if (openResult == 0) {
            // 5. ��ȡ��¼ֵ
            int camera_number = 0;
            db.Mz_GetRecordeValue(L"camera_number", camera_number);
            std::wcout << L"camera_number = " << camera_number << std::endl;

            int camera_type = 0;
            db.Mz_GetRecordeValue(L"camera_type", camera_type);
            std::wcout << L"camera_type = " << camera_type << std::endl;

            int camera_station = 0;
            db.Mz_GetRecordeValue(L"camera_station", camera_station);
            std::wcout << L"camera_station = " << camera_station << std::endl;

            // ������ֵ����鷵��ֵ
            int setResult;
            //������������޷��޸�
           // setResult = db.Mz_SetRecordeValue(L"camera_number", 100);
            

            setResult = db.Mz_SetRecordeValue(L"camera_type", 200);
            std::wcout << L"Set camera_type result: " << setResult << std::endl;

            setResult = db.Mz_SetRecordeValue(L"camera_station", 3);
            std::wcout << L"Set camera_station result: " << setResult << std::endl;
            //����֮��Ż�ʧЧ��ǰ����޸�ֻ���޸ļ�¼���ı��ػ���
            db.Mz_UpdataRecorde();

            // �ٴλ�ȡ����֤
            db.Mz_GetRecordeValue(L"camera_number", camera_number);
            std::wcout << L"After set, camera_number = " << camera_number << std::endl;

            db.Mz_GetRecordeValue(L"camera_type", camera_type);
            std::wcout << L"After set, camera_type = " << camera_type << std::endl;

            db.Mz_GetRecordeValue(L"camera_station", camera_station);
            std::wcout << L"After set, camera_station = " << camera_station << std::endl;

            // 4. �رռ�¼��
            db.Mz_CloseRecord();
        }

        // 2. �ر����ݿ�����
        db.Mz_CloseDB();
    }

    return 0;
}

//����������
int test2() {

    Mz_DB::DBOperation db;
    wchar_t connStr[] = L"Driver={MySQL ODBC 8.0 Unicode Driver};Server=localhost;Database=mz_test;Uid=root;Pwd=123456;";

    int connectResult = db.Mz_ConnectDB(connStr);
    std::wcout << L"Connect result: " << connectResult << std::endl;
    if (connectResult == 0) {
        // 1. ���� cameras
        wchar_t cameraStr[] = L"SELECT * FROM cameras";
        int openCameraResult = db.Mz_OpenRecord(cameraStr);
        std::wcout << L"Open cameras result: " << openCameraResult << std::endl;

        if (openCameraResult == 0) {
            int addCameraResult = db.Mz_AddNewRecorde();
            std::wcout << L"Add cameras result: " << addCameraResult << std::endl;
            if (addCameraResult == 0) {
                db.Mz_SetRecordeValue(L"camera_number", 7);
                db.Mz_SetRecordeValue(L"trigger_mode", 2);
            }
            db.Mz_UpdataRecorde();
            db.Mz_CloseRecord();
        }

        // �������״̬
        std::wcout << L"Connection still active: " << (db.Mz_ConnectDB(connStr) == 0 ? L"Yes" : L"No") << std::endl;

        // 2. ���� camera_parameters
        wchar_t recordStr[] = L"SELECT * FROM camera_parameters";
        int openResult = db.Mz_OpenRecord(recordStr);
        std::wcout << L"Open camera_parameters result: " << openResult << std::endl;

        if (openResult == 0) {
            int addResult = db.Mz_AddNewRecorde();
            std::wcout << L"Add camera_parameters result: " << addResult << std::endl;
            if (addResult == 0) {
                db.Mz_SetRecordeValue(L"camera_number", 7);
                db.Mz_SetRecordeValue(L"camera_name", _bstr_t(L"cameral2"));
                db.Mz_SetRecordeValue(L"camera_sn", _bstr_t(L"SN002"));
                db.Mz_SetRecordeValue(L"camera_ip", _bstr_t(L"192.168.0.1"));
                db.Mz_SetRecordeValue(L"camera_type", 25);
                db.Mz_SetRecordeValue(L"camera_station", 35);
                db.Mz_SetRecordeValue(L"camera_ad", 500);
                db.Mz_SetRecordeValue(L"camera_gain", 650);
                db.Mz_SetRecordeValue(L"camera_exposure", 1500);
            }
            db.Mz_UpdataRecorde();
            db.Mz_CloseRecord();
        }

        db.Mz_CloseDB();
    }


    return 0;
}

// ��������������ֺŷָ�������
int countSemicolons(const wchar_t* str) {
    int count = 1; // ����һ��
    while (*str) {
        if (*str == L';') count++;
        str++;
    }
    return count;
}

//�㷨����޸�
int test3() {
    Mz_DB::DBOperation db;
    wchar_t connStr[] = L"Driver={MySQL ODBC 8.0 Unicode Driver};Server=localhost;Database=mz_test;Uid=root;Pwd=123456;";

    // 1. �������ݿ�
    int connectResult = db.Mz_ConnectDB(connStr);
    std::wcout << L"Connect result: " << connectResult << std::endl;
    if (connectResult == 0) {
        // 2. �򿪼�¼��
        wchar_t recordStr[] = L"SELECT * FROM algorithm_parameters WHERE camera_number=8";
        int openResult = db.Mz_OpenRecord(recordStr);
        std::wcout << L"Open result: " << openResult << std::endl;

        if (openResult == 0) {
            // 3. ��ȡ��ʼֵ
            int camera_number = 0;
            db.Mz_GetRecordeValue(L"camera_number", camera_number);
            std::wcout << L"camera_number = " << camera_number << std::endl;

            _bstr_t algorithm_name;
            db.Mz_GetRecordeValue(L"algorithm_name", algorithm_name);
            std::wcout << L"algorithm_name = " << (const wchar_t*)algorithm_name << std::endl;

            _bstr_t value_names;
            db.Mz_GetRecordeValue(L"value_names", value_names);
            std::wcout << L"value_names = " << (const wchar_t*)value_names << std::endl;

            _bstr_t threshold_weigh;
            db.Mz_GetRecordeValue(L"threshold_weigh", threshold_weigh);
            std::wcout << L"threshold_weigh = " << (const wchar_t*)threshold_weigh << std::endl;

            _bstr_t updated_at;
            db.Mz_GetRecordeValue(L"updated_at", updated_at);
            std::wcout << L"updated_at = " << (const wchar_t*)updated_at << std::endl;

            // 4. ������ֵ�����һ����
            int setResult;
            const wchar_t* new_value_names = L"threshold;weigh;extra";
            const wchar_t* new_threshold_weigh = L"1.0;2.0;3.0";

            // ��֤����ƥ��
            int names_count = countSemicolons(new_value_names);
            int values_count = countSemicolons(new_threshold_weigh);
            if (names_count != values_count) {
                std::wcout << L"Error: value_names and threshold_weigh counts do not match!" << std::endl;
            }
            else {
                setResult = db.Mz_SetRecordeValue(L"algorithm_name", _bstr_t(L"Algo2"));
                std::wcout << L"Set algorithm_name result: " << setResult << std::endl;

                setResult = db.Mz_SetRecordeValue(L"value_names", _bstr_t(new_value_names));
                std::wcout << L"Set value_names result: " << setResult << std::endl;

                setResult = db.Mz_SetRecordeValue(L"threshold_weigh", _bstr_t(new_threshold_weigh));
                std::wcout << L"Set threshold_weigh result: " << setResult << std::endl;

                setResult = db.Mz_SetRecordeValue(L"updated_at", _bstr_t(L"2025-04-03 14:00:00"));
                std::wcout << L"Set updated_at result: " << setResult << std::endl;

                // 5. �ύ����
                db.Mz_UpdataRecorde(); // ����Ƿ�ӦΪ Mz_UpdateRecorde
            }

            // 6. ��֤�޸Ľ��
            db.Mz_GetRecordeValue(L"camera_number", camera_number);
            std::wcout << L"After set, camera_number = " << camera_number << std::endl;

            db.Mz_GetRecordeValue(L"algorithm_name", algorithm_name);
            std::wcout << L"After set, algorithm_name = " << (const wchar_t*)algorithm_name << std::endl;

            db.Mz_GetRecordeValue(L"value_names", value_names);
            std::wcout << L"After set, value_names = " << (const wchar_t*)value_names << std::endl;

            db.Mz_GetRecordeValue(L"threshold_weigh", threshold_weigh);
            std::wcout << L"After set, threshold_weigh = " << (const wchar_t*)threshold_weigh << std::endl;

            db.Mz_GetRecordeValue(L"updated_at", updated_at);
            std::wcout << L"After set, updated_at = " << (const wchar_t*)updated_at << std::endl;

            // 7. �رռ�¼��
            db.Mz_CloseRecord();
        }

        // 8. �ر����ݿ�����
        db.Mz_CloseDB();
    }

    return 0;
}

//�㷨������
int test4() {


    Mz_DB::DBOperation db;
    wchar_t connStr[] = L"Driver={MySQL ODBC 8.0 Unicode Driver};Server=localhost;Database=mz_test;Uid=root;Pwd=123456;";

    int connectResult = db.Mz_ConnectDB(connStr);
    std::wcout << L"Connect result: " << connectResult << std::endl;
    if (connectResult == 0) {
        // 1. ���� cameras
        wchar_t cameraStr[] = L"SELECT * FROM cameras";
        int openCameraResult = db.Mz_OpenRecord(cameraStr);
        std::wcout << L"Open cameras result: " << openCameraResult << std::endl;

        if (openCameraResult == 0) {
            int addCameraResult = db.Mz_AddNewRecorde();
            std::wcout << L"Add cameras result: " << addCameraResult << std::endl;
            if (addCameraResult == 0) {
                db.Mz_SetRecordeValue(L"camera_number", 8);
                db.Mz_SetRecordeValue(L"trigger_mode", 2);
            }
            int updateResult = db.Mz_UpdataRecorde();
            std::wcout << L"Update cameras result: " << updateResult << std::endl;
            if (updateResult != 0) {
                std::wcout << L"Failed to update cameras" << std::endl;
            }
            db.Mz_CloseRecord();
        }

        // �������״̬
        std::wcout << L"Connection still active: " << (db.Mz_ConnectDB(connStr) == 0 ? L"Yes" : L"No") << std::endl;

        // 2. ���� algorithm_parameters
        wchar_t algoStr[] = L"SELECT * FROM algorithm_parameters";
        int openAlgoResult = db.Mz_OpenRecord(algoStr);
        std::wcout << L"Open algorithm_parameters result: " << openAlgoResult << std::endl;

        if (openAlgoResult == 0) {
            int addAlgoResult = db.Mz_AddNewRecorde();
            std::wcout << L"Add algorithm_parameters result: " << addAlgoResult << std::endl;
            if (addAlgoResult == 0) {
                int setResult;
                setResult = db.Mz_SetRecordeValue(L"camera_number", 8);
                std::wcout << L"Set camera_number result: " << setResult << std::endl;
                setResult = db.Mz_SetRecordeValue(L"algorithm_name", _bstr_t(L"Algo1"));
                std::wcout << L"Set algorithm_name result: " << setResult << std::endl;
                setResult = db.Mz_SetRecordeValue(L"value_names", _bstr_t(L"threshold;weight")); // ����ƴд
                std::wcout << L"Set value_names result: " << setResult << std::endl;
                setResult = db.Mz_SetRecordeValue(L"value_values", _bstr_t(L"0.8;1.7")); // �����ֶ���
                std::wcout << L"Set value_values result: " << setResult << std::endl;
                if (setResult != 0) {
                    std::wcout << L"Failed to set value_values" << std::endl;
                }
                setResult = db.Mz_SetRecordeValue(L"updated_at", _bstr_t(L"2025-04-02 15:00:00"));
                std::wcout << L"Set updated_at result: " << setResult << std::endl;
            }
            int updateAlgoResult = db.Mz_UpdataRecorde();
            std::wcout << L"Update algorithm_parameters result: " << updateAlgoResult << std::endl;
            if (updateAlgoResult != 0) {
                std::wcout << L"Failed to update algorithm_parameters" << std::endl;
            }
            db.Mz_CloseRecord();
        }

        db.Mz_CloseDB();
    }

    return 0;
}

int main()
{
    test3();
    return 0;
}