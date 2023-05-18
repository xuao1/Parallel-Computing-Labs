#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <queue>
#include <chrono>
#include <cstring>

using namespace std;
using namespace chrono;

struct NodeRequests {
	int d;
	int s;
	int counts; // 这里的counts是在这个排班的请求数
	// vector<int> aunts;
	bool aunts[200];
};

struct Node {
	int d;
	int s;
	int counts = 0; // 是可选的剩余值
	int chose; // 选择的阿姨
	// vector<int> aunts; // 可选的阿姨集合
	bool aunts[200];
};

int N, D, S; // N 个阿姨，D 个值班天数，每日轮班次数 S
int request[200][400][6]; // request[i][j][k] 第i个阿姨请求值第j天的第k轮班
int shifts[400][6]; // shifts[j][k]=i 第j天第k个班次排的是第i个阿姨
int counts[200]; // 第i个阿姨的排班数
int ans = -1;
int base;
int Top;

NodeRequests Requests[200][400];
Node Shifts[400][6];

// 先按照 request 尽可能安排
// 然后对于每个排班，视为结点，进行 CSP

void Update() {
	// 是否所有阿姨的排班都满足最低排班次数要求
	//for (int i = 0; i < N; i++) counts[i] = 0;
	//for (int j = 0; j < D; j++)
	//	for (int k = 0; k < S; k++)
	//		counts[Shifts[j][k].chose]++;
	for (int i = 0; i < N; i++) {
		if (counts[i] < base) {
			return;
		}
	}

	int cnt = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < D; j++) {
			for (int k = 0; k < S; k++) {
				if (request[i][j][k] == 1 && Shifts[j][k].chose == i) cnt++;
			}
		}
	}
	if (cnt > ans) {
		ans = cnt;
		cout << ans << endl;
		for (int j = 0; j < D; j++) {
			for (int k = 0; k < S; k++) {
				shifts[j][k] = Shifts[j][k].chose;
			}
		}
	}
}



