#include <iostream>
#include<list>

using namespace std;

// 最大容量
int MAX;

// 内存块的数据结构
// 将空闲分区以及分配区结构合并通过status进行区分
typedef struct MemoryBlock {
	int start;// 内存块起始地址
	int length;// 内存块长度  分区大小
	string status;// 内存块情况    已分配显示进程号，未分配显示空闲分区
	
	void set(int start, int length, string status) {
		this->start = start;
		this->length = length;
		this->status = status;
	}
	
	bool operator<(MemoryBlock b) {
		return this->start < b.start;
	}
} MemoryBlock;

// 首次适应的比较 按照地址递增进行排序
bool compare(MemoryBlock a, MemoryBlock b) {
	return a.start < b.start;
}

void Init(int MAX, list<MemoryBlock> &f, list<MemoryBlock> &w) {
//	//初始化空闲内存内存总大小;
	MemoryBlock f1;
	f1.set(0, 256, "free");
	f.push_back(f1);
//	f2.set(48, 20, "free");
//	f.push_back(f2);
//	f3.set(80, 30, "free");
//	f.push_back(f3);
//	// 已分配区
//	MemoryBlock m1, m2, m3, m4;
//	m1.set(0, 15, "job1");
//	w.push_back(m1);
//	m2.set(38, 10, "job2");
//	w.push_back(m2);
//	m3.set(68, 12, "job3");
//	w.push_back(m3);
//	m4.set(110, 10, "job4");
//	w.push_back(m4);
}

// 显示空闲分区表
void print_free(list<MemoryBlock> p) {
	p.sort(compare);
	cout << "空闲分区表:" << endl;
	cout << "起始地址\t" << "长度\t" << "状态" << endl;
	for (auto it = p.begin(); it != p.end(); it++) {
		cout << it->start << "\t\t" << it->length << "\t" << it->status << endl;
	}
	cout << endl;
	
}

//输出空闲分区和已分配分区信息，包括分区号、起始地址、分区大小、（进程号）等
void print(list<MemoryBlock> f, list<MemoryBlock> w) {
	cout << "内存分配情况:" << endl;
	cout << "起始地址\t" << "长度\t" << "状态" << endl;
	w.sort(compare);
	f.merge(w, compare);
	for (auto it = f.begin(); it != f.end(); it++) {
		cout << it->start << "\t\t" << it->length << "\t" << it->status << endl;
	}
	cout << endl;
	
}

/* 分配主存空间的函数，参数为：进程号，申请空间量
分配成功，返回分配区的起始地址，不成功：返回-1   */
int allocate(string job, int applySize, list<MemoryBlock> &f, list<MemoryBlock> &w) {
	// 按地址递增顺序对空闲分区进行遍历
	for (auto it = f.begin(); it != f.end(); ++it) {
		// 空闲分区能够进行内存分配
		if (applySize <= it->length) {
			// 添加到已工作分区
			MemoryBlock w1;
			int start_adr = it->start;
			w1.set(it->start, applySize, job);
			w.push_back(w1);
			// 修改空闲分区
			if (it->length - applySize > 0) {
				MemoryBlock f1;
				f1.set(it->start + applySize, it->length - applySize, "free");
				f.insert(it, f1);
			}
			f.erase(it);
			return start_adr;
		}
	}
	return -1;
}

//空闲分区表中的空闲区调整的函数
//按照不同的算法进行调整
//首先适应算法：按空闲区地址从小到大排列
//最佳适应算法，按空闲区大小从小到大排列
//最坏适应算法，按空闲区大小从大到小排列
void adjust(list<MemoryBlock> &f, list<MemoryBlock> &w, bool (*cmp)(MemoryBlock, MemoryBlock)) {
	string job;
	int size;
	char choice = 'y';
	while (choice == 'y') {
		if (f.size() == 0) {
			cout << "内存已满，分配失败";
			return;
		}
		int flag = -1;
		f.sort(cmp);
		// 对空闲分区进行排序并且显示
		print_free(f);
		cout << "请输入作业名称以及大小(任务名称不能重复):" << endl;
		cin >> job >> size;
		flag = allocate(job, size, f, w);
		if (flag != -1) {
			cout << "分配成功" << endl;
			print_free(f);
		} else {
			cout << "没有找到满足大小的空闲区，进行紧凑" << endl;
			// 进行紧凑，将空表项留在后面
			//首先对分配区进行排序，然后进行移位
			list<MemoryBlock> w_after;// 紧凑之后的分配区
			w.sort(cmp);
			int end_last = 0;
			for (auto it = w.begin(); it != w.end(); ++it) {
				if (it == w.begin()) {
					end_last = 0 + it->length;
					MemoryBlock w1;
					w1.set(0, it->length, it->status);
					w_after.push_back(w1);
				} else {
					MemoryBlock w2;
					w2.set(end_last, it->length, it->status);
					w_after.push_back(w2);
					end_last += it->length;
				}
			}
			// 空闲分区全部移出
			while (f.size() != 0) {
				f.pop_back();
			}
			// 空闲区分配
			MemoryBlock f1;
			f1.set(end_last, MAX - end_last, "free");
			f.push_back(f1);
			// 紧凑结束 再次进行分配
			
			int flag_after = -1;
			
			flag_after = allocate(job, size, f, w_after);
			if (flag_after != -1) {
				cout << "紧凑之后，分配成功" << endl;
				print_free(f);
			} else {
				cout << "紧凑后仍然分配失败，未找到满足大小的空闲区" << endl;
				return;
			}
			w = w_after;
		}
		cout << "是否要继续分配?选择(y/n):";
		cin >> choice;
	}
	f.sort(cmp);
	w.sort(cmp);
}

