#pragma once
#include "uEye.h"
#include "opencv2/opencv.hpp"
class IDS_Cam
{
public:
    IDS_Cam();
    ~IDS_Cam();

    int InitCamera(HIDS *hCam, HWND hWnd);

    bool OpenCamera();

    void CloseCamera();

    void GetMaxImageSize(INT *pnSizeX, INT *pnSizeY);

    void SetAutoParam();

    cv::Mat Read();

    bool IsOpened();

    void PrintSensorInfo();

	void SetBinning(int binning_num);

private:
    HIDS    m_hCam;
    HWND	m_hWndDisplay;	// handle to diplay window
    INT		m_Ret;			// return value of uEye SDK functions
    INT		m_nColorMode;	// Y8/RGB16/RGB24/REG32
    INT		m_nBitsPerPixel;// number of bits needed store one pixel
    int		img_width;		// width of video 
    int		img_height;		// height of video
    INT		m_lMemoryId;	// grabber memory - buffer ID
    char*	m_ImageMemory;  // grabber memory - pointer to buffer
    INT     m_nRenderMode;  // render  mode
    SENSORINFO m_sInfo;	    // sensor information struct
	int		m_binning_x;
	int		m_binning_y;
};

