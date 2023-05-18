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

void Update(int cnt) {
	if (cnt <= ans) return;
	for (int i = 0; i < N; i++) {
		if (counts[i] < base) return;
	}
	ans = cnt;
	for (int i = 0; i < D * S; i++)
		ans_shifts[i] = shifts[i];
}

void Search(int num, int pre, int cnt) {
	// cout << "num = " << num << endl;
	if (num == D * S) {
		Update(cnt);
		return;
	}
	for (int i = 0; i < N; i++) {
		if (i == pre) continue;
		if (counts[i] >= Top) continue;
		if (request[num][i] == 0) continue;
		shifts[num] = i;
		counts[i]++;
		Search(num + 1, i, cnt + 1);
		counts[i]--;
	}
	for (int i = 0; i < N; i++) {
		if (i == pre) continue;
		if (counts[i] >= Top) continue;
		if (request[num][i] == 1) continue;
		shifts[num] = i;
		counts[i]++;
		Search(num + 1, i, cnt);
		counts[i]--;
	}
}

int main()
{
	string inputfile;
	cin >> inputfile;
	ifstream input_file("input1.txt");
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
				request[j * S + k][stoi(line)] = 1;
			}
		}
			
	input_file.close();
	
	Search(0, -1, 0);

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