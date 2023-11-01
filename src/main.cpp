/*
 * Controle da aplicação: realiza as tarefas de captura da imagem, aplicação do efeito 
 * selecionado e redimensionamento da janela.
 *  
 * Desenvolvido por CTA - Centro Tecnológico de Acessibilidade do IFRS.
 * Autor: Lucas Schw
*/

#include "screencapture.hpp"
#include "imgwindow.hpp"
#include "aboutwindow.hpp"
#include "effect.hpp"
#include "effects/colorblindness_brettel.hpp"
#include "effects/noeffect.hpp"
#include <opencv2/opencv.hpp>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <memory>
#include <mutex>
#include <chrono>

cta::NoEffect NO_EFFECT;
cta::ColorBlindness_Brettel PROTANOPIA_EFFECT(cta::ColorBlindness_Brettel::CBType::PROTANOPIA);
cta::ColorBlindness_Brettel DEUTERANOPIA_EFFECT(cta::ColorBlindness_Brettel::CBType::DEUTERANOPIA);
cta::ColorBlindness_Brettel TRITANOPIA_EFFECT(cta::ColorBlindness_Brettel::CBType::TRITANOPIA);

cta::Effect* effect = &PROTANOPIA_EFFECT;

cv::Mat mat(400,300,CV_8UC4);
cv::Mat matBuffer(400, 300, CV_8UC3);
cv::Mat matToDraw(400,300,CV_8UC3);
std::mutex updateMatToDraw;
std::mutex updateMat;

cta::AboutWindow* aboutWin;

static void menuCb(Fl_Widget* widget, void* data) {
    Fl_Menu_Button* menuBt = dynamic_cast<Fl_Menu_Button*>(widget);
    effect = static_cast<cta::Effect*>(data);
    menuBt->window()->label(menuBt->text(menuBt->value()));
}

static void aboutCb(Fl_Widget* widget, void* data) {
    aboutWin->position((Fl::w() - aboutWin->w()) / 2, (Fl::h() - aboutWin->h()) / 2);
    aboutWin->show();
}

static void saveFileCb(Fl_Widget* widget, void* data) {
    Fl_Native_File_Chooser nfc;
    nfc.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    nfc.title("Informe o nome do arquivo");
    nfc.filter("PNG\t*.png");
    nfc.preset_file("imagem.png");
    if (nfc.show() == 0) {
        // Cria uma thread para aguardar antes de salvar o arquivo, evitando que a janela(salvar arquivo) 
        // e o menu popup apareçam na captura no Windows.
        // TODO: Verificar possibilidade de alterar a forma como a captura da tela é
        // feita no Windows de forma a ignorar janelas que fiquem sobre essa.
        std::thread savingThread([](std::string str){
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            cv::Mat sample(matToDraw.size(), CV_8UC3);
            cv::cvtColor(matToDraw, sample, cv::COLOR_RGB2BGR);
            cv::imwrite(str.c_str(), sample);
        }, nfc.filename());
        savingThread.detach();
    }
    
}

static const Fl_Menu_Item popupItems[] = {
    {"Sem Efeito", 0, menuCb, &NO_EFFECT},
    {"Protanopia", 0, menuCb, &PROTANOPIA_EFFECT},
    {"Deuteranopia", 0, menuCb, &DEUTERANOPIA_EFFECT},
    {"Tritanopia", 0, menuCb, &TRITANOPIA_EFFECT, FL_MENU_DIVIDER},
    {"Salvar imagem...", 0, saveFileCb, nullptr, FL_MENU_DIVIDER},
    {"Sobre", 0, aboutCb, nullptr},
    {nullptr}
};

int main(int argc, char* argv[]) {
    Fl::visual(FL_DOUBLE|FL_INDEX|FL_RGB);
    Fl::lock();

    
    cta::ScreenCapture sc(mat);
    cta::ImageWindow imgWin(10,10,300,400,"Protanopia");
    Fl_Menu_Button* popupMenu = new Fl_Menu_Button(0,0,300,400,nullptr);
    popupMenu->type(Fl_Menu_Button::POPUP3);
    popupMenu->menu(popupItems);
    imgWin.add(popupMenu);
    imgWin.resizable(imgWin);
    //imgWin.clear_border();
    imgWin.end();
    
    aboutWin = new cta::AboutWindow();
    aboutWin->set_modal();
    aboutWin->end();

    auto captureAndDraw = [&imgWin, &sc]() {
        {
            std::lock_guard lg(updateMat);
            auto& buffer = sc.captureWindowBackContentArea(&imgWin);
            cv::cvtColor(buffer, matBuffer, cv::COLOR_RGBA2RGB);
            effect->apply(matBuffer);
        }

        {
            std::scoped_lock lg(updateMatToDraw, updateMat);
            matBuffer.copyTo(matToDraw);
        }
    };

    imgWin.setDrawCallback([](){
        std::lock_guard lock(updateMatToDraw);
        fl_draw_image(matToDraw.data, 0, 0, matToDraw.cols, matToDraw.rows, 3);
    });

    imgWin.setUpdateThreadCallback([&imgWin, &captureAndDraw](){
        captureAndDraw();
        Fl::awake([](void* data){
            cta::ImageWindow* imgWin = static_cast<cta::ImageWindow*>(data);
            imgWin->redraw();
        }, &imgWin);        
    });
    
    imgWin.setResizeCallback([&imgWin, &captureAndDraw](int w, int h) {
        if (w != mat.cols || w != matToDraw.cols || h != mat.rows || h != matToDraw.rows) {
            {
                std::scoped_lock lg(updateMat, updateMatToDraw);
                mat.create(cv::Size(w,h), CV_8UC4);
                matBuffer.create(cv::Size(w,h), CV_8UC3);
                matToDraw.create(cv::Size(w,h), CV_8UC3);
            }

            captureAndDraw();
            imgWin.redraw();
        }
    });
   
    imgWin.show();
    return Fl::run();
}

