/*
 * [MacOS]: Captura a área atrás da janela do usuário, delimitado pelas coordenadas
 * da área onde o conteúdo será exibido (área do usuário).
 *  
 * Desenvolvido por CTA - Centro Tecnológico de Acessibilidade do IFRS.
 * Autor: Lucas Schw
*/

#if defined(__MACH__) && defined(__APPLE__)

#include "../screencapture.hpp"
extern "C" {
    #include <unistd.h>
    #include <stdio.h>
    #include <ApplicationServices/ApplicationServices.h>
}
#include <FL/platform.H>
#import <Cocoa/Cocoa.h>

namespace cta {

    struct ScreenCapture::OSImpl {
        cv::Mat* outputMatrix_;
        CGColorSpaceRef colorSpace_;
        CGContextRef contextRef_;
        CGImageRef imageRef_ = nullptr;
        int w_, h_;
        CGRect outputRect_;

        void resetContext() {
            CGContextRelease(contextRef_);
            contextRef_ = CGBitmapContextCreate( outputMatrix_->data, outputMatrix_->cols, outputMatrix_->rows, 8, outputMatrix_->step[0],
                                                    colorSpace_, kCGImageAlphaPremultipliedLast|kCGBitmapByteOrderDefault);
            w_ = outputMatrix_->cols;
            h_ = outputMatrix_->rows;
            outputRect_.origin.x = 0;
            outputRect_.origin.y = 0;
            outputRect_.size.width = w_;
            outputRect_.size.height = h_;
        }

        cta::Rect contentRect(const Fl_Window* currWin) {
            NSWindow* w = static_cast<NSWindow*>(fl_xid(currWin));
            size_t screenHeight = CGDisplayPixelsHigh(CGMainDisplayID());
            NSRect r = [w contentRectForFrameRect: [w frame]];
            r.origin.y =  screenHeight - (r.origin.y + r.size.height);
            return {r.origin.x, r.origin.y, r.size.width, r.size.height};
        }

        CGWindowID windowID(const Fl_Window* currWin) const{
            return static_cast<CGWindowID>(static_cast<NSWindow*>(fl_xid(currWin)).windowNumber);
        }
    };

    ScreenCapture::ScreenCapture(cv::Mat& buffer) : osImpl_(new OSImpl()) {
        osImpl_->outputMatrix_ = &buffer;
        osImpl_->colorSpace_ = CGColorSpaceCreateDeviceRGB();
        osImpl_->contextRef_ = CGBitmapContextCreate( osImpl_->outputMatrix_->data, 
                                                    osImpl_->outputMatrix_->cols, 
                                                    osImpl_->outputMatrix_->rows, 
                                                    8, 
                                                    osImpl_->outputMatrix_->step[0],
                                                    osImpl_->colorSpace_, 
                                kCGImageAlphaPremultipliedLast|kCGBitmapByteOrderDefault);
        osImpl_->imageRef_ = nullptr;
        osImpl_->w_ = osImpl_->outputMatrix_->cols;
        osImpl_->h_ = osImpl_->outputMatrix_->rows;
        osImpl_->outputRect_.origin.x = 0;
        osImpl_->outputRect_.origin.y = 0;
        osImpl_->outputRect_.size.width = osImpl_->w_;
        osImpl_->outputRect_.size.height = osImpl_->h_;
    }

    cv::Mat& ScreenCapture::captureWindowBackContentArea(const Fl_Window* currWindow) {
        if (osImpl_->imageRef_ != nullptr) CGImageRelease(osImpl_->imageRef_);
        if (osImpl_->outputMatrix_->cols != osImpl_->w_ || osImpl_->outputMatrix_->rows != osImpl_->h_) {
            osImpl_->resetContext();
        }
        Rect area = osImpl_->contentRect(currWindow);
        NSRect r {{area.x, area.y}, {area.w, area.h}};
        osImpl_->imageRef_ = CGWindowListCreateImage(r, kCGWindowListOptionOnScreenBelowWindow,
            osImpl_->windowID(currWindow), kCGWindowImageBoundsIgnoreFraming);
        CGContextDrawImage(osImpl_->contextRef_,
                        osImpl_->outputRect_,
                        osImpl_->imageRef_);
        
        return *(osImpl_->outputMatrix_);
    }

    ScreenCapture::~ScreenCapture() {
        if (osImpl_->imageRef_ != nullptr) CGImageRelease(osImpl_->imageRef_);
        CGContextRelease(osImpl_->contextRef_);
        CGColorSpaceRelease(osImpl_->colorSpace_);
    }
}


 #endif // End MacOS Impl

