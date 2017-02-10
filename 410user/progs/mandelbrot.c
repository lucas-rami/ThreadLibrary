/*!
 * @file mandelbrot.c
 *
 * @brief A very cool, fractal test.
 *
 * Written by Keith Bare sometime aeons ago, improved by sully
 * to support panning and zooming in Spring 2014.
 *
 * This version compromises on some of the original behaviors of
 * mandelbrot in order to get panning/zooming to perform better
 * in Simics.  Both versions run beautifully in QEMU, though.
 *
 * In particular, we skip displaying a blue + in squares about to be
 * updated and we only update squares that don't need it with
 * probability 1/REDO_FACTOR.
 *
 * Setting TRADITIONAL to 1 recovers the old behaviors.
 *
 * @author Keith Bare
 * @author Michael Sullivan (mjsulliv@)
 */

/* Whether to do certain traditional mandelbrot behaviors or to skip them
 * in favor of being faster. */
#define TRADITIONAL 0

#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <syscall.h>
#include <simics.h>
#include <stdlib.h>

#include <malloc.h>
#include <thread.h>
#include <cond.h>
#include <mutex.h>
#include <thrgrp.h>

// We define this as assert for convenience, but it isn't really intended
// to be the same. We use expect to check for fatal errors that require
// us to abort the program.
#define expect assert

/* Not all kernels support getchar(). Try to use it, but if it doesn't work,
 * fall back to readline. */
static int fallback_getchar(void)
{
    char buf[2];
    readline(2, buf);
    return (unsigned char)buf[0]; // cast prevents sign extension
}
static int try_getchar(void)
{
	static int no_getchar = 0;
	int c;
	if (no_getchar || (c = getchar()) < 0) {
		no_getchar = 1;
		return fallback_getchar();
	}
	return c;
}
#define getchar try_getchar


// Filled in by find_console_size
int console_width;  /*!<@brief Height of the console */
int console_height; /*!<@brief Width of the console */

// Some tunables
#define YIELD_FACTOR 8 /*!<@brief Higher => thread is less likely to yield */
#define REDO_FACTOR 4 /*!<@brief Higher => thread is less likely to redo */

#define MAX_TRAPS 30   /*!<@brief Maximum number of support traps */

// Fixed point stuff
typedef int fixed_t;

#define FRACTIONAL_BITS 24 /*!<@brief Number of fixed point fractional bits */

// A bunch of macros for constructing fixed point numbers.
#define SIGN(n) ((n) < 0 ? -1 : 1)
#define INT_TO_FIXED(n) ((n) << FRACTIONAL_BITS)
#define FRAC_TO_FIXED(n, d) ((fixed_t)(INT_TO_FIXED((long long)n) / d))
#define IMPROPER_TO_FIXED(i, n, d) FRAC_TO_FIXED(i*d + SIGN(i)*n, d)
#define DEC3_TO_FIXED(i, dec) IMPROPER_TO_FIXED(i, dec, 1000)

#define INITIAL_COL_BASE DEC3_TO_FIXED(-2,500)
#define INITIAL_COL_LAST DEC3_TO_FIXED( 1,450)
#define INITIAL_ROW_BASE DEC3_TO_FIXED(-1,500)
#define INITIAL_ROW_LAST DEC3_TO_FIXED( 1,375)


/*!<@brief The real coordinates for the left and right edges of the screen */
fixed_t col_base = INITIAL_COL_BASE;
fixed_t col_last = INITIAL_COL_LAST;
/*!<@brief The imaginary coordinates for the top and bottom edges */
fixed_t row_base = INITIAL_ROW_BASE;
fixed_t row_last = INITIAL_ROW_LAST;

// These get set by recompute_steps()
fixed_t col_step;
fixed_t row_step;

int cur_generation = 0;

/*!
 * @brief Type for state that the threads modify
 */
