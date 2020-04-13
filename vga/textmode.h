/*
 * Copyright (c) 2020 Kenneth Umenthum
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include <gtkmm.h>
#include "vga_common.h"

class vgpu : public Gtk::DrawingArea {
private:
    uint8_t (&text_array)[text_rows][2 * text_cols];
    uint8_t (&font_data)[256][char_rows];
    uint32_t (&color_palette)[num_colors];

    Cairo::RefPtr<Cairo::ImageSurface> m_pix;
    Cairo::RefPtr<Cairo::Context> m_gc;
    uint32_t* m_data;

    void initialize(void)
    {
        set_has_window(true);
        Widget::set_can_focus(false);
        set_size_request(screen_width, screen_height);
    }

    uint32_t color18to24(uint32_t color);
    void draw_chars(void);

public:

    vgpu(uint8_t (&_text_array)[text_rows][2 * text_cols],
            uint8_t (&_font_data)[256][char_rows],
            uint32_t (&_color_palette)[num_colors]) :
        text_array(_text_array), font_data(_font_data),
        color_palette(_color_palette), Gtk::DrawingArea()
    {
        initialize();
    }

    void get_preferred_width_vfunc(int& min, int& nw) const {
        min = screen_width;
        nw = screen_width;
    }

    void get_preferred_height_vfunc(int& min, int& nw) const {
        min = screen_height;
        nw = screen_height;
    }

    void get_preferred_height_for_width_vfunc(int w, int& min, int& nw) const {
        min = screen_height;
        nw = screen_height;
    }

    void get_preferred_width_for_height_vfunc(int w, int& min, int& nw) const {
        min = screen_width;
        nw = screen_width;
    }

    virtual void on_show() { Gtk::DrawingArea::on_show(); }
    virtual void on_hide() { Gtk::DrawingArea::on_hide(); }
    virtual void on_map(void) { Gtk::Widget::on_map(); }
    virtual void on_my_map(void) {}
    virtual void signal_map(void) { Gtk::DrawingArea::signal_map(); }
    virtual bool on_configure_event(GdkEventConfigure* ev) { return Gtk::DrawingArea::on_configure_event(ev); }
    virtual void on_realize();
    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& gc);
    int width(void) { return screen_width; }
    int height(void) { return screen_height; }
    int raw_width(void) { return screen_width; }
    int raw_height(void) { return screen_height; }
};

class vgpu_window : public Gtk::Window {
private:
    vgpu* m_vgasim;
    void init(void) {
        m_vgasim->set_size_request(m_vgasim->width(), m_vgasim->height());
        set_border_width(0);
        add(*m_vgasim);
        show_all();
        Gtk::Window::set_title(Glib::ustring("Frizzle VGA Emulator"));
    };

public:
    vgpu_window(uint8_t (&text_array)[text_rows][2 * text_cols],
                uint8_t (&font_data)[256][char_rows],
                uint32_t (&color_palette)[num_colors]) {
        m_vgasim = new vgpu(text_array, font_data, color_palette);
        init();
    }

    ~vgpu_window(void) { delete m_vgasim; }
    int width(void) { return m_vgasim->width(); }
    int height(void) { return m_vgasim->height(); }
    int raw_width(void) { return m_vgasim->raw_width(); }
    int raw_height(void) { return m_vgasim->raw_height(); }
};
