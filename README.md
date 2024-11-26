# Ozzylogic-test-task - Embedded Linux C/C++ developer (2.0)
**Тестове завдання:
Виконати на С++/boost
Зробити з використанням бібліотеки boost/STL
Вважати що на комп’ютері де буде перевірятись завдання бібліотека boost наявна, а з’єднання з мережею інтернет - відсутнє.**
1. Зробити програму клієнт, яка :
- Генерує та зберігає на диск csv файл з 1024 записів по 6 стовпців, заповнених рядками випадкових символів (цифри та латинські літери) завдовжки по 8 символів.
- Відправляє цей файл на сервер для подальшої обробки.
- Зберігає отриманий з сервера файл на диск, та виводить статистику отриману від сервера в консоль.
- Імена файлів програма отримає як аргументи командного рядка.
2. Зробити програму сервер TCP, яка: 
- Отримує від клієнта файл і замінює непарні цифри символом #, видаляє рядки, в яких хоча б одне з полів починається з голосної літери,  надсилає  оброблений файл і статистику назад клієнту.
- Статистика являє собою два лічильники: кількості зроблених замін і кількість зроблених видалень.
- Формат обміну даними між клієнтом і сервером - JSON.
- Множинне підключення клієнтів не потрібне, але буде плюсом.

*Усі інші “незрозумілі вам моменти” та “не однозначні формулювання” у ТЗ - ви можете зробити на власний розсуд.*

## Description

The system consists of two main components: a **client** and a **server**. 

## Prerequisites

1. **Boost Libraries**:
   Ensure Boost libraries are installed on the machine. No internet connection is required during execution, but the libraries must already be present.

2. **C++17 or higher**:
   The project uses modern C++ features.

3. **CMake**:
   Version 3.10 or higher is required to build the project.

## Build Instructions

Follow the steps below to build the client and server:

### For the Server
1. Navigate to the server source directory.
2. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```
3. Run CMake to generate build files:
   ```bash
   cmake ..
   ```
4. Build the project:
   ```bash
   make
   ```
5. Run the server:
   ```bash
   ./server
   ```
   The server will start listening on `127.0.0.1` at port `12345`.

### For the Client
1. Navigate to the client source directory.
2. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```
3. Run CMake to generate build files:
   ```bash
   cmake ..
   ```
4. Build the project:
   ```bash
   make
   ```
5. Run the client:
   ```bash
   ./client <csv_filename> <server_ip> <server_port>
   ```

Example:
```bash
./client file1.csv 127.0.0.1 12345
```

---

## How It Works

### Client
1. Generates a CSV file (e.g., `file1.csv`) with the following format:
   - 1024 rows, each row contains 6 columns of random 8-character strings.
   - Example row: `aBc12F45,6xYZ890a,A23mNo67,DrsTUV89,0123wGyA,kSqr5678`.
2. Sends the CSV file to the server in **JSON format**.
3. Receives a processed **JSON data**, convert it to a CSV file and statistical data from the server:
   - The processed file is saved with the same name but prefixed with `processed_` (e.g., `processed_file1.csv`).
   - Example statistics printed to the console:
     ```
     Total number of replacements: 1024
     Total number of deletions: 256
     ```

### Server
1. Starts a server on 127.0.0.1 12345.
2. Accepts a connection from a client.
3. Receives the JSON-converted CSV file.
4. Processes the file:
   - Replaces **odd digits** in the data with `#`.
   - Removes rows where any column starts with a **vowel** (`a, e, i, o, u, A, E, I, O, U`).
5. Sends the processed **JSON data** and statistics back to the client.

---

## **Features**
1. **Support for multiple clients**:
   The server supports multiple clients simultaneously. Example of running multiple clients:
   ```bash
   ./client file1.csv 127.0.0.1 12345 &
   ./client file2.csv 127.0.0.1 12345 &
   ./client file3.csv 127.0.0.1 12345 &
   wait
   ```
2. **Statistics**:
   The server returns two counters to the client:
   - **Total number of replacements**.
   - **Total number of deletions**.

---

## **Example Execution**

### **Running the Server**
1. Start the server:
   ```bash
   ./server
   ```
   Output:
   ```
   Server is listening on port 12345...
   Client connected
   Data from the client read
   Process received json data
   Results sent back to the client
   Client disconnected
   ```

### **Running the Client**
1. Start the client:
   ```bash
   ./client file1.csv 127.0.0.1 12345
   ```
   Output:
   ```
   File generated: file1.csv
   Sending file to the server
   Processed file saved as: processed_file1.csv
   Statistics:
   Total number of replacements: 1024
   Total number of deletions: 256
   ```
