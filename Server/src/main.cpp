#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <vector>

namespace json = boost::json;

// Function to check if a word starts with a vowel
bool startsWithVowel(const std::string &word)
{
    if (word.empty())
    {
        return false;
    }

    char firstChar = tolower(word[0]);
    return (firstChar == 'a' || firstChar == 'e' || firstChar == 'i' || firstChar == 'o' || firstChar == 'u');
}

// Function to process a jsonArray
std::pair<json::array, std::string> processJson(const json::array &jsonArray)
{
    json::array processedArray;
    std::ostringstream statistics;            // To store statistics about the processing
    uint16_t replacements = 0, deletions = 0; // Count of digit replacements and deletions

    // Process each line in the jsonArray
    for (const auto &row : jsonArray)
    {
        bool deleteLine = false;
        json::array processedRow;

        for (const auto &cell : row.as_array())
        {
            std::string word = cell.as_string().c_str();

            // Replace odd digits with '#'
            for (char &ch : word)
            {
                if (isdigit(ch) && (ch - '0') % 2 != 0)
                {
                    ch = '#';
                    replacements++;
                }
            }

            // Check if the word starts with a vowel and delete it
            if (!deleteLine && startsWithVowel(word))
            {
                deleteLine = true;
                deletions++;
                break;
            }

            processedRow.push_back(boost::json::value(word));
        }

        // Add the line to the result if it is not marked for deletion
        if (!deleteLine)
        {
            processedArray.push_back(processedRow);
        }
    }

    statistics << "Total number of replacements: " << replacements << "\n";
    statistics << "Total number of deletions: " << deletions << "\n";

    return {processedArray, statistics.str()};
}

// Function to handle a client connection
void handleClient(boost::asio::ip::tcp::socket socket)
{
    try
    {
        boost::asio::streambuf buffer;
        std::string totalData;

        // Read data from the client until the <END> marker is found
        while (boost::asio::read(socket, buffer, boost::asio::transfer_at_least(1)))
        {
            std::istream stream(&buffer);
            std::string data((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
            totalData += data;

            if (totalData.find("<END>") != std::string::npos)
            {
                totalData.erase(totalData.find("<END>"));
                break;
            }
        }

        std::cout << "Data from the client read\n";

        // Parse the received JSON data
        json::array jsonArray = json::parse(totalData).as_array();
        auto [processedArray, statistics] = processJson(jsonArray);
        std::cout << "Process received json data\n";

        // Send the results back to the client
        std::string result = json::serialize(processedArray) + "\n\n" + statistics;
        boost::asio::write(socket, boost::asio::buffer(result + "<END>"));

        std::cout << "Results send back to the client\nClient disconnected\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main()
{
    try
    {
        boost::asio::io_service io_service;

        // Create a socket and start listening on port 12345
        boost::asio::ip::tcp::acceptor acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 12345));
        std::cout << "Server is listening on port 12345...\n";

        // Create threads for handling multiple clients
        std::vector<std::thread> threadPool;
        const uint8_t maxThreads = 4;

        for (uint8_t i = 0; i < maxThreads; ++i)
        {
            threadPool.emplace_back([&]()
            {
                while (true) 
                {
                    // Create a socket for the client
                    boost::asio::ip::tcp::socket socket(io_service);

                    // Accept a client connection
                    acceptor.accept(socket);
                    std::cout << "Client connected\n";

                    // Handle the client in a separate thread
                    handleClient(std::move(socket));
                } 
            });
        }

        // Join all threads
        for (auto &thread : threadPool)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
    }

    return 0;
}
