# Mô tả các thông tin cơ bản, ý nghĩa của từng file code.

## 1. list_syscall:
- Struct này để lưu các syscall chặn/cho phép chạy thành 1 danh sách để dễ quản lý, thực thi chặn/cho phép chạy.
- Struct này có con trỏ `value` và `num_sys` đi kèm là các hàm hỗ trợ việc khởi tạo, hủy bỏ, thêm phần tử, kiểm tra phần tử có tồn tại không trong struct.
- Và chức năng quan trọng nhất của list_syscall này: `parse_list` được dùng để trích xuất danh sách syscall được điền ở câu lệnh, push vào trong list_syscall.

## 2. log:
- Module này để sinh ra các log debug, có thể lọc các log theo từng phần (Log info, log warning hay log eror).

## 3. monitor:
- Phần logic quan trọng nhất của chương trình với code logic chính là hàm `monitor_loop`:
    + Thực hiện trace process con với pid của process con.
    + Tùy theo mode lúc nhập vào (d: deny, e: enable):
        + Nếu là deny thì sẽ chặn các syscall trong list_syscall.
        + Nếu là enable thì chỉ cho phép các syscall trong list_syscall chạy. Nếu không trong list_syscall thì không được chạy.
    + Thêm 1 tham số là `kill_mode`: Tham số này chỉ có ý nghĩa với mode deny. Nếu không thêm -kill thì chương trình chỉ chặn syscall và tiếp tục cho chương trình chạy tiếp. Nhưng khi có -kill thì gặp syscall trong list, chương trình sẽ kill luôn process con.

## 4. syscall_table:
- Phần này được ghi ra bởi file configure.sh. File này sẽ đọc thư viện chứa các syscall để tạo ra 1 bảng để ta có thể lookup bảng ra 1 số đại diện cho syscall đó. Khi chặn bằng ptrace, ta cần số đại điện cho syscall để chặn syscall
- Với code configure.sh: Có viết thêm các hàm look-up để tra cứu từ số sang tên syscall và ngược lại.

## Lưu ý:
- Chương trình có thể make install để dễ dàng thực hiện với các chương trình con khác trong máy mà không phải điền địa chỉ cụ thể của file safe run.