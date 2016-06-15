#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

// biblioteca padrão I/O de C++
using namespace std;

void check_program_arguments(int argc) {
	if (argc != 2) {
		std::cout << "Error! Program usage:" << std::endl;
		std::cout << "./circle_detect image_circles_path" << std::endl;
		std::exit(-1);
	}
}

void check_if_image_exist(const cv::Mat &img, const std::string &path) {
	if (img.empty()) {
		std::cout << "Error! Unable to load image: " << path << std::endl;
		std::exit(-1);
	}
}


int main(int argc, char **argv) {
	// Usage: ./circle_detect image_circles_path
	check_program_arguments(argc);

	// Carrega a imagem 
	std::string path_image{ argv[1] };
	cv::Mat bgr_image = cv::imread(path_image);

	// Verifica se a imagem foi carregada
	check_if_image_exist(bgr_image, path_image);

	//--------------------------------------------
	
	cv::Mat orig_image = bgr_image.clone();
	//diminue os ruídos da imagem
	cv::medianBlur(bgr_image, bgr_image, 1);

	//--------------------------------------------

	// Converte a imagem para hsv
	cv::Mat hsv_image;
	cv::cvtColor(bgr_image, hsv_image, cv::COLOR_BGR2HSV);

	// limita a imagem HSV para manter apenas os pixels vermelhos
	cv::Mat lower_red_hue_range;
	cv::Mat upper_red_hue_range;
	cv::inRange(hsv_image, cv::Scalar(0, 80, 80), cv::Scalar(10, 255, 255), lower_red_hue_range);
	cv::inRange(hsv_image, cv::Scalar(130, 80, 80), cv::Scalar(190, 255, 255), upper_red_hue_range);

	// Combinas a duas imagens com menor e maior tons de cores
	cv::Mat red_hue_image;
	cv::addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);

	//	cv::GaussianBlur(red_hue_image, red_hue_image, cv::Size(9, 9), 2, 2);
	cv::GaussianBlur(red_hue_image, red_hue_image, cv::Size(9, 9), 2, 5);

	// Usando a transformada de Hough para detectar círculos na imagem combinada
	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(red_hue_image, circles, CV_HOUGH_GRADIENT, 1, red_hue_image.rows / 8, 100, 20, 0, 0);

	/* Loop para percorrer todos os círculos detectados e delineá-los na imagem original e
	   contar a quantidade de objetos detectados
	*/
	int contador = 0;
	if (circles.size() == 0) std::exit(-1);
	for (size_t current_circle = 0; current_circle < circles.size(); ++current_circle) {
		cv::Point center(std::round(circles[current_circle][0]), std::round(circles[current_circle][1]));
		int radius = std::round(circles[current_circle][2]);
		if (radius < 64 && radius > 8) {
			++contador;
			char valor[3] = { 0 };
			_itoa(contador, valor, 10);
			cv::putText(orig_image, valor, center, cv::FONT_HERSHEY_COMPLEX, 0.6, 105);
			cv::circle(orig_image, center, radius, cv::Scalar(0, 255, 255), 2);
		}
	}

	// Show images
	//cv::namedWindow("Threshold lower image", cv::WINDOW_AUTOSIZE);
	//cv::imshow("Threshold lower image", lower_red_hue_range);
	//cv::namedWindow("Threshold upper image", cv::WINDOW_AUTOSIZE);
	//cv::imshow("Threshold upper image", upper_red_hue_range);
	//cv::namedWindow("Combined threshold images", cv::WINDOW_AUTOSIZE);
	//cv::imshow("Combined threshold images", red_hue_image);

	cv::namedWindow("Detected red circles on the input image", cv::WINDOW_AUTOSIZE);
	cv::imshow("Detected red circles on the input image", orig_image);

	cv::waitKey(0);
	return 0;
}