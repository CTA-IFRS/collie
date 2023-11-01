/*
 * [Windows]: Captura a área atrás da janela do usuário, delimitado pelas coordenadas
 * da área onde o conteúdo será exibido (área do usuário).
 *  
 * Desenvolvido por CTA - Centro Tecnológico de Acessibilidade do IFRS.
 * Autor: Lucas Schw
*/

#if defined(_WIN32)  
#ifndef WIN32  
    #define WIN32  // Usado pelo FLTK
#endif 
#include "../screencapture.hpp"  
#include <opencv2/opencv.hpp>
#include <FL/platform.H>
#include <windows.h>

namespace cta {
    
    struct ScreenCapture::OSImpl {
        cv::Mat* mat_;
        BITMAPINFOHEADER bmpHeader_;
        HDC winDC_;
        HDC bmpDC_;
        HBITMAP bmpHandle_;
        int w_;
        int h_;

        void resetCompatibleBitmap() {
            DeleteObject(bmpHandle_);
            w_ = mat_->cols;
            h_ = mat_->rows;
            bmpHeader_.biWidth = w_;
            bmpHeader_.biHeight = -h_;
            bmpHandle_ = CreateCompatibleBitmap(winDC_, w_, h_);
            SelectObject(bmpDC_, bmpHandle_);
        }
    };

    ScreenCapture::ScreenCapture(cv::Mat& buffer) : osImpl_(new OSImpl()) {
        osImpl_->mat_ = &buffer;

        osImpl_->bmpHeader_.biSize = sizeof(BITMAPINFOHEADER);
        osImpl_->bmpHeader_.biWidth = buffer.cols;
        osImpl_->bmpHeader_.biHeight = -buffer.rows;
        osImpl_->bmpHeader_.biPlanes = 1;
        osImpl_->bmpHeader_.biBitCount = 32;
        osImpl_->bmpHeader_.biCompression = BI_RGB;
        osImpl_->bmpHeader_.biSizeImage = 0;
        osImpl_->bmpHeader_.biXPelsPerMeter = 0;
        osImpl_->bmpHeader_.biYPelsPerMeter = 0;
        osImpl_->bmpHeader_.biClrUsed = 0;
        osImpl_->bmpHeader_.biClrImportant = 0;

        osImpl_->w_ = buffer.cols;
        osImpl_->h_ = buffer.rows;
        osImpl_->winDC_ = GetDC(nullptr);
        osImpl_->bmpDC_ = CreateCompatibleDC(osImpl_->winDC_);
        osImpl_->bmpHandle_ = CreateCompatibleBitmap(osImpl_->winDC_, buffer.cols, buffer.rows);
        SelectObject(osImpl_->bmpDC_, osImpl_->bmpHandle_);
    }

    ScreenCapture::~ScreenCapture() {
        DeleteObject(osImpl_->bmpHandle_);
        DeleteDC(osImpl_->bmpDC_);
        ReleaseDC(nullptr, osImpl_->winDC_);
    }

    cv::Mat& ScreenCapture::captureWindowBackContentArea(const Fl_Window* currWindow) {
        HWND winHandle = static_cast<HWND>(fl_xid(currWindow)); 
        RECT r;
        GetClientRect(winHandle, &r);
        POINT screenCoord {r.left, r.top};
        ClientToScreen(winHandle, &screenCoord);
        // Com OS inferior ao Windows 10 2004, o WDA_EXCLUDEFROMCAPTURE torna-se WDA_MONITOR
        SetWindowDisplayAffinity(winHandle, WDA_EXCLUDEFROMCAPTURE);
        // Resize
        if (osImpl_->w_ != osImpl_->mat_->cols || osImpl_->h_ != osImpl_->mat_->rows) {
            osImpl_->resetCompatibleBitmap();
        }
        int sx = screenCoord.x;
        int sy = screenCoord.y;
        int w = currWindow->w(); //GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int h = currWindow->h(); //GetSystemMetrics(SM_CYVIRTUALSCREEN);
        BitBlt(osImpl_->bmpDC_,0,0,w,h, osImpl_->winDC_,sx,sy,SRCCOPY);
        GetDIBits(osImpl_->bmpDC_, osImpl_->bmpHandle_, 0, h, osImpl_->mat_->data, 
            (BITMAPINFO*)&(osImpl_->bmpHeader_), DIB_RGB_COLORS);
        cv::cvtColor(*(osImpl_->mat_), *(osImpl_->mat_), cv::COLOR_BGR2RGBA);
        
        return *(osImpl_->mat_);
    }
}

#endif