#include <iostream>
#include <vector>
#include <string>
using namespace std;

enum Order {
    EARLIER,
    LATER,
    SAME,
    PARALLEL
};

string to_text(Order o) {
    if (o == EARLIER)  return "раньше";
    if (o == LATER)    return "позже";
    if (o == PARALLEL) return "параллельно";
    return "равно";
}

class IClock {
protected:
    int pid;
public:
    IClock(int p) : pid(p) {}
    virtual ~IClock() {}
    virtual void event() = 0;
};

class CounterClock : public IClock {
    int value = 0;
public:
    CounterClock(int p) : IClock(p) {}

    int get() const {
        return value;
    }

    void event() override {
        value = value + 1;
    }

    void merge(const CounterClock& other) {
        int m = value > other.value ? value : other.value;
        value = m + 1;
    }

    Order compare(const CounterClock& other) const {
        if (value < other.value) return EARLIER;
        if (value > other.value) return LATER;
        return SAME;
    }
};

class VecClock : public IClock {
    vector<int> stamp;
public:
    VecClock(int p, int n) : IClock(p), stamp(n, 0) {}

    void event() override {
        stamp[pid] = stamp[pid] + 1;
    }

    void merge(const VecClock& other) {
        for (int i = 0; i < (int)stamp.size(); i++) {
            if (other.stamp[i] > stamp[i]) {
                stamp[i] = other.stamp[i];
            }
        }
        stamp[pid] = stamp[pid] + 1;
    }

    Order compare(const VecClock& other) const {
        bool lower = false;
        bool higher = false;
        for (int i = 0; i < (int)stamp.size(); i++) {
            if (stamp[i] < other.stamp[i]) lower = true;
            if (stamp[i] > other.stamp[i]) higher = true;
        }
        if (lower && higher) return PARALLEL;
        if (lower) return EARLIER;
        if (higher) return LATER;
        return SAME;
    }

    friend ostream& operator<<(ostream& os, const VecClock& c) {
        os << "[";
        for (int i = 0; i < (int)c.stamp.size(); i++) {
            if (i > 0) os << ",";
            os << c.stamp[i];
        }
        os << "]";
        return os;
    }
};

int main() {
    CounterClock a(0);
    CounterClock b(1);

    a.event();
    a.event();
    b.event();

    cout << "CounterClock" << endl;
    cout << "  a = " << a.get() << endl;
    cout << "  b = " << b.get() << endl;
    cout << "  a/b: " << to_text(a.compare(b)) << endl;
    cout << "  b/a: " << to_text(b.compare(a)) << endl;

    CounterClock c(2);
    c.event();
    c.event();
    c.event();

    CounterClock d(3);
    d.merge(c);
    cout << "  c = " << c.get() << endl;
    cout << "  d.merge(c) = " << d.get() << endl;

    cout << endl;

    VecClock x(0, 3);
    VecClock y(1, 3);
    VecClock z(2, 3);

    x.event();
    x.event();
    y.event();

    cout << "VecClock" << endl;
    cout << "  x = " << x << endl;
    cout << "  y = " << y << endl;
    cout << "  x/y: " << to_text(x.compare(y)) << endl;

    z.merge(x);
    cout << "  z.merge(x) = " << z << endl;
    cout << "  x/z: " << to_text(x.compare(z)) << endl;
    cout << "  z/x: " << to_text(z.compare(x)) << endl;

    return 0;
}