# README
RSA device by : Hero Nguyen

#### Hướng dẫn sử dụng:
0. **Biên dịch**
```
$ make

```

1. **Cài đặt, xóa bỏ module**
```
$ sudo insmod encoder.ko
$ sudo rmmod encoder
$ sudo insmod decoder.ko
$ sudo rmmod decoder
```

2. **Tạo file thiết bị**
```
$ sudo mknod /dev/encoder c 50 0
$ sudo chmod 666 /dev/encoder
$ sudo mknod /dev/decoder c 60 0
$ sudo chmod 666 /dev/decoder
```

3. **Xem log**
```
$ dmesg
```
4. **Xem danh sách các module đã cài**
```
$ lsmod | head -10

```
5. **Xem major number của các device**
```
$ cat /proc/devices
```