#!/usr/bin/env bash

SIZES=(64 128 256 512 1024)
CPUS=(0.5 1.0 2.0)

RESULTS_FILE="results.csv"

echo "MSize,CPU_limit,Full_time_sec,Compute_time_ms" > $RESULTS_FILE

update_cpu_limit() {
    local cpulimit=$1
    sed -i '' "s/cpus: '.*'/cpus: '$cpulimit'/" docker-compose.yaml
}

for SIZE in "${SIZES[@]}"; do
    for CPU in "${CPUS[@]}"; do
        echo "MSize=$SIZE" > MSize.env

        update_cpu_limit $CPU

        sudo docker compose down --remove-orphans --volumes > /dev/null 2>&1

        LOGFILE="logs/test_M${SIZE}_CPU${CPU}.log"
        mkdir -p logs

        { sudo time docker compose up --build; } &> $LOGFILE

        COMPUTE_TIME_MS=$(grep "Time:" $LOGFILE | awk '{print $2}')

        REAL_TIME_STR=$(grep "^real" $LOGFILE)
        REAL_TIME_STR=$(echo $REAL_TIME_STR | sed 's/real[ \t]*//')
        MINUTES=$(echo $REAL_TIME_STR | sed 's/m.*//')
        SECONDS=$(echo $REAL_TIME_STR | sed 's/.*m//' | sed 's/s//')
        FULL_TIME_SEC=$(echo "$MINUTES * 60 + $SECONDS" | bc)

        echo "$SIZE,$CPU,$FULL_TIME_SEC,$COMPUTE_TIME_MS" >> $RESULTS_FILE

        sudo docker compose down --remove-orphans --volumes > /dev/null 2>&1
    done
done

echo "Тестирование завершено. Результаты в $RESULTS_FILE."
