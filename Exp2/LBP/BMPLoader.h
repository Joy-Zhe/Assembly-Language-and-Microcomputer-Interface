#ifndef LBP_BMPLOADER_H
#define LBP_BMPLOADER_H
// Load a RGB(24bit) BMP img to a 2D array

#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <Windows.h>
#include <immintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>
#include <chrono>

struct RGB {
    uint8_t B;
    uint8_t G;
    uint8_t R;
};

enum TYPE {
    COLOR = 0,
    GRAY = 1,
    BINARY = 2
};

class BMPImage {
public:
    explicit BMPImage(const std::string &filename) {
        read(filename);
    }

    void runTests() {
        originAdaptTest();
        SSETest();
    }

    void save(const std::string &filename, const int &type = COLOR);

    void read(const std::string &filename);

    [[nodiscard]] int getWidth() const {
        return bmp_info_header.biWidth;
    }

    [[nodiscard]] int getHeight() const {
        return bmp_info_header.biHeight;
    }

    [[nodiscard]] int getBitDepth() const {
        return bmp_info_header.biBitCount;
    }

private:
    BITMAPFILEHEADER bmp_header{};
    BITMAPINFOHEADER bmp_info_header{};
    std::vector<RGB> img_data;
    std::vector<uint16_t> img_gray; // 8bit grayscale,16 bit for SIMD
    std::vector<uint8_t> img_OSTU;

    void originAdaptTest(const int &regionSize = 4, const int &offset = 10) {
        auto start = std::chrono::high_resolution_clock::now();
        adaptiveThreshold(regionSize, offset);
        auto end = std::chrono::high_resolution_clock::now();
        auto diff = end - start;
        std::cout << "Origin Time taken: " << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;
    }

    void SSETest(const int &regionSize = 4, const int &offset = 10) {
        auto start = std::chrono::high_resolution_clock::now();
        adaptiveThresholdSSE(regionSize, offset);
        auto end = std::chrono::high_resolution_clock::now();
        auto diff = end - start;
        std::cout << "SSE Time taken: " << std::chrono::duration<double, std::milli>(diff).count() << " ms" << std::endl;
    }

    void convertToGrayscale();

    void OSTU() {
        int h = bmp_info_header.biHeight;
        int w = bmp_info_header.biWidth;
        double max_sigma = -1;
        int final_T = 0;

        for (int T = 0; T <= 255; T++) {
            int cnt_fore = 0, cnt_back = 0;
            double sum_fore = 0, sum_back = 0;

            for (int i = 0; i < h; i++) {
                for (int j = 0; j < w; j++) {
                    if (img_data[i * w + j].R >= T) {
                        cnt_fore++;
                        sum_fore += img_data[i * w + j].R;
                    } else {
                        cnt_back++;
                        sum_back += img_data[i * w + j].R;
                    }
                }
            }

            if (cnt_fore == 0 || cnt_back == 0) continue;

            double uf = sum_fore / cnt_fore;
            double ub = sum_back / cnt_back;
            double w1 = static_cast<double>(cnt_fore) / (h * w);
            double w2 = static_cast<double>(cnt_back) / (h * w);
            double sigma = w1 * w2 * (uf - ub) * (uf - ub);

            if (sigma > max_sigma) {
                max_sigma = sigma;
                final_T = T;
            }
        }

        img_OSTU.resize(w * h);
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                if (img_data[i * w + j].R < final_T) {
                    img_OSTU[i * w + j] = 1;
                } else {
                    img_OSTU[i * w + j] = 0;
                }
            }
        }
    }

    void adaptiveThreshold(int regionSize, int offset) {
        int h = bmp_info_header.biHeight;
        int w = bmp_info_header.biWidth;
        img_OSTU.resize(w * h);

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                int sum = 0;
                int count = 0;
                for (int dy = -regionSize; dy <= regionSize; ++dy) {
                    for (int dx = -regionSize; dx <= regionSize; ++dx) {
                        int ny = y + dy;
                        int nx = x + dx;
                        if (ny >= 0 && ny < h && nx >= 0 && nx < w) {
                            sum += img_data[ny * w + nx].R;
                            ++count;
                        }
                    }
                }
                int threshold = sum / count - offset;
                img_OSTU[y * w + x] = img_data[y * w + x].R < threshold ? 1 : 0;
            }
        }
    }

    void adaptiveThresholdSSE(int regionSize, int offset) {
        int h = bmp_info_header.biHeight;
        int w = bmp_info_header.biWidth;
        img_OSTU.resize(w * h);
        int regionWidth = 2 * regionSize + 1;
        int sumArea = regionWidth * regionWidth;

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w - 16; x += 16) { // step = 32
                __m256i sum = _mm256_setzero_si256();
                int count = 0;

                for (int dy = -regionSize; dy <= regionSize; ++dy) {
                    for (int dx = -regionSize; dx <= regionSize; ++dx) {
                        int ny = y + dy;
                        int nx = x + dx;

                        if (ny >= 0 && ny < h && nx >= 0 && nx < w) {
                            // load 32 consecutive pixels
                            __m256i data = _mm256_loadu_si256((__m256i*)&img_gray[ny * w + nx]); // actually load 16 pixels
                            sum = _mm256_add_epi32(sum, data);
                            count += 32;
                        } else {
                            // load 8 consecutive pixels
                            __m256i data = _mm256_setzero_si256();
                            sum = _mm256_add_epi32(sum, data);
                            count += 32;
                        }
                    }
                }

                // calculate average
//                __m256i avg = _mm256_div_epi32(sum, _mm256_set1_epi32(regionWidth * regionWidth));

                // calculate threshold
                for (int i = 0; i < 16; ++i) {
//                    uint8_t threshold = avg.m256i_i8[i] - offset;
                    int threshold = sum.m256i_i16[i] / sumArea - offset;
//                    std::cout << (int)threshold << std::endl;
                    img_OSTU[y * w + x + i] = img_gray[y * w + x + i] < threshold ? 1 : 0;
                }
            }
        }
    }

};

