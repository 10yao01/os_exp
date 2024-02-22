#include<iostream>
#include<vector>

using namespace std;

const int m = 8;    // 8个字
const int n = 16;   // 每个字16位

typedef struct File_Info {
	string name;         // 文件名
	int length;          // 文件长度
	int time;            // 创建时间
	vector<int> disk_id; // 文件分配的相对磁盘号

	void set(string _name, int _length, int _time) {
		this->name = _name;
		this->length = _length;
		this->time = _time;
	}

} File_Info;

vector<File_Info> File_list;   //文件基本信息表
int map[m][n];                 //位示图
int free_num = 16 * 8 - 16;    // 空闲块的个数

// 初始情况下第一个字为1，分配给操作系统
void init() {
	// 位示图初始化
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 16; j++) {
			// 分配操作系统文件
			if (i == 0) map[i][j] = 1;
			else map[i][j] = 0;
		}
	}
	// 文件基本信息表初始化
	File_Info file0;
	// 操作系统文件初始化
	file0.set("os", 16, 0);
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 16; j++) {
			// 位示图到相对地址的转换
			if (map[i][j] == 1) file0.disk_id.push_back(i * 16 + j);
		}
	}
	// 将操作系统文件添加到文件基本信息表
	File_list.push_back(file0);
}

// 输出位示图
void print_map() {
	cout << "==========位示图==========" << endl;
	cout << "磁盘中空闲块的个数为" << free_num << endl;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 16; j++) {
			cout << map[i][j] << "  ";
		}
		cout << endl;
	}
}

// 相对地址计算CHS
void count_adr(int adr) {
	//有8个柱面。每个柱面有4个磁道，每个磁道又划分成4个物理盘块
	int zhu_mian = adr / (4 * 4);
	int ci_dao = (adr % (4 * 4)) / 4;
	int pan_kuai = (adr % (4 * 4)) % 4;
	cout << "当前分配的磁盘块位于" << zhu_mian << "柱面" << ci_dao << "磁头" << pan_kuai << "扇区" << endl;
}

// 输出所有文件信息
void print_file() {
	if (File_list.size() == 1) {
		cout << "当前磁盘空间只有操作系统文件" << endl;
	} else {
		cout << "==========文件信息==========" << endl;
		// 这里不输出第一个字的文件即操作系统文件
		// 需要输出时，将i修改为1即可
		for (int i = 1; i < File_list.size(); i++) {
			cout << "------------------------------" << endl;
			cout << "文件名" << File_list[i].name << "文件长度" << File_list[i].length << "文件创建时间" << File_list[i].time << endl;
			for (int j = 0; j < File_list[i].disk_id.size(); j++) {
				cout << "磁盘块相对地址" << File_list[i].disk_id[j] << endl;
				count_adr(File_list[i].disk_id[j]);
			}
		}
	}
}

// 创建文件
void create_file() {
	string name;
	int length;
	int time;
	cout << "请输入文件名" << endl;
	cin >> name;
	cout << "请输入文件大小" << endl;
	cin >> length;
	cout << "请输入文件创建时间" << endl;
	cin >> time;
	// 文件名不能重复
	bool name_confict = false;
	for (int k = 0; k < File_list.size(); k++) {
		if (name == File_list[k].name) {
			name_confict = true;
			break;
		}
	}
	if (name_confict) {
		cout << "文件名冲突，请重新操作" << endl;
		return;
	}
	if(length > free_num){
		cout << "磁盘中的空闲块不足" << endl;
		return;
	}
	File_Info file;
	file.set(name, length, time);
	// 为文件分配length个盘块
	// 位示图修改
	bool ready = false;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 16; j++) {
			if (map[i][j] == 0) {
				// 当前没有被分配，将本块分配给文件
				map[i][j] = 1;
				int re_adr = i * 16 + j;
				file.disk_id.push_back(re_adr);
				cout << "相对块号:" << re_adr << endl;
				count_adr(re_adr);
				length --;
				free_num--;
				if (length == 0) {
					ready = true;
					break;
				}
			}
		}
		if (ready) break;
	}
	if (ready) {
		// 文件基本信息表修改
		File_list.push_back(file);
	} else {
		cout << "文件过大，无法分配磁盘空间" << endl;
	}
}

// 释放文件的磁盘空间
void re_file() {
	string name;
	cout << "请输入文件名，输入all将所有文件清空，仅保留操作系统文件" << endl;
	cin >> name;
	// 清空所有文件
	if (name == "all") {
		// 位示图
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 16; j++) {
				if (i == 0) map[i][j] = 1;
				else map[i][j] = 0;
			}
		}
		// 文件信息表
		File_list.clear();
		File_Info file0;
		// 操作系统文件初始化
		file0.set("os", 16, 0);
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 16; j++) {
				// 位示图到相对地址的转换
				if (map[i][j] == 1) file0.disk_id.push_back(i * 16 + j);
			}
		}
		// 将操作系统文件添加到文件基本信息表
		File_list.push_back(file0);
		// 磁盘中空闲块的个数
		free_num = 16 * 8 - 16;
	} else {
		// 查找匹配的文件名
		int i;
		bool flag = false;
		for (i = 0; i < File_list.size(); i++) {
			if (File_list[i].name == name) {
				flag = true;
				break;
			}
		}
		if (!flag) {
			cout << "没有找到匹配的文件名" << endl;
			return;
		}
		// 进行文件磁盘空间的释放
		// 位示图的处理
		for (int j = 0; j < File_list[i].disk_id.size(); j++) {
			// 相对块号计算位示图字位号
			int adr = File_list[i].disk_id[j];
			int zi = adr / 16;
			int wei = adr % 16;
			// 相应位置0
			map[zi][wei] = 0;
			free_num ++;
		}
		// 文件基本信息表的处理
		File_list.erase(File_list.begin() + i);
	}
}


int main() {
	// 初始化 包括位示图以及文件信息表
	init();
	int t;
	while (1) {
		cout << "========================" << endl;
		cout << "请选择功能" << endl;
		cout << "1.分配磁盘空间" << endl;
		cout << "2.释放空闲空间" << endl;
		cout << "3.查看位示图" << endl;
		cout << "4.查看所有文件信息" << endl;
		cout << "5.查看空闲块的个数" << endl;
		cout << "0.退出" << endl;
		cout << "========================" << endl;
		cout << "请输入您选择的功能序号:" << endl;
		cin >> t;
		switch (t) {
			case 1:
				// 创建文件
				create_file();
				print_map();
				break;
			case 2:
				// 删除文件
				re_file();
				print_map();
				break;
			case 3:
				// 输出位示图
				print_map();
				break;
			case 4:
				// 输出文件信息表
				print_file();
				break;
			case 5:
				cout << "空闲块的个数为" << free_num << endl;
				break;
			case 0:
				// 结束程序运行
				return 0;
			default:
				cout << "输入有误，请重新输入" << endl;
				break;
		}
	}
	return 0;
}

