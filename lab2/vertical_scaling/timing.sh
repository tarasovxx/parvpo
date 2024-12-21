#!/usr/bin/env bash

CPUS=(0.5 1 1.5 2)

MSIZES=(64 128 256 512)

RESULTS_FILE="benchmark_results.csv"

echo "CPU,MSize,Time_ms" > $RESULTS_FILE

for CPU in "${CPUS[@]}"; do
    echo "Устанавливаем CPU: $CPU"

    sed -i '' 's/cpus: ".*"/cpus: "'$CPU'"/' docker-compose.yaml

    echo "Пересборка контейнеров для CPU=$CPU..."
    sudo docker compose build --no-cache

    for SIZE in "${MSIZES[@]}"; do
        echo "Запуск теста для CPU=$CPU и MSize=$SIZE"

        echo "MSize=$SIZE" > MSize.env

        sudo docker compose down --remove-orphans --volumes > /dev/null 2>&1

        OUTPUT=$(sudo docker compose up --build 2>&1)

        TIME_MS=$(echo "$OUTPUT" | grep "Time:" | grep -Eo '[0-9]+\.[0-9]+')

        if [ -z "$TIME_MS" ]; then
            TIME_MS="N/A"
            echo "Не удалось извлечь время для CPU=$CPU и MSize=$SIZE!"
        else
            echo "Получено время: $TIME_MS ms для CPU=$CPU и MSize=$SIZE"
        fi

        echo "$CPU,$SIZE,$TIME_MS" >> $RESULTS_FILE

        sudo docker compose down --remove-orphans --volumes > /dev/null 2>&1
    done
done

echo "Тестирование завершено. Результаты в $RESULTS_FILE."
