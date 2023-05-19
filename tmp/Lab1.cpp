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


int N, D, S; // N 个阿姨，D 个值班天数，每日轮班次数 S
bool request[2400][200]; // 第 i 个班次，第 j 个阿姨是否申请 
int shifts[2400];
int counts[200];
int ans = -1;
int ans_shifts[2400];
int base;
int Top;
int resum[2400];
bool HaveRequests[2400];
int HavaRequestcnt;
int TotalRequestcnt;
int begin_index;

void Update(int cnt) {
	if (cnt <= ans) return;
	for (int i = 0; i < N; i++) {
		if (counts[i] < base) return;
	}
	// cout << "*********" << endl;
	ans = cnt;
	cout << ans << endl;
	for (int i = 0; i < D * S; i++)
		ans_shifts[i] = shifts[i];
}

void Search0(int index, int num, int pre, int cnt) {
	if (cnt + resum[num] <= ans) return;
	if (num == D * S) {
		if (cnt <= ans) return;
		else {
			begin_index = index;
			ans = cnt;
			return;
		}
	}
	if (num == 0) {
		for (int i = 0; i < N; i++) {
			if (counts[i] >= Top) continue;
			counts[i]++;
			if (request[num][i] == 1) Search0(i, num + 1, i, cnt + 1);
			else Search0(i, num + 1, i, cnt);
			counts[i]--;
		}
	}
	else {
		int i = (pre + 1) % N;
		counts[i]++;
		if (request[num][i] == 1) Search0(index, num + 1, i, cnt + 1);
		else Search0(index, num + 1, i, cnt);
		counts[i]--;
	}
}

void Search1(int num, int pre, int cnt) {
	if (cnt + resum[num] <= ans) return;
	if (num == D * S) {
		Update(cnt);
		return;
	}
	if (num == 0) {
		int i = begin_index;
		do {
			if (counts[i] >= Top) continue;
			counts[i]++;
			shifts[num] = i;
			if (request[num][i] == 1) Search1(num + 1, i, cnt + 1);
			else Search1(num + 1, i, cnt);
			counts[i]--;
			i = (i + 1) % N;
		} while (i != begin_index);
	}
	else {
		int i = (pre + 1) % N;
		while (i != pre) {
			// cout << num << " "<< i << endl;
			if (counts[i] >= Top) {
				i = (i + 1) % N;
				continue;
			}
			shifts[num] = i;
			counts[i]++;
			if (request[num][i] == 1) Search1(num + 1, i, cnt + 1);
			else Search1(num + 1, i, cnt);
			counts[i]--;
			i = (i + 1) % N;
		}
	}
}

int main()
{
	string inputfile;
	cin >> inputfile;
	cout << inputfile << endl;
	ifstream input_file(inputfile);
	if (!input_file) {
		cerr << "Unable to open input file\n";
		exit(1);
	}
	auto start = high_resolution_clock::now();
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
	base = D * S / N;
	Top = base + D * S - base * N;
	for (int i = 0; i < N; i++)
		for (int j = 0; j < D; j++) {
			getline(input_file, line);
			istringstream iss2(line);
			for (int k = 0; k < S; k++) {
				getline(iss2, line, ',');
				if (stoi(line) == 1) {
					request[j * S + k][i] = 1;
					HaveRequests[j * S + k] = 1;
					TotalRequestcnt++;
				}

			}
		}
	for (int i = D * S - 1; i >= 0; i--) {
		resum[i] = resum[i + 1];
		if (HaveRequests[i]) {
			HavaRequestcnt++;
			resum[i]++;
		}
	}

	input_file.close();
	cout << HavaRequestcnt << " " << TotalRequestcnt << endl;

	// ans = HavaRequestcnt - 1;
	Search0(0, -1, 0, -1);

	Search1(0, -1, 0);

	cout << "ans = " << ans << endl;
	for (int j = 0; j < D; j++) {
		for (int k = 0; k < S; k++) {
			cout << ans_shifts[j * S + k] << " ";
		}
		cout << endl;
	}
	auto end = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(end - start);
	cout << "Runtime: " << duration.count() << " ms" << endl;
	return 0;
}