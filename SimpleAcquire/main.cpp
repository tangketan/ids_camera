#include <uEye.h>
#include <uEye_tools.h>
#include <ueye_deprecated.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "ids_cam.h"
using namespace cv;
using namespace std;
int main(int argc, char *argv[])
{
#if 0
    HIDS hCam = 0;
    char* imgMem;
    int memId;
    if (is_InitCamera(&hCam, NULL) != IS_SUCCESS){
        return 0;
    }

    int img_width = 2*640, img_height = 2*480, img_bpp = 24, img_step, img_data_size;
    is_AllocImageMem(hCam, img_width, img_height, img_bpp, &imgMem, &memId);
    is_SetImageMem(hCam, imgMem, memId);
    is_SetDisplayMode(hCam, IS_SET_DM_DIB);
    is_SetColorMode(hCam, IS_CM_RGB8_PACKED);
    is_SetImageSize(hCam, img_width, img_height);


    double enable = 1;
    double disable = 0;
    is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_GAIN, &enable, 0);
    is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_WHITEBALANCE, &enable, 0);
    is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_FRAMERATE, &disable, 0);
    is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_SHUTTER, &disable, 0);
    is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_SENSOR_GAIN, &enable, 0);
    is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_SENSOR_WHITEBALANCE, &enable, 0);
    is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_SENSOR_SHUTTER, &disable, 0);

    double FPS, NEWFPS;
    FPS = 15;
    double fps;
    is_SetFrameRate(hCam, FPS, &NEWFPS);

    double parameter = 50;
    int error = is_Exposure(hCam, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&parameter, sizeof(parameter));
    if (error != IS_SUCCESS) {
        cout << "): failed" << parameter;
    }

    error = is_Exposure(hCam, IS_EXPOSURE_CMD_GET_EXPOSURE, (void*)&parameter, sizeof(parameter));

    UINT uiCaps = 0;

    INT nRet = is_Focus(hCam, FDT_CMD_GET_CAPABILITIES, &uiCaps, sizeof (uiCaps));

    if (nRet == IS_SUCCESS && (uiCaps & FOC_CAP_AUTOFOCUS_SUPPORTED))
    {
        cout << "If supported, enable auto focus";
        nRet = is_Focus(hCam, FOC_CMD_SET_DISABLE_AUTOFOCUS, NULL, 0);
    }

    //nRet = is_SetGamma(hCam, 600);
    char filename[512];

    for (int ii = 0; ; ii++)
    {
        if (is_FreezeVideo(hCam, IS_WAIT) == IS_SUCCESS){
            void *pMemVoid; //pointer to where the image is stored
            is_GetImageMem(hCam, &pMemVoid);
            Mat im(Size(img_width, img_height), CV_8UC3, pMemVoid);
            imshow("im", im);

            char c = cv::waitKey(5);
            if (c == 'q'){
                break;
            }
        }
        else{
            cout << "ERROR FREEZE";
        }
    }

    is_ExitCamera(hCam);

#endif

#if 1
    IDS_Cam cam;
    if (cam.IsOpened()){
        cam.PrintSensorInfo();
    }
    else
    {
        printf("can't open cam!\n");
    }

    Mat im;
    for (int i = 0; i < 1000;i++)
    {
        im = cam.Read();

        imshow("im", im);
        char c = cv::waitKey(5);
        if (c == 'q'){
            cam.CloseCamera();
            return 0;
        }
    }

#endif
    return 0;
}