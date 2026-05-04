#pragma once
#include "Image.hpp"
#include "../utilities/RGBColor.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

class Bloom {
public:
    Bloom(float threshold = 0.5f, int radius = 14, float strength = 1.5f)
        : threshold(threshold), radius(radius), strength(strength) {}

    void apply(Image& img) {
        int w = img.hres;
        int h = img.vres;

        std::vector<std::vector<RGBColor>> glow(w, std::vector<RGBColor>(h, RGBColor(0,0,0)));

        for (int x = 0; x < w; x++) {
            for (int y = 0; y < h; y++) {
                RGBColor c = img.colors[x][y];

                // Only bloom pixels that are saturated neon pink
                // Neon pink:  high red, low green, medium-high blue
                // Reject white/warm pixels 

                float warmness = std::min(c.r, std::min(c.g, c.b)); // high = white/warm
                float pinkness = c.r - c.g;   // high = reddish/pink
                float blueness = c.b - c.g;   // high = blue

                // Pink must have SOME blue component — pure red (pumps) has c.b near 0
                bool is_neon_pink = (pinkness > 0.3f && c.r > threshold && c.b > 0.15f && warmness < 0.4f);
            
                if (is_neon_pink ) {
                    glow[x][y] = RGBColor(c.r, c.g, c.b);
                }
            }
        }

        // Gaussian kernel
        int ksize = radius * 2 + 1;
        std::vector<float> kernel(ksize);
        float sigma = radius / 2.0f;
        float sum = 0.0f;
        for (int i = 0; i < ksize; i++) {
            int d = i - radius;
            kernel[i] = std::exp(-(d * d) / (2.0f * sigma * sigma));
            sum += kernel[i];
        }
        for (auto& k : kernel) k /= sum;

        // Horizontal blur
        std::vector<std::vector<RGBColor>> tmp(w, std::vector<RGBColor>(h, RGBColor(0,0,0)));
        for (int x = 0; x < w; x++) {
            for (int y = 0; y < h; y++) {
                float r = 0, g = 0, b = 0;
                for (int k = 0; k < ksize; k++) {
                    int sx = std::max(0, std::min(w-1, x + k - radius));
                    r += glow[sx][y].r * kernel[k];
                    g += glow[sx][y].g * kernel[k];
                    b += glow[sx][y].b * kernel[k];
                }
                tmp[x][y] = RGBColor(r, g, b);
            }
        }

        // Vertical blur
        std::vector<std::vector<RGBColor>> blurred(w, std::vector<RGBColor>(h, RGBColor(0,0,0)));
        for (int x = 0; x < w; x++) {
            for (int y = 0; y < h; y++) {
                float r = 0, g = 0, b = 0;
                for (int k = 0; k < ksize; k++) {
                    int sy = std::max(0, std::min(h-1, y + k - radius));
                    r += tmp[x][sy].r * kernel[k];
                    g += tmp[x][sy].g * kernel[k];
                    b += tmp[x][sy].b * kernel[k];
                }
                blurred[x][y] = RGBColor(r, g, b);
            }
        }

        // Add glow back only onto dark/non-white areas so warm surfaces unaffected
        for (int x = 0; x < w; x++) {
            for (int y = 0; y < h; y++) {
                RGBColor& p = img.colors[x][y];
                RGBColor& gl = blurred[x][y];
                p = RGBColor(
                    std::min(1.0f, p.r + gl.r * strength),
                    std::min(1.0f, p.g + gl.g * strength),
                    std::min(1.0f, p.b + gl.b * strength)
                );
            }
        }
    }

private:
    float threshold;
    int   radius;
    float strength;
};