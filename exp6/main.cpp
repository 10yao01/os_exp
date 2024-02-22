#include<iostream>

using namespace std;


#define OPEN_FILE_MAX    4    // 用户最多打开文件的个数
#define FILE_NAME_MAX    20
#define FILE_MAX 	10   //系统中可创建的文件数量最大值
#define FILE_BLOCK_MAX 	10   //文件磁盘块最大值

// 磁盘块的大小
const int block_size =  128;    // 磁盘块大小128字节
// 位示图相关变量
const int m = 8;    // 8个字
const int n = 16;   // 每个字16位


// 文件用户目录表,最多可创建文件10个(FILE_MAX)
struct  FCB {
	// 文件控制块
	int    file_descriptor;            // 文件标识符
	string   file_name;  // 文件名
	int    protected_mode;             // 1:可读；2:可写；3:可读写执行
	int    file_current_length;          // 文件当前长度（字节）
	int    file_max_length;            // 文件最大长度（字节）
	// 以下是文件存储空间分配时产生的值，不需要用户定义
	int    length;                     // 文件的块数
	int    block[FILE_BLOCK_MAX];   // 存放文件的具体盘块号

} UFD_table[FILE_MAX];// UFD表信息，文件目录结构


// 用户打开文件表 最多打开4(OPEN_FILE_MAX)个文件
struct  UOF {
	string file_name;
	int operate_mode;   // 打开后要执行的操作    1只读 2读写 3读写执行
	// 读写指针:指出文件的存取位置
	int read_pointer;   // 初始状态下为1
	int write_pointer;  // 初始状态下为文件长度
	int file_current_length; // 文件当前长度
	int file_max_length;    // 添加文件最大长度
	int file_path; // 文件所在位置 -1 为没有文件 1为有效
	int state; // 状态 指用户打开文件表的各个表项为空表目或被占用
	// -1 没有文件，1状态为建立/占用

} UOF_table[OPEN_FILE_MAX];       // 用户打开文件表


// 位示图
int map[m][n];                 //位示图
// 打开文件数
int C = 0;
// 空闲块的个数
int free_num = 16 * 8 - 16;


// 初始化
void init();
// 创建文件
int create(string name, int max_length, int protected_mode);
// 写文件
int write(string name, int length);
// 打开文件
int open(string name, int mode);
// 读文件
int read(string name, int length);
// 关闭文件
int close(string name);
// 删除文件
int delete_file(string name);
// 显示 UFD 和UOF
void print();
// 显示位示图
void print_map();


// 初始情况下第一个字为1，分配给操作系统
// 位示图，UFD，UOF
void init() {
	// 位示图初始化
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 16; j++) {
			// 分配操作系统文件
			// 第一个字为1
			if (i == 0) map[i][j] = 1;
			else map[i][j] = 0;
		}
	}
	// UFD初始化
	for (int ufd_id = 0; ufd_id < FILE_MAX; ufd_id++) {
		// UFD表的修改
		UFD_table[ufd_id].file_descriptor = -1;
		UFD_table[ufd_id].file_name = "";
		UFD_table[ufd_id].file_max_length = -1;
		UFD_table[ufd_id].file_current_length = -1;
		UFD_table[ufd_id].protected_mode = -1;
		int N = UFD_table[ufd_id].length;
		UFD_table[ufd_id].length = -1;
		for (int k = 0; k < N; k++) {
			UFD_table[ufd_id].block[k] = -1;
		}
	}
	// UOF初始化
	for (int uof_id = 0; uof_id < OPEN_FILE_MAX; uof_id++) {
		UOF_table[uof_id].file_name = "";
		UOF_table[uof_id].state = -1;
		UOF_table[uof_id].file_current_length = -1;
		UOF_table[uof_id].file_max_length = -1;
		UOF_table[uof_id].file_path = -1;
		UOF_table[uof_id].write_pointer = -1;
		UOF_table[uof_id].operate_mode = -1;
		UOF_table[uof_id].read_pointer = -1;
	}
}

