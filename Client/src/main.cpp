#include <iostream>
#include <fstream>
#include <boost/random.hpp>
#include <boost/random/random_device.hpp>

// A function for generating a random string with numbers and Latin letters
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

// A function for generating a csv file
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

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <output_csv_file>" << std::endl;
        return 1;
    }

    std::string outputFile = argv[1];
    generateCSV(outputFile);

    return 0;
}
