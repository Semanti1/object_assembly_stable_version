#include <iostream>
#include <cstdlib>
#include "chatgpt.h"

int main() {
    const char* api_key_env = std::getenv("CHATGPT_API_KEY");
    if (api_key_env == nullptr) {
        std::cerr << "Error: CHATGPT_API_KEY environment variable is not set." << std::endl;
        return 1;
    }
    std::string api_key = api_key_env;

    std::string user_message = "Hello, how are you?";
    std::cout << "Calling ChatGPT API..." << std::endl;

    try {
        std::string full_response = callChatGPTAPI(api_key, user_message);
        std::cout << "API call completed." << std::endl;
        
        std::string content = extractContentFromResponse(full_response);
        std::cout << "Response content from ChatGPT: " << content << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
    }

    std::cout << "Program finished." << std::endl;
    return 0;
}