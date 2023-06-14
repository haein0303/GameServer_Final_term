#define SFML_STATIC 1
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <unordered_map>
#include <Windows.h>
#include <chrono>
#include <fstream>
using namespace std;

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "ws2_32.lib")

#include "..\gs_fianl_Server\protocol_2023.h"

sf::TcpSocket s_socket;

constexpr auto SCREEN_WIDTH = 16;
constexpr auto SCREEN_HEIGHT = 16;

constexpr auto TILE_WIDTH = 48;
constexpr auto WINDOW_WIDTH = SCREEN_WIDTH * TILE_WIDTH;   // size of window
constexpr auto WINDOW_HEIGHT = SCREEN_WIDTH * TILE_WIDTH;

int g_left_x;
int g_top_y;
int g_myid;

sf::RenderWindow* g_window;
sf::Font g_font;

std::vector<std::vector<char>> mapData;

class OBJECT {
protected:
	bool m_showing;
	sf::Sprite m_sprite;

	sf::Text m_name;
	sf::Text m_chat;
	sf::Text m_hp;
	chrono::system_clock::time_point m_mess_end_time;
public:
	int id;
	int m_x, m_y;
	char name[NAME_SIZE];
	int _hp = 100;
	int _max_hp = 100;

	OBJECT(sf::Texture& t, int x, int y, int x2, int y2) {
		m_showing = false;
		m_sprite.setTexture(t);
		m_sprite.setTextureRect(sf::IntRect(x, y, x2, y2));
		set_name("NONAME");
		m_mess_end_time = chrono::system_clock::now();
	}
	OBJECT() {
		m_showing = false;
	}
	void scale(float scale) {
		m_sprite.setScale(scale, scale);
	}
	void show()
	{
		m_showing = true;
	}
	void hide()
	{
		m_showing = false;
	}

	void a_move(int x, int y) {
		m_sprite.setPosition((float)x, (float)y);
	}

	void a_draw() {
		g_window->draw(m_sprite);
	}

	void move(int x, int y) {
		m_x = x;
		m_y = y;
	}
	void draw() {
		if (false == m_showing) return;
		float rx = (m_x - g_left_x) * TILE_WIDTH + 1;
		float ry = (m_y - g_top_y) * TILE_WIDTH + 1;
		m_sprite.setPosition(rx, ry);
		g_window->draw(m_sprite);
		auto size = m_name.getGlobalBounds();
		if (m_mess_end_time < chrono::system_clock::now()) {
			m_name.setPosition(rx - size.width / 2, ry - 10);
			g_window->draw(m_name);
		}
		else {
			m_chat.setPosition(rx- size.width / 2, ry - 10);
			g_window->draw(m_chat);
		}
		m_hp.setPosition(rx - size.width / 2, ry - 30);
		g_window->draw(m_hp);
	}
	void set_name(const char str[]) {
		m_name.setFont(g_font);
		m_name.setString(str);
		if (id < MAX_USER) m_name.setFillColor(sf::Color(255, 255, 255));
		else m_name.setFillColor(sf::Color(255, 255, 0));
		m_name.setStyle(sf::Text::Bold);
	}

	void set_chat(const char str[]) {
		m_chat.setFont(g_font);
		m_chat.setString(str);
		m_chat.setFillColor(sf::Color(255, 255, 255));
		m_chat.setStyle(sf::Text::Bold);
		m_mess_end_time = chrono::system_clock::now() + chrono::seconds(3);
	}

	void set_hp(int hp) {
		_hp = hp;
		char txt[20];
		sprintf_s(txt, "%d/%d", _hp , _max_hp);
		m_hp.setFont(g_font);
		m_hp.setString(txt);
		m_hp.setFillColor(sf::Color(255, 255, 255));
		m_hp.setStyle(sf::Text::Bold);
	}
};

class PLAYER :public OBJECT {
private:
	int _anim = 0;
	int _direction = 0;
	sf::Sprite m_sprite[4][3];
public:
	int		exp;
	int		level;
	