// 主存回收函数
void setfree(list<MemoryBlock> &f, list<MemoryBlock> &w) {
	string name;
	int start = 0;
	int length = 0;
	bool isPre = false;
	bool isNext = false;
	cout << "请输入要回收的作业名称:";
	cin >> name;
	for (auto it = w.begin(); it != w.end(); ++it) {
		if (it->status == name) {
			start = it->start;
			length = it->length;
			break;
		}
	}
	if (length == 0) {
		cout << "未找到作业号" << endl;
		return;
	}
	MemoryBlock f1;
	f1.set(start, length, "free");
	if (f.size() == 0) {
		f.insert(f.cbegin(), f1);
	} else {
		for (auto it = f.begin(); it != f.end(); ++it) {
			if (start + length < it->start) {
				if (!isNext)                 //前后不相邻
					f.insert(it, f1);
				break;                      // 仅与第一个空闲区后相邻
			} else if (start + length == it->start) {
				isPre = true;
				if (isPre && isNext) break;               //前后都相邻,在下边能进行判断
				it->start = start;                        //前相邻
				it->length += length;
				break;
			} else if (start == it->start + it->length) {
				isNext = true;                        //后相邻，多加一层判断
				it->length += length;                 //记录后相邻，判断是否与下一个分区前相邻，如果不是 break，如果是update
				auto it4 = next(it);
				if (it4 != f.end() && it4->start == it->length + it->start) {
					it->length += it4->length;
					f.erase(it4);
				}
				break;
			} else if (start > it->start + it->length) {     //要回收的块在it后面
				auto it4 = next(it);
				if (it4 != f.end()) continue;                  //如果下一个空闲区不为空，for进入下一轮
				else {
					f.insert(it, f1);                 //如果为空，在此处新建空闲区
					break;
				}
			}
		}
	}
	//删去工作块
	for (auto it = w.begin(); it != w.end(); ++it) {
		if (start == it->start) {
			w.erase(it);
			break;
		}
	}
	cout << "回收完成" << endl;
}

int main() {
	// 空闲分区
	list<MemoryBlock> f;
	// 已分配分区
	list<MemoryBlock> w;
	cout << "请输入内存容量" << endl;
	cin >> MAX;
	// 进行随机初始化
	Init(MAX, f, w);
	char t;
	cout << "========================" << endl;
	while (1) {
		cout << "请选择功能" << endl;
		cout << "p.分配内存" << endl;
		cout << "r.回收内存" << endl;
		cout << "l.查看内存分配情况" << endl;
		cout << "q.退出" << endl;
		cout << "========================" << endl;
		cout << "请输入您选择的功能序号:" << endl;
		cin >> t;
		switch (t) {
		case 'p':
			//进程申请内存空间
			//输入进程信息和内存申请量，判断是否合理
			//首次不合理时，进行紧凑再次判断
			adjust(f, w, compare); // 为进程分配内存
			print(f, w); // 输出分区信息
			break;
		case 'r':
			//作业回收
			//输入释放的进程号
			setfree(f, w); // 主存回收函数
			print(f, w); // 输出分区信息
			break;
		case 'l':
			print(f, w); // 输出分区信息
			break;
		case 'q':
			// 结束程序运行
			return 0;
			default:
				cout << "输入有误，请重新输入" << endl;
				break;
		}
	}
	return 0;
}

