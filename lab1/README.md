# НИЯУ МИФИ. 2024. ПАРВПО
## Лабораторная работа №1.
## Тарасов Артём Б22-525

### Описание алгоритма (действия)
- Настроена рабочая среда для работы с Docker.
- Написана программа на cpp загружающая процессор и [загружена на гх](https://github.com/tarasovxx/parvpo/blob/main/lab1/load_cpu.cpp)
- Написан и протестирован Dockerfile, запускающий изолированно программу выше
- Проведен анализ оверхеда запуска программы выше в контейнере и на хост машине

## Dockerfile
```dockerfile
# Используем образ Alpine Linux с тегом latest
FROM alpine:latest

# Устанавливаем необходимые инструменты, для работы программы компилятор и гит
RUN apk add g++ git

# Устанавливаем рабочую директорию
WORKDIR /app

# Клонируем с гита рабочую программу, которую бдуем выполнять
RUN git clone https://github.com/tarasovxx/parvpo/

# Компилируем программу
RUN g++ parvpo/lab1/load_cpu.cpp -o load_cpu

# Устанавливаем volume для передачи результата на хост
VOLUME ["/Downolads"]

# Запускаем программу и сохраняем результат в файл
CMD ./load_cpu > /Downolads/result.txt
```

### Запуск и блилд контейнера
```dockerfile
docker build -t cpu-load-app .
docker run -v $(pwd)/result:/Downolads cpu-load-app 
```

Запуск программы локально:
```dockerfile
g++ load_cpu.cpp -o load_cpu
./load_cpu
```

### Результаты
- Локально
```text
Execution time: 49.4266 seconds
```

- В контейнере
```text
Execution time: 33.4317 seconds
```

### Заключение
Программа была протестирована на хосте и в контейнере.
Время выполнения программы в контейнере оказалось меньше (33.4317 секунды), чем на хосте (49.4266 секунды).

Это может быть объяснено минималистичной средой контейнера, меньшими накладными расходами и более изолированным окружением, что позволяет контейнеру использовать ресурсы более эффективно.

Docker предоставляет удобный инструмент для изолированного выполнения программ, позволяя минимизировать влияние окружения на производительность.
Использование контейнеров может ускорить выполнение задач за счёт оптимизированного окружения и минимизации фоновых процессов.

### Приложение

[Репозиторий с файлами](https://github.com/tarasovxx/parvpo/blob/main/lab1/load_cpu.cpp)
```c++
#include <iostream>
#include <chrono>
#include <cmath>

bool checkPrime(int value) {
    if (value < 2) return false;
    int limit = (int)sqrt(value);
    for (int divisor = 2; divisor <= limit; divisor++) {
        if (value % divisor == 0) {
            return false;
        }
    }
    return true;
}

int main()
{
    const long long count = 100000000; // 10 mlrd
    long long cnt = 0;
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < count; ++i)
    {
        checkPrime(i);
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::cout << "Execution time: " << elapsed.count() << " seconds" << std::endl;
    return 0;
}
```