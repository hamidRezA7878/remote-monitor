
FROM ubuntu:22.04

WORKDIR /app

RUN apt update && apt install -y cmake g++ openssh-client sshpass curl

COPY . .

RUN mkdir build && cd build && cmake .. && make

CMD ["./build/remote_monitor", "root@ssh_server", "root", "80", "70"]
