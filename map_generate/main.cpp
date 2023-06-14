#include <iostream>
#include <fstream>
#include <random>

// ���� ũ��
const int mapWidth = 2000;
const int mapHeight = 2000;

// �� ���� �ۼ�Ʈ
const int emptyPercent = 10;

// �� ���� �Լ�
void generateMap() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);

    std::ofstream file("map.txt");
    if (!file.is_open()) {
        std::cout << "������ �� �� �����ϴ�." << std::endl;
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
    std::cout << "�� ������ �Ϸ�Ǿ����ϴ�." << std::endl;
}

int main() {
    generateMap();
    return 0;
}
