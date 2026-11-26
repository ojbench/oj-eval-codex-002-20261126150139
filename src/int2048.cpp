#include "include/int2048.h"

namespace sjtu {
void int2048::normalize() {
    while (!digits.empty() && digits.back() == 0) digits.pop_back();
    if (digits.empty()) negative = false;
}
int2048::int2048() = default;
int2048::int2048(const int2048 &) = default;
int2048 &int2048::operator=(const int2048 &) = default;
int2048::int2048(long long x) : negative(x < 0) {
    unsigned long long v = x < 0 ? 0ULL - static_cast<unsigned long long>(x) : x;
    while (v) { digits.push_back(v % base); v /= base; }
}
int2048::int2048(const std::string &s) { read(s); }
void int2048::read(const std::string &s) {
    digits.clear();
    negative = !s.empty() && s[0] == '-';
    int start = !s.empty() && (s[0] == '-' || s[0] == '+');
    for (int end = static_cast<int>(s.size()); end > start; end -= 4) {
        int begin = end - 4 > start ? end - 4 : start, v = 0;
        for (int i = begin; i < end; ++i) v = v * 10 + s[i] - '0';
        digits.push_back(v);
    }
    normalize();
}
void int2048::print() { std::cout << *this; }
std::ostream &operator<<(std::ostream &out, const int2048 &x) {
    if (x.digits.empty()) return out << '0';
    std::string s = x.negative ? "-" : "";
    s += std::to_string(x.digits.back());
    for (int i = static_cast<int>(x.digits.size()) - 2; i >= 0; --i) {
        int v = x.digits[i];
        s += char('0' + v / 1000); s += char('0' + v / 100 % 10);
        s += char('0' + v / 10 % 10); s += char('0' + v % 10);
    }
    return out << s;
}
std::istream &operator>>(std::istream &in, int2048 &x) {
    std::string s; if (in >> s) x.read(s); return in;
}
int int2048::compare_abs(const int2048 &a, const int2048 &b) {
    if (a.digits.size() != b.digits.size()) return a.digits.size() < b.digits.size() ? -1 : 1;
    for (int i = static_cast<int>(a.digits.size()) - 1; i >= 0; --i)
        if (a.digits[i] != b.digits[i]) return a.digits[i] < b.digits[i] ? -1 : 1;
    return 0;
}
bool operator==(const int2048 &a, const int2048 &b) { return a.negative == b.negative && a.digits == b.digits; }
bool operator!=(const int2048 &a, const int2048 &b) { return !(a == b); }
bool operator<(const int2048 &a, const int2048 &b) {
    if (a.negative != b.negative) return a.negative;
    int c = int2048::compare_abs(a, b); return a.negative ? c > 0 : c < 0;
}
bool operator>(const int2048 &a, const int2048 &b) { return b < a; }
bool operator<=(const int2048 &a, const int2048 &b) { return !(b < a); }
bool operator>=(const int2048 &a, const int2048 &b) { return !(a < b); }
int2048 int2048::operator+() const { return *this; }
int2048 int2048::operator-() const {
    int2048 r = *this; if (!digits.empty()) r.negative = !negative; return r;
}
int2048 &int2048::operator+=(const int2048 &b) {
    if (negative == b.negative) {
        size_t n = digits.size() > b.digits.size() ? digits.size() : b.digits.size();
        digits.resize(n, 0);
        int carry = 0;
        for (size_t i = 0; i < n; ++i) {
            int sum = digits[i] + (i < b.digits.size() ? b.digits[i] : 0) + carry;
            digits[i] = sum % base; carry = sum / base;
        }
        if (carry) digits.push_back(carry);
    } else {
        int cmp = compare_abs(*this, b);
        if (!cmp) { digits.clear(); negative = false; return *this; }
        if (cmp < 0) { int2048 r = b; r += *this; return *this = r; }
        int borrow = 0;
        for (size_t i = 0; i < digits.size(); ++i) {
            int v = digits[i] - (i < b.digits.size() ? b.digits[i] : 0) - borrow;
            borrow = v < 0; digits[i] = v + borrow * base;
        }
        normalize();
    }
    return *this;
}
int2048 &int2048::operator-=(const int2048 &b) { return *this += -b; }
int2048 &int2048::add(const int2048 &b) { return *this += b; }
int2048 &int2048::minus(const int2048 &b) { return *this -= b; }
int2048 add(int2048 a, const int2048 &b) { return a += b; }
int2048 minus(int2048 a, const int2048 &b) { return a -= b; }
int2048 operator+(int2048 a, const int2048 &b) { return a += b; }
int2048 operator-(int2048 a, const int2048 &b) { return a -= b; }
namespace {
void fft(std::vector<std::complex<double>> &a, bool inverse) {
    int n = a.size();
    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) { auto tmp = a[i]; a[i] = a[j]; a[j] = tmp; }
    }
    const double pi = std::acos(-1.0);
    std::vector<std::complex<double>> roots(n/2);
    for (int i = 0; i < n/2; ++i) {
        double angle = (inverse ? -2 : 2)*pi*i/n;
        roots[i] = {std::cos(angle),std::sin(angle)};
    }
    for (int len = 2; len <= n; len <<= 1) {
        for (int i = 0; i < n; i += len) {
            for (int j = 0; j < len / 2; ++j) {
                auto u = a[i + j], v = a[i + j + len / 2] * roots[j*(n/len)];
                a[i + j] = u + v; a[i + j + len / 2] = u - v;
            }
        }
    }
    if (inverse) for (auto &v : a) v /= n;
}
}
int2048 int2048::multiply_small(const int2048 &a, int b) {
    int2048 r; long long carry = 0;
    for (int v : a.digits) {
        carry += 1LL * v * b; r.digits.push_back(carry % base); carry /= base;
    }
    while (carry) { r.digits.push_back(carry % base); carry /= base; }
    r.normalize(); return r;
}
int2048 &int2048::operator*=(const int2048 &b) {
    int2048 r;
    if (digits.empty() || b.digits.empty()) return *this = r;
    size_t n = digits.size(), m = b.digits.size();
    if (n * m <= 4096 || n < 16 || m < 16) {
        r.digits.assign(n + m, 0);
        for (size_t i = 0; i < n; ++i) {
            long long carry = 0;
            for (size_t j = 0; j < m || carry; ++j) {
                long long v = r.digits[i+j] + carry + (j < m ? 1LL * digits[i] * b.digits[j] : 0);
                r.digits[i+j] = v % base; carry = v / base;
            }
        }
    } else {
        int size = 1;
        while (size < static_cast<int>(n + m)) size <<= 1;
        // Two real transforms share one complex FFT.
        std::vector<std::complex<double>> a(size), product(size);
        for (size_t i = 0; i < n; ++i) a[i].real(digits[i]);
        for (size_t i = 0; i < m; ++i) a[i].imag(b.digits[i]);
        fft(a, false);
        for (int i = 0; i < size; ++i) {
            auto reflected = std::conj(a[(size-i) % size]);
            product[i] = (a[i]+reflected) * (a[i]-reflected) * std::complex<double>(0, -0.25);
        }
        fft(product, true);
        long long carry = 0;
        for (size_t i = 0; i < n+m-1; ++i) {
            carry += static_cast<long long>(std::llround(product[i].real()));
            r.digits.push_back(carry % base); carry /= base;
        }
        while (carry) { r.digits.push_back(carry % base); carry /= base; }
    }
    r.negative = negative != b.negative; r.normalize(); return *this = r;
}
int2048 operator*(int2048 a, const int2048 &b) { return a *= b; }
int2048 int2048::shifted(const int2048 &a, int count) {
    if (a.digits.empty()) return a;
    int2048 r = a; r.digits.insert(r.digits.begin(), count, 0); return r;
}
int2048 int2048::high(const int2048 &a, int count) {
    int2048 r;
    if (count < static_cast<int>(a.digits.size())) r.digits.assign(a.digits.begin()+count, a.digits.end());
    return r;
}
void int2048::divide_school(const int2048 &a, const int2048 &b, int2048 &q, int2048 &r) {
    q = 0; r = 0;
    if (b.digits.size() == 1) {
        q = a; long long rem = 0;
        for (int i = static_cast<int>(a.digits.size())-1; i >= 0; --i) {
            rem = rem*base+a.digits[i]; q.digits[i] = rem/b.digits[0]; rem %= b.digits[0];
        }
        q.normalize(); r = rem; return;
    }
    q.digits.resize(a.digits.size());
    for (int i = static_cast<int>(a.digits.size())-1; i >= 0; --i) {
        r.digits.insert(r.digits.begin(), a.digits[i]); r.normalize();
        if (compare_abs(r,b) < 0) continue;
        int lo = 1, hi = base-1;
        while (lo < hi) {
            int mid = (lo+hi+1)/2;
            if (compare_abs(multiply_small(b,mid),r) <= 0) lo = mid; else hi = mid-1;
        }
        q.digits[i] = lo; r -= multiply_small(b,lo);
    }
    q.normalize();
}
int2048 int2048::reciprocal(const int2048 &v) {
    int n = v.digits.size();
    int2048 scale = shifted(int2048(1),2*n);
    if (n <= 32) { int2048 q,r; divide_school(scale,v,q,r); return q; }
    int m = (n+1)/2;
    int2048 x = shifted(reciprocal(high(v,n-m)),n-m);
    // Newton refinement at scale base^(2*n), followed by exact correction.
    for (int step = 0; step < 2; ++step) x = x+x-high(v*x*x,2*n);
    int2048 product = x*v;
    while (product > scale) { x -= 1; product -= v; }
    while (product+v <= scale) { x += 1; product += v; }
    return x;
}
void int2048::divide_abs(const int2048 &a, const int2048 &b, int2048 &q, int2048 &r) {
    if (compare_abs(a,b) < 0) { q = 0; r = a; return; }
    int n = b.digits.size();
    if (n < 64 || a.digits.size()-b.digits.size() < 32) { divide_school(a,b,q,r); return; }
    int factor = base/(b.digits.back()+1);
    int2048 divisor = multiply_small(b,factor), dividend = multiply_small(a,factor);
    int2048 inv = reciprocal(divisor);
    q = 0; r = 0;
    int blocks = (static_cast<int>(dividend.digits.size())+n-1)/n;
    for (int block = blocks-1; block >= 0; --block) {
        int2048 chunk;
        int end = (block+1)*n;
        if (end > static_cast<int>(dividend.digits.size())) end = dividend.digits.size();
        chunk.digits.assign(dividend.digits.begin()+block*n, dividend.digits.begin()+end);
        chunk.normalize(); r = shifted(r,n)+chunk;
        int2048 digit = high(r*inv,2*n);
        r -= digit*divisor;
        while (r >= divisor) { r -= divisor; digit += 1; }
        q = shifted(q,n)+digit;
    }
    int2048 unscaled, ignored;
    divide_school(r,int2048(factor),unscaled,ignored); r = unscaled;
}
int2048 &int2048::operator/=(const int2048 &b) {
    bool sign = negative != b.negative;
    int2048 a = *this, divisor = b, q, r; a.negative = divisor.negative = false;
    divide_abs(a,divisor,q,r);
    if (sign && !r.digits.empty()) q += 1;
    q.negative = sign && !q.digits.empty(); return *this = q;
}
int2048 &int2048::operator%=(const int2048 &b) {
    bool sign = negative != b.negative;
    int2048 a = *this, divisor = b, q, r; a.negative = divisor.negative = false;
    divide_abs(a,divisor,q,r);
    if (sign && !r.digits.empty()) r = divisor-r;
    r.negative = b.negative && !r.digits.empty(); return *this = r;
}
int2048 operator/(int2048 a, const int2048 &b) { return a /= b; }
int2048 operator%(int2048 a, const int2048 &b) { return a %= b; }
} // namespace sjtu
