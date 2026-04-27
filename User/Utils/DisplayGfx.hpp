#pragma once

#include <stddef.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Math.hpp"

namespace Display {

struct Point {
    int16_t x = 0;
    int16_t y = 0;
};

struct Rect {
    int16_t x = 0;
    int16_t y = 0;
    uint16_t w = 0;
    uint16_t h = 0;
};

FORCE_INLINE constexpr int16_t RectRight(const Rect& rect) noexcept {
    return static_cast<int16_t> (rect.x + rect.w);
}

FORCE_INLINE constexpr int16_t RectBottom(const Rect& rect) noexcept {
    return static_cast<int16_t> (rect.y + rect.h);
}

FORCE_INLINE constexpr bool RectEmpty(const Rect& rect) noexcept {
    return rect.w == 0 || rect.h == 0;
}

FORCE_INLINE constexpr bool RectIntersects(const Rect& a, const Rect& b) noexcept {
    return a.x < RectRight (b) && RectRight (a) > b.x &&
           a.y < RectBottom (b) && RectBottom (a) > b.y;
}

FORCE_INLINE constexpr bool RectTouches(const Rect& a, const Rect& b) noexcept {
    return a.x <= RectRight (b) && RectRight (a) >= b.x &&
           a.y <= RectBottom (b) && RectBottom (a) >= b.y;
}

FORCE_INLINE constexpr Rect RectUnion(const Rect& a, const Rect& b) noexcept {
    const int16_t x0 = a.x < b.x ? a.x : b.x;
    const int16_t y0 = a.y < b.y ? a.y : b.y;
    const int16_t x1 = RectRight (a) > RectRight (b) ? RectRight (a) : RectRight (b);
    const int16_t y1 = RectBottom (a) > RectBottom (b) ? RectBottom (a) : RectBottom (b);
    return Rect{x0, y0, static_cast<uint16_t> (x1 - x0), static_cast<uint16_t> (y1 - y0)};
}

template <size_t MaxRects = 8>
class DirtyRects {
  public:
    void Clear() {
        count_ = 0;
    }

    bool Empty() const {
        return count_ == 0;
    }

    size_t Count() const {
        return count_;
    }

    const Rect& operator[](size_t index) const {
        return rects_[index];
    }

    void Add(Rect rect) {
        if (RectEmpty (rect)) return;

        size_t i = 0;
        while (i < count_) {
            if (RectTouches (rects_[i], rect)) {
                rect = RectUnion (rects_[i], rect);
                RemoveAt (i);
                i = 0;
                continue;
            }
            ++i;
        }

        if (count_ < MaxRects) {
            rects_[count_++] = rect;
            return;
        }

        size_t best_index = 0;
        uint32_t best_cost = 0xFFFFFFFFu;
        for (size_t idx = 0; idx < count_; ++idx) {
            const Rect merged = RectUnion (rects_[idx], rect);
            const uint32_t old_area = static_cast<uint32_t> (rects_[idx].w) * rects_[idx].h;
            const uint32_t new_area = static_cast<uint32_t> (merged.w) * merged.h;
            const uint32_t cost = new_area - old_area;
            if (cost < best_cost) {
                best_cost = cost;
                best_index = idx;
            }
        }
        rects_[best_index] = RectUnion (rects_[best_index], rect);
        MergeAll();
    }

  private:
    void RemoveAt(size_t index) {
        if (index >= count_) return;
        for (size_t i = index + 1; i < count_; ++i) {
            rects_[i - 1] = rects_[i];
        }
        --count_;
    }

    void MergeAll() {
        for (size_t i = 0; i < count_; ++i) {
            size_t j = i + 1;
            while (j < count_) {
                if (RectTouches (rects_[i], rects_[j])) {
                    rects_[i] = RectUnion (rects_[i], rects_[j]);
                    RemoveAt (j);
                    j = i + 1;
                    continue;
                }
                ++j;
            }
        }
    }