// 共有两种DFS
// 先考虑满足所有带申请的班次，turn = 1
// 当所有带申请的班次都考虑完，进入第二种考虑，那就是不考虑任何申请，唯一目标就是搜索可行解
bool DFS1(int turn, int curans, int depth, int doneshifts) {
	if (turn == 0 && curans <= ans) return false;
	// cout << "curans = " << curans << endl;
	// cout << "turn = "<<turn<<"  depth = " << depth << endl;
	//bool flag_end = true; // 是否所有班次都被排班
	//for (int j = 0; j < D; j++)
	//	for (int k = 0; k < S; k++) {
	//		if (Shifts[j][k].chose < 0) {
	//			flag_end = false;
	//			if (Shifts[j][k].counts == 0) {
	//				return false; // 存在某个排班时间，未被排班并且没有可选的阿姨，那么一定无法满足，直接 return false 
	//			}
	//		}
	//	}
	//if (flag_end) {
	//	Update();
	//	if (curans == ans) cout << "YES!!!!!!!!!!!" << endl;
	//	return true;
	//}

	if (doneshifts == D * S) {
		Update();
		if (curans == ans) cout << "YES!!!!!!!!!!!" << endl;
		return true;
	}

	// 先找出当前未排班的、有申请的、申请数量最少的班次
	int tmp_d = 0, tmp_s = 0, tmp_c = 0;
	bool flag = false; // 是否找到符合条件的班次
	if (turn == 1) {
		for (int j = 0; j < D; j++)
			for (int k = 0; k < S; k++) {
				if (Shifts[j][k].chose == -1 && Requests[j][k].counts > 0) { // 所有有申请的且未排班的班次，而且之前没有搜索过
					if (flag == false) {
						flag = true;
						tmp_c = Requests[j][k].counts;
						tmp_d = Requests[j][k].d; tmp_s = Requests[j][k].s;
					}
					else if (Requests[j][k].counts < tmp_c) {
						tmp_c = Requests[j][k].counts;
						tmp_d = Requests[j][k].d; tmp_s = Requests[j][k].s;
					}
				}
			}
	}
	if (flag) {
		// 找到当前未排班，并且有申请的班次，有多个的话，选择的是其中可选阿姨数最少的那个
		// 先满足这个
		// cout << "tmp_d = " << tmp_d << " tmp_s = " << tmp_s << endl;
		Shifts[tmp_d][tmp_s].chose = -1;
		int aunt = -1;

		for (int i = 0; i < N; i++) {
			// 对于其可选择的阿姨，选择已经排班数最少的
			//aunt = -1;
			//tmp_c = D * S;
			//for (int i = 0; i < N; i++) {
			//	if (Shifts[tmp_d][tmp_s].aunts[i] == 1 && counts[i] < tmp_c) {
			//		aunt = i;
			//		tmp_c = counts[i];
			//	}
			//}
			//if (aunt == -1) break;
			if (counts[i] > Top) continue;
			if (Shifts[tmp_d][tmp_s].aunts[i] == 0) continue;
			if (Requests[tmp_d][tmp_s].aunts[i] == 0) continue;

			aunt = i;
			Shifts[tmp_d][tmp_s].chose = aunt;
			counts[aunt]++;
			bool flag_l = false;
			bool flag_r = false;
			bool flag_n = false;
			if (tmp_s > 0) {
				// auto it0 = find(Shifts[tmp_d][tmp_s - 1].aunts.begin(), Shifts[tmp_d][tmp_s - 1].aunts.end(), aunt);
				if (Shifts[tmp_d][tmp_s - 1].aunts[aunt] == 1) {
					if (Shifts[tmp_d][tmp_s - 1].counts <= 1) continue;
					flag_l = true;
					Shifts[tmp_d][tmp_s - 1].counts--;
					Shifts[tmp_d][tmp_s - 1].aunts[aunt] = 0;
				}
			}
			if (tmp_s < N - 1) {
				// auto it0 = find(Shifts[tmp_d][tmp_s + 1].aunts.begin(), Shifts[tmp_d][tmp_s + 1].aunts.end(), aunt);
				if (Shifts[tmp_d][tmp_s + 1].aunts[aunt] == 1) {
					if (Shifts[tmp_d][tmp_s + 1].counts <= 1) continue;
					flag_r = true;
					Shifts[tmp_d][tmp_s + 1].counts--;
					Shifts[tmp_d][tmp_s + 1].aunts[aunt] = 0;
				}
			}
			if (tmp_s == N - 1 && tmp_d < D - 1) {
				// auto it0 = find(Shifts[tmp_d][tmp_s + 1].aunts.begin(), Shifts[tmp_d][tmp_s + 1].aunts.end(), aunt);
				if (Shifts[tmp_d + 1][0].aunts[aunt] == 1) {
					if (Shifts[tmp_d + 1][0].counts <= 1) continue;
					flag_n = true;
					Shifts[tmp_d + 1][0].counts--;
					Shifts[tmp_d + 1][0].aunts[aunt] = 0;
				}
			}
			DFS1(1, curans + 1, depth + 1, doneshifts + 1); // 当前班次安排了一位阿姨，然后DFS
			Shifts[tmp_d][tmp_s].chose = -2;
			counts[aunt]--;
			if (flag_l) {
				Shifts[tmp_d][tmp_s - 1].counts++;
				Shifts[tmp_d][tmp_s - 1].aunts[aunt] = 1;
			}
			if (flag_r) {
				Shifts[tmp_d][tmp_s + 1].counts++;
				Shifts[tmp_d][tmp_s + 1].aunts[aunt] = 1;
			}
			if (flag_n) {
				Shifts[tmp_d + 1][0].counts++;
				Shifts[tmp_d + 1][0].aunts[aunt] = 1;
			}
		}
		// cout << "DFS(1) no aunt: " << endl;
		Shifts[tmp_d][tmp_s].chose = -2;
		DFS1(1, curans, depth + 1, doneshifts); // 当前班次未安排人，且之后该班次均视为无申请，继续DFS
		Shifts[tmp_d][tmp_s].chose = -1;
		// cout << "DFS(1) end" << aunt << endl;
		return false;
	}

	tmp_c = 0x3f3f3f3f;
	// 所有有申请的班次都考虑过了
	// 剩下的不需要再考虑“申请”，只需找到满足条件的解即可
	if (flag == false) {
		// 找到未排班的、可选阿姨数最少的班次
		for (int j = 0; j < D; j++)
			for (int k = 0; k < S; k++) {
				if (Shifts[j][k].chose < 0) { // 未排班的班次
					if (Shifts[j][k].counts < tmp_c) {
						tmp_c = Requests[j][k].counts;
						tmp_d = Requests[j][k].d; tmp_s = Requests[j][k].s;
					}
				}
			}
		int aunt = -1;
		for (int i = 0; i < N; i++) {
			if (counts[i] > Top) continue;
			if (Shifts[tmp_d][tmp_s].aunts[i] == 0) continue;
			aunt = i;
			Shifts[tmp_d][tmp_s].chose = aunt;
			counts[aunt]++;
			bool flag_l = false;
			bool flag_r = false;
			bool flag_n = false;
			if (tmp_s > 0) {
				// auto it0 = find(Shifts[tmp_d][tmp_s - 1].aunts.begin(), Shifts[tmp_d][tmp_s - 1].aunts.end(), aunt);
				if (Shifts[tmp_d][tmp_s - 1].aunts[aunt] == 1) {
					if (Shifts[tmp_d][tmp_s - 1].counts <= 1) continue;
					flag_l = true;
					Shifts[tmp_d][tmp_s - 1].counts--;
					Shifts[tmp_d][tmp_s - 1].aunts[aunt] = 0;
				}
			}
			if (tmp_s < N - 1) {
				// auto it0 = find(Shifts[tmp_d][tmp_s + 1].aunts.begin(), Shifts[tmp_d][tmp_s + 1].aunts.end(), aunt);
				if (Shifts[tmp_d][tmp_s + 1].aunts[aunt] == 1) {
					if (Shifts[tmp_d][tmp_s + 1].counts <= 1) continue;
					flag_r = true;
					Shifts[tmp_d][tmp_s + 1].counts--;
					Shifts[tmp_d][tmp_s + 1].aunts[aunt] = 0;
				}
			}
			if (tmp_s == N - 1 && tmp_d < D - 1) {
				// auto it0 = find(Shifts[tmp_d][tmp_s + 1].aunts.begin(), Shifts[tmp_d][tmp_s + 1].aunts.end(), aunt);
				if (Shifts[tmp_d + 1][0].aunts[aunt] == 1) {
					if (Shifts[tmp_d + 1][0].counts <= 1) continue;
					flag_n = true;
					Shifts[tmp_d + 1][0].counts--;
					Shifts[tmp_d + 1][0].aunts[aunt] = 0;
				}
			}
			DFS1(0, curans, depth + 1, doneshifts + 1); // 当前班次安排了一位阿姨，然后DFS
			Shifts[tmp_d][tmp_s].chose = -2; // 未排班，但是搜索了
			counts[aunt]--;
			if (flag_l) {
				Shifts[tmp_d][tmp_s - 1].counts++;
				Shifts[tmp_d][tmp_s - 1].aunts[aunt] = 1;
			}
			if (flag_r) {
				Shifts[tmp_d][tmp_s + 1].counts++;
				Shifts[tmp_d][tmp_s + 1].aunts[aunt] = 1;
			}
			if (flag_n) {
				Shifts[tmp_d + 1][0].counts++;
				Shifts[tmp_d + 1][0].aunts[aunt] = 1;
			}
		}
		return false;
	}
	return false;
}

