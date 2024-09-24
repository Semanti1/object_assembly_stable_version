#include "chatgpt.h"
#include <iostream>
#include <curl/curl.h>
#include <json/json.h>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string extractContentFromResponse(const std::string& response) {
    Json::Value root;
    Json::Reader reader;
    
    if (!reader.parse(response, root)) {
        throw std::runtime_error("Failed to parse JSON response");
    }
    
    if (root["choices"].isArray() && root["choices"].size() > 0) {
        const Json::Value& firstChoice = root["choices"][0];
        if (firstChoice.isMember("message") && firstChoice["message"].isMember("content")) {
            return firstChoice["message"]["content"].asString();
        }
    }
    
    throw std::runtime_error("Unable to find content in the response");
}

std::string callChatGPTAPI(const std::string& api_key, const std::string& user_message) {
    std::string api_url = "https://api.openai.com/v1/chat/completions";
    Json::Value request_data;
    request_data["model"] = "gpt-3.5-turbo"; // or any other GPT model, we can upgrade
    request_data["messages"] = Json::arrayValue;
    
    Json::Value message;
    message["role"] = "user";
    message["content"] = user_message;

    request_data["messages"].append(message);
    
    CURL* curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    std::string response_string;

    if(curl) {
        Json::StreamWriterBuilder writer;
        std::string request_body = Json::writeString(writer, request_data);
        curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        std::string auth_header = "Authorization: Bearer " + api_key;
        headers = curl_slist_append(headers, auth_header.c_str());

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return response_string;
}
