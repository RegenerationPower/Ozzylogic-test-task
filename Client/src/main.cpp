#include <iostream>
#include <fstream>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>

namespace json = boost::json;

// Function to convert CSV content to a JSON array
json::array csvToJson(const std::string &csvContent)
{
    json::array jsonArray;
    std::istringstream csvStream(csvContent);
    std::string line;

    while (std::getline(csvStream, line))
    {
        json::array jsonRow;
        std::istringstream lineStream(line);
        std::string cell;

        // Split each line into cells using a comma as the delimiter
        while (std::getline(lineStream, cell, ','))
        {
            jsonRow.push_back(boost::json::value(cell));
        }

        jsonArray.push_back(jsonRow);
    }

    return jsonArray;
}

// Function to convert a JSON array to CSV content
std::string jsonToCsv(const json::array &jsonArray)
{
    std::ostringstream csvContent;

    for (const auto &row : jsonArray)
    {
        const auto &jsonRow = row.as_array();
        for (uint16_t i = 0; i < jsonRow.size(); ++i)
        {
            std::string cellValue = std::string(jsonRow[i].as_string());

            // Remove surrounding double quotes
            if (cellValue.front() == '"' && cellValue.back() == '"')
            {
                cellValue = cellValue.substr(1, cellValue.size() - 2); // Delete the first and the last quote
            }

            csvContent << cellValue;

            if (i < jsonRow.size() - 1)
            {
                csvContent << ",";
            }
        }
        csvContent << "\n";
    }

    return csvContent.str();
}

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

    // Fill csv file with random data
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

// Function to send json to a server and receive file and statistics from a server
void sendFileToServer(const std::string &serverAddress, const uint16_t serverPort, const std::string &filename)
{
    try
    {
        boost::asio::io_service io_service;

        // Connect to the server
        boost::asio::ip::tcp::socket socket(io_service);
        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::connect(socket, resolver.resolve({serverAddress, std::to_string(serverPort)}));

        // Read the CSV file content
        std::ifstream file(filename);
        if (!file.is_open())
        {
            std::cerr << "Error: Unable to open file " << filename << std::endl;
            return;
        }

        std::string csvContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        // Convert CSV to JSON format
        json::array jsonArray = csvToJson(csvContent);
        std::string jsonContent = json::serialize(jsonArray);

        // Send JSON content to the server
        boost::asio::write(socket, boost::asio::buffer(jsonContent + "<END>"));

        // Receive the processed JSON from the server
        boost::asio::streambuf response;
        std::string totalData;
        boost::system::error_code error;

        while (true)
        {
            boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error);
            std::istream stream(&response);
            std::string data((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
            totalData += data;

            if (totalData.find("<END>") != std::string::npos)
            {
                totalData.erase(totalData.find("<END>"));
                break;
            }
        }

        // Separate the processed JSON and statistics
        auto delimiterPos = totalData.find("\n\n");
        std::string processedJson = totalData.substr(0, delimiterPos);
        std::string statistics = totalData.substr(delimiterPos + 2);

        // Convert JSON to CSV format
        json::array processedArray = json::parse(processedJson).as_array();
        std::string processedCsv = jsonToCsv(processedArray);

        // Save the processed CSV file
        std::string processedFilename = "processed_" + filename;
        std::ofstream processedFile(processedFilename);
        processedFile << processedCsv;
        processedFile.close();

        std::cout << "Processed file saved as: " << processedFilename << std::endl;

        std::cout << "Statistics:\n"
                  << statistics << std::endl;
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