typedef struct mandelbrot_state {
    mutex_t cell_mutex; /*!< Mutex protecting this cell */
    int count;          /*!< Number of threads that have visited this cell */
    int trap_num;       /*!< -1, or the index of the condition variable in
                             which visited threads are trapped */
    int trapped_threads;/*!< Number of threads trapped at this cell */
    int exiting;        /*!< Indicates that the program is terminating */
    int generation;     /*!< What generation the cell was last updated at */
    int color;
} mandelbrot_state_t;


mutex_t console_lock;    /*!<@brief Mutex protecting the console */

cond_t traps[MAX_TRAPS]; /*!<@brief Array of condition variables in which
                              threads can get &quot;stuck&quot; */

int exiting = 0;         /*!<@brief Flag the indicates the test is exiting */

int wakeup_threshold = 1;/*!<@brief Maximum number of waiting threads */

/*!
 * @brief Shared state that the threads modify.
 */
mandelbrot_state_t *state;

unsigned long getrand(void) {
    // FIXME: XXX: rand is not threadsafe!
    return rand();
}

/*!
 * @brief Fixed point multiplication.
 *
 * This is so I can do cool things, even though Pebbles doesn't allow programs
 * to use the FPU.
 */
fixed_t fixed_mult(fixed_t a, fixed_t b)
{
    // Multiply the numbers, getting a result with FRACTIONAL_BITS*2
    // fractional bits.
    long long mult_result = (long long)a * (long long)b;
    // Round off
    mult_result += (1 << (FRACTIONAL_BITS-1));
    return (fixed_t)(mult_result >> FRACTIONAL_BITS);
}

/*! @brief Compute the squared magnitude of a complex number. */
fixed_t mag2(fixed_t r, fixed_t i) {
    return fixed_mult(r, r) + fixed_mult(i, i);
}

/*!
 * @brief Calculates the color that a (row, column) pair of the
 * mandelbrot set should be.
 *
 * Returns the number of iterations of the Mandelbrot equation, Z =
 * (Z_{n - 1})^2 + Z_0, occur before Z is more than a distance of one
 * from the complex origin.
 */
int mandelbrot_calc(int row, int col)
{
    int i;

    fixed_t imag_coord_init = row_base + row*row_step;
    fixed_t real_coord_init = col_base + col*col_step;
    fixed_t imag_coord = imag_coord_init;
    fixed_t real_coord = real_coord_init;

    for (i = 0; i < 14 && mag2(real_coord,imag_coord) < INT_TO_FIXED(4); i++) {
        int tmp_imag_coord = 2*fixed_mult(real_coord, imag_coord) +
                             imag_coord_init;
        real_coord = fixed_mult(real_coord, real_coord) -
                     fixed_mult(imag_coord, imag_coord) +
                     real_coord_init;
        imag_coord = tmp_imag_coord;
    }

    return i;
}

/*!
 * @brief Processes and draws a "pixel".
 *
 * Assumes that the caller has console_lock.
 *
 * @param row row of the pixel to draw
 * @param col column of the pixel to draw
 */
int process_pixel(int row, int col)
{
    mandelbrot_state_t *st = &state[row*console_width + col];
    int wokeup = 0;

    mutex_lock(&st->cell_mutex);

    if (st->exiting) {
        mutex_unlock(&st->cell_mutex);
        return -1;
    }

    // Do wakeups before considering whether to bail on this location.
    if (st->trapped_threads >= wakeup_threshold) {
        cond_broadcast(&traps[st->trap_num]);
        wokeup = 1;
    }

    // Compute the color if it is out of date. If it isn't,
    // skip the rest of the processing "most of the time".
    if (st->generation < cur_generation) {
        st->color = mandelbrot_calc(row, col);
        st->generation = cur_generation;
    } else if (!TRADITIONAL && getrand() % REDO_FACTOR != 0) {
        mutex_unlock(&st->cell_mutex);
        return -1;
    }

    // If there is a trap, get caught in it. (Unless we freed threads from it.)
    if (!wokeup && st->trap_num >= 0) {
        st->trapped_threads++;

        mutex_lock(&console_lock);
        set_cursor_pos(row, col);
        set_term_color((15 - st->color) | BGND_RED);
        putchar('0' + st->trapped_threads % 10);
        mutex_unlock(&console_lock);

        cond_wait(&traps[st->trap_num],
                  &st->cell_mutex);

        st->trapped_threads--;
    }

    st->count++;

    mutex_lock(&console_lock);
    set_cursor_pos(row, col);
    set_term_color((15 - st->color) | BGND_BLACK);
    putchar('a' + st->count % 26);
    mutex_unlock(&console_lock);

    mutex_unlock(&st->cell_mutex);

    return 0;
}