    Rect rects_[MaxRects] = {};
    size_t count_ = 0;
};

namespace Color565 {
static constexpr uint16_t Black = 0x0000;
static constexpr uint16_t White = 0xFFFF;
static constexpr uint16_t Red   = 0xF800;
static constexpr uint16_t Green = 0x07E0;
static constexpr uint16_t Blue  = 0x001F;
static constexpr uint16_t Yellow = 0xFFE0;
static constexpr uint16_t Cyan   = 0x07FF;
static constexpr uint16_t Magenta = 0xF81F;

FORCE_INLINE constexpr uint16_t Pack(uint8_t r, uint8_t g, uint8_t b) noexcept {
    return static_cast<uint16_t> (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

FORCE_INLINE constexpr uint8_t R(uint16_t color) noexcept { return static_cast<uint8_t> ((color >> 8) & 0xF8); }
FORCE_INLINE constexpr uint8_t G(uint16_t color) noexcept { return static_cast<uint8_t> ((color >> 3) & 0xFC); }
FORCE_INLINE constexpr uint8_t B(uint16_t color) noexcept { return static_cast<uint8_t> ((color << 3) & 0xF8); }

FORCE_INLINE uint16_t Lerp(uint16_t from, uint16_t to, float t) noexcept {
    const uint8_t r = Math::lerp<uint8_t> (R (from), R (to), t);
    const uint8_t g = Math::lerp<uint8_t> (G (from), G (to), t);
    const uint8_t b = Math::lerp<uint8_t> (B (from), B (to), t);
    return Pack (r, g, b);
}
}  // namespace Color565

template <typename TPanel>
class Surface {
  public:
    FORCE_INLINE bool Contains(int16_t x, int16_t y) const noexcept {
        return x >= 0 && y >= 0 && x < static_cast<int16_t> (Self().Width()) && y < static_cast<int16_t> (Self().Height());
    }

    Rect ClipRectToSurface(const Rect& rect) const noexcept {
        if (RectEmpty (rect)) return {};

        const int16_t x0 = rect.x < 0 ? 0 : rect.x;
        const int16_t y0 = rect.y < 0 ? 0 : rect.y;
        const int16_t x1 = RectRight (rect) > static_cast<int16_t> (Self().Width()) ? static_cast<int16_t> (Self().Width()) : RectRight (rect);
        const int16_t y1 = RectBottom (rect) > static_cast<int16_t> (Self().Height()) ? static_cast<int16_t> (Self().Height()) : RectBottom (rect);

        if (x0 >= x1 || y0 >= y1) return {};
        return {x0, y0, static_cast<uint16_t> (x1 - x0), static_cast<uint16_t> (y1 - y0)};
    }

    bool ClipRectToSurface(int16_t& x, int16_t& y, uint16_t& w, uint16_t& h) const noexcept {
        const Rect clipped = ClipRectToSurface ({x, y, w, h});
        if (RectEmpty (clipped)) return false;
        x = clipped.x;
        y = clipped.y;
        w = clipped.w;
        h = clipped.h;
        return true;
    }

    void Clear(uint16_t color) {
        Self().FillRectFast (0, 0, Self().Width(), Self().Height(), color);
    }

    void DrawHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
        if (w <= 0 || y < 0 || y >= static_cast<int16_t> (Self().Height())) return;
        if (x < 0) {
            w += x;
            x = 0;
        }
        const int16_t max_w = static_cast<int16_t> (Self().Width()) - x;
        if (x >= static_cast<int16_t> (Self().Width()) || max_w <= 0) return;
        if (w > max_w) w = max_w;
        Self().FillRectFast (x, y, static_cast<uint16_t> (w), 1, color);
    }

    void DrawVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
        if (h <= 0 || x < 0 || x >= static_cast<int16_t> (Self().Width())) return;
        if (y < 0) {
            h += y;
            y = 0;
        }
        const int16_t max_h = static_cast<int16_t> (Self().Height()) - y;
        if (y >= static_cast<int16_t> (Self().Height()) || max_h <= 0) return;
        if (h > max_h) h = max_h;
        Self().FillRectFast (x, y, 1, static_cast<uint16_t> (h), color);
    }

