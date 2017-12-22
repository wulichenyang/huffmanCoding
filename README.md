# huffmanCoding 测试文件
哈夫曼编码解压缩程序-huffmanCoding

# 使用命令行运行
## 首先编译所有文件 
- 压缩: ./main.exe -c filename

- 解压: ./main.exe -u source target
## 问题
- 该版本读、写文件改为buffer减少了I/O时间，提高速度