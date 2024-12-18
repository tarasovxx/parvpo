for n in 10000 100000 1000000 10000000 100000000
do
    docker run -v $(pwd)/result:/Downolads cpu-load-app ./load_cpu $n >> container_times.txt
done
