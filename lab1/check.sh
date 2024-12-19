for n in 10000000 100000000 1000000000 10000000000
do
   docker run -it --rm cpu-load-app /usr/bin/time ./load_cpu $n 2>> container_cpu_times.txt
done
