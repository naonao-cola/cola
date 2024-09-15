FROM ubuntu:23.04

# https://github.com/boostorg/mysql/blob/6d08247bab36da0cb9360f7c932620e167ec390b/tools/docker/build-gcc13.dockerfile#L13
ENV NAO_PATH /home/COLA
ENV DOCKER_BUILDKIT=1
ENV XMAKE_ROOT=y

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
    iputils-ping \
    gpg \
    curl \
    wget \
    make \
    gpg-agent \
    ninja-build \
    unrar \
    xz-utils \
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

RUN curl -fsSL https://xmake.io/shget.text | bash
ENV XMAKE_PROFILE_PATH /root/.xmake/profile
RUN echo 'source /root/.xmake/profile' >> ~/.bashrc

RUN git clone https://github.com/naonao-cola/cola.git && cd cola && /bin/bash -c "source $XMAKE_PROFILE_PATH && xmake build -rvD -y && xmake run -a"

CMD /bin/sh -c "/bin/bash"
