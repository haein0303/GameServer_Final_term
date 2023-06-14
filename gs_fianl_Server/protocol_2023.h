constexpr int PORT_NUM = 4000;
constexpr int BUF_SIZE = 200;
constexpr int NAME_SIZE = 20;
constexpr int CHAT_SIZE = 100;

constexpr int MAX_USER = 10000;
constexpr int MAX_NPC = 20000;
constexpr int MAX_PARTY = 4;

constexpr int W_WIDTH = 2000;
constexpr int W_HEIGHT = 2000;

// Packet ID
constexpr char CS_LOGIN = 0;
constexpr char CS_MOVE = 1;
constexpr char CS_CHAT = 2;
constexpr char CS_ATTACK = 3;			// 4 방향 공격
constexpr char CS_TELEPORT = 4;			// RANDOM한 위치로 Teleport, Stress Test할 때 Hot Spot현상을 피하기 위해 구현
constexpr char CS_LOGOUT = 5;			// 클라이언트에서 정상적으로 접속을 종료하는 패킷

constexpr char CS_P_CREATE = 6;
constexpr char CS_P_JOIN = 7;
constexpr char CS_P_EXIT = 8;


constexpr char SC_LOGIN_INFO = 2;
constexpr char SC_ADD_OBJECT = 3;
constexpr char SC_REMOVE_OBJECT = 4;
constexpr char SC_MOVE_OBJECT = 5;
constexpr char SC_CHAT = 6;
constexpr char SC_LOGIN_OK = 7;
constexpr char SC_LOGIN_FAIL = 8;
constexpr char SC_STAT_CHANGE = 9;
constexpr char SC_DIE_OBJECT = 10;

constexpr char SC_P_CREATE = 11;
constexpr char SC_P_JOIN = 12;
constexpr char SC_P_STAT = 13;
constexpr char SC_P_EXIT = 14; 
constexpr char SC_P_ENTER = 15;


#pragma pack (push, 1)
struct CS_LOGIN_PACKET {
	unsigned short size;
	char	type;
	char	name[NAME_SIZE];
};

struct CS_MOVE_PACKET {
	unsigned short size;
	char	type;
	char	direction;  // 0 : UP, 1 : DOWN, 2 : LEFT, 3 : RIGHT
	unsigned int move_time;
};

struct CS_CHAT_PACKET {
	unsigned short size;			// 크기가 가변이다, mess가 작으면 size도 줄이자.
	char	type;
	char	mess[CHAT_SIZE];
};

struct CS_TELEPORT_PACKET {
	unsigned short size;
	char	type;
	char	direction;
};

struct CS_LOGOUT_PACKET {
	unsigned short size;
	char	type;
};

struct CS_ATTACK_PACKET {
	unsigned short size;
	char	type;
	char	atk_type;
};

struct CS_P_CREATE_PACKET {
	unsigned short size;
	char	type;
};

struct CS_P_JOIN_PACKET {
	unsigned short size;
	char	type;
	int		p_num;
};

struct CS_P_EXIT_PACKET {
	unsigned short size;
	char	type;
};


struct SC_LOGIN_INFO_PACKET {
	unsigned short size;
	char	type;
	int		id;
	int		hp;
	int		max_hp;
	int		exp;
	int		level;
	short	x, y;
};

struct SC_ADD_OBJECT_PACKET {
	unsigned short size;
	char	type;
	int		id;
	short	x, y;
	int		hp;
	char	name[NAME_SIZE];
};

struct SC_REMOVE_OBJECT_PACKET {
	unsigned short size;
	char	type;
	int		id;
};

struct SC_MOVE_OBJECT_PACKET {
	unsigned short size;
	char	type;
	int		id;
	short	x, y;
	int		hp;
	unsigned int move_time;
};

struct SC_CHAT_PACKET {
	unsigned short size;
	char	type;
	int		id;
	char	name[NAME_SIZE];
	char	mess[CHAT_SIZE];
};

struct SC_LOGIN_OK_PACKET {
	unsigned short size;
	char	type;
};

struct SC_LOGIN_FAIL_PACKET {
	unsigned short size;
	char	type;

};

struct SC_STAT_CHANGE_PACKET {
	unsigned short size;
	char	type;
	int		hp;
	int		max_hp;
	int		exp;
	int		level;
};

struct SC_DIE_OBJECT_PACKET {
	unsigned short size;
	char	type;
	int		id;
	int		exp;
};

struct SC_P_CREATE_PACKET {
	unsigned short size;
	char	type;
	int		id;
	int		p_id;
};

struct SC_P_JOIN_PACKET {
	unsigned short size;
	char	type;
	int		id;
	int		hp;
	int		max_hp;
	char	name[NAME_SIZE];
};

struct SC_P_STAT_PACKET {
	unsigned short size;
	char	type;
	int		id;
	int		hp;
};

struct SC_P_EXIT_PACKET {
	unsigned short size;
	char	type;
	int		id;
};

struct SC_P_ENTER_PACKET {
	unsigned short size;
	char	type;
	int		id;
	int		p_id;
};

#pragma pack (pop)