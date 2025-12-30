#include <cstdio>
#include <cmath>
#include <chrono>
struct vec4 {
    float x;
    float y;
    float z;
    float w;

    vec4(float x, float y, float z, float w): x(x), y(y), z(z), w(w) { }
    vec4():x(0.), y(0.), z(0.), w(0.) { };
};

struct vec2 {
    float x;
    float y;
    vec2 yx() const { return {y,x}; }
    vec4 xyyx() const { return {x,y,y,x}; }

    vec2(float x, float y): x(x), y(y) { }
    vec2():x(0.),y(0.) { };
};



vec2 operator*(const vec2& v, float s) { return {v.x * s, v.y * s}; }
vec2 operator*(float s, const vec2& v) { return v * s; }
vec2 operator+(const vec2& v, float s) { return {v.x + s, v.y + s}; }
vec2 operator/(const vec2& v, float s) { return {v.x / s, v.y / s}; }
vec2 operator*(const vec2& a, const vec2& b) { return {a.x * b.x, a.y * b.y}; }
vec2 operator+(const vec2& a, const vec2& b) { return {a.x + b.x, a.y + b.y}; }
vec2 operator-(const vec2& a, const vec2& b) { return {a.x - b.x, a.y - b.y}; }
float dot(const vec2& a, const vec2& b) { return a.x * b.x + a.y * b.y; }
vec2 abs(const vec2& v) { return {fabsf(v.x),fabsf(v.y)}; }
vec2& operator +=(vec2& a, const vec2& b) { a = a + b; return a; }
vec2& operator +=(vec2& a, float s) { a = a + s; return a; }
vec2 cos(const vec2& v) { return {cosf(v.x),cosf(v.y)}; }




vec4 sin(const vec4& v) { return {sinf(v.x), sinf(v.y),sinf(v.z),sinf(v.w)}; }
vec4 exp(const vec4& v) { return {expf(v.x), expf(v.y),expf(v.z),expf(v.w)}; }
vec4 tanh(const vec4& v) { return {tanhf(v.x),tanhf(v.y),tanhf(v.z),tanhf(v.w)}; }
vec4 operator*(const vec4& v, float s) { return {v.x * s, v.y * s,v.z * s,v.w * s}; }
vec4 operator*(float s, const vec4& v) { return v * s; }
vec4 operator+(const vec4& v, float s) { return {v.x + s, v.y + s,v.z + s,v.w + s}; }
vec4 operator+(const vec4& a, const vec4& b) { return {a.x + b.x, a.y + b.y,a.z + b.z,a.w + b.w}; }
vec4 operator-(float s, const vec4& v) { return {s - v.x,s - v.y,s - v.z,s - v.w}; }
vec4 operator/(const vec4& a, const vec4& b) { return {a.x / b.x, a.y / b.y, a.z / b.z,a.w / b.w}; }
vec4& operator +=(vec4& a, const vec4& b) { a = a + b; return a; }


int main(int argc, char* argv[]) {
    auto start = std::chrono::high_resolution_clock::now();
    char ppm_file[256];
    for(int i = 0; i < 100; ++i) {
        snprintf(ppm_file, sizeof(ppm_file), "output-%02d.ppm", i);
        FILE* f = fopen(ppm_file, "wb");
        if(f == nullptr) {
            printf("Failed to create file: %s\n", ppm_file);
            return -1;
        }
        int w = 16 * 60;
        int h = 9 * 60;

        fprintf(f, "P6\n%d %d\n255\n", w, h);
        vec2 r = {(float) w, (float) h};
        float t = (float) i / 60;
        for(int y = 0; y < h; ++y) {
            for(int x = 0; x < w; ++x) {
                vec4 o;
                vec2 FC = {(float) x,float(y)};
                vec2 p = (FC * 2. - r) / r.y, l, i, v = p * (l += 4. - 4. * fabsf(.7 - dot(p, p)));
                for(; i.y++ < 8.; o += (sin(v.xyyx()) + 1.) * fabsf(v.x - v.y))v += cos(v.yx() * i.y + i + t) / i.y + .7;
                o = tanh(5. * exp(l.x - 4. - p.y * vec4(-1., 1., 2., 0.)) / o);
                fputc(o.x * 255, f);
                fputc(o.y * 255, f);
                fputc(o.z * 255, f);
            }
        }
        fclose(f);
        printf("Generated %s\n", ppm_file);
    }

    auto end = std::chrono::high_resolution_clock::now();

    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    printf("Time spent %lds\n",elapsed.count());

    return 0;
}
