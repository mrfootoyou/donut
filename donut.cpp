#include <chrono>
#include <thread>
#include <array>
#include <iostream>
#include <string_view>
#include <numbers>
#include <csignal>

constexpr auto pi2 = std::numbers::pi * 2;
constexpr auto lines = 25u;
constexpr auto linelen = 80u;
constexpr auto speed = 0.0704;
constexpr auto delay = std::chrono::milliseconds{1};

typedef std::array<int, lines * linelen> data_array;
typedef std::array<std::string_view, 13u> glyph_array;

constexpr glyph_array glyphs{
    " ",
    ".",
    ",",
    "-",
    "~",
    ":",
    ";",
    "=",
    "!",
    "*",
    "#",
    "$",
    "@",
};

constexpr glyph_array shades{
    " ",
    "\x1b[38;5;234m@", // dark gray
    "\x1b[38;5;235m@", //
    "\x1b[38;5;236m@", //
    "\x1b[38;5;238m@", //
    "\x1b[38;5;240m@", //
    "\x1b[38;5;243m@", //
    "\x1b[38;5;246m@", //
    "\x1b[38;5;249m@", //
    "\x1b[38;5;251m@", //
    "\x1b[38;5;253m@", //
    "\x1b[38;5;254m@", //
    "\x1b[38;5;255m@", // light gray
};

void draw(const data_array &b, const glyph_array &glyphs)
{
    std::cout << "\x1b[H"; // move cursor to home position (0,0)
    for (auto y = 0u; y < b.size(); y += linelen)
    {
        for (auto x = 0u; x < linelen; ++x)
        {
            const auto n = b[y + x];
            std::cout << glyphs[n];
        }
        std::cout << '\n';
    }
}

void exec(double A, double B, data_array &b)
{
    b.fill(0);
    auto z = std::array<double, lines * linelen>{};
    const auto sinA = sin(A);
    const auto cosA = cos(A);
    const auto cosB = cos(B);
    const auto sinB = sin(B);
    for (auto j = 0.; j < pi2; j += pi2 / 90.)
    {
        const auto cosj = cos(j);
        const auto sinj = sin(j);
        const auto h = cosj + 2;
        for (auto i = 0.; i < pi2; i += pi2 / 200)
        {
            const auto sini = sin(i);
            const auto cosi = cos(i);
            const auto D = 1 / ((sini * h * sinA) + (sinj * cosA) + 5);
            const auto t = (sini * h * cosA) - (sinj * sinA);
            const auto x = static_cast<int>(1 + (linelen / 2) + (30 * D * ((cosi * h * cosB) - (t * sinB))));
            const auto y = static_cast<int>(1 + (lines / 2) + (15 * D * ((cosi * h * sinB) + (t * cosB))));
            const auto o = x + (linelen * y);
            if (0 < y && y < lines && 0 < x && x < linelen && D > z[o])
            {
                const auto N = static_cast<int>(8 * ((((sinj * sinA) - (sini * cosj * cosA)) * cosB) - (sini * cosj * sinA) - (sinj * cosA) - (cosi * cosj * sinB)));
                z[o] = D;
                b[o] = N > 0 ? N : 1;
            }
        }
    }
}

volatile std::sig_atomic_t shouldQuit = 0;

void sigint_handler(int n)
{
    shouldQuit = 1;
    std::signal(n, SIG_ACK);
}

int main()
{
    std::signal(SIGINT, sigint_handler);

    std::cout << "\x1b[?25l"; // hide cursor (reduces flicker)
    std::cout << "\x1b[2J";   // erase screen

    auto A = 0.0, B = 0.0;
    data_array data;
    while (!shouldQuit)
    {
        exec(A, B, data);

        // draw(data, glyphs);
        draw(data, shades);

        std::this_thread::sleep_for(delay);
        A += speed;
        B += speed / 2.0;
    }

    std::cout << "\x1b[?25h"; // show cursor
    return 0;
}
