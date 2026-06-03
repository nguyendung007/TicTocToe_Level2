# I. GIỚI THIỆU (INTRODUCTION)

Đây là game Caro (Tic Tac Toe) được viết bằng ngôn ngữ C++ hỗ trợ cả hai giao diện:
- Terminal Interface (chạy trên console)
- GUI Interface (sử dụng thư viện SDL2)

Tính năng nổi bật:
- Hỗ trợ nhiều kích thước bàn cờ (3x3 đến 12x12)
- Hỗ trợ nhiều luật kết thúc (EndRule): NONE / OPEN_ONE / OPEN_TWO
- Ba chế độ chơi: PVP (Người vs Người), PVE (Người vs Máy), EVE (Máy vs Máy)
- Ba cấp độ Bot: EASY, MEDIUM, HARD 
- Tính năng Undo/Redo, Surrender
- Judge mode (-j) để chấm điểm tự động

# II. YÊU CẦU HỆ THỐNG (REQUIREMENTS)

1. Trình biên dịch: g++ (MinGW/GCC) hỗ trợ C++17 trở lên
2. CMake (phiên bản 3.10 trở lên)
3. Thư viện SDL2 và các bản sdl image,ttf
4. Python 3 (để chạy grader chấm điểm testcase)

# III. CÁCH BIÊN DỊCH (BUILD INSTRUCTIONS)

cd /d/BaiTapLon/level-2-final
mkdir -p build
cd build
cmake .. -G "MinGW Makefiles" -DCMAKE_MAKE_PROGRAM=mingw32-make
cmake --build . --config Release 

# IV. CÁCH CHẠY GAME (HOW TO RUN)

1. Chạy với giao diện Terminal (mặc định):
   ./build/game

2. Chạy với giao diện GUI (SDL):
   ./build/game -g

3. Chạy với Judge mode (chạy tự động, không hiển thị giao diện):
   ./build/game -j < input.txt

4. Xem danh sách các tùy chọn:
   ./build/game --help

Các tùy chọn dòng lệnh:
  -g, --gui          : Chạy với giao diện SDL (mặc định là terminal)
  -j, --judge        : Chạy ở chế độ judge (đọc input từ stdin)
  -l, --log <file>   : Ghi log ra file chỉ định
  -h, --help         : Hiển thị trợ giúp


# V. KIỂM TRA TESTCASE (GRADING)

Sử dụng script grader.py để tự động chấm điểm tất cả testcase:

1. Đảm bảo đã biên dịch thành công file thực thi tại build/game

2. Chạy grader:
   python grader.py --target build/game --testcase_dir testcase/

3. Kết quả dự kiến: 10/10

# VI. TÍNH NĂNG NỔI BẬT (FEATURES)

1. EndRule (Luật kết thúc mở rộng):
   - NONE: Luật thắng thông thường
   - OPEN_ONE: Kiểm tra đường thắng có 2 đầu mở không
   - OPEN_TWO: Kiểm tra điều kiện chặt chẽ hơn

2. Undo thông minh:
   - PVP: Undo 1 nước
   - PVE: Undo 2 nước (cả nước người và nước bot)

3. Swap giao diện linh hoạt:
   - Chuyển đổi Terminal ↔ GUI chỉ bằng flag -g
   - Không cần sửa bất kỳ dòng code nào trong Engine


# VII. LOG VÀ DEBUG

Để bật chế độ ghi log:
  ./build/game -l game.log

Log file ghi lại:
- Các khởi tạo của ván đấu 
- Thời gian bắt đầu/kết thúc game
- Từng nước đi của người chơi và bot
- Kết quả checkWin và WinLine
- Thời gian thực thi của bot (measureExecutionTime)
- Các exception 





