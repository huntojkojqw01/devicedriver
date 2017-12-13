# README
RSA device by : Hero Nguyen

#### Hướng dẫn sử dụng:
0. **Biên dịch**
```
$ make

```

1. **Cài đặt, xóa bỏ module**
```
$ sudo insmod memory.ko
$ sudo rmmod memory
```

2. **Tạo file thiết bị**
```
$ sudo mknod /dev/memory c 60 0
$ sudo chmod 666 /dev/memory
```
	
3. **Đọc ghi với file**
```
$ echo -n abcdef > /dev/memory
$ cat /dev/memory
```

4. **Xem log**
```
$ dmesg
```
5. **Xem danh sách các module đã cài**
```
$ lsmod | head -10

```
6. **Xem major number của các device**
```
$ cat /proc/devices
```