	PLAYER() {
		m_showing = false;
	}
	PLAYER(sf::Texture& t) {
		m_showing = false;
	
		set_name("NONAME");
		//up
		for (int j = 0; j < 3; ++j) {
			m_sprite[0][j].setTexture(t);
			m_sprite[0][j].setTextureRect(sf::IntRect(16 * j, 16 * 1, 16, 16));
			m_sprite[0][j].setScale(4.f, 4.f);
		}
		//down
		for (int j = 0; j < 3; ++j) {
			m_sprite[1][j].setTexture(t);
			m_sprite[1][j].setTextureRect(sf::IntRect(16 * j, 16 * 2, 16, 16));
			m_sprite[1][j].setScale(4.f, 4.f);
		}
			
		//left
		for (int j = 0; j < 3; ++j) {
			m_sprite[2][j].setTexture(t);
			m_sprite[2][j].setTextureRect(sf::IntRect(16 * j, 16 * 0, 16, 16));
			m_sprite[2][j].setScale(4.f, 4.f);
		}
		//right
		for (int j = 0; j < 3; ++j) {
			m_sprite[3][j].setTexture(t);
			m_sprite[3][j].setTextureRect(sf::IntRect(16 * j, 16 * 0, 16, 16));
			m_sprite[3][j].setScale(-4.f, 4.f);
		}

		m_mess_end_time = chrono::system_clock::now();


	}

	void set_direction(const int& dir) {
		_direction = dir;
		_anim = (_anim + 1) % 3;
	}
	
	int get_direction() {
		return _direction;
	}

	void draw() {
		if (false == m_showing) return;
		float rx = (m_x - g_left_x) * TILE_WIDTH + 1;
		float ry = (m_y - g_top_y) * TILE_WIDTH + 1;
		m_sprite[_direction][_anim].setPosition(rx, ry);
		g_window->draw(m_sprite[_direction][_anim]);
		auto size = m_name.getGlobalBounds();
		if (m_mess_end_time < chrono::system_clock::now()) {
			m_name.setPosition(rx - size.width / 2, ry - 10);
			g_window->draw(m_name);
		}
		else {
			m_chat.setPosition(rx - size.width / 2, ry - 10);
			g_window->draw(m_chat);
		}
		m_hp.setPosition(rx + 32 - size.width / 2, ry - 30);
		g_window->draw(m_hp);
	}

};

OBJECT avatar;
unordered_map <int, OBJECT> players;

OBJECT white_tile;
OBJECT black_tile;

PLAYER player;


sf::Texture* board;
sf::Texture* pieces;

sf::Texture* player_tex;
sf::Texture* bg;

sf::RectangleShape* rectangle;

bool is_chat_input = false;
int msg_count = 0;
char my_msg[CHAT_SIZE];

int party_num = -1;

struct P_data {
	bool is_active = false;
	int id;
	int hp;
	int max_hp;
	char name[NAME_SIZE];
};

P_data party_data[MAX_PARTY];



struct chat {
	bool active = false;
	char name[NAME_SIZE+2];
	char msg[CHAT_SIZE+2];
};

int act_chat = 1;
chat msg_data[10];

void client_initialize()
{
	board = new sf::Texture;
	pieces = new sf::Texture;
	bg = new sf::Texture;
	player_tex = new sf::Texture;

	rectangle = new sf::RectangleShape(sf::Vector2f(TILE_WIDTH * (SCREEN_WIDTH / 2), TILE_WIDTH * (SCREEN_WIDTH / 3)));

	board->loadFromFile(".\\resource\\grass-tile.png");
	bg->loadFromFile(".\\resource\\ground2.png");

	pieces->loadFromFile("chess2.png");

	player_tex->loadFromFile(".\\resource\\hero.png");

	if (false == g_font.loadFromFile("cour.ttf")) {
		cout << "Font Loading Error!\n";
		exit(-1);
	}
	white_tile = OBJECT{ *board, 0, 0, TILE_WIDTH, TILE_WIDTH };
	black_tile = OBJECT{ *bg, 0, 0, TILE_WIDTH, TILE_WIDTH };
	avatar = OBJECT{ *pieces, 128, 0, -64, 64 };
	player = PLAYER{ *player_tex };
	
	rectangle->setFillColor(sf::Color(0,0,0,128));
	rectangle->setPosition(WINDOW_WIDTH - TILE_WIDTH * (SCREEN_WIDTH / 2), WINDOW_HEIGHT - TILE_WIDTH * (SCREEN_WIDTH / 3));
	

}

