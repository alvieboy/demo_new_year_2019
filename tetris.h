#include <inttypes.h>

#ifdef SMALLPIECES
// Panel is 64*32. Reserve 12 pix at top.

#define BLOCKS_X 16
#define BLOCKS_Y 26
#define BLOCKSIZE 2
static const int board_x0 = 0;
static const int board_y0 = 11;

#else

#define BLOCKS_X 16
#define BLOCKS_Y 39
//#define BLOCKS_Y 10
#define BLOCKSIZE 3

static const int board_x0 = 1;
static const int board_y0 = 10;


#endif

static const int board_width = (BLOCKS_X * BLOCKSIZE);
static const int board_height = (BLOCKS_Y * BLOCKSIZE);

#define PIECESIZE_MAX 4

enum event_t {
    event_none,
    event_left,
    event_right,
    event_down,
    event_rotate
};

typedef unsigned char piecedef[PIECESIZE_MAX][PIECESIZE_MAX];

struct piece {
    uint8_t size;
    uint8_t x_offset;
    uint8_t y_offset;
    uint8_t rsvd;
    piecedef *layout[4];
};

typedef uint8_t color_type;//[BLOCKSIZE*BLOCKSIZE];

struct level {
    unsigned lines_required;
    unsigned char area[BLOCKS_X][BLOCKS_Y];
};

class ScreenDrawer;

class Tetris
{
public:
    Tetris();
    void setup_game();
    void game_loop(ScreenDrawer*);
    void game_render(ScreenDrawer*);
    void game_event_check(ScreenDrawer*gfx);
    event_t ai_play();
    bool ai_busy;
};

#define TODO
