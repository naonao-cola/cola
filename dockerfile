FROM ubuntu:23.04

# https://github.com/boostorg/mysql/blob/6d08247bab36da0cb9360f7c932620e167ec390b/tools/docker/build-gcc13.dockerfile#L13
ENV NAO_PATH /home/COLA
WORKDIR $NAO_PATH

RUN sed -i 's/deb.debian.org/mirrors.ustc.edu.cn/g' /etc/apt/sources.list
RUN sed -i 's/security.debian.org/mirrors.ustc.edu.cn/g' /etc/apt/sources.list
RUN sed -i 's/archive.ubuntu.com/mirrors.ustc.edu.cn/g' /etc/apt/sources.list

RUN \
    apt-get update && \
    apt-get --no-install-recommends -y install \
    software-properties-common \
    cmake \
    git \
    gpg \
    gpg-agent \
    valgrind \
    libssl-dev \
    ca-certificates \
    gcc-13 \
    7zip \
    unzip \
    g++-13 && \
    ln -s /usr/bin/g++-13 /usr/bin/g++ && \
    ln -s /usr/bin/gcc-13 /usr/bin/gcc && \
    g++ --version

RUN add-apt-repository ppa:xmake-io/xmake && apt-get update && apt-get install -y xmake && \
    git clone https://github.com/naonao-cola/cola.git && \
    cd cola

CMD /bin/sh -c "/bin/bash"