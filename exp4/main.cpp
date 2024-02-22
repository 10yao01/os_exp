#include <iostream>
#include<list>

using namespace std;

// �������
int MAX;

// �ڴ������ݽṹ
// �����з����Լ��������ṹ�ϲ�ͨ��status��������
typedef struct MemoryBlock {
	int start;// �ڴ����ʼ��ַ
	int length;// �ڴ�鳤��  ������С
	string status;// �ڴ�����    �ѷ�����ʾ���̺ţ�δ������ʾ���з���
	
	void set(int start, int length, string status) {
		this->start = start;
		this->length = length;
		this->status = status;
	}
	
	bool operator<(MemoryBlock b) {
		return this->start < b.start;
	}
} MemoryBlock;

// �״���Ӧ�ıȽ� ���յ�ַ������������
bool compare(MemoryBlock a, MemoryBlock b) {
	return a.start < b.start;
}

void Init(int MAX, list<MemoryBlock> &f, list<MemoryBlock> &w) {
//	//��ʼ�������ڴ��ڴ��ܴ�С;
	MemoryBlock f1;
	f1.set(0, 256, "free");
	f.push_back(f1);
//	f2.set(48, 20, "free");
//	f.push_back(f2);
//	f3.set(80, 30, "free");
//	f.push_back(f3);
//	// �ѷ�����
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

// ��ʾ���з�����
void print_free(list<MemoryBlock> p) {
	p.sort(compare);
	cout << "���з�����:" << endl;
	cout << "��ʼ��ַ\t" << "����\t" << "״̬" << endl;
	for (auto it = p.begin(); it != p.end(); it++) {
		cout << it->start << "\t\t" << it->length << "\t" << it->status << endl;
	}
	cout << endl;
	
}

//������з������ѷ��������Ϣ�����������š���ʼ��ַ��������С�������̺ţ���
void print(list<MemoryBlock> f, list<MemoryBlock> w) {
	cout << "�ڴ�������:" << endl;
	cout << "��ʼ��ַ\t" << "����\t" << "״̬" << endl;
	w.sort(compare);
	f.merge(w, compare);
	for (auto it = f.begin(); it != f.end(); it++) {
		cout << it->start << "\t\t" << it->length << "\t" << it->status << endl;
	}
	cout << endl;
	
}

/* ��������ռ�ĺ���������Ϊ�����̺ţ�����ռ���
����ɹ������ط���������ʼ��ַ�����ɹ�������-1   */
int allocate(string job, int applySize, list<MemoryBlock> &f, list<MemoryBlock> &w) {
	// ����ַ����˳��Կ��з������б���
	for (auto it = f.begin(); it != f.end(); ++it) {
		// ���з����ܹ������ڴ����
		if (applySize <= it->length) {
			// ��ӵ��ѹ�������
			MemoryBlock w1;
			int start_adr = it->start;
			w1.set(it->start, applySize, job);
			w.push_back(w1);
			// �޸Ŀ��з���
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

//���з������еĿ����������ĺ���
//���ղ�ͬ���㷨���е���
//������Ӧ�㷨������������ַ��С��������
//�����Ӧ�㷨������������С��С��������
//���Ӧ�㷨������������С�Ӵ�С����
void adjust(list<MemoryBlock> &f, list<MemoryBlock> &w, bool (*cmp)(MemoryBlock, MemoryBlock)) {
	string job;
	int size;
	char choice = 'y';
	while (choice == 'y') {
		if (f.size() == 0) {
			cout << "�ڴ�����������ʧ��";
			return;
		}
		int flag = -1;
		f.sort(cmp);
		// �Կ��з���������������ʾ
		print_free(f);
		cout << "��������ҵ�����Լ���С(�������Ʋ����ظ�):" << endl;
		cin >> job >> size;
		flag = allocate(job, size, f, w);
		if (flag != -1) {
			cout << "����ɹ�" << endl;
			print_free(f);
		} else {
			cout << "û���ҵ������С�Ŀ����������н���" << endl;
			// ���н��գ����ձ������ں���
			//���ȶԷ�������������Ȼ�������λ
			list<MemoryBlock> w_after;// ����֮��ķ�����
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
			// ���з���ȫ���Ƴ�
			while (f.size() != 0) {
				f.pop_back();
			}
			// ����������
			MemoryBlock f1;
			f1.set(end_last, MAX - end_last, "free");
			f.push_back(f1);
			// ���ս��� �ٴν��з���
			
			int flag_after = -1;
			
			flag_after = allocate(job, size, f, w_after);
			if (flag_after != -1) {
				cout << "����֮�󣬷���ɹ�" << endl;
				print_free(f);
			} else {
				cout << "���պ���Ȼ����ʧ�ܣ�δ�ҵ������С�Ŀ�����" << endl;
				return;
			}
			w = w_after;
		}
		cout << "�Ƿ�Ҫ��������?ѡ��(y/n):";
		cin >> choice;
	}
	f.sort(cmp);
	w.sort(cmp);
}

// ������պ���
void setfree(list<MemoryBlock> &f, list<MemoryBlock> &w) {
	string name;
	int start = 0;
	int length = 0;
	bool isPre = false;
	bool isNext = false;
	cout << "������Ҫ���յ���ҵ����:";
	cin >> name;
	for (auto it = w.begin(); it != w.end(); ++it) {
		if (it->status == name) {
			start = it->start;
			length = it->length;
			break;
		}
	}
	if (length == 0) {
		cout << "δ�ҵ���ҵ��" << endl;
		return;
	}
	MemoryBlock f1;
	f1.set(start, length, "free");
	if (f.size() == 0) {
		f.insert(f.cbegin(), f1);
	} else {
		for (auto it = f.begin(); it != f.end(); ++it) {
			if (start + length < it->start) {
				if (!isNext)                 //ǰ������
					f.insert(it, f1);
				break;                      // �����һ��������������
			} else if (start + length == it->start) {
				isPre = true;
				if (isPre && isNext) break;               //ǰ������,���±��ܽ����ж�
				it->start = start;                        //ǰ����
				it->length += length;
				break;
			} else if (start == it->start + it->length) {
				isNext = true;                        //�����ڣ����һ���ж�
				it->length += length;                 //��¼�����ڣ��ж��Ƿ�����һ������ǰ���ڣ�������� break�������update
				auto it4 = next(it);
				if (it4 != f.end() && it4->start == it->length + it->start) {
					it->length += it4->length;
					f.erase(it4);
				}
				break;
			} else if (start > it->start + it->length) {     //Ҫ���յĿ���it����
				auto it4 = next(it);
				if (it4 != f.end()) continue;                  //�����һ����������Ϊ�գ�for������һ��
				else {
					f.insert(it, f1);                 //���Ϊ�գ��ڴ˴��½�������
					break;
				}
			}
		}
	}
	//ɾȥ������
	for (auto it = w.begin(); it != w.end(); ++it) {
		if (start == it->start) {
			w.erase(it);
			break;
		}
	}
	cout << "�������" << endl;
}

int main() {
	// ���з���
	list<MemoryBlock> f;
	// �ѷ������
	list<MemoryBlock> w;
	cout << "�������ڴ�����" << endl;
	cin >> MAX;
	// ���������ʼ��
	Init(MAX, f, w);
	char t;
	cout << "========================" << endl;
	while (1) {
		cout << "��ѡ����" << endl;
		cout << "p.�����ڴ�" << endl;
		cout << "r.�����ڴ�" << endl;
		cout << "l.�鿴�ڴ�������" << endl;
		cout << "q.�˳�" << endl;
		cout << "========================" << endl;
		cout << "��������ѡ��Ĺ������:" << endl;
		cin >> t;
		switch (t) {
		case 'p':
			//���������ڴ�ռ�
			//���������Ϣ���ڴ����������ж��Ƿ����
			//�״β�����ʱ�����н����ٴ��ж�
			adjust(f, w, compare); // Ϊ���̷����ڴ�
			print(f, w); // ���������Ϣ
			break;
		case 'r':
			//��ҵ����
			//�����ͷŵĽ��̺�
			setfree(f, w); // ������պ���
			print(f, w); // ���������Ϣ
			break;
		case 'l':
			print(f, w); // ���������Ϣ
			break;
		case 'q':
			// ������������
			return 0;
			default:
				cout << "������������������" << endl;
				break;
		}
	}
	return 0;
}