void print() {
	cout << "=================UFD 文件目录结构=================" << endl;
	for (int i = 0; i < FILE_MAX; i++) {
		// 判断是否有文件
		if (UFD_table[i].file_name != "") {
			cout << "文件名" << UFD_table[i].file_name << "文件保护模式" << UFD_table[i].protected_mode << endl;
			cout << "文件当前长度" << UFD_table[i].file_current_length << "文件最大长度" << UFD_table[i].file_max_length << endl;
			cout << "文件标识符" << UFD_table[i].file_descriptor << "文件的块数为" << UFD_table[i].length << endl;
			int N = UFD_table[i].length;
			cout << "文件在磁盘的逻辑块号为 :";
			for (int j = 0; j < N; j++) {
				cout << UFD_table[i].block[j] << " ";
			}
			cout << endl << "--------------------------------------------------" << endl;
		}
	}
	cout << "=================UOF 用户打开文件表=================" << endl;
	cout << "当前已经打开" << C << "个文件" << endl;
	for (int i = 0; i < OPEN_FILE_MAX; i++) {
		if (UOF_table[i].state != -1) {
			cout << "文件名" << UOF_table[i].file_name << "文件打开后要执行的操作" << UOF_table[i].operate_mode << endl;
			cout << "文件的读指针位置" << UOF_table[i].read_pointer << "文件的写指针的位置" << UOF_table[i].write_pointer << endl;
			cout << "文件的当前长度" << UOF_table[i].file_current_length;
			cout << endl << "--------------------------------------------------" << endl;
		}
	}
}


int create(string name, int max_length, int protected_mode) {
	/*
	name:文件名
	max_length:文件最大字节长度
	protected_mode:文件保护方式
	return:
		创建成功，返回文件描述符，即在UOF表中的序号
		不成功，返回-1
	*/
	// 查询UFD是否有该文件
	for (int i = 0; i < FILE_MAX; i++) {
		if (name == UFD_table[i].file_name) {
			cout << "文件名重复，不能创建文件" << endl;
			return -1;
		}
	}
	// 查询打开文件数量
	if (C == OPEN_FILE_MAX) {
		cout << "打开文件数量过多，不能创建" << endl;
		return -1;
	}
	// 计算文件所需磁盘块的个数
	int N = (max_length - 1) / block_size + 1; // 向上取整
	if (N > free_num) {
		cout << "磁盘中的空闲块不足" << endl;
		return -1;
	} else if (N > FILE_BLOCK_MAX) {
		cout << "文件大小过大，超过一个文件所能容纳的最大盘块数" << endl;
		return -1;
	}
	// UFD
	int ufd_id = -1;
	for (int i = 0; i < FILE_MAX; i++) {
		if (UFD_table[i].file_name == "") {
			// 填上文件信息
			ufd_id = i;
			// 创建文件 UFD表
			UFD_table[i].file_name = name;
			UFD_table[i].file_max_length = max_length; // 字节
			UFD_table[i].file_current_length = 0; // 字节
			UFD_table[i].protected_mode = protected_mode;
			UFD_table[i].length = N;
			break;
		}
	}
	if (ufd_id == -1) {
		cout << "没有空闲的文件目录项" << endl;
		return -1;
	}
	// 位示图修改
	// 文件的盘块的记录
	bool ready = false;
	int k = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 16; j++) {
			if (map[i][j] == 0) {
				// 当前没有被分配，将本块分配给文件
				map[i][j] = 1;
				int re_adr = i * 16 + j;
				UFD_table[ufd_id].block[k] = re_adr;
				k++;
				//cout << "相对块号:" << re_adr << endl;
				//count_adr(re_adr);
				N --;
				free_num--;
				if (N == 0) {
					ready = true;
					break;
				}
			}
		}
		if (ready) break;
	}
	// UOF
	int uof_id;
	for (int i = 0; i < OPEN_FILE_MAX; i++) {
		if (UOF_table[i].state == -1) {
			uof_id = i;
			// 创建文件 UOF表
			UOF_table[i].file_name = name;
			UOF_table[i].file_current_length = 0;// 文件当前长度
			UOF_table[i].file_max_length = UFD_table[ufd_id].file_max_length;
			UOF_table[i].read_pointer = 0;// 初始状态0
			UOF_table[i].write_pointer = 0;//初始状态文件当前长度
			UOF_table[i].operate_mode = protected_mode; // 文件的打开方式通过输入确定
			UOF_table[i].file_path = 1;// 有文件
			UOF_table[i].state = 1;//建立
			break;
		}
	}
	UFD_table[ufd_id].file_descriptor = uof_id;
	C += 1;
	cout << "文件创建成功" << endl;
	return uof_id;
}

