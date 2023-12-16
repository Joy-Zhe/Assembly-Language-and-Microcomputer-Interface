#include "BMPLoader.h"

int main() {

    BMPImage bmp(R"(D:\code\Courses\LBP\saber.bmp)");
    std::cout << bmp.getWidth() << "*" << bmp.getHeight() << std::endl;
    bmp.runTests();
    bmp.save(R"(D:\code\Courses\LBP\binary.bmp)", BINARY);
    bmp.save(R"(D:\code\Courses\LBP\gray.bmp)", GRAY);
    bmp.save(R"(D:\code\Courses\LBP\color.bmp)", COLOR);


    return 0;

#ifdef NO_SIMD
    std::cout << "Hello, World!" << std::endl;
#endif
#ifdef SSE2
    std::cout << "Hello, World!" << std::endl;
#endif
    return 0;
}
