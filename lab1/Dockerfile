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
