FROM alpine:latest

RUN apk add g++ git

RUN git clone

WORKDIR /app
RUN g++ cpu_load.cpp -o cpu_load

CMD ./cpu_load > /host/result.txt

ENTRYPOINT ["top", "-b"]