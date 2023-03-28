#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <chrono>

using namespace cv;

Mat negativeVectorized(Mat input) {
	Mat output = input;
	__m128i value = _mm_set1_epi8(255);
	int remainder = input.rows % 8;
	int column1;
	int width = input.cols;
	for (int row = 0; row < input.rows; row++) {
		for (int column = 0; column < input.cols - remainder; column += 8) {
			__m128i color1 = _mm_sub_epi8(value, _mm_loadu_si128((__m128i*) & input.ptr<int>(row, column)[0]));
			__m128i color2 = _mm_sub_epi8(value, _mm_loadu_si128((__m128i*) & input.ptr<int>(row, column)[1]));
			__m128i color3 = _mm_sub_epi8(value, _mm_loadu_si128((__m128i*) & input.ptr<int>(row, column)[2]));

			_mm_storeu_si128((__m128i*) & output.ptr<int>(row, column)[0], color1);
			_mm_storeu_si128((__m128i*) & output.ptr<int>(row, column)[1], color2);
			_mm_storeu_si128((__m128i*) & output.ptr<int>(row, column)[2], color3);

			column1 = column;
		}
		for (int i = 0, j = remainder; i < remainder; i++, column1++, j--) {
			output.at<Vec3b>(row, width - j)[0] = 255 - input.at<Vec3b>(row, width - j)[0];
			output.at<Vec3b>(row, width - j)[1] = 255 - input.at<Vec3b>(row, width - j)[1];
			output.at<Vec3b>(row, width - j)[2] = 255 - input.at<Vec3b>(row, width - j)[2];
		}


	}
	return output;
}


Mat filterToMedian(Mat image, int k)
{
	Mat medianImage = Mat::zeros(image.size(), image.type());

	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			std::vector<unsigned char> red, blue, green;
			int size = 0;

			for (int rowOfSurrondingsPixels = -k; rowOfSurrondingsPixels <= k; rowOfSurrondingsPixels++)
			{
				for (int colOfSurrondingsPixels = -k; colOfSurrondingsPixels <= k;)
				{
					if (i + rowOfSurrondingsPixels >= 0 && i + rowOfSurrondingsPixels < image.rows && j + colOfSurrondingsPixels >= 0 && j + colOfSurrondingsPixels < image.cols)
					{
						if (j + colOfSurrondingsPixels + 4 < image.cols)
						{
							__m128i sourceValue = _mm_loadu_si128((__m128i*)(image.ptr<Vec3b>(i + rowOfSurrondingsPixels, j + colOfSurrondingsPixels)));

							uint8_t redChannelValue = _mm_extract_epi8(sourceValue, 0);
							uint8_t blueChannelValue = _mm_extract_epi8(sourceValue, 1);
							uint8_t greenChannelValue = _mm_extract_epi8(sourceValue, 2);

							red.push_back(redChannelValue);
							blue.push_back(blueChannelValue);
							green.push_back(greenChannelValue);

							redChannelValue = _mm_extract_epi8(sourceValue, 3);
							blueChannelValue = _mm_extract_epi8(sourceValue, 4);
							greenChannelValue = _mm_extract_epi8(sourceValue, 5);

							red.push_back(redChannelValue);
							blue.push_back(blueChannelValue);
							green.push_back(greenChannelValue);

							redChannelValue = _mm_extract_epi8(sourceValue, 6);
							blueChannelValue = _mm_extract_epi8(sourceValue, 7);
							greenChannelValue = _mm_extract_epi8(sourceValue, 8);

							red.push_back(redChannelValue);
							blue.push_back(blueChannelValue);
							green.push_back(greenChannelValue);

							redChannelValue = _mm_extract_epi8(sourceValue, 9);
							blueChannelValue = _mm_extract_epi8(sourceValue, 10);
							greenChannelValue = _mm_extract_epi8(sourceValue, 11);

							red.push_back(redChannelValue);
							blue.push_back(blueChannelValue);
							green.push_back(greenChannelValue);

							size += 4;
							colOfSurrondingsPixels += 4;
						}
						else
						{
							red.push_back(image.at<Vec3b>(i + rowOfSurrondingsPixels, j + colOfSurrondingsPixels)[0]);
							blue.push_back(image.at<Vec3b>(i + rowOfSurrondingsPixels, j + colOfSurrondingsPixels)[1]);
							green.push_back(image.at<Vec3b>(i + rowOfSurrondingsPixels, j + colOfSurrondingsPixels)[2]);
							size++;
							colOfSurrondingsPixels++;
						}
					}
					else
					{
						colOfSurrondingsPixels++;
					}
				}
			}

			sort(red.begin(), red.end());
			sort(blue.begin(), blue.end());
			sort(green.begin(), green.end());

			medianImage.at<Vec3b>(i, j)[0] = (size & 1) ? red[size / 2] : (red[size / 2] + red[size / 2 - 1]) / 2;
			medianImage.at<Vec3b>(i, j)[1] = (size & 1) ? blue[size / 2] : (blue[size / 2] + blue[size / 2 - 1]) / 2;
			medianImage.at<Vec3b>(i, j)[2] = (size & 1) ? green[size / 2] : (green[size / 2] + green[size / 2 - 1]) / 2;
		}
	}

	return medianImage;
}

int main()
{
	int answer;
	std::cout << "1. Median filter vectorized" << std::endl << "2. Negative filter vectorized" << std::endl;
	std::cin >> answer;
	if (answer == 1) {
		Mat firstImage = imread("C:\\Users\\NikSu\\source\\repos\\median filter vectorized\\median filter vectorized\\300x300.png");
		auto start = std::chrono::high_resolution_clock::now();
		Mat medianImage = filterToMedian(firstImage, 7);
		auto end = std::chrono::high_resolution_clock::now();
		auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		std::cout << "Image processed in " << duration_ms << " ms." << std::endl;
		imshow("Median", medianImage);
		imwrite("C:\\Users\\NikSu\\source\\repos\\median filter vectorized\\median filter vectorized\\medianImage.png", medianImage);
	}
	if (answer == 2) {
		Mat firstImage = imread("C:\\Users\\NikSu\\source\\repos\\median filter vectorized\\median filter vectorized\\950x950.png");
		auto start = std::chrono::high_resolution_clock::now();
		Mat negativeImage;
		for (size_t i = 0; i < 501; i++)
		{
			negativeImage = negativeVectorized(firstImage);
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		std::cout << "Image processed in " << duration_ms << " ms." << std::endl;
		imshow("Negative", negativeImage);
		imwrite("C:\\Users\\NikSu\\source\\repos\\median filter vectorized\\median filter vectorized\\negativeImage.png", negativeImage);
	}
	waitKey(0);
	destroyAllWindows();

	return 0;
}