int write(string name, int length) {
	/*
	name:文件名
	length:要写的字节个数
	return:
		不成功:-1
		成功:返回写入字节个数
	*/
	// 查看UOF表中文件的状态
	int uof_id;
	for (int i = 0; i < OPEN_FILE_MAX; i++) {
		if (UOF_table[i].file_name == name) {
			uof_id = i;
			break;
		}
	}
	int ufd_id;
	for (int i = 0; i < FILE_MAX; i++) {
		if (UFD_table[i].file_name == name) {
			ufd_id = i;
			break;
		}
	}
	// 建立状态
	if (UOF_table[uof_id].state != 1) { // 非建立状态
		cout << "当前文件为非建立状态" << endl;
		return - 1;
	}
	if (UFD_table[ufd_id].protected_mode != 2 && UFD_table[ufd_id].protected_mode != 3) {
		cout << "文件不允许写" << endl;
		return -1;
	}
	if (UOF_table[uof_id].file_current_length + length > UOF_table[uof_id].file_max_length) {
		cout << "文件超过允许长度" << endl;
		return -1;
	}
	// 写内容
	// 依据磁盘块显写的内容
	cout << "把文件内容写到写指针指示的磁盘块中" << endl;
	UOF_table[uof_id].write_pointer += length;
	UOF_table[uof_id].file_current_length += length;
	// UFD_table[ufd_id].file_current_length += length;文件关闭之后进行修改
	cout << "写成功" << endl;
	return length;
}

int read(string name, int length) {
	/*
	name:文件名
	length:要读的字节数
	return:
		成功：返回读取的字节数
		不成功：-1
	*/
	int uof_id = -1;
	for (int i = 0; i < OPEN_FILE_MAX; i++) {
		if (UOF_table[i].file_name == name) {
			uof_id = i;
		}
	}
	if (uof_id == -1) {
		cout << "文件还未打开，不能读" << endl;
		return -1;
	}
	int ufd_id;
	for (int i = 0; i < FILE_MAX; i++) {
		if (UFD_table[i].file_name == name) {
			ufd_id = i;
		}
	}
	if (UFD_table[ufd_id].protected_mode < 1) {
		cout << "文件不可读" << endl;
		return -1;
	}
	// 这里应该是读指针加上要读取的长度 大于 文件当前长度 会出现读文件的问题
	if (UOF_table[uof_id].read_pointer + length > UOF_table[uof_id].file_current_length) {
		int read_length = UOF_table[uof_id].file_current_length - UOF_table[uof_id].read_pointer;
		if (read_length > 0) {
			UOF_table[uof_id].read_pointer += read_length;
		}
		cout << "超过文件长度，不允许再读" << endl;
		if (read_length > 0) {
			return read_length;
		} else {
			return -1;
		}
	}
	// 修改读指针
	UOF_table[uof_id].read_pointer += length;
	// 读取显示
	cout << "读取文件指定字节" << endl;
	cout << "读取成功" << endl;
	return length;
}

