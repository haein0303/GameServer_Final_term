#include <iostream>
#include <fstream>
#include <random>

// 맵의 크기
const int mapWidth = 2000;
const int mapHeight = 2000;

// 빈 공간 퍼센트
const int emptyPercent = 10;

// 맵 생성 함수
void generateMap() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);

    std::ofstream file("map.txt");
    if (!file.is_open()) {
        std::cout << "파일을 열 수 없습니다." << std::endl;
        return;
    }

    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            if (dis(gen) <= emptyPercent) {
                file << "0";
            }
            else {
                file << "1";
            }
        }
        file << std::endl;
    }

    file.close();
    std::cout << "맵 생성이 완료되었습니다." << std::endl;
}

int main() {
    generateMap();
    return 0;
}