int main()
{
	string inputfile;
	cin >> inputfile;
	ifstream input_file(inputfile);
	if (!input_file) {
		cerr << "Unable to open input file\n";
		exit(1);
	}
	string line;
	getline(input_file, line);
	istringstream iss1(line);
	getline(iss1, line, ',');
	N = stoi(line);
	getline(iss1, line, ',');
	D = stoi(line);
	getline(iss1, line, ',');
	S = stoi(line);
	cout << N << " " << D << " " << S << endl;
	// cin >> N >> D >> S;
	// N = 2; D = 2; S = 2;

	base = D * S / N;
	Top = base + D * S - base * N;
	for (int j = 0; j < D; j++)
		for (int k = 0; k < S; k++) {
			Shifts[j][k].d = j; Shifts[j][k].d = k;
			Shifts[j][k].chose = -1;
			Shifts[j][k].counts = N; // 可选阿姨数
			for (int i = 0; i < N; i++) {
				Shifts[j][k].aunts[i] = 1;
			}
		}
	for (int i = 0; i < N; i++)
		for (int j = 0; j < D; j++) {
			getline(input_file, line);
			istringstream iss2(line);
			for (int k = 0; k < S; k++) {
				Requests[j][k].d = j; Requests[j][k].s = k;
				getline(iss2, line, ',');
				request[i][j][k] = stoi(line);
				// request[i][j][k] = 0;
				// cin >> request[i][j][k];
				if (request[i][j][k]) {
					Requests[j][k].counts++;
					Requests[j][k].aunts[i] = 1;
				}
			}
		}

	input_file.close();
	// 两次搜索
	// 第一次先搜索有 request 的，先尽可能满足
	// 越少的阿姨申请的，优先满足
	// 第二次搜索剩下的
	// 目前暂不考虑第一轮这样选择后，第二轮选择失败
	DFS1(1, 0, 0, 0);
	cout << "ans = " << ans << endl;
	for (int j = 0; j < D; j++) {
		for (int k = 0; k < S; k++) {
			cout << shifts[j][k] << " ";
		}
		cout << endl;
	}
	return 0;
}