void client_finish()
{
	players.clear();
	delete board;
	delete pieces;
}

void print_chat(const char* msg, const char* name) {
	sprintf_s(msg_data[act_chat].msg, "%s", msg);
	sprintf_s(msg_data[act_chat].name, "%s", name);
	msg_data[act_chat].active = true;
	printf("[%s]%s\n", msg_data[act_chat].name, msg_data[act_chat].msg);
	act_chat++;
}

void ProcessPacket(char* ptr)
{
	static bool first_time = true;
	switch (ptr[2])
	{
	case SC_LOGIN_INFO:
	{
		SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(ptr);
		g_myid = packet->id;
		avatar.id = g_myid;
		player.move(packet->x, packet->y);
		g_left_x = packet->x - SCREEN_WIDTH / 2;
		g_top_y = packet->y - SCREEN_HEIGHT / 2;
		player.set_hp(packet->hp);
		player.show();		
	}
	break;

	case SC_ADD_OBJECT:
	{
		SC_ADD_OBJECT_PACKET* my_packet = reinterpret_cast<SC_ADD_OBJECT_PACKET*>(ptr);
		int id = my_packet->id;

		if (id == g_myid) {
			player.move(my_packet->x, my_packet->y);
			g_left_x = my_packet->x - SCREEN_WIDTH / 2;
			g_top_y = my_packet->y - SCREEN_HEIGHT / 2;
			player.show();
		}
		else if (id < MAX_USER) {
			players[id] = OBJECT{ *pieces, 0, 0, 64, 64 };
			players[id].id = id;
			players[id].move(my_packet->x, my_packet->y);
			players[id].set_name(my_packet->name);
			players[id].set_hp(my_packet->hp);
			players[id].show();
		}
		else {
			players[id] = OBJECT{ *pieces, 256, 0, 64, 64 };
			players[id].id = id;
			players[id].move(my_packet->x, my_packet->y);
			players[id].set_name(my_packet->name);
			players[id].set_hp(my_packet->hp);
			players[id].show();
		}
		break;
	}
	case SC_MOVE_OBJECT:
	{
		SC_MOVE_OBJECT_PACKET* my_packet = reinterpret_cast<SC_MOVE_OBJECT_PACKET*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {			
			player.move(my_packet->x, my_packet->y);
			player.set_hp(my_packet->hp);
			g_left_x = my_packet->x - SCREEN_WIDTH / 2;
			g_top_y = my_packet->y - SCREEN_HEIGHT / 2;
		}
		else {
			players[other_id].move(my_packet->x, my_packet->y);
			players[other_id].set_hp(my_packet->hp);
		}
		break;
	}

	case SC_REMOVE_OBJECT:
	{
		SC_REMOVE_OBJECT_PACKET* my_packet = reinterpret_cast<SC_REMOVE_OBJECT_PACKET*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			player.hide();
		}
		else {
			players.erase(other_id);
		}
		break;
	}
	case SC_DIE_OBJECT:
	{
		SC_DIE_OBJECT_PACKET* my_packet = reinterpret_cast<SC_DIE_OBJECT_PACKET*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			player.hide();
			printf("YOU DIE : %d", my_packet->exp);
		}
		else {
			players.erase(other_id);
		}
		printf("GET EXP : %d", my_packet->exp);
		break;
	}
	case SC_CHAT:
	{
		SC_CHAT_PACKET* my_packet = reinterpret_cast<SC_CHAT_PACKET*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			player.set_chat(my_packet->mess);
		}
		else {
			players[other_id].set_chat(my_packet->mess);
		}
		print_chat(my_packet->mess, my_packet->name);
		break;
	}
	case SC_STAT_CHANGE: {
		SC_STAT_CHANGE_PACKET* my_packet = reinterpret_cast<SC_STAT_CHANGE_PACKET*>(ptr);
		player._hp = my_packet->hp;
		player._max_hp = my_packet->max_hp;
		player.exp = my_packet->exp;
		player.level = my_packet->level;
		break;
	}
	case SC_P_CREATE: {
		SC_P_CREATE_PACKET* my_packet = reinterpret_cast<SC_P_CREATE_PACKET*>(ptr);
		party_num = my_packet->p_id;
		print_chat("CREAT PARTY : GOOD", "SYSTEM");
		break;
	}
	case SC_P_ENTER: {
		SC_P_ENTER_PACKET* my_packet = reinterpret_cast<SC_P_ENTER_PACKET*>(ptr);
		party_num = my_packet->p_id;
		print_chat("JOIN PARTY : GOOD", "SYSTEM");
		break;
	}
	case SC_P_JOIN: {
		SC_P_JOIN_PACKET* my_packet = reinterpret_cast<SC_P_JOIN_PACKET*>(ptr);
		int i;
		for (i = 0; i < MAX_PARTY; ++i) {
			if (party_data[i].is_active != true) break;
		}
		party_data[i].is_active = true;
		party_data[i].id = my_packet->id;
		party_data[i].hp = my_packet->hp;
		party_data[i].max_hp = my_packet->max_hp;
		sprintf_s(party_data[i].name, "%s", my_packet->name);
		char tmp[100];
		sprintf_s(tmp, "JOIN PARTY [%s]", party_data[i].name);
		print_chat(tmp, "SYSTEM");
		break;
	}
	case SC_P_EXIT: {
		SC_P_EXIT_PACKET* my_packet = reinterpret_cast<SC_P_EXIT_PACKET*>(ptr);
		for (int i = 0; i < MAX_PARTY; ++i) {
			if (party_data[i].id == my_packet->id) {
				party_data[i].is_active = false;
				party_data[i].id = 0;
				party_data[i].hp = 0;
				party_data[i].max_hp = 0;

				char tmp[100];
				sprintf_s(tmp, "EXIT PARTY [%s]", party_data[i].name);
				print_chat(tmp, "SYSTEM");

				memset(party_data[i].name, '\0', NAME_SIZE);

				break;
			}
		}
		break;
	}
	case SC_P_STAT: {
		SC_P_STAT_PACKET* my_packet = reinterpret_cast<SC_P_STAT_PACKET*>(ptr);
		for (int i = 0; i < MAX_PARTY; ++i) {
			if (party_data[i].id == my_packet->id) {
			}
		}
	}
	default:
		printf("Unknown PACKET type [%d]\n", ptr[2]);
	}
}

