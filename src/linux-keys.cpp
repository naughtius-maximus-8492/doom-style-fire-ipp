#include "linux-keys.h"
#include <chrono>

LT::LinuxKeyHandler::LinuxKeyHandler():
    key_down_thread(&LT::LinuxKeyHandler::key_down_func, this)
{
    setRawMode(true);
}

LT::LinuxKeyHandler::~LinuxKeyHandler(){
    this->running = false;
    this->key_down_thread.join();
    setRawMode(false);
}


void LT::LinuxKeyHandler::key_down_func(){
    while(this->running){
        char ch;
        read(STDIN_FILENO, &ch, 1);
        auto time_now = std::chrono::steady_clock::now();
        this->pressed_keys.emplace(ch, time_now);
    }
}

bool LT::LinuxKeyHandler::GetAsyncKeyState(const char& key){
    auto time = this->pressed_keys.find(key);
    if(time != this->pressed_keys.cend()){
        if(time->second - std::chrono::steady_clock::now() > std::chrono::milliseconds(delay)){
            return true;
        } else {
            this->pressed_keys.erase(key);
        }
    }
    return false;
};
