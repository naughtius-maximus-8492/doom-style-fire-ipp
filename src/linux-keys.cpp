#include "linux-keys.h"
#include "linux-virtual-keys.h"
#include <chrono>
#include <iostream>
#include <unistd.h>


LT::LinuxKeyHandler::LinuxKeyHandler():
    key_down_thread(&LT::LinuxKeyHandler::key_down_func, this)
{
    setRawMode(true);
}

LT::LinuxKeyHandler::~LinuxKeyHandler(){
    std::cout << "Press Any Key to leave!" << std::endl;
    this->running = false;
    this->key_down_thread.join();
    std::cout << "Thanks come again!" << std::endl;
    setRawMode(false);
}


void LT::LinuxKeyHandler::key_down_func(){
#ifdef __linux__
    while(this->running){
        char ch[7];
        ssize_t size;
        size = read(STDIN_FILENO, &ch, 6);
        auto time_now = std::chrono::steady_clock::now();
        this->pressed_keys.emplace(decode_key(ch, size), time_now);
    }
#endif
}

bool LT::LinuxKeyHandler::GetAsyncKeyState(const Key& key){
    auto time = this->pressed_keys.find(key);
    if(time != this->pressed_keys.cend()){
        if(std::chrono::steady_clock::now() - time->second < std::chrono::milliseconds(delay)){
            return true;
        } else {
            this->pressed_keys.erase(key);
        }
    }
    return false;
};
