#ifndef CHATGPT_H
#define CHATGPT_H

#include <string>

std::string callChatGPTAPI(const std::string& api_key, const std::string& user_message);
std::string extractContentFromResponse(const std::string& response);

#endif