void process_data(char* net_buf, size_t io_byte)
{
	char* ptr = net_buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static char packet_buffer[BUF_SIZE];

	while (0 != io_byte) {
		if (0 == in_packet_size) in_packet_size = *reinterpret_cast<short*>(ptr);
		if (io_byte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			io_byte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
			saved_packet_size += io_byte;
			io_byte = 0;
		}
	}
}

void client_main()
{
	char net_buf[BUF_SIZE];
	size_t	received;

	auto recv_result = s_socket.receive(net_buf, BUF_SIZE, received);
	if (recv_result == sf::Socket::Error)
	{
		wcout << L"Recv fail!";
		exit(-1);
	}
	if (recv_result == sf::Socket::Disconnected) {
		wcout << L"Disconnected\n";
		exit(-1);
	}
	if (recv_result != sf::Socket::NotReady) 
		if (received > 0) process_data(net_buf, received);
	
		
		

	for (int i = 0; i < SCREEN_WIDTH; ++i) {
		for (int j = 0; j < SCREEN_HEIGHT; ++j)
		{
			int tile_x = i + g_left_x;
			int tile_y = j + g_top_y;
			if ((tile_x < 0) || (tile_y < 0)) continue;
			if (mapData[tile_y][tile_x] == 1) {
				white_tile.a_move(TILE_WIDTH * i, TILE_WIDTH * j);
				white_tile.a_draw();
			}
			else
			{
				black_tile.a_move(TILE_WIDTH * i, TILE_WIDTH * j);
				black_tile.a_draw();
			}
		}
	}

	player.draw();
	for (auto& pl : players) pl.second.draw();
	sf::Text text;
	text.setFont(g_font);
	char buf[100];
	sprintf_s(buf, "(%d, %d)", player.m_x, player.m_y);
	text.setString(buf);
	g_window->draw(text);

	g_window->draw(*rectangle);
	text.setFont(g_font);
	text.setCharacterSize(15);
	int now_print_chat = 0;
	for (int i = 10; i > 0; --i) {
		int tmp = (i + act_chat) % 10;
		if (msg_data[tmp].active != true) continue;
		
		sprintf_s(buf, "[%s]%s", msg_data[tmp].name, msg_data[tmp].msg);
		text.setString(buf);
		text.setPosition(WINDOW_WIDTH - TILE_WIDTH * (SCREEN_WIDTH / 2), WINDOW_HEIGHT - (now_print_chat * 20) - 40);
		g_window->draw(text);
		now_print_chat++;
	}
	text.setString(my_msg);
	text.setPosition(WINDOW_WIDTH - TILE_WIDTH * (SCREEN_WIDTH / 2), WINDOW_HEIGHT - 20);
	g_window->draw(text);

	text.setCharacterSize(20);
	if (party_num != -1) {
		int counter = 0;
		for (int i = 0; i < MAX_PARTY; ++i) {
			if (party_data[i].is_active == true) {
				sprintf_s(buf, "[%s]%d/%d", party_data[i].name, party_data[i].hp, party_data[i].max_hp);
				text.setString(buf);
				text.setPosition(WINDOW_WIDTH, WINDOW_HEIGHT + counter*23 - (WINDOW_HEIGHT /2));
				counter++;
			}
		}
	}

}

