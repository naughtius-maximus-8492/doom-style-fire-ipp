#include "linux-keys.h"

LT::LinuxKeyHandler::LinuxKeyHandler():
key_down_thread(this->key_down_func()){
    setRawMode(true);
}

LT::LinuxKeyHandler::~LinuxKeyHandler(){
    this->running = false;
    this->key_down_thread.join();
    this->key_up_thread.join();
    setRawMode(false);
}

void LT::LinuxKeyHandler::key_up_func(){
    while(this->running){
        auto current_time = std::chrono::steady_clock::now();
        std::vector<char> removed_values;

        for(const auto& pair: this->pressed_keys){
            if(pair.second - current_time > std::chrono::milliseconds(100)){
                removed_values.push_back(pair.first);
            }
        }

        for(char remove_char: removed_values){
            this->pressed_keys.erase(remove_char);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
};

void LT::LinuxKeyHandler::key_down_func(){
    while(this->running){
        char ch;
        read(STDIN_FILENO, &ch, 1);
        auto time_now = std::chrono::steady_clock::now();
        this->pressed_keys.emplace(ch, time_now);
    }
}

