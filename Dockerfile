FROM ubuntu:24.04

RUN apt update && apt install -y gcc-13 g++-13 && apt clean
RUN apt update && apt install -y gdb && apt clean && apt install -y gdbserver
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100 \
 && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100

WORKDIR /project
CMD ["/bin/bash"]