int open(string name, int mode) {
	/*
	name:文件名
	mode:对打开文件的操作方式
	return:
		打开成功，返回文件在uof表中的序号
		打开失败，返回-1
	*/
	// 查询打开文件数量
	// 查看UFD
	bool has_file = false;
	int ufd_id;
	for (int i = 0; i < FILE_MAX; i++) {
		if (UFD_table[i].file_name == name) {
			has_file = true;
			ufd_id = i;
			break;
		}
	}
	if (!has_file) {
		cout << "文件还未创建，不能打开" << endl;
		return -1;
	}
	if (C == OPEN_FILE_MAX) {
		cout << "打开文件数量过多，不能创建" << endl;
		return -1;
	}
	// 查看UOF
	for (int i = 0; i < OPEN_FILE_MAX; i++) {
		if (UOF_table[i].file_name == name) {
			cout << "文件已经打开，不允许重复打开" << endl;
			return -1;
		}
	}
	// 判断操作是否相符
	if (UFD_table[ufd_id].protected_mode < mode) {
		cout << "操作方式和文件规定的操作方式不符" << endl;
		return -1;
	}
	// 写入空UOF
	int uof_id;
	for (int i = 0; i < OPEN_FILE_MAX; i++) {
		if (UOF_table[i].state == -1) {
			uof_id = i;
			UOF_table[i].file_name = name;
			UOF_table[i].file_current_length = UFD_table[ufd_id].file_current_length;// 文件当前长度
			UOF_table[i].file_max_length = UFD_table[ufd_id].file_max_length;
			UOF_table[i].read_pointer = 0;// 初始状态0
			UOF_table[i].write_pointer = UFD_table[ufd_id].file_current_length;//初始状态文件当前长度
			UOF_table[i].operate_mode = mode;
			UOF_table[i].file_path = 1;// 有文件
			UOF_table[i].state = 1;//建立
			break;
		}
	}
	// 文件描述符
	UFD_table[ufd_id].file_descriptor = uof_id;
	C += 1;
	cout << "文件打开成功" << endl;
	return uof_id;
}

int close(string name) {
	/*
	name:文件名
	return:成功 0 不成功-1
	*/
	int uof_id = -1;
	for (int i = 0; i < OPEN_FILE_MAX; i++) {
		if (UOF_table[i].file_name == name) {
			uof_id = i;
			break;
		}
	}
	if (uof_id == -1) {
		cout << "文件没有打开" << endl;
		return -1;
	}
	int ufd_id;
	for (int i = 0; i < FILE_MAX; i++) {
		if (UFD_table[i].file_name == name) {
			ufd_id = i;
			break;
		}
	}
	// 根据UOF表完成对UFD表的修改
	// 修改文件当前长度
	UFD_table[ufd_id].file_current_length = UOF_table[uof_id].file_current_length;
	// 修改文件描述符（文件描述符指示当前文件在UOF的序号）
	UFD_table[ufd_id].file_descriptor = -1;
	// 将UOF表变为空表
	UOF_table[uof_id].file_name = "";
	UOF_table[uof_id].state = -1;
	UOF_table[uof_id].file_current_length = -1;
	UOF_table[uof_id].file_max_length = -1;
	UOF_table[uof_id].file_path = -1;
	UOF_table[uof_id].write_pointer = -1;
	UOF_table[uof_id].operate_mode = -1;
	UOF_table[uof_id].read_pointer = -1;
	// 打开文件数量
	C -= 1;
	cout << "关闭成功" << endl;
	return 0;
}

