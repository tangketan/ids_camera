#include "ids_cam.h"
#include <ueye_deprecated.h>
#include <string.h>
#include <string>

IDS_Cam::IDS_Cam()
{
    m_hCam = 0;

    img_width = 640*2;
    img_height = 480*2;
    m_nRenderMode = IS_RENDER_FIT_TO_WINDOW;

    OpenCamera();		// open a camera handle

    SetAutoParam();
}

IDS_Cam::~IDS_Cam()
{
    CloseCamera();
    m_hCam = 0;
}

bool IDS_Cam::OpenCamera()
{
    if (m_hCam != 0)
    {
        //free old image mem.
        is_FreeImageMem(m_hCam, m_ImageMemory, m_lMemoryId);
        is_ExitCamera(m_hCam);
    }

    // init camera
    m_hCam = (HIDS)0;						// open next camera
    m_Ret = InitCamera(&m_hCam, NULL);		// init camera - no window handle required
    if (m_Ret != IS_SUCCESS)
    {
        printf("InitCamera failed!\n");
        return false;
    }

    // Get sensor info
    is_GetSensorInfo(m_hCam, &m_sInfo);

    int maxSizeX, maxSizeY;
    GetMaxImageSize(&maxSizeX, &maxSizeY);
    printf("max size: %d %d\n", maxSizeX, maxSizeY);

    // setup the color depth to the current windows setting
    is_GetColorDepth(m_hCam, &m_nBitsPerPixel, &m_nColorMode);
    is_SetColorMode(m_hCam, /*m_nColorMode*/IS_CM_RGB8_PACKED);
    printf("m_nBitsPerPixel, m_nColorMode: %d %d\n", m_nBitsPerPixel, m_nColorMode);

    int img_bpp = 24;
    // memory initialization
    is_AllocImageMem(m_hCam, img_width, img_height, img_bpp, &m_ImageMemory, &m_lMemoryId);
    is_SetImageMem(m_hCam, m_ImageMemory, m_lMemoryId);	// set memory active

    is_SetImageSize(m_hCam, img_width, img_height);

    // display initialization
    IS_SIZE_2D imageSize;
    imageSize.s32Width = img_width;
    imageSize.s32Height = img_height;
    //is_AOI(m_hCam, IS_AOI_IMAGE_SET_SIZE, (void*)&imageSize, sizeof(imageSize));

    is_SetDisplayMode(m_hCam, IS_SET_DM_DIB);

    return true;
}

void IDS_Cam::CloseCamera()
{
    if (m_hCam != 0)
    {
        //free old image mem.
        is_FreeImageMem(m_hCam, m_ImageMemory, m_lMemoryId);
        is_ExitCamera(m_hCam);
    }
}

int IDS_Cam::InitCamera(HIDS *hCam, HWND hWnd)
{
    int nRet = is_InitCamera(hCam, hWnd);
    /************************************************************************************************/
    /*  If the camera returns with "IS_STARTER_FW_UPLOAD_NEEDED", an upload of a new firmware       */
    /*  is necessary. This upload can take several seconds. We recommend to check the required      */
    /*  time with the function is_GetDuration().                                                    */
    /*  In this case, the camera can only be opened if the flag "IS_ALLOW_STARTER_FW_UPLOAD"        */
    /*  is "OR"-ed to m_hCam. This flag allows an automatic upload of the firmware.                 */
    /************************************************************************************************/
    if (nRet == IS_STARTER_FW_UPLOAD_NEEDED)
    {
        // Time for the firmware upload = 25 seconds by default
        int nUploadTime = 25000;
        is_GetDuration(*hCam, IS_STARTER_FW_UPLOAD, &nUploadTime);

        std::string Str1, Str2;
        Str1 = "This camera requires a new firmware. The upload will take about";
        Str2 = "seconds. Please wait ...";
        printf("%s %d %s", Str1.c_str(), nUploadTime / 1000, Str2.c_str());


        // Try again to open the camera. This time we allow the automatic upload of the firmware by
        // specifying "IS_ALLOW_STARTER_FIRMWARE_UPLOAD"
        *hCam = (HIDS)(((int)*hCam) | IS_ALLOW_STARTER_FW_UPLOAD);
        nRet = is_InitCamera(hCam, hWnd);
    }

    return nRet;
}

