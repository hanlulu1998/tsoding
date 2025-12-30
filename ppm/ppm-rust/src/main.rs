use std::fs::File;
use std::io::prelude::*;
use std::ops::{Add, AddAssign, Div, Mul, Sub};
use std::time::Instant;
#[derive(Debug, Clone, Copy)]
struct Vec2 {
    x: f32,
    y: f32,
}

impl Vec2 {
    #[inline]
    fn new(x: f32, y: f32) -> Vec2 {
        Vec2 { x, y }
    }

    #[inline]
    fn zero() -> Vec2 {
        Vec2 { x: 0.0, y: 0.0 }
    }

    #[inline]
    fn yx(&self) -> Vec2 {
        Vec2::new(self.y, self.x)
    }

    #[inline]
    fn xyyx(&self) -> Vec4 {
        Vec4::new(self.x, self.y, self.y, self.x)
    }
}

impl Mul for Vec2 {
    type Output = Vec2;

    #[inline]
    fn mul(self, rhs: Self) -> Self::Output {
        Vec2::new(self.x * rhs.x, self.y * rhs.y)
    }
}

impl Mul<f32> for Vec2 {
    type Output = Vec2;

    #[inline]
    fn mul(self, rhs: f32) -> Self::Output {
        Vec2::new(self.x * rhs, self.y * rhs)
    }
}

impl Mul<Vec2> for f32 {
    type Output = Vec2;

    #[inline]
    fn mul(self, rhs: Vec2) -> Self::Output {
        rhs * self
    }
}

impl Add for Vec2 {
    type Output = Vec2;
    #[inline]
    fn add(self, rhs: Self) -> Self::Output {
        Vec2::new(self.x + rhs.x, self.y + rhs.y)
    }
}

impl Add<f32> for Vec2 {
    type Output = Vec2;

    #[inline]
    fn add(self, rhs: f32) -> Self::Output {
        Vec2::new(self.x + rhs, self.y + rhs)
    }
}

impl Div<f32> for Vec2 {
    type Output = Vec2;

    #[inline]
    fn div(self, rhs: f32) -> Self::Output {
        Vec2::new(self.x / rhs, self.y / rhs)
    }
}

impl Sub for Vec2 {
    type Output = Vec2;

    #[inline]
    fn sub(self, rhs: Self) -> Self::Output {
        Vec2::new(self.x - rhs.x, self.y - rhs.y)
    }
}

impl AddAssign for Vec2 {
    #[inline]
    fn add_assign(&mut self, rhs: Self) {
        self.x = self.x + rhs.x;
        self.y = self.y + rhs.y;
    }
}

impl AddAssign<f32> for Vec2 {
    #[inline]
    fn add_assign(&mut self, rhs: f32) {
        self.x = self.x + rhs;
        self.y = self.y + rhs;
    }
}

#[inline]
fn dot(a: Vec2, b: Vec2) -> f32 {
    a.x * b.x + a.y * b.y
}

#[inline]
fn cos(v: Vec2) -> Vec2 {
    Vec2::new(v.x.cos(), v.y.cos())
}

#[derive(Debug, Clone, Copy)]
struct Vec4 {
    x: f32,
    y: f32,
    z: f32,
    w: f32,
}

impl Vec4 {
    #[inline]
    fn new(x: f32, y: f32, z: f32, w: f32) -> Vec4 {
        Vec4 { x, y, z, w }
    }

    #[inline]
    fn zero() -> Vec4 {
        Vec4 {
            x: 0.0,
            y: 0.0,
            z: 0.0,
            w: 0.0,
        }
    }
}

#[inline]
fn sin(v: Vec4) -> Vec4 {
    Vec4::new(v.x.sin(), v.y.sin(), v.z.sin(), v.w.sin())
}

#[inline]
fn exp(v: Vec4) -> Vec4 {
    Vec4::new(v.x.exp(), v.y.exp(), v.z.exp(), v.w.exp())
}