void send_packet(void* packet)
{
	unsigned char* p = reinterpret_cast<unsigned char*>(packet);
	size_t sent = 0;
	s_socket.send(packet, p[0], sent);
}


void loadMap(const std::string& filename) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cout << "파일을 열 수 없습니다." << std::endl;
		return;
	}

	mapData.resize(W_HEIGHT, std::vector<char>(W_WIDTH, 0));

	std::string line;
	for (int y = 0; y < W_HEIGHT; ++y) {
		if (std::getline(file, line)) {
			for (int x = 0; x < W_WIDTH; ++x) {
				if (line[x] == '1') {
					mapData[y][x] = 1;
				}
			}
		}
	}

	file.close();
	std::cout << "맵 데이터를 불러왔습니다." << std::endl;
}


int main()
{
	wcout.imbue(locale("korean"));
	sf::Socket::Status status = s_socket.connect("127.0.0.1", PORT_NUM);
	s_socket.setBlocking(false);

	if (status != sf::Socket::Done) {
		wcout << L"server is not open.\n";
		exit(-1);
	}

	std::string filename = "map.txt";
	loadMap(filename);


	client_initialize();
	CS_LOGIN_PACKET p;
	p.size = sizeof(p);
	p.type = CS_LOGIN;
	

	string player_name{ "P" };
	player_name += to_string(GetCurrentProcessId());

	strcpy_s(p.name, player_name.c_str());
	send_packet(&p);
	player.set_name(p.name);
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "2D CLIENT");
	g_window = &window;

	

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				int direction = -1;
				
				switch (event.key.code) {
				case sf::Keyboard::Left:
					direction = 2;
					player.set_direction(direction);
					break;
				case sf::Keyboard::Right:
					direction = 3;
					player.set_direction(direction);
					break;
				case sf::Keyboard::Up:
					direction = 0;
					player.set_direction(direction);
					break;
				case sf::Keyboard::Down:
					direction = 1;
					player.set_direction(direction);
					break;
				case sf::Keyboard::Escape: {
					window.close();
					break;
				}
				case sf::Keyboard::Slash: {
					is_chat_input = !is_chat_input;
					break;
				}
				}

				if (is_chat_input) {
					if (event.key.code == sf::Keyboard::Return) {
						bool toggle = true;
						if (!strcmp(my_msg, "P CREATE")) {
							if (party_num != -1) {
								print_chat("YOU ENTER PARTY NOW", "SYSTEM");
								continue;
							}
							CS_P_CREATE_PACKET p;
							p.size = sizeof(p);
							p.type = CS_P_CREATE;
							send_packet(&p);
							cout << "CREATE PARTY" << endl;
							toggle = false;
						}

						if (!strcmp(my_msg, "P EXIT")) {
							CS_P_EXIT_PACKET p;
							p.size = sizeof(p);
							p.type = CS_P_EXIT;
							send_packet(&p);
							cout << "EXIT PARTY" << endl;
							toggle = false;
						}

						char* ptr = strstr(my_msg, "P JOIN");
						if (ptr != nullptr) {
							if (party_num != -1) {
								print_chat("YOU TRY TO ENTER PARTY", "SYSTEM");
								continue;
							}
							CS_P_JOIN_PACKET p;
							p.size = sizeof(p);
							p.type = CS_P_JOIN;
							p.p_num = atoi(ptr + 7);
							send_packet(&p);
							toggle = false;
						}

						if (toggle) {
							CS_CHAT_PACKET p;
							p.size = sizeof(p);
							p.type = CS_CHAT;
							my_msg[msg_count] = '\0';
							sprintf_s(p.mess, "%s", my_msg);
							send_packet(&p);
						}	

						memset(my_msg, '\0', CHAT_SIZE);

						msg_count = 0;
						continue;
					}

					if (event.key.code == sf::Keyboard::BackSpace) {
						if (msg_count <= 0) {
							my_msg[0] = '\0';
							continue;
						}
						my_msg[msg_count--] = '\0';
						continue;
					}
					if (event.key.code >= sf::Keyboard::Numpad0 && event.key.code <= sf::Keyboard::Numpad9) {
						my_msg[msg_count++] = '0' + event.key.code - sf::Keyboard::Numpad0;
						continue;
					}
					if (event.key.code >= sf::Keyboard::Num0 && event.key.code <= sf::Keyboard::Num9) {
						my_msg[msg_count++] = '0' + event.key.code - sf::Keyboard::Num0;
						continue;
					}
					if (event.key.code < 0 && event.key.code >('Z' - 'A')) {
						continue;
					}
					if (msg_count > CHAT_SIZE - 2) continue;
					if (event.key.code == sf::Keyboard::Space) {
						my_msg[msg_count++] = ' ';
						continue;
					}

					my_msg[msg_count++] = 'A' + event.key.code;

				}
				else {
					switch (event.key.code) {
					case sf::Keyboard::A: { //ATTACK
						CS_ATTACK_PACKET p;
						p.size = sizeof(p);
						p.type = CS_ATTACK;
						p.atk_type = 0;
						send_packet(&p);
						printf("ATK\n");
						break;
					}
					case sf::Keyboard::S: { //ATTACK
						CS_ATTACK_PACKET p;
						p.size = sizeof(p);
						p.type = CS_ATTACK;
						p.atk_type = 1;
						send_packet(&p);
						printf("ATK_SKILL\n");
						break;
					}
					case sf::Keyboard::T: {//Teleport
						CS_TELEPORT_PACKET p;
						p.size = sizeof(p);
						p.type = CS_TELEPORT;
						p.direction = player.get_direction();
						send_packet(&p);
						break;
					}
					}
				}
				

				if (-1 != direction) {
					CS_MOVE_PACKET p;
					p.size = sizeof(p);
					p.type = CS_MOVE;
					p.direction = direction;
					send_packet(&p);
				}
				

			}
		}

		window.clear();
		client_main();
		window.display();
	}
	client_finish();

	return 0;
}