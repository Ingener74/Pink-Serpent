#include <iostream>
#include <fstream>

#include <cocos2d.h>
#include <json/document.h>
#include <json/filereadstream.h>
#include <json/filestream.h>

#include "JsonFileListener.h"

using namespace std;
using namespace cocos2d;
using namespace rapidjson;

JsonFileListener::JsonFileListener(const std::function<void(int)>& func) {
    m_work = true;
    if (!func)
        return;
    m_thread = thread([=] {
        while(m_work) {

            try {
                if(auto file = fopen("test.json", "rb")) {
                    FileStream frs(file);
                    Document js;
                    js.ParseStream<0>(frs);
                    fclose(file);

                    if(js.HasMember("test")) {
                        func(js["test"].GetInt());
                    }
                }
            } catch (const exception& e) {
                cerr << e.what() << endl;
            }

            {
                unique_lock<mutex> lock(m_mutex);
                m_cond.wait_for(lock, chrono::seconds(1));
            }
        }
    });
}

JsonFileListener::~JsonFileListener() {
    m_work = false;
    m_cond.notify_all();
    if (m_thread.joinable())
        m_thread.join();
}

