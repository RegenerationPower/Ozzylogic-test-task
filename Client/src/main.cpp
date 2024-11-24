#include <iostream>
#include <fstream>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/asio.hpp>

// Function for generating a random string with numbers and Latin letters
std::string generateRandomString(const uint8_t length)
{
    static const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static boost::random::random_device randomDevice;
    static boost::random::mt19937 generator(randomDevice());
    static boost::random::uniform_int_distribution<> distribution(0, characters.size() - 1);

    std::string randomString;

    for (uint8_t i = 0; i < length; ++i)
    {
        randomString += characters[distribution(generator)];
    }
    return randomString;
}

// Function for generating a csv file
void generateCSV(const std::string &filename)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    const uint16_t rows = 1024;
    const uint8_t columns = 6;
    const uint8_t stringLength = 8;

    for (uint16_t i = 0; i < rows; i++)
    {
        for (uint8_t j = 0; j < columns; j++)
        {
            file << generateRandomString(stringLength);
            if (j < columns - 1)
            {
                file << ",";
            }
        }
        file << "\n";
    }

    file.close();
    std::cout << "CSV file generated: " << filename << std::endl;
}

// Function to send a file to the server
void sendFileToServer(const std::string &serverAddress, const uint16_t serverPort, const std::string &filename)
{
    try
    {
        boost::asio::io_service io_service;

        // Create a socket and connect to the server
        boost::asio::ip::tcp::socket socket(io_service);
        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::connect(socket, resolver.resolve({serverAddress, std::to_string(serverPort)}));

        std::cout << "Connected to server " << serverAddress << ":" << serverPort << std::endl;

        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open())
        {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            return;
        }

        // Read the file content
        std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        // Send the file content to the server
        boost::asio::write(socket, boost::asio::buffer(fileContent));

        std::cout << "File sent successfully: " << filename << std::endl;

        // Receive the processed file content from the server
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\0");

        std::istream response_stream(&response);
        std::string processedContent((std::istreambuf_iterator<char>(response_stream)), std::istreambuf_iterator<char>());

        // Save the processed content to a new file
        std::string processedFilename = "processed_" + filename;
        std::ofstream processedFile(processedFilename, std::ios::binary);
        processedFile << processedContent;
        processedFile.close();

        std::cout << "Processed file saved as: " << processedFilename << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " <output_csv_file> <server_address> <server_port>" << std::endl;
        return 1;
    }

    std::string outputFile = argv[1];
    std::string serverAddress = argv[2];
    uint16_t serverPort = static_cast<uint16_t>(std::stoi(argv[3]));

    // Generate the CSV file
    generateCSV(outputFile);

    // Send the file to the server
    sendFileToServer(serverAddress, serverPort, outputFile);

    return 0;
}
