# cola

在线编译环境 [cola on line](https://gitpod.io/new/#github.com/naonao-cola/cola) ,安装xmake进行编译


```bash
sudo add-apt-repository ppa:xmake-io/xmake
sudo apt update
sudo apt install xmake
```

# docker 环境

```bash
#去除缓存
docker builder prune
sudo docker build -t cola-env .
docker run -itd --net=host --name cola-test cola-env
docker exec -it af72aead6bfb  /bin/bash

```