void IDS_Cam::GetMaxImageSize(INT *pnSizeX, INT *pnSizeY)
{
    // Check if the camera supports an arbitrary AOI
    // Only the ueye xs does not support an arbitrary AOI
    INT nAOISupported = 0;
    BOOL bAOISupported = TRUE;
    if (is_ImageFormat(m_hCam,
        IMGFRMT_CMD_GET_ARBITRARY_AOI_SUPPORTED,
        (void*)&nAOISupported,
        sizeof(nAOISupported)) == IS_SUCCESS)
    {
        bAOISupported = (nAOISupported != 0);
    }

    if (bAOISupported)
    {
        // All other sensors
        // Get maximum image size
        SENSORINFO sInfo;
        is_GetSensorInfo(m_hCam, &sInfo);
        *pnSizeX = sInfo.nMaxWidth;
        *pnSizeY = sInfo.nMaxHeight;
    }
    else
    {
        // Only ueye xs
        // Get image size of the current format
        IS_SIZE_2D imageSize;
        is_AOI(m_hCam, IS_AOI_IMAGE_GET_SIZE, (void*)&imageSize, sizeof(imageSize));

        *pnSizeX = imageSize.s32Width;
        *pnSizeY = imageSize.s32Height;
    }
}

void IDS_Cam::SetAutoParam()
{
    double enable = 1;
    double disable = 0;
    is_SetAutoParameter(m_hCam, IS_SET_ENABLE_AUTO_GAIN, &enable, 0);
    is_SetAutoParameter(m_hCam, IS_SET_ENABLE_AUTO_WHITEBALANCE, &enable, 0);
    is_SetAutoParameter(m_hCam, IS_SET_ENABLE_AUTO_FRAMERATE, &disable, 0);
    is_SetAutoParameter(m_hCam, IS_SET_ENABLE_AUTO_SHUTTER, &enable, 0);
    is_SetAutoParameter(m_hCam, IS_SET_ENABLE_AUTO_SENSOR_GAIN, &enable, 0);
    is_SetAutoParameter(m_hCam, IS_SET_ENABLE_AUTO_SENSOR_WHITEBALANCE, &enable, 0);
    is_SetAutoParameter(m_hCam, IS_SET_ENABLE_AUTO_SENSOR_SHUTTER, &disable, 0);
    
    double FPS, NEWFPS;
    FPS = 15;
    is_SetFrameRate(m_hCam, FPS, &NEWFPS);

#if 0
    double parameter = 10;// 50;
    int error = is_Exposure(m_hCam, IS_EXPOSURE_CMD_SET_EXPOSURE, (void*)&parameter, sizeof(parameter));
    if (error != IS_SUCCESS) {
        printf("set exposure failed: %f\n", parameter);
        return;
    }

    error = is_Exposure(m_hCam, IS_EXPOSURE_CMD_GET_EXPOSURE, (void*)&parameter, sizeof(parameter));
#endif

    UINT uiCaps = 0;
    INT nRet = is_Focus(m_hCam, FDT_CMD_GET_CAPABILITIES, &uiCaps, sizeof (uiCaps));
    if (nRet == IS_SUCCESS && (uiCaps & FOC_CAP_AUTOFOCUS_SUPPORTED))
    {
        printf("If supported, enable auto focus\n");
        nRet = is_Focus(m_hCam, FOC_CMD_SET_DISABLE_AUTOFOCUS, NULL, 0);
    }
}

cv::Mat IDS_Cam::Read()
{
    cv::Mat im;
    if (m_hCam){
        if (is_FreezeVideo(m_hCam, IS_WAIT) == IS_SUCCESS){
            void *pMemVoid; //pointer to where the image is stored
            is_GetImageMem(m_hCam, &pMemVoid);
            im = cv::Mat(cv::Size(img_width, img_height), CV_8UC3, pMemVoid).clone();
        }
    }

    return im;
}

bool IDS_Cam::IsOpened()
{
    return m_hCam != 0;
}

void IDS_Cam::PrintSensorInfo()
{
    printf("Sensor Info: \n"
        "\t SensorID: 0x%x\n"
        "\t strSensorName: %s\n"
        "\t nColorMode: %d\n"
        "\t nMaxWidth: %d\n"
        "\t nMaxHeight: %d\n"
        "\t wPixelSize: %d\n"
        , m_sInfo.SensorID
        , m_sInfo.strSensorName
        , m_sInfo.nColorMode
        , m_sInfo.nMaxWidth
        , m_sInfo.nMaxHeight
        , m_sInfo.wPixelSize
        );
}