    void FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        if (w <= 0 || h <= 0) return;
        if (x < 0) {
            w += x;
            x = 0;
        }
        if (y < 0) {
            h += y;
            y = 0;
        }
        if (w <= 0 || h <= 0) return;
        if (x >= static_cast<int16_t> (Self().Width()) || y >= static_cast<int16_t> (Self().Height())) return;
        if (x + w > static_cast<int16_t> (Self().Width())) w = static_cast<int16_t> (Self().Width()) - x;
        if (y + h > static_cast<int16_t> (Self().Height())) h = static_cast<int16_t> (Self().Height()) - y;
        Self().FillRectFast (x, y, static_cast<uint16_t> (w), static_cast<uint16_t> (h), color);
    }

    void DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        if (w <= 0 || h <= 0) return;
        DrawHLine (x, y, w, color);
        DrawHLine (x, y + h - 1, w, color);
        DrawVLine (x, y, h, color);
        DrawVLine (x + w - 1, y, h, color);
    }

    void DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
        int16_t dx = Math::abs<int16_t> (x1 - x0);
        const int16_t sx = x0 < x1 ? 1 : -1;
        int16_t dy = -Math::abs<int16_t> (y1 - y0);
        const int16_t sy = y0 < y1 ? 1 : -1;
        int16_t err = dx + dy;

        while (true) {
            if (Contains (x0, y0)) Self().DrawPixel (x0, y0, color);
            if (x0 == x1 && y0 == y1) break;
            const int16_t e2 = static_cast<int16_t> (err << 1);
            if (e2 >= dy) {
                err += dy;
                x0 += sx;
            }
            if (e2 <= dx) {
                err += dx;
                y0 += sy;
            }
        }
    }

    void DrawCircle(int16_t x0, int16_t y0, int16_t radius, uint16_t color) {
        if (radius < 0) return;
        int16_t x = radius;
        int16_t y = 0;
        int16_t err = 1 - radius;

        while (x >= y) {
            PlotCirclePoints (x0, y0, x, y, color);
            ++y;
            if (err < 0) {
                err += 2 * y + 1;
            } else {
                --x;
                err += 2 * (y - x) + 1;
            }
        }
    }

    void FillCircle(int16_t x0, int16_t y0, int16_t radius, uint16_t color) {
        if (radius < 0) return;
        int16_t x = radius;
        int16_t y = 0;
        int16_t err = 1 - radius;

        while (x >= y) {
            DrawHLine (x0 - x, y0 + y, 2 * x + 1, color);
            DrawHLine (x0 - x, y0 - y, 2 * x + 1, color);
            DrawHLine (x0 - y, y0 + x, 2 * y + 1, color);
            DrawHLine (x0 - y, y0 - x, 2 * y + 1, color);
            ++y;
            if (err < 0) {
                err += 2 * y + 1;
            } else {
                --x;
                err += 2 * (y - x) + 1;
            }
        }
    }

    void DrawFrame(const Rect& rect, uint16_t color, uint8_t thickness = 1) {
        for (uint8_t i = 0; i < thickness; ++i) {
            DrawRect (rect.x + i, rect.y + i, rect.w - (i * 2), rect.h - (i * 2), color);
        }
    }

    void DrawGradientV(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t top, uint16_t bottom) {
        if (w <= 0 || h <= 0) return;
        for (int16_t row = 0; row < h; ++row) {
            const float t = h <= 1 ? 1.0f : static_cast<float> (row) / static_cast<float> (h - 1);
            DrawHLine (x, y + row, w, Color565::Lerp (top, bottom, t));
        }
    }

    void DrawGradientH(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t left, uint16_t right) {
        if (w <= 0 || h <= 0) return;
        for (int16_t col = 0; col < w; ++col) {
            const float t = w <= 1 ? 1.0f : static_cast<float> (col) / static_cast<float> (w - 1);
            DrawVLine (x + col, y, h, Color565::Lerp (left, right, t));
        }
    }

  protected:
    FORCE_INLINE const TPanel& Self() const noexcept { return *static_cast<const TPanel*> (this); }
    FORCE_INLINE TPanel& Self() noexcept { return *static_cast<TPanel*> (this); }

    void PlotCirclePoints(int16_t x0, int16_t y0, int16_t x, int16_t y, uint16_t color) {
        if (Contains (x0 + x, y0 + y)) Self().DrawPixel (x0 + x, y0 + y, color);
        if (Contains (x0 - x, y0 + y)) Self().DrawPixel (x0 - x, y0 + y, color);
        if (Contains (x0 + x, y0 - y)) Self().DrawPixel (x0 + x, y0 - y, color);
        if (Contains (x0 - x, y0 - y)) Self().DrawPixel (x0 - x, y0 - y, color);
        if (Contains (x0 + y, y0 + x)) Self().DrawPixel (x0 + y, y0 + x, color);
        if (Contains (x0 - y, y0 + x)) Self().DrawPixel (x0 - y, y0 + x, color);
        if (Contains (x0 + y, y0 - x)) Self().DrawPixel (x0 + y, y0 - x, color);
        if (Contains (x0 - y, y0 - x)) Self().DrawPixel (x0 - y, y0 - x, color);
    }
};

