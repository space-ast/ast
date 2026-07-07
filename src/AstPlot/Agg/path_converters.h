// 从 matplotlib src/path_converters.h 复制，去除 Python/pybind11 依赖
// 原始版权: matplotlib project
#pragma once

#include <cmath>
#include <cstdint>
#include <limits>

#if defined(AST_WITH_AGG)

#include "agg/agg_clip_liang_barsky.h"
#include "agg/agg_conv_segmentator.h"

#include "mpl_utils.h"

/************************************************************
 * EmbeddedQueue — 顶点队列基类，处理一个输入顶点产生多个输出
 ************************************************************/
template <int QueueSize>
class EmbeddedQueue {
protected:
    EmbeddedQueue() : m_queue_read(0), m_queue_write(0) {}

    struct item {
        item() = default;
        inline void set(const unsigned cmd_, const double x_, const double y_) {
            cmd = cmd_; x = x_; y = y_;
        }
        unsigned cmd;
        double x, y;
    };
    int m_queue_read, m_queue_write;
    item m_queue[QueueSize];

    inline void queue_push(const unsigned cmd, const double x, const double y) {
        m_queue[m_queue_write++].set(cmd, x, y);
    }
    inline bool queue_nonempty() { return m_queue_read < m_queue_write; }
    inline bool queue_pop(unsigned *cmd, double *x, double *y) {
        if (queue_nonempty()) {
            const item &front = m_queue[m_queue_read++];
            *cmd = front.cmd; *x = front.x; *y = front.y;
            return true;
        }
        m_queue_read = m_queue_write = 0;
        return false;
    }
    inline void queue_clear() { m_queue_read = m_queue_write = 0; }
};

