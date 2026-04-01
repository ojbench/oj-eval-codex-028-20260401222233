#include <bits/stdc++.h>
using namespace std;

struct Student {
    string name;
    char gender; // 'M' or 'F'
    int cls;
    int score[9];
};

static inline long long avg_floor(const int sc[9]) {
    long long sum = 0;
    for (int i = 0; i < 9; ++i) sum += sc[i];
    return sum / 9; // floor
}

struct CmpSnapshot {
    const unordered_map<string, Student>* pdata;
    CmpSnapshot(const unordered_map<string, Student>* mp) : pdata(mp) {}
    bool operator()(const string &a, const string &b) const {
        if (a == b) return false;
        const Student &sa = pdata->at(a);
        const Student &sb = pdata->at(b);
        long long avga = avg_floor(sa.score);
        long long avgb = avg_floor(sb.score);
        if (avga != avgb) return avga > avgb;
        for (int i = 0; i < 9; ++i) {
            if (sa.score[i] != sb.score[i]) return sa.score[i] > sb.score[i];
        }
        return sa.name < sb.name;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    unordered_map<string, Student> data;
    data.reserve(20000);

    bool started = false;
    vector<string> rank_order; // snapshot order of names
    unordered_map<string, int> rank_index; // name -> 1-based rank
    rank_index.reserve(20000);

    string cmd;
    while (cin >> cmd) {
        if (cmd == "ADD") {
            string name; char gender; int cls; int sc[9];
            cin >> name >> gender >> cls;
            for (int i = 0; i < 9; ++i) cin >> sc[i];
            if (started) {
                cout << "[Error]Cannot add student now.\n";
                continue;
            }
            if (data.find(name) != data.end()) {
                cout << "[Error]Add failed.\n";
                continue;
            }
            Student s; s.name = name; s.gender = gender; s.cls = cls; for (int i=0;i<9;++i) s.score[i]=sc[i];
            data.emplace(name, std::move(s));
        } else if (cmd == "START") {
            // build initial ranking
            started = true;
            rank_order.clear(); rank_order.reserve(data.size());
            for (auto &p : data) rank_order.push_back(p.first);
            sort(rank_order.begin(), rank_order.end(), CmpSnapshot(&data));
            rank_index.clear(); rank_index.reserve(rank_order.size()*2+1);
            for (size_t i = 0; i < rank_order.size(); ++i) rank_index[rank_order[i]] = (int)i + 1;
        } else if (cmd == "UPDATE") {
            string name; int code; int score;
            cin >> name >> code >> score;
            auto it = data.find(name);
            if (it == data.end()) {
                cout << "[Error]Update failed.\n";
                continue;
            }
            if (code >= 0 && code < 9) {
                it->second.score[code] = score;
            }
        } else if (cmd == "FLUSH") {
            // recompute ranking snapshot based on current data
            rank_order.clear(); rank_order.reserve(data.size());
            for (auto &p : data) rank_order.push_back(p.first);
            sort(rank_order.begin(), rank_order.end(), CmpSnapshot(&data));
            rank_index.clear(); rank_index.reserve(rank_order.size()*2+1);
            for (size_t i = 0; i < rank_order.size(); ++i) rank_index[rank_order[i]] = (int)i + 1;
        } else if (cmd == "PRINTLIST") {
            // Output N lines in snapshot order; data content reflects current scores
            for (size_t i = 0; i < rank_order.size(); ++i) {
                const string &name = rank_order[i];
                const Student &s = data.at(name);
                long long avg = avg_floor(s.score);
                cout << (i+1) << ' ' << s.name << ' ' << (s.gender=='M'?"male":"female")
                     << ' ' << s.cls << ' ' << avg << '\n';
            }
        } else if (cmd == "QUERY") {
            string name; cin >> name;
            auto it = data.find(name);
            if (it == data.end() || rank_index.find(name) == rank_index.end()) {
                cout << "[Error]Query failed.\n";
                continue;
            }
            cout << "STUDENT " << name << " NOW AT RANKING " << rank_index[name] << '\n';
        } else if (cmd == "END") {
            break;
        } else {
            // Unknown command; per statement, inputs are valid
        }
    }
    return 0;
}
