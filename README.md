ServerLibevent
==============
1.三个项目的输出文件都在解决方案（*.sln）同级目录的debug/release下。
2.运行server之前需要配置Config.ini文件
  ListenNum : 服务器监听的数量
  ListenPort :服务器监听的端口
3.运行client 对服务器进行连接，请求数据以及断开连接等。
  client开启了200个线程访问server并请求数据。每个线程都会循环从服务器请求数据。