/*!
 * @brief Main function for wanderer_threads.
 *
 * @param arg
 */
void *wanderer_main(void *arg)
{
    int row, col;

    row = getrand() % (console_height - 1);
    col = getrand() % console_width;

    while (!exiting) {
        process_pixel(row, col);

        row = getrand() % (console_height - 1);
        col = getrand() % console_width;

        if (getrand() % YIELD_FACTOR == 0) {
            yield(-1);
        }

#if TRADITIONAL
        mutex_lock(&console_lock);
        set_cursor_pos(row, col);
        set_term_color(FGND_BLACK | BGND_BLUE);
        printf("%c", '+');
        mutex_unlock(&console_lock);

        if (getrand() % YIELD_FACTOR == 0) {
            yield(-1);
        }
#endif
    }

    return NULL;
}

/*!
 * @brief Prints a usage message.
 */
void usage() {
    printf("mandelbrot [num_threads] [num_traps] [wakeup_threshold]\n\n");
    printf("num_traps must be <= %d\n", MAX_TRAPS);
    printf("if num_traps * (wakeup_threshold - 1) >= num_threads "
           "deadlock may result\n");
}

// Dynamically determine the console size. Most Pebbles implementations
// are 80x25, but there have been students who handed in window managers,
// and there exist at least two projects that attempt to run Pebbles
// binaries on Linux.
static void find_console_size() {
    int orig_row, orig_col;
    get_cursor_pos(&orig_row, &orig_col);

    /* determine the height of the console */
    console_height = 0;
    while (set_cursor_pos(console_height, 0) == 0) {
        console_height++;
    }
    /* determine the width of the console */
    console_width = 0;
    while (set_cursor_pos(0, console_width) == 0) {
        console_width++;
    }

    set_cursor_pos(orig_row, orig_col);
}

// Recompute the step amounts after something changes.
void recompute_steps(void) {
    col_step = ((col_last-col_base)/(console_width-1));
    row_step = ((row_last-row_base)/(console_height-2));
}

// When we pan around, we update the parameters without synchronizing
// with computation threads at all. This is sort of hokey but ultimately
// we don't care. The worst that can happen is some pixels are the
// wrong color for a bit.
void input_loop(void) {
    fixed_t row_move;
    fixed_t col_move;

    while (1) {
        char c = getchar();
        switch (c) {
        case 'q':
            return;
        case 'w':
            row_base -= row_step;
            row_last -= row_step;
            break;
        case 's':
            row_base += row_step;
            row_last += row_step;
            break;
        case 'a':
            col_base -= col_step;
            col_last -= col_step;
            break;
        case 'd':
            col_base += col_step;
            col_last += col_step;
            break;
        case 'i':
            // We want to cut the distance between the edges by a factor
            // of 2, which means moving each edge
            // current_distance/4.
            row_move = (row_last-row_base)/4;
            row_base += row_move;
            row_last -= row_move;
            col_move = (col_last-col_base)/4;
            col_base += col_move;
            col_last -= col_move;

            recompute_steps();

            break;

        case 'o':
            // We want to increase the distance between the edges by a factor
            // of 2, which means moving each edge
            // current_distance/2?
            row_move = (row_last-row_base)/2;
            row_base -= row_move;
            row_last += row_move;
            col_move = (col_last-col_base)/2;
            col_base -= col_move;
            col_last += col_move;

            recompute_steps();

            break;

        case 'r':
            col_base = INITIAL_COL_BASE;
            col_last = INITIAL_COL_LAST;
            row_base = INITIAL_ROW_BASE;
            row_last = INITIAL_ROW_LAST;

            recompute_steps();

            break;
        default:
            break;
        }

        cur_generation++;
    }
}