#[inline]
fn tanh(v: Vec4) -> Vec4 {
    Vec4::new(v.x.tanh(), v.y.tanh(), v.z.tanh(), v.w.tanh())
}

impl Mul<f32> for Vec4 {
    type Output = Vec4;

    #[inline]
    fn mul(self, rhs: f32) -> Self::Output {
        Vec4::new(self.x * rhs, self.y * rhs, self.z * rhs, self.w * rhs)
    }
}

impl Mul<Vec4> for f32 {
    type Output = Vec4;

    #[inline]
    fn mul(self, rhs: Vec4) -> Self::Output {
        rhs * self
    }
}

impl Div for Vec4 {
    type Output = Vec4;

    #[inline]
    fn div(self, rhs: Self) -> Self::Output {
        Vec4::new(
            self.x / rhs.x,
            self.y / rhs.y,
            self.z / rhs.z,
            self.w / rhs.w,
        )
    }
}

impl AddAssign for Vec4 {
    #[inline]
    fn add_assign(&mut self, rhs: Self) {
        self.x = self.x + rhs.x;
        self.y = self.y + rhs.y;
        self.z = self.z + rhs.z;
        self.w = self.w + rhs.w;
    }
}

impl Add for Vec4 {
    type Output = Vec4;

    #[inline]
    fn add(self, rhs: Self) -> Self::Output {
        Vec4::new(
            self.x + rhs.x,
            self.y + rhs.y,
            self.z + rhs.z,
            self.w + rhs.w,
        )
    }
}

impl Add<f32> for Vec4 {
    type Output = Vec4;

    #[inline]
    fn add(self, rhs: f32) -> Self::Output {
        Vec4::new(self.x + rhs, self.y + rhs, self.z + rhs, self.w + rhs)
    }
}

impl Sub<Vec4> for f32 {
    type Output = Vec4;

    #[inline]
    fn sub(self, rhs: Vec4) -> Self::Output {
        Vec4::new(self - rhs.x, self - rhs.y, self - rhs.z, self - rhs.w)
    }
}

fn main() {
    let timer = Instant::now();
    let w = 16 * 60;
    let h = 9 * 60;
    let header = format!("P6\n{} {}\n255\n", w, h);
    let r = Vec2::new(w as f32, h as f32);
    let mut buffer = Vec::<u8>::with_capacity((w * h * 3) as usize + header.as_bytes().len());
    for i in 0..300 {
        let file_name = format!("output-{:02}.ppm", i);
        let mut file = File::create(&file_name);

        if let Ok(ref mut file) = file {
            buffer.extend_from_slice(header.as_bytes());

            let t = i as f32 / 60.0;
            for y in 0..h {
                for x in 0..w {
                    let mut o = Vec4::zero();
                    let fc = Vec2::new(x as f32, y as f32);
                    let p = (fc * 2.0 - r) / r.y;
                    let mut l = Vec2::zero();
                    let mut i = Vec2::zero();

                    l += 4.0 - 4.0 * (0.7 - dot(p, p)).abs();
                    let mut v = p * l;
                    while i.y < 8.0 {
                        i.y += 1.0;
                        v += cos(v.yx() * i.y + i + t) / i.y + 0.7;
                        o += (sin(v.xyyx()) + 1.0) * (v.x - v.y).abs();
                    }
                    o = tanh(5.0 * exp(l.x - 4. - p.y * Vec4::new(-1.0, 1.0, 2.0, 0.0)) / o);

                    buffer.push((o.x * 255.0) as u8);
                    buffer.push((o.y * 255.0) as u8);
                    buffer.push((o.z * 255.0) as u8);
                }
            }
            file.write_all(&buffer).unwrap();
            buffer.clear();
            println!("Generated file: {}", &file_name);
        } else {
            println!("ERROR: failed to create file: {}", file_name);
        }
    }
    println!("Time spent: {} s", timer.elapsed().as_secs_f32());
}