/* 每种路径段类型的额外顶点数 */
static const size_t num_extra_points_map[] = {0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/************************************************************
 * RandomNumberGenerator — 简单 LCG 随机数 (用于 Sketch)
 ************************************************************/
class RandomNumberGenerator {
    static const uint32_t a = 214013, c = 2531011;
    uint32_t m_seed;
public:
    RandomNumberGenerator() : m_seed(0) {}
    explicit RandomNumberGenerator(int seed) : m_seed(seed) {}
    void seed(int seed) { m_seed = seed; }
    double get_double() {
        m_seed = a * m_seed + c;
        return (double)m_seed / (double)(1LL << 32);
    }
};

/************************************************************
 * PathNanRemover — 跳过含 NaN/Inf 的线段
 ************************************************************/
template <class VertexSource>
class PathNanRemover : protected EmbeddedQueue<4> {
    VertexSource *m_source;
    bool m_remove_nans, m_has_codes;
    bool valid_segment_exists, m_last_segment_valid, m_was_broken;
    double m_initX, m_initY;

public:
    PathNanRemover(VertexSource &source, bool remove_nans, bool has_codes)
        : m_source(&source), m_remove_nans(remove_nans), m_has_codes(has_codes),
          m_last_segment_valid(false), m_was_broken(false) {
        m_initX = std::nan(""); m_initY = std::nan("");
        valid_segment_exists = false;
    }

    inline void rewind(unsigned path_id) {
        queue_clear();
        m_source->rewind(path_id);
    }

    inline unsigned vertex(double *x, double *y) {
        unsigned code;
        if (!m_remove_nans) return m_source->vertex(x, y);

        if (m_has_codes) {
            if (queue_pop(&code, x, y)) return code;

            bool needs_move_to = false;
            while (true) {
                code = m_source->vertex(x, y);
                if (code == agg::path_cmd_stop) return code;
                if (code == (agg::path_cmd_end_poly | agg::path_flags_close) && valid_segment_exists) {
                    if (m_was_broken) {
                        if (m_last_segment_valid && std::isfinite(m_initX) && std::isfinite(m_initY)) {
                            queue_push(agg::path_cmd_line_to, m_initX, m_initY);
                            break;
                        } else { continue; }
                        m_was_broken = false; break;
                    } else { return code; }
                } else if (code == agg::path_cmd_move_to) {
                    m_initX = *x; m_initY = *y; m_was_broken = false;
                }

                if (needs_move_to) queue_push(agg::path_cmd_move_to, *x, *y);

                size_t num_extra = num_extra_points_map[code & 0xF];
                m_last_segment_valid = std::isfinite(*x) && std::isfinite(*y);
                queue_push(code, *x, *y);
                for (size_t i = 0; i < num_extra; ++i) {
                    m_source->vertex(x, y);
                    m_last_segment_valid = m_last_segment_valid && std::isfinite(*x) && std::isfinite(*y);
                    queue_push(code, *x, *y);
                }
                if (m_last_segment_valid) { valid_segment_exists = true; break; }

                m_was_broken = true; queue_clear();
                if (std::isfinite(*x) && std::isfinite(*y)) {
                    queue_push(agg::path_cmd_move_to, *x, *y); needs_move_to = false;
                } else { needs_move_to = true; }
            }
            if (queue_pop(&code, x, y)) return code;
            return agg::path_cmd_stop;
        } else {
            code = m_source->vertex(x, y);
            if (code == agg::path_cmd_stop ||
                (code == (agg::path_cmd_end_poly | agg::path_flags_close) && valid_segment_exists))
                return code;
            if (!(std::isfinite(*x) && std::isfinite(*y))) {
                do {
                    code = m_source->vertex(x, y);
                    if (code == agg::path_cmd_stop ||
                        (code == (agg::path_cmd_end_poly | agg::path_flags_close) && valid_segment_exists))
                        return code;
                } while (!(std::isfinite(*x) && std::isfinite(*y)));
                return agg::path_cmd_move_to;
            }
            valid_segment_exists = true;
            return code;
        }
    }
};

/************************************************************
 * PathClipper — Liang-Barsky 矩形裁剪
 ************************************************************/
template <class VertexSource>
class PathClipper : public EmbeddedQueue<3> {
    VertexSource *m_source;
    bool m_do_clipping;
    agg::rect_base<double> m_cliprect;
    double m_lastX, m_lastY, m_initX, m_initY;
    bool m_moveto, m_has_init, m_was_clipped;

public:
    PathClipper(VertexSource &source, bool do_clipping, double width, double height)
        : m_source(&source), m_do_clipping(do_clipping),
          m_cliprect(-1.0, -1.0, width + 1.0, height + 1.0),
          m_moveto(true), m_has_init(false), m_was_clipped(false) {
        m_lastX = std::nan(""); m_lastY = std::nan("");
        m_initX = std::nan(""); m_initY = std::nan("");
    }

    PathClipper(VertexSource &source, bool do_clipping, const agg::rect_base<double> &rect)
        : m_source(&source), m_do_clipping(do_clipping), m_cliprect(rect),
          m_moveto(true), m_has_init(false), m_was_clipped(false) {
        m_lastX = std::nan(""); m_lastY = std::nan("");
        m_initX = std::nan(""); m_initY = std::nan("");
        m_cliprect.x1 -= 1.0; m_cliprect.y1 -= 1.0; m_cliprect.x2 += 1.0; m_cliprect.y2 += 1.0;
    }

    inline void rewind(unsigned path_id) {
        m_has_init = m_was_clipped = false; m_moveto = true;
        m_source->rewind(path_id);
    }

    int draw_clipped_line(double x0, double y0, double x1, double y1, bool closed = false) {
        unsigned moved = agg::clip_line_segment(&x0, &y0, &x1, &y1, m_cliprect);
        m_was_clipped = m_was_clipped || (moved != 0);
        if (moved < 4) {
            if (moved & 1 || m_moveto) queue_push(agg::path_cmd_move_to, x0, y0);
            queue_push(agg::path_cmd_line_to, x1, y1);
            if (closed && !m_was_clipped)
                queue_push(agg::path_cmd_end_poly | agg::path_flags_close, x1, y1);
            m_moveto = false;
            return 1;
        }
        return 0;
    }

    unsigned vertex(double *x, double *y) {
        unsigned code; bool emit_moveto = false;
        if (!m_do_clipping) return m_source->vertex(x, y);
        if (queue_pop(&code, x, y)) return code;

        while ((code = m_source->vertex(x, y)) != agg::path_cmd_stop) {
            emit_moveto = false;
            switch (code) {
            case (agg::path_cmd_end_poly | agg::path_flags_close):
                if (m_has_init) draw_clipped_line(m_lastX, m_lastY, m_initX, m_initY, true);
                else queue_push(agg::path_cmd_end_poly | agg::path_flags_close, m_lastX, m_lastY);
                if (queue_nonempty()) goto exit_loop;
                break;
            case agg::path_cmd_move_to:
                if (m_moveto && m_has_init && m_lastX >= m_cliprect.x1 && m_lastX <= m_cliprect.x2
                    && m_lastY >= m_cliprect.y1 && m_lastY <= m_cliprect.y2) {
                    queue_push(agg::path_cmd_move_to, m_lastX, m_lastY); emit_moveto = true;
                }
                m_initX = m_lastX = *x; m_initY = m_lastY = *y;
                m_has_init = true; m_moveto = true; m_was_clipped = false;
                if (emit_moveto) goto exit_loop;
                break;
            case agg::path_cmd_line_to:
                if (draw_clipped_line(m_lastX, m_lastY, *x, *y)) { m_lastX = *x; m_lastY = *y; goto exit_loop; }
                m_lastX = *x; m_lastY = *y;
                break;
            default:
                if (m_moveto) { queue_push(agg::path_cmd_move_to, m_lastX, m_lastY); m_moveto = false; }
                queue_push(code, *x, *y); m_lastX = *x; m_lastY = *y;
                goto exit_loop;
            }
        }
    exit_loop:
        if (queue_pop(&code, x, y)) return code;
        if (m_moveto && m_has_init && m_lastX >= m_cliprect.x1 && m_lastX <= m_cliprect.x2
            && m_lastY >= m_cliprect.y1 && m_lastY <= m_cliprect.y2) {
            *x = m_lastX; *y = m_lastY; m_moveto = false;
            return agg::path_cmd_move_to;
        }
        return agg::path_cmd_stop;
    }
};

/************************************************************
 * e_snap_mode — 像素对齐模式
 ************************************************************/
enum e_snap_mode { SNAP_AUTO, SNAP_FALSE, SNAP_TRUE };

/************************************************************
 * PathSnapper — 顶点像素对齐
 ************************************************************/
template <class VertexSource>
class PathSnapper {
    VertexSource *m_source;
    bool m_snap;
    double m_snap_value;

public:
    static bool should_snap(VertexSource &path, e_snap_mode snap_mode, unsigned total_vertices) {
        double x0 = 0, y0 = 0, x1 = 0, y1 = 0; unsigned code;
        switch (snap_mode) {
        case SNAP_AUTO:
            if (total_vertices > 1024) return false;
            code = path.vertex(&x0, &y0);
            if (code == agg::path_cmd_stop) return false;
            while ((code = path.vertex(&x1, &y1)) != agg::path_cmd_stop) {
                switch (code) {
                case agg::path_cmd_curve3:
                case agg::path_cmd_curve4: return false;
                case agg::path_cmd_line_to:
                    if (fabs(x0 - x1) >= 1e-4 && fabs(y0 - y1) >= 1e-4) return false;
                }
                x0 = x1; y0 = y1;
            }
            return true;
        case SNAP_FALSE: return false;
        case SNAP_TRUE:  return true;
        }
        return false;
    }

    PathSnapper(VertexSource &source, e_snap_mode snap_mode, unsigned total_vertices = 15,
                double stroke_width = 0.0) : m_source(&source) {
        m_snap = should_snap(source, snap_mode, total_vertices);
        if (m_snap) {
            int is_odd = mpl_round_to_int(stroke_width) % 2;
            m_snap_value = (is_odd) ? 0.5 : 0.0;
        }
        source.rewind(0);
    }

    inline void rewind(unsigned path_id) { m_source->rewind(path_id); }
    inline unsigned vertex(double *x, double *y) {
        unsigned code = m_source->vertex(x, y);
        if (m_snap && agg::is_vertex(code)) {
            *x = floor(*x + 0.5) + m_snap_value;
            *y = floor(*y + 0.5) + m_snap_value;
        }
        return code;
    }
    inline bool is_snapping() { return m_snap; }
};

/************************************************************
 * PathSimplifier — 在线路径简化 (Douglas-Peucker 变体)
 ************************************************************/
template <class VertexSource>
class PathSimplifier : protected EmbeddedQueue<9> {
public:
    PathSimplifier(VertexSource &source, bool do_simplify, double simplify_threshold)
        : m_source(&source), m_simplify(do_simplify),
          m_moveto(true), m_after_moveto(false), m_clipped(false),
          m_has_init(false), m_simplify_threshold(simplify_threshold * simplify_threshold),
          m_initX(0), m_initY(0),
          m_lastx(0), m_lasty(0), m_origdx(0), m_origdy(0), m_origdNorm2(0),
          m_dnorm2ForwardMax(0), m_dnorm2BackwardMax(0),
          m_nextX(0), m_nextY(0), m_nextBackwardX(0), m_nextBackwardY(0),
          m_currVecStartX(0), m_currVecStartY(0),
          m_lastForwardMax(false), m_lastBackwardMax(false) {}

    inline void rewind(unsigned path_id) { queue_clear(); m_moveto = true; m_source->rewind(path_id); }

    unsigned vertex(double *x, double *y) {
        unsigned cmd;
        if (!m_simplify) return m_source->vertex(x, y);
        if (queue_pop(&cmd, x, y)) return cmd;

        while ((cmd = m_source->vertex(x, y)) != agg::path_cmd_stop) {
            if (m_moveto || cmd == agg::path_cmd_move_to) {
                if (m_origdNorm2 != 0.0 && !m_after_moveto) _push(x, y);
                m_after_moveto = true;
                m_has_init = std::isfinite(*x) && std::isfinite(*y);
                if (m_has_init) { m_initX = *x; m_initY = *y; }
                m_lastx = *x; m_lasty = *y;
                m_moveto = false; m_origdNorm2 = 0.0; m_dnorm2BackwardMax = 0.0; m_clipped = true;
                if (queue_nonempty()) break;
                continue;
            }
            m_after_moveto = false;

            if (agg::is_close(cmd)) {
                if (m_has_init) { *x = m_initX; *y = m_initY; } else continue;
            }

            if (m_origdNorm2 == 0.0) {
                if (m_clipped) { queue_push(agg::path_cmd_move_to, m_lastx, m_lasty); m_clipped = false; }
                m_origdx = *x - m_lastx; m_origdy = *y - m_lasty;
                m_origdNorm2 = m_origdx * m_origdx + m_origdy * m_origdy;
                m_dnorm2ForwardMax = m_origdNorm2; m_dnorm2BackwardMax = 0.0;
                m_lastForwardMax = true; m_lastBackwardMax = false;
                m_currVecStartX = m_lastx; m_currVecStartY = m_lasty;
                m_nextX = m_lastx = *x; m_nextY = m_lasty = *y;
                continue;
            }

            double totdx = *x - m_currVecStartX, totdy = *y - m_currVecStartY;
            double totdot = m_origdx * totdx + m_origdy * totdy;
            double paradx = totdot * m_origdx / m_origdNorm2, parady = totdot * m_origdy / m_origdNorm2;
            double perpdx = totdx - paradx, perpdy = totdy - parady;
            double perpdNorm2 = perpdx * perpdx + perpdy * perpdy;

            if (perpdNorm2 < m_simplify_threshold) {
                double paradNorm2 = paradx * paradx + parady * parady;
                m_lastForwardMax = m_lastBackwardMax = false;
                if (totdot > 0.0) {
                    if (paradNorm2 > m_dnorm2ForwardMax) { m_lastForwardMax = true; m_dnorm2ForwardMax = paradNorm2; m_nextX = *x; m_nextY = *y; }
                } else {
                    if (paradNorm2 > m_dnorm2BackwardMax) { m_lastBackwardMax = true; m_dnorm2BackwardMax = paradNorm2; m_nextBackwardX = *x; m_nextBackwardY = *y; }
                }
                m_lastx = *x; m_lasty = *y;
                continue;
            }
            _push(x, y);
            break;
        }

        if (cmd == agg::path_cmd_stop) {
            if (m_origdNorm2 != 0.0) {
                queue_push((m_moveto || m_after_moveto) ? agg::path_cmd_move_to : agg::path_cmd_line_to, m_nextX, m_nextY);
                if (m_dnorm2BackwardMax > 0.0)
                    queue_push((m_moveto || m_after_moveto) ? agg::path_cmd_move_to : agg::path_cmd_line_to, m_nextBackwardX, m_nextBackwardY);
                m_moveto = false;
            }
            queue_push((m_moveto || m_after_moveto) ? agg::path_cmd_move_to : agg::path_cmd_line_to, m_lastx, m_lasty);
            m_moveto = false;
            queue_push(agg::path_cmd_stop, 0.0, 0.0);
        }

        if (queue_pop(&cmd, x, y)) return cmd;
        return agg::path_cmd_stop;
    }

private:
    VertexSource *m_source;
    bool m_simplify, m_moveto, m_after_moveto, m_clipped, m_has_init;
    double m_simplify_threshold;
    double m_initX, m_initY, m_lastx, m_lasty, m_origdx, m_origdy, m_origdNorm2;
    double m_dnorm2ForwardMax, m_dnorm2BackwardMax, m_nextX, m_nextY;
    double m_nextBackwardX, m_nextBackwardY, m_currVecStartX, m_currVecStartY;
    bool m_lastForwardMax, m_lastBackwardMax;

    inline void _push(double *x, double *y) {
        bool needToPushBack = (m_dnorm2BackwardMax > 0.0);
        if (needToPushBack) {
            if (m_lastForwardMax) { queue_push(agg::path_cmd_line_to, m_nextBackwardX, m_nextBackwardY); queue_push(agg::path_cmd_line_to, m_nextX, m_nextY); }
            else { queue_push(agg::path_cmd_line_to, m_nextX, m_nextY); queue_push(agg::path_cmd_line_to, m_nextBackwardX, m_nextBackwardY); }
        } else { queue_push(agg::path_cmd_line_to, m_nextX, m_nextY); }
        if (m_clipped) queue_push(agg::path_cmd_move_to, m_lastx, m_lasty);
        else if (!m_lastForwardMax && !m_lastBackwardMax) queue_push(agg::path_cmd_line_to, m_lastx, m_lasty);
        m_origdx = *x - m_lastx; m_origdy = *y - m_lasty;
        m_origdNorm2 = m_origdx * m_origdx + m_origdy * m_origdy;
        m_dnorm2ForwardMax = m_origdNorm2; m_lastForwardMax = true;
        m_currVecStartX = m_queue[m_queue_write - 1].x;
        m_currVecStartY = m_queue[m_queue_write - 1].y;
        m_lastx = m_nextX = *x; m_lasty = m_nextY = *y;
        m_dnorm2BackwardMax = 0.0; m_lastBackwardMax = false;
        m_clipped = false;
    }
};

/************************************************************
 * Sketch — 手绘风格路径扰动
 ************************************************************/
template <class VertexSource>
class Sketch {
public:
    Sketch(VertexSource &source, double scale, double length, double randomness)
        : m_source(&source), m_scale(scale), m_length(length), m_randomness(randomness),
          m_segmented(source), m_last_x(0), m_last_y(0), m_p(0), m_has_last(false), m_rand(0)
    {
        rewind(0);
        const double d_M_PI = 3.14159265358979323846;
        if (m_length <= std::numeric_limits<double>::epsilon() || m_randomness <= std::numeric_limits<double>::epsilon()) {
            m_p_scale = 0.0;
        } else {
            m_p_scale = (2.0 * d_M_PI) / (m_length * m_randomness);
        }
        m_log_randomness = (m_randomness <= std::numeric_limits<double>::epsilon()) ? 0.0 : 2.0 * log(m_randomness);
    }

    unsigned vertex(double *x, double *y) {
        if (m_scale == 0.0) return m_source->vertex(x, y);
        unsigned code = m_segmented.vertex(x, y);
        if (code == agg::path_cmd_move_to) { m_has_last = false; m_p = 0.0; }
        if (m_has_last) {
            double d_rand = m_rand.get_double();
            m_p += exp(d_rand * m_log_randomness);
            double den = m_last_x - *x, num = m_last_y - *y;
            double len = num * num + den * den;
            m_last_x = *x; m_last_y = *y;
            if (len != 0) {
                len = sqrt(len);
                double r = sin(m_p * m_p_scale) * m_scale;
                double roverlen = r / len;
                *x += roverlen * num;
                *y -= roverlen * den;
            }
        } else { m_last_x = *x; m_last_y = *y; }
        m_has_last = true;
        return code;
    }

    inline void rewind(unsigned path_id) {
        m_has_last = false; m_p = 0.0;
        if (m_scale != 0.0) { m_rand.seed(0); m_segmented.rewind(path_id); }
        else m_source->rewind(path_id);
    }

private:
    VertexSource *m_source;
    double m_scale, m_length, m_randomness;
    agg::conv_segmentator<VertexSource> m_segmented;
    double m_last_x, m_last_y, m_p, m_p_scale, m_log_randomness;
    bool m_has_last;
    RandomNumberGenerator m_rand;
};


#endif // AST_WITH_AGG