void BMPImage::read(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open input file: " + filename);
    }

    file.read(reinterpret_cast<char*>(&bmp_header), sizeof(BITMAPFILEHEADER));
    file.read(reinterpret_cast<char*>(&bmp_info_header), sizeof(BITMAPINFOHEADER));

    int width = bmp_info_header.biWidth;
    int height = bmp_info_header.biHeight;

    int offset = (width * 3) % 4;
    if (offset != 0) {
        offset = 4 - offset;
    }

    img_data.resize(width * height);

    char trash;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            file.read(reinterpret_cast<char*>(&img_data[i * width + j]), sizeof(RGB));
        }
        for (int j = 0; j < offset; ++j) {
            file.read(&trash, sizeof(char));
        }
    }

    convertToGrayscale();

    OSTU();

    if (!file) {
        throw std::runtime_error("Error occurred while reading the BMP file.");
    }
}

void BMPImage::save(const std::string &filename, const int &type) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open output file: " + filename);
    }

    file.write(reinterpret_cast<const char*>(&bmp_header), sizeof(BITMAPFILEHEADER));
    file.write(reinterpret_cast<const char*>(&bmp_info_header), sizeof(BITMAPINFOHEADER));

    int width = bmp_info_header.biWidth;
    int height = bmp_info_header.biHeight;

    int offset = (width * 3) % 4;
    if (offset != 0) {
        offset = 4 - offset;
    }

    uint8_t tail = 0;
    if (type == COLOR) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                file.write(reinterpret_cast<const char *>(&img_data[i * width + j]), sizeof(RGB));
            }
            for (int k = 0; k < offset; k++) {
                file.write(reinterpret_cast<const char *>(&tail), sizeof(BYTE));
            }
        }
    }
    else if (type == GRAY) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                RGB pixel = RGB{static_cast<uint8_t>(img_gray[i * width + j]), static_cast<uint8_t>(img_gray[i * width + j]), static_cast<uint8_t>(img_gray[i * width + j])};
                file.write(reinterpret_cast<const char *>(&pixel), sizeof(RGB));
            }
            for (int k = 0; k < offset; k++) {
                file.write(reinterpret_cast<const char *>(&tail), sizeof(BYTE));
            }
        }
    }
    else{
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                RGB pixel = img_OSTU[i * width + j] ? RGB{0, 0, 0} : RGB{255, 255, 255};
                file.write(reinterpret_cast<const char *>(&pixel), sizeof(RGB));
            }
            for (int k = 0; k < offset; k++) {
                file.write(reinterpret_cast<const char *>(&tail), sizeof(BYTE));
            }
        }
    }

    if (!file.good()) {
        throw std::runtime_error("Error occurred while writing the RGB output file.");
    }
}

void BMPImage::convertToGrayscale() {
    int h = bmp_info_header.biHeight;
    int w = bmp_info_header.biWidth;
    img_gray.resize(w * h);
    for (int i = 0; i < img_data.size(); i++) {
        RGB &pixel = img_data[i];
        double r = pixel.R;
        double g = pixel.G;
        double b = pixel.B;
        double gray = 0.299 * r + 0.587 * g + 0.114 * b;
//        pixel.R = pixel.G = pixel.B = static_cast<uint8_t>(gray);
        img_gray[i] = static_cast<uint8_t>(gray);
    }
}


#endif //LBP_BMPLOADER_H