// 文件删除
int delete_file(string name) {
	/*
	name:文件名
	return:成功 0 不成功-1
	*/
	// 查询UFD是否有该文件
	int ufd_id = -1;
	for (int i = 0; i < FILE_MAX; i++) {
		if (name == UFD_table[i].file_name) {
			ufd_id = i;
		}
	}
	if (ufd_id == -1) {
		cout << "没有文件名匹配的文件，不能删除" << endl;
		return -1;
	}
	// 查询当前文件是否打开
	for (int i = 0; i < OPEN_FILE_MAX; i++) {
		if (UOF_table[i].file_name == name) {
			cout << "文件当前还在打开状态，请先关闭文件，在进行文件删除" << endl;
			return -1;
		}
	}
	// 位示图的修改
	// 磁盘块的回收
	for (int j = 0; j < UFD_table[ufd_id].length; j++) {
		// 相对块号计算位示图字位号
		int adr = UFD_table[ufd_id].block[j];
		int zi = adr / 16;
		int wei = adr % 16;
		// 相应位置0
		map[zi][wei] = 0;
		free_num ++;
	}
	// UFD表的修改
	UFD_table[ufd_id].file_descriptor = -1;
	UFD_table[ufd_id].file_name = "";
	UFD_table[ufd_id].file_max_length = -1;
	UFD_table[ufd_id].file_current_length = -1;
	UFD_table[ufd_id].protected_mode = -1;
	int N = UFD_table[ufd_id].length;
	UFD_table[ufd_id].length = -1;
	for (int k = 0; k < N; k++) {
		UFD_table[ufd_id].block[k] = -1;
	}
	return 0;
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

int main() {
	// 初始化 包括位示图 UFD UOF
	init();
	int t;
	while (1) {
		cout << "========================" << endl;
		cout << "请选择功能" << endl;
		cout << "0.显示UFD和UOF" << endl;
		cout << "1.创建文件" << endl;
		cout << "2.打开文件" << endl;
		cout << "3.写文件" << endl;
		cout << "4.读文件" << endl;
		cout << "5.删除文件" << endl;
		cout << "6.关闭文件" << endl;
		cout << "7.显示位示图" << endl;
		cout << "9.退出" << endl;
		cout << "========================" << endl;
		cout << "请输入您选择的功能序号:" << endl;
		cin >> t;
		switch (t) {
			case 0:
				print();
				break;
			case 1: {
				string name;
				int length;
				int mode;
				cout << "请输入要创建的文件名" << endl;
				cin >> name;
				cout << "请输入文件的最大字节长度" << endl;
				cin >> length;
				cout << "请输入文件的保护方式 \n 1:可读；2:可写；3:可读写执行" << endl;
				cin >> mode;
				int id = create(name, length, mode);
				if (id == -1) {
					cout << "创建失败" << endl;
				} else {
					cout << "文件在UOF表的序号为" << id << endl;
				}
				break;
			}
			case 2: {
				string name;
				int mode;
				cout << "请输入要打开的文件名" << endl;
				cin >> name;
				cout << "请输入操作方式( 1只读 2读写 3读写执行)" << endl;
				cin >> mode;
				int id = open(name, mode);
				if (id == -1) {
					cout << "打开失败" << endl;
				} else {
					cout << "打开的文件在UOF表的序号为" << id << endl;
				}
				break;
			}
			case 3: {
				string name;
				int length;
				cout << "请输入要写入的文件名" << endl;
				cin >> name;
				cout << "请输入要写的字节个数" << endl;
				cin >> length;
				int write_length = write(name, length);
				if (write_length == -1) {
					cout << "写失败" << endl;
				} else {
					cout << "成功写入" << write_length << "个字节" << endl;
				}
				break;
			}
			case 4: {
				string name;
				int length;
				cout << "请输入要读取的文件名" << endl;
				cin >> name;
				cout << "请输入读取的字节个数" << endl;
				cin >> length;
				int read_length = read(name, length);
				if (read_length == -1) {
					cout << "读失败" << endl;
				} else {
					cout << "成功读" << read_length << "个字节" << endl;
				}
				break;
			}
			case 5: {
				string name;
				cout << "请输入要删除的文件名" << endl;
				cin >> name;
				int _delete = delete_file(name);
				if (_delete == -1) cout << "文件删除失败" << endl;
				else cout << " 文件删除成功" << endl;
				break;
			}
			case 6: {
				string name;
				cout << "请输入要关闭的文件名" << endl;
				cin >> name;
				int _close = close(name);
				if (_close == -1) cout << "文件关闭失败" << endl;
				break;
			}
			case 7: {
				print_map();
				break;
			}
			case 9: {
				// 退出
				return 0;
			}
			default:
				cout << "输入有误，请重新输入" << endl;
				break;
		}
	}
	return 0;
}