/*!
 * @brief Code entry-point.
 *
 * @param argc
 * @param argv
 */
int main(int argc, char *argv[])
{
    thrgrp_group_t thread_group;
    int num_threads = 15;
    int num_traps = 8;
    void *status;

    int i;

    if (argc >= 2) {
        if (sscanf(argv[1], "%d", &num_threads) < 1 || num_threads <= 0) {
            usage();
            return 0;
        }
    }
    if (argc >= 3) {
        if (sscanf(argv[2], "%d", &num_traps) < 1 || num_traps < 0 ||
            num_traps > MAX_TRAPS) {
            usage();
            return 0;
        }
    }
    if (argc >= 4) {
        if (sscanf(argv[3], "%d", &wakeup_threshold) < 1) {
            usage();
            return 0;
        }
    }

    srand(get_ticks());

    lprintf("Initializing thread library...");
    expect(thr_init(PAGE_SIZE * 10) >= 0);

    lprintf("Initializing console lock...");
    expect(mutex_init(&console_lock) >= 0);

    lprintf("Finding console size...");
    find_console_size();

    lprintf("Initializing state...");
    int num_spots = console_width*(console_height-1);
    state = malloc(num_spots*sizeof(*state));
    expect(state != NULL);

    for (i = 0; i < num_spots; i++) {
        expect(mutex_init(&state[i].cell_mutex) >= 0);
        state[i].count = -1;
        state[i].trap_num = -1;
        state[i].trapped_threads = 0;
        state[i].exiting = 0;
        state[i].generation = -1;
    }

    lprintf("Initializing traps...");
    /* May not assign all the traps, but it doesn't matter. */
    for (i = 0; i < num_traps; i++) {
        expect(cond_init(&traps[i]) >= 0);
        state[getrand() % num_spots].trap_num = i;
    }

    // Set up the values
    recompute_steps();

    lprintf("Clearing console...");
    for (i = 0; i < console_height; i++) {
        printf("\n");
    }

    set_cursor_pos(console_height-1, 0);
    printf("wasd to pan, i = zoom in, o = zoom out, r = reset, q = quit");

    lprintf("Initializing thread group...");
    expect(thrgrp_init_group(&thread_group) >= 0);

    lprintf("Creating wanderer threads...");
    for (i = 0; i < num_threads; i++) {
        thrgrp_create(&thread_group, &wanderer_main, NULL);
    }

    lprintf("Waiting for input...");
    input_loop();

    exiting = 1;

    for (i = 0; i < num_spots; i++) {
        mutex_lock(&state[i].cell_mutex);
        state[i].exiting = 1;
        mutex_unlock(&state[i].cell_mutex);
    }

    for (i = 0; i < num_traps; i++) {
        cond_broadcast(&traps[i]);
    }

    lprintf("Joining with wanderer threads...");
    for (i = 0; i < num_threads; i++) {
        thrgrp_join(&thread_group, &status);
    }

    lprintf("Destroying thread group...");
    thrgrp_destroy_group(&thread_group);

    for (i = 0; i < num_spots; i++) {
        mutex_destroy(&state[i].cell_mutex);
    }

    for (i = 0; i < num_traps; i++) {
        cond_destroy(&traps[i]);
    }

    mutex_destroy(&console_lock);

    set_cursor_pos(console_height - 1, 0);
    set_term_color(FGND_CYAN | BGND_BLACK);

    thr_exit(NULL);
    return 0;
}
