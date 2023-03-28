#pragma GCC optimize("O3")
#pragma GCC target("avx2")

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

unsigned char *image_data;

#include "stb_image.h"
#include "stb_image_write.h"
#include <vector>
#include <algorithm>
#include <chrono>
#include <iostream>

using namespace std;

void negative_filter_OMP(unsigned char *image_data, int width, int height) {
#pragma omp parallel for
    for (int i = 0; i < width * height * 4; i += 4) {
        image_data[i] = 255 - image_data[i];
        image_data[i + 1] = 255 - image_data[i + 1];
        image_data[i + 2] = 255 - image_data[i + 2];
    }
}

void negative_filter_consistently(unsigned char *image_data, int width, int height) {
    for (int i = 0; i < width * height * 4; i += 4) {
        image_data[i] = 255 - image_data[i];
        image_data[i + 1] = 255 - image_data[i + 1];
        image_data[i + 2] = 255 - image_data[i + 2];
    }
}


unsigned char *median_filter_OMP(unsigned char *image_data, int width, int height, int channels, int filterSize) {
    unsigned char *outputImg = new unsigned char[width * height * channels];
#pragma omp parallel for collapse(3)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channels; c++) {
                vector<unsigned char> pixels;
                for (int ky = -filterSize; ky <= filterSize; ky++) {
                    for (int kx = -filterSize; kx <= filterSize; kx++) {
                        int px = x + kx;
                        int py = y + ky;
                        if (px >= 0 and px < width and py >= 0 and py < height) {
                            pixels.push_back(image_data[(py * width + px) * channels + c]);
                        }
                    }
                }
                sort(pixels.begin(), pixels.end());
                unsigned char medianValue = pixels[pixels.size() / 2];

                outputImg[(y * width + x) * channels + c] = medianValue;
            }
        }
    }
    return outputImg;
}

unsigned char *
median_filter_consistently(unsigned char *image_data, int width, int height, int channels, int filterSize) {
    unsigned char *outputImg = new unsigned char[width * height * channels];
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channels; c++) {
                vector<unsigned char> pixels;
                for (int ky = -filterSize; ky <= filterSize; ky++) {
                    for (int kx = -filterSize; kx <= filterSize; kx++) {
                        int px = x + kx;
                        int py = y + ky;
                        if (px >= 0 and px < width and py >= 0 and py < height) {
                            pixels.push_back(image_data[(py * width + px) * channels + c]);
                        }
                    }
                }
                sort(pixels.begin(), pixels.end());
                unsigned char medianValue = pixels[pixels.size() / 2];

                outputImg[(y * width + x) * channels + c] = medianValue;
            }
        }
    }
    return outputImg;
}


int main() {
    int width, height, channels, answerPic, answer1, answer2;
    cout << "Pick the image:" << endl << "1. 300x300" << endl << "2. 400x400" << endl << "3. 500x500" << endl
         << "4. 600x600" << endl << "5. 950x950" << endl << "6. 2400x2400" << endl;
    cin >> answerPic;
    if (answerPic == 1) image_data = stbi_load("300x300.png", &width, &height, &channels, STBI_rgb_alpha);
    if (answerPic == 2) image_data = stbi_load("400x400.png", &width, &height, &channels, STBI_rgb_alpha);
    if (answerPic == 3) image_data = stbi_load("500x500.png", &width, &height, &channels, STBI_rgb_alpha);
    if (answerPic == 4) image_data = stbi_load("600x600.png", &width, &height, &channels, STBI_rgb_alpha);
    if (answerPic == 5) image_data = stbi_load("950x950.png", &width, &height, &channels, STBI_rgb_alpha);
    if (answerPic == 6) image_data = stbi_load("2400x2400.png", &width, &height, &channels, STBI_rgb_alpha);
    cout << "1. Negative Filter" << endl << "2. Median Filter" << endl;
    cin >> answer1;
    cout << "1. Consistently" << endl << "2. OMP" << endl;
    cin >> answer2;

    if (answer1 == 1) {
        if (answer2 == 1) {
            auto start = chrono::high_resolution_clock::now();
            for (int i = 0; i < 501; ++i) {
                negative_filter_consistently(image_data, width, height);
            }
            auto end = chrono::high_resolution_clock::now();
            auto duration_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
            stbi_write_jpg("output.png", width, height, STBI_rgb_alpha, image_data, 100);
            cout << "Image processed in " << duration_ms << " ms." << endl;
        }
        if (answer2 == 2) {
            auto start = chrono::high_resolution_clock::now();
            negative_filter_OMP(image_data, width, height);
            auto end = chrono::high_resolution_clock::now();
            auto duration_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
            stbi_write_jpg("output.png", width, height, STBI_rgb_alpha, image_data, 100);
            cout << "Image processed in " << duration_ms << " ms." << endl;
        }
    }
    if (answer1 == 2) {
        if (answer2 == 1) {
            auto start = chrono::high_resolution_clock::now();
            unsigned char *outputImg = median_filter_consistently(image_data, width, height, channels, 7);
            stbi_write_png("output.png", width, height, STBI_rgb_alpha, outputImg, width * channels);
            auto end = chrono::high_resolution_clock::now();
            auto duration_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
            cout << "Image processed in " << duration_ms << " ms." << endl;
        }
        if (answer2 == 2) {
            auto start = chrono::high_resolution_clock::now();
            unsigned char *outputImg = median_filter_OMP(image_data, width, height, channels, 7);
            stbi_write_png("output.png", width, height, STBI_rgb_alpha, outputImg, width * channels);
            auto end = chrono::high_resolution_clock::now();
            auto duration_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
            cout << "Image processed in " << duration_ms << " ms." << endl;
        }
        if (answer2 == 3) cout << "Go to Visual Studio";
    }

    return 0;
}