template <typename TPanel, size_t MaxRects = 8>
class DirtySurface : public Surface<TPanel> {
  public:
    void InvalidateRect(int16_t x, int16_t y, int16_t w, int16_t h) {
        if (w <= 0 || h <= 0) return;
        InvalidateRect ({x, y, static_cast<uint16_t> (w), static_cast<uint16_t> (h)});
    }

    void InvalidateRect(const Rect& rect) {
        const Rect clipped = this->ClipRectToSurface (rect);
        if (!RectEmpty (clipped)) {
            dirty_rects_.Add (clipped);
        }
    }

    void InvalidateFull() {
        dirty_rects_.Clear();
        dirty_rects_.Add ({0, 0, this->Self().Width(), this->Self().Height()});
    }

    void ClearDirty() {
        dirty_rects_.Clear();
    }

    template <typename TRenderer>
    void FlushDirty(TRenderer renderer) {
        for (size_t i = 0; i < dirty_rects_.Count(); ++i) {
            renderer (this->Self(), dirty_rects_[i]);
        }
        dirty_rects_.Clear();
    }

  protected:
    DirtyRects<MaxRects> dirty_rects_ = {};
};

namespace Anim {
template <typename TDisplay>
void ColorWipe(TDisplay& display, uint16_t color, bool horizontal = true, TickType_t step_delay = 0) {
    if (horizontal) {
        for (uint16_t x = 0; x < display.Width(); ++x) {
            display.DrawVLine (static_cast<int16_t> (x), 0, display.Height(), color);
            if (step_delay > 0) vTaskDelay (step_delay);
        }
        return;
    }

    for (uint16_t y = 0; y < display.Height(); ++y) {
        display.DrawHLine (0, static_cast<int16_t> (y), display.Width(), color);
        if (step_delay > 0) vTaskDelay (step_delay);
    }
}

template <typename TDisplay>
void FadeFill(TDisplay& display, uint16_t from, uint16_t to, uint8_t steps, TickType_t step_delay = 0) {
    if (steps == 0) {
        display.Clear (to);
        return;
    }

    for (uint8_t i = 0; i <= steps; ++i) {
        const float t = static_cast<float> (i) / static_cast<float> (steps);
        display.Clear (Color565::Lerp (from, to, t));
        if (step_delay > 0) vTaskDelay (step_delay);
    }
}
}  // namespace Anim

}  // namespace Display
