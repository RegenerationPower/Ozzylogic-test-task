#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// A function to check if a word starts with a vowel
bool startsWithVowel(const std::string &word) {
    if (word.empty()) return false;
    char firstChar = tolower(word[0]);
    return (firstChar == 'a' || firstChar == 'e' || firstChar == 'i' || firstChar == 'o' || firstChar == 'u' );
}

// Helper function to split a string by a specified delimiter
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// A function to process file content
std::string processFileContent(const std::string &content) {
    std::string result;
    std::istringstream stream(content);
    std::string line;
    uint16_t replacements = 0; // Count of digit replacements
    uint16_t deletions = 0;    // Count of deleted lines
    std::ostringstream statistics; // To store statistics about the processing

    std::vector<std::string> deletedLines;

// Process each line in the input content
    while (std::getline(stream, line)) {
        bool deleteLine = false;
        std::string originalLine = line;
        std::string processedLine = line;
        bool lineModified = false;

        // Split the line into words using ',' as the delimiter
        std::vector<std::string> words = split(line, ',');
        uint16_t wordCount = 0;

        for (std::string& word : words) {
            // Replace odd digits with '#'
            for (char& ch : word) {
                if (isdigit(ch) && (ch - '0') % 2 != 0) {
                    char originalChar = ch;
                    ch = '#';
                    if (!lineModified) lineModified = true;
                    replacements++;
                    std::cout << originalChar << " -> # (in word: " << word << ")\n";
                }
            }

            // Check if the word starts with a vowel
            if (!deleteLine && startsWithVowel(word)) {
                deleteLine = true;
                deletions++;
                deletedLines.push_back(originalLine);  // Add to deleted lines log
                break;  // No need to process further words in this line
            }

            wordCount++;
        }

        // Add the line to the result if it is not marked for deletion
        if (!deleteLine) {
            result += processedLine + "\n";
        }
    }

    // Generate statistics about replacements and deletions
    statistics << "Replacements: " << replacements << "\n";
    statistics << "Deletions: " << deletions << "\n";

    std::cout << "Replacements: " << replacements << std::endl;
    std::cout << "Deletions: " << deletions << std::endl;

    // If there were deletions, log the deleted lines
    if (deletions > 0) {
        statistics << "Deleted lines due to vowel start:\n";
        std::cout << "Deleted lines due to vowel start:\n";
        for (const auto& line : deletedLines) {
            statistics << line << "\n";
            std::cout << line << std::endl;
        }
    }

    return result + statistics.str();
}

int main() {
    try {
        boost::asio::io_service io_service;

        // Create a socket and start listening on port 12345
        boost::asio::ip::tcp::acceptor acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 12345));
        std::cout << "Server is listening on port 12345...\n";

        // Accept a client connection
        boost::asio::ip::tcp::socket socket(io_service);
        acceptor.accept(socket);
        std::cout << "Client connected.\n";

        // Receive file content from the client
        boost::asio::streambuf request;
        boost::asio::read_until(socket, request, "\0");  // Read until the null terminator
        std::istream request_stream(&request);
        std::string file_content((std::istreambuf_iterator<char>(request_stream)), std::istreambuf_iterator<char>());

        // Process the received file content
        std::string processed_content = processFileContent(file_content);

        // Send the processed content back to the client
        boost::asio::write(socket, boost::asio::buffer(processed_content));

        std::cout << "Processed file sent back to client.\n";
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}
