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

struct RankKey {
    long long avg;
    array<int,9> sc;
    string name;
};

struct RankCmp {
    bool operator()(const RankKey &a, const RankKey &b) const {
        if (a.avg != b.avg) return a.avg > b.avg; // higher avg first
        for (int i = 0; i < 9; ++i) {
            if (a.sc[i] != b.sc[i]) return a.sc[i] > b.sc[i];
        }
        return a.name < b.name; // lexicographically smaller first
    }
};

static inline RankKey make_key(const Student &s) {
    RankKey k;
    k.avg = avg_floor(s.score);
    for (int i = 0; i < 9; ++i) k.sc[i] = s.score[i];
    k.name = s.name;
    return k;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    unordered_map<string, Student> data;
    data.reserve(20000);

    bool started = false;

    set<RankKey, RankCmp> live_rank; // always up-to-date after START
    vector<string> snap_order;       // last flushed order of names
    unordered_map<string, int> snap_index; // name -> 1-based rank
    snap_index.reserve(20000);

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
            started = true;
            live_rank.clear();
            for (auto &p : data) live_rank.insert(make_key(p.second));
            snap_order.clear(); snap_order.reserve(live_rank.size());
            snap_index.clear(); snap_index.reserve(live_rank.size()*2+1);
            int r = 1;
            for (const auto &k : live_rank) { snap_order.push_back(k.name); snap_index[k.name] = r++; }
        } else if (cmd == "UPDATE") {
            string name; int code; int score;
            cin >> name >> code >> score;
            auto it = data.find(name);
            if (it == data.end()) {
                cout << "[Error]Update failed.\n";
                continue;
            }
            if (started) {
                // erase old key from live set
                RankKey oldk = make_key(it->second);
                auto pos = live_rank.find(oldk);
                if (pos != live_rank.end()) live_rank.erase(pos);
            }
            if (code >= 0 && code < 9) it->second.score[code] = score;
            if (started) {
                live_rank.insert(make_key(it->second));
            }
        } else if (cmd == "FLUSH") {
            snap_order.clear(); snap_order.reserve(live_rank.size());
            snap_index.clear(); snap_index.reserve(live_rank.size()*2+1);
            int r = 1;
            for (const auto &k : live_rank) { snap_order.push_back(k.name); snap_index[k.name] = r++; }
        } else if (cmd == "PRINTLIST") {
            for (size_t i = 0; i < snap_order.size(); ++i) {
                const string &name = snap_order[i];
                const Student &s = data.at(name);
                long long avg = avg_floor(s.score);
                cout << (i+1) << ' ' << s.name << ' ' << (s.gender=='M'?"male":"female")
                     << ' ' << s.cls << ' ' << avg << '\n';
            }
        } else if (cmd == "QUERY") {
            string name; cin >> name;
            if (data.find(name) == data.end() || snap_index.find(name) == snap_index.end()) {
                cout << "[Error]Query failed.\n";
                continue;
            }
            cout << "STUDENT " << name << " NOW AT RANKING " << snap_index[name] << '\n';
        } else if (cmd == "END") {
            break;
        }
    }
    return 0;
}
