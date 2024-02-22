#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<iostream>
#include<string.h>

const int N = 1000;  // 进程名最大容量
int n;               //进程数量

typedef   struct  pcb {	    //进程控制块定义
	int  pid;               //进程ID
	char  pname[N];		    //进程名
	int  runtime;			//运行时间
	int  arrivetime;		//到达时间
	int subtime;            //进队时间(记录进入就绪队列时间,用于优先级相同情况下的调度)
	char state;			    //进程状态
	int  priority;			//进程优先级
	int   finishtime;  		//进程结束时间
	int ex_time;            // 程序需要的执行时间
	struct  pcb  *next;	    //链接指针
}  PCB;
PCB  *head_input;		    //队列头指针
PCB  *head_run;			    //运行进程指针
unsigned long current = 0;  //记录系统当前时间的变量
void  inputprocess( );      //建立进程，输入进程数目N，输入各个进程信息，并建立链表;
void  printreadylist( );    //输出就绪队列
int   readyprocess( );	    //检查就绪队列并运行进程
void   runprocess( );       //运行进程的函数
int count_ready_list();     //计算当前就绪进程的个数
bool judge();               //判断是否所有进程执行结束
void print_result();        // 输出结果 计算周转时间

int main() {	//主函数
	//fp=open（"result.txt”, “w”）;
	current = 0;      // 当前时间
	inputprocess( );  // 初始化
	printreadylist();
	readyprocess();
	if(judge()){
		printf("=========================== \n");
		printf("当前所有进程执行结束，输出执行结果\n");
		printreadylist();
		print_result();
	}
	getch( ); // 读取字符
	//fclose(fp);
	return 0;
}
void  inputprocess( ) {
	//建立进程，输入进程数目N，输入各个进程信息，并建立链表;
	// 输入需要运行的进程数量n
	// 输入进程信息：进程名，运行时间，到达时间，优先级等信息
	printf("请输入进程数量");
	scanf("%d", &n);
	PCB *tail = NULL;
	for (int i = 0; i < n; i++) {
		PCB* p = (PCB*)malloc(sizeof(PCB));
		p->pid = i + 1;
		printf("请输入第%d个进程的名字：\n", i + 1);
		char str[N];
		scanf("%s", str);
		strcpy(p->pname, str);
		printf("请输入第%d个进程的运行时间: \n", i + 1);
		int run;
		scanf("%d", &run);
		p->runtime = run;
		p->ex_time = run;// 用于计算周转时间
		printf("请输入第%d个进程的到达时间: \n", i + 1);
		int arrive;
		scanf("%d", &arrive);
		p->arrivetime = arrive;
		p->subtime = arrive;
		printf("请输入第%d个进程的优先级: \n", i + 1);
		int pri;
		scanf("%d", &pri);
		p->priority = pri;
		// 创建后就处于就绪状态
		p->state = 'R';
		// 进程未结束运行
		p->finishtime = -1;
		//按照创建顺序将进程连接起来
		if (head_input == NULL) {
			head_input = p;
			tail = p;
			tail->next = NULL;
		} else {
			tail->next = p;
			tail = p;
			tail->next = NULL;
		}
	}
}

void printreadylist() {
	PCB* temp = head_input;
	int i = 0;
	while (temp != NULL) {
		printf("第%d进程", i + 1);
		printf("进程ID%d，进程名%s，运行时间%d，到达时间%d，完成时间%d，进程状态%c，进程优先级%d \n",
			temp->pid, temp->pname, temp->runtime, temp->arrivetime, temp->finishtime, temp->state, temp->priority);
		i++;
		temp = temp->next;
	}
}

int  readyprocess() {
	//检查就绪队列并运行进程
	//就绪队列为空时，结束运行
	int ready_list_num = count_ready_list();
	while((ready_list_num) != 0){
		//根据调度算法选择一个进程
		runprocess();
		printf("%d运行\n",head_run->pid);
		//修改进程运行状态
		head_run->state = 'W';
		//如果是时间片调度算法，则将时间减1
		head_run->runtime = head_run->runtime - 1;
		//当前运行时间加1
		current += 1;
		if(head_run->runtime == 0){
			// 进程处于完成状态
			head_run->state = 'C';
			printf("进程%d在%d完成 \n",head_run->pid,current);
			head_run->finishtime = current;
		}
		else{
			// 修改状态
			// 修改提交时间
			head_run->state = 'R';
			head_run->subtime = current;
		}
		//如果是FCFS调度算法，则该进程运行结束，将当前时间+运行时间
		//如果是优先级调度算法
		head_run->priority = head_run->priority - 1;
		//输出进程运行信息
		ready_list_num = count_ready_list();
	}
	return 1;
}

void runprocess() {
	// 检查就绪队列
	int max = -1;
	int max_time = 0;
	PCB* temp = head_input;
	// 首先判断就绪进程的个数选择优先级最大的
	while (temp != NULL) {
		if (temp->state == 'R' && temp->arrivetime <= current) {
			if (temp->priority > max) {
				max = temp->priority;
				head_run = temp;
				max_time = 1;
			}
			else if(temp->priority == max){
				max_time ++;
			}
		}
		temp = temp->next;
	}
	// 显示进程排序
	if(max_time == 1){
		printf("当前就绪队列中有%d个最大的优先级\n",max_time);
		printf("进程号为%d当前优先级为%d\n",head_run->pid,head_run->priority);
	}
	else{
		printf("当前就绪队列中有%d个相同且最大的优先级\n",max_time);
		printf("优先级为%d\n",head_run->priority);
		printf("根据提交时间对进程进行排序\n");
	}
	//当有最大优先级个数大于1，比较提交时间
	//选择最早提交的
	if(max_time > 1){
		// 判断所在就绪队列的顺序
		// 选择最早提交的进程
		int sub_min = 100;
		temp = head_input;
		while (temp != NULL) {
			if (temp->state == 'R' && temp->arrivetime <= current && temp->priority == max) {
				if(sub_min > temp->subtime){
					head_run = temp;
					sub_min = temp->subtime;
				}
				printf("%d进程在%d提交\n",temp->pid,temp->subtime);
			}
			temp = temp->next;
		}
	}
}

// 计算当前就绪进程的个数
int count_ready_list() {
	int num = 0;
	PCB* temp = head_input;
	while (temp != NULL) {
		if((temp->state == 'R') && temp->arrivetime <= current){
			num++;	
		}
		temp = temp->next;
	}
	return num;
}

// 判断所有进程是否全部完成
bool judge(){
	PCB* temp = head_input;
	int C_num = 0;
	while (temp != NULL) {
		if(temp->state == 'C'){
			C_num++;	
		}
		temp = temp->next;
	}
	return C_num == n;
}

//计算带权周转时间
void print_result(){
	double avg_t = 0;
	double avg_w_t = 0;
	PCB* temp = head_input;
	while (temp != NULL) {
		avg_t += (temp->finishtime - temp->arrivetime);
		avg_w_t += ((temp->finishtime - temp->arrivetime) / (double)temp->ex_time);
		temp = temp->next;
	}
	avg_t = double(avg_t / n);
	avg_w_t = double(avg_w_t / n);
	printf("平均带权周转时间%lf \n",avg_t);
	printf("平均带权带权周转时间%lf",avg_w_t);
}
