#include <iostream>
#include <vector>
#include <cmath>
#include <list>
#include <chrono>
#include <cstdlib>   // для getenv
#include <sstream>
#include <cstring>   // для strcpy
#include "crow_all.h"
#include "json.hpp"

// Глобальные хранилища матриц
static std::vector<int> matrix1_demo;
static std::vector<int> matrix2_demo;
static int got_data = 0;

// Служебные функции
template<typename T>
std::list<T> vectorToList(const std::vector<T>& vec) {
    return std::list<T>(vec.begin(), vec.end());
}

void adjustMatrixSizes(std::vector<int>& matrix1, std::vector<int>& matrix2) {
    // Вычисляем размеры текущих матриц
    int size1 = (int)matrix1.size();
    int size2 = (int)matrix2.size();

    // Вычисляем ближайший допустимый размер (квадрат)
    int new_size1 = std::ceil(std::sqrt(size1));
    int new_size2 = std::ceil(std::sqrt(size2));

    // Приводим обе матрицы к одному размеру (максимальному из двух)
    int final_size = std::max(new_size1, new_size2);
    int final_total_size = final_size * final_size;

    // Заполняем недостающие элементы нулями
    matrix1.resize(final_total_size, 0);
    matrix2.resize(final_total_size, 0);

    std::cout << "Adjusted matrix sizes to " << final_size << "x" << final_size << " ("
              << final_total_size << " elements)." << std::endl;
}


std::list<int> pieceMatrixMultiply(const std::list<int>& matrix1,
                                   const std::list<int>& matrix2,
                                   int size,
                                   int pnum,
                                   int pid)
{
    // Каждому consumer-у достаются строки [pid*(size/pnum) ... (pid+1)*(size/pnum)-1]
    int rows_per_consumer = size / pnum;
    int start_row = pid * rows_per_consumer;
    int end_row   = start_row + rows_per_consumer;  // не включительно

    std::list<int> result(rows_per_consumer * size, 0);

    auto it = result.begin();

    for (int i = start_row; i < end_row; ++i) {
        for (int j = 0; j < size; ++j) {
            auto it1 = matrix1.begin();
            std::advance(it1, i * size);

            auto it2 = matrix2.begin();
            std::advance(it2, j);

            for (int k = 0; k < size; ++k) {
                *it += (*it1) * (*it2);
                std::advance(it1, 1);
                std::advance(it2, size);
            }
            ++it;
        }
    }

    return result;
}

bool isPerfectSquare(int x) {
    int r = (int)std::sqrt(x);
    return (r*r == x);
}

// Превращаем list<int> в строку, чтобы отправить её в aggregator
std::string listToString(const std::list<int>& data) {
    std::ostringstream oss;
    for (auto it = data.begin(); it != data.end(); ++it) {
        if (it != data.begin()) oss << ",";
        oss << *it;
    }
    return oss.str();
}

// =============== MAIN ===============
int main() {
    // Считываем PID и PNUM из переменных окружения
    const char* pid_str = std::getenv("PID");
    const char* pnum_str = std::getenv("PNUM");
    int pid   = (pid_str  ? std::atoi(pid_str)  : 0);
    int pnum  = (pnum_str ? std::atoi(pnum_str) : 1);

    crow::SimpleApp app;

    // 1) /  POST: приходят данные (части матрицы)
    CROW_ROUTE(app, "/").methods("POST"_method)(
    [&](const crow::request& req, crow::response& res){
        auto body_json = nlohmann::json::parse(req.body);
        int msg_type    = body_json["message_type"];
        int msg_content = body_json["message_content"];

        if (msg_type == 1) {
            if (msg_content == -1) {
                got_data += 1;
            } else {
                matrix1_demo.push_back(msg_content);
            }
        } else if (msg_type == 2) {
            if (msg_content == -1) {
                got_data += 1;
            } else {
                matrix2_demo.push_back(msg_content);
            }
        }

        res.code = 200;
        res.end();
    });

    // 2) /end POST: когда получим все данные, делаем pieceMatrixMultiply
    CROW_ROUTE(app, "/end").methods("POST"_method)(
    [&](const crow::request& req, crow::response& res){
        got_data += 1;
        if (got_data == 2) {
            adjustMatrixSizes(matrix1_demo, matrix2_demo);

            // Все данные от producers пришли
            // Проверяем, что размеры корректны
            if (isPerfectSquare((int)matrix1_demo.size()) &&
                isPerfectSquare((int)matrix2_demo.size()) &&
                matrix1_demo.size() == matrix2_demo.size())
            {
                int size = (int)std::sqrt(matrix1_demo.size());

                auto t1 = std::chrono::high_resolution_clock::now();
                // Вычисляем часть матрицы
                auto partResult = pieceMatrixMultiply(
                    vectorToList(matrix1_demo),
                    vectorToList(matrix2_demo),
                    size,
                    pnum,  // кол-во consumer-ов
                    pid    // id текущего consumer-а
                );
                auto t2 = std::chrono::high_resolution_clock::now();
                double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

                std::cout << "[Consumer " << pid << "] Computed piece: "
                          << partResult.size() << " elements in "
                          << ms << " ms\n";

                // Отправляем partial result в aggregator
                std::string final_str = listToString(partResult);

                // используем простейшую C++ http либу / libcurl / crow не очень удобно для POST
                // Для наглядности "в лоб" через system("curl ..."), но лучше libcurl
                std::ostringstream cmd;
                cmd << "curl -s -X POST http://aggregator:8080/partial "
                    << "-H \"pid: " << pid << "\" "
                    << "-H \"pnum: " << pnum << "\" "
                    << "-H \"time-spent: " << ms << "\" "
                    << "-d \"" << final_str << "\"";

                // Выполняем system-call (для примера)
//                system(cmd.str().c_str());

                // Завершаемся
//                exit(0);
//            } else {
//                std::cerr << "[Consumer " << pid << "] Wrong matrix sizes!\n";
//                exit(1);
                res.code = 200;
                res.write("Computation started\n");
                res.end();
            }

        // НЕ вызываем exit(0) тут — пускай Consumer живёт, пока докер не будет остановлен
        // Или пока не придёт ещё один запрос, сообщающий "всё точно закончено".
        } else {
            res.code = 200;
            res.write("Waiting for more data\n");
            res.end();
        }
    });

    // Дополнительно /partial_result GET: если хотим посмотреть, что накопилось
    CROW_ROUTE(app, "/partial_result").methods("GET"_method)(
    [&](const crow::request& req, crow::response& res){
        nlohmann::json j;
        j["matrix1_size"] = matrix1_demo.size();
        j["matrix2_size"] = matrix2_demo.size();
        j["got_data"] = got_data;
        res.set_header("Content-Type", "application/json");
        res.write(j.dump());
        res.end();
    });

    std::cout << "[Consumer " << pid << "] Starting on port 8080..." << std::endl;
    app.port(8080).multithreaded().run();
}
