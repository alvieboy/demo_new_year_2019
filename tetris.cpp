#include "tetris.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "color_defs.h"
#include "ScreenDrawer.h"
#include "font.h"
#include <limits.h>
#include <vector>
#include <time.h>

#if 0
//def __linux__
#define DEBUG(x...)  printf(x)
#else
#define DEBUG(x...)
#endif

static int px = -1; /* Piece current X position */
static int py = 0; /* Piece current Y position */
static int tick = 0;
static int tickmax = 5;
static unsigned score=0; /* Our score */
//static unsigned level=0; /* Current level */
static unsigned lines=0; /* Number of lines made (all levels) */
//static unsigned currentlevel_lines=0; /* This level number of lines made */
static unsigned lines_total=0;

static uint8_t lval[4], cval[4];

/* This will hold the current game area */
static unsigned char area[BLOCKS_X][BLOCKS_Y];

/* Current piece falling */
static struct piece *currentpiece;
static uint8_t currentrotate;

/* Next piece */
static struct piece *nextpiece;

/* Whether the current piece is valid, or if we need to allocate a new one */
static bool currentpiecevalid=false;

#define LINEDONE_COUNTER_MAX 30
#define LINEDONE_SELECTOR (1<<2)

static unsigned linedone_counter;

const int OPERATION_CLEAR=0;
const int OPERATION_DRAW=1;

typedef void (*loopfunc)(ScreenDrawer*);

static void draw_block(ScreenDrawer *gfx, int x, int y, uint8_t color_index);
static void draw_block_small(ScreenDrawer *gfx, int x, int y, uint8_t color_index);

static const font_t *font = NULL;

static int8_t completed_lines[5]; // Never more than 4. Extra placeholder for end.
static int8_t lines_made;

static uint32_t colors[] = {
    CRGB(0x00,0x00, 0x00), // Unused.
    CRGB(0x00,0x00,0xff), // Green
    CRGB(0xFF,0x00,0xFF), // Purple
    CRGB(0xff,0xff,0x00), // Yellow
    CRGB(0xFF,0xF0,0x00), // Orange
    CRGB(0x00,0xFF,0xFF), // Cyan
    CRGB(0x00,0x00,0xFF), // Blue
    CRGB(0xFF,0x00,0x00), // RED
};


enum {
    PALETTE,
    START,
    PLAY,
    LINEDONE
} game_state = PALETTE;

extern int isDown();
extern int isLeft();
extern int isRight();
extern int isRotate();

static void draw_hline(ScreenDrawer*gfx, int x, int y, int w, uint32_t color);


static int checkPress(uint8_t current, uint8_t *latest, uint8_t delay, uint8_t repeat)
{
    if (current==0) {
        (*latest) = 0;
        return 0;
    }
    // Pressed.
    if ((*latest)==0) {
        *latest = *latest + 1;
        return 1; // Press event.
    }
    delay++;

    // Now, check for delay
    if (*latest<delay) {
        (*latest)++;
        return 0;
    }
    // Delayed now, check repeat.
    if (*latest==delay) {
        (*latest)++;
        return 1;
    }
    // Increase by repeat
    (*latest)++;

    if ((*latest)>(delay+repeat)) {
        (*latest)=delay;
    }
    return 0;
}

enum event_t hasEvent()
{
    enum event_t ret = event_none;
#if 0
    cval[0] = isDown();
    cval[1] = isRotate();
    cval[2] = isLeft();
    cval[3] = isRight();

    do {
        if (checkPress(cval[0], &lval[0], /* Delay */ 1, /* Repeat */ 2)) {
            ret = event_down;
            break;
        }
        if (checkPress(cval[1], &lval[1], /* Delay */ 10, /* Repeat */ 10)) {
            ret = event_rotate;
            break;
        }
        if (checkPress(cval[2], &lval[2], /* Delay */ 5, /* Repeat */ 5)) {
            ret = event_left;
            break;
        }
        if (checkPress(cval[3], &lval[3], /* Delay */ 5, /* Repeat */ 5)) {
            ret=  event_right;
            break;
        }
    } while (0);

#if 0
    lval[0] = cval[0];
    lval[1] = cval[1];
    lval[2] = cval[2];
    lval[3] = cval[3];
#endif
#endif
    if ( isDown() )
        return event_down;

    if ( isRotate() )
        return event_rotate;
    if ( isLeft() )
        return event_left;
    if  (isRight())
        return event_right;

    return ret;
}

#include "pieces.h"

/* Random function (sorta) */
#ifndef __linux__
static unsigned xrand() {
    // TODO
    return rand();
}
#else
#define xrand rand
#endif

static struct piece * getRandomPiece()
{
    int i = xrand() % (sizeof(allpieces)/sizeof(struct piece));
    DEBUG("Returning piece %i\n",i);
    return &allpieces[i];
}

static void area_init()
{
    memset(area,0,sizeof(area));
}

static void line_done(int y)
{
    int x,py;

    DEBUG("Line done at %d\n",y);
    // Shift down area
    for (py=y;py>=0;py--) {
        for (x=0;x<BLOCKS_X;x++) {
            area[x][py] = py>0 ? area[x][py-1] : 0; // Clear
        }
    }
}

static bool can_place(uint8_t xarea[BLOCKS_X][BLOCKS_Y], int x, int y, struct piece *p, int rotate)
{
    int i,j;
    for (i=0;i<p->size;i++)
        for (j=0;j<p->size;j++) {
            if ((*p->layout[rotate])[j][i]) {
                if ( (x+i) >= BLOCKS_X || (x+i) <0 ) {
                    DEBUG("X overflow %d, %d\n",x,i);
                    return false;
                }
                if ( (y+j) >= BLOCKS_Y || (x+y) <0 ) {
                    DEBUG("Y overflow %d, %d\n",y,j);
                    return false;
                }

                if (xarea[x+i][y+j]) {
                    DEBUG("Collision at %d %d\n", x+i, y+j);
                    return false;
                } else {
                    //DEBUG("Placement OK at %d %d\n", x+i, y+j);
                }
            }
        }
    return true;
}

static void do_place(uint8_t xarea[BLOCKS_X][BLOCKS_Y], int x, int y, int piece_size, piecedef *p)
{
    int i,j;
    for (i=0;i<piece_size;i++) {
        for (j=0;j<piece_size;j++) {
            if ((*p)[j][i]) {
                DEBUG("Marking %d %d -> %d\n", x+i,y+j, (*p)[j][i]);
                xarea[x+i][y+j] = (*p)[j][i];
            }
        }
    }
}

typedef void (*draw_block_func)(ScreenDrawer*, int, int, uint8_t color_index);

static void draw_piece_impl(ScreenDrawer *gfx,
                            int x,
                            int y,
                            int piece_size,
                            piecedef *p,
                            int operation,
                            int size,
                            draw_block_func fn)
{
    int i,j,ax,ay;
    ay = board_y0 + y*size;

    for (i=0;i<piece_size;i++) {
        ax= board_x0 + x*size;

        for (j=0;j<piece_size;j++) {
            if ((*p)[i][j]) {
                if (operation==OPERATION_CLEAR) {
                    fn(gfx , ax, ay, 0);
                } else {
                    fn( gfx, ax, ay, (*p)[i][j]);
                }
            }
            ax+=size;
        }
        ay+=size;
    }
}

static void draw_piece(ScreenDrawer*gfx, int x, int y, int piecesize, piecedef *p, int operation)
{
    draw_piece_impl(gfx, x, y, piecesize, p, operation, BLOCKSIZE, &draw_block );
}


static void update_score(ScreenDrawer *gfx)
{
#if 0
    char tscore[16];

    textrendersettings_t render;

    if (NULL==font)
        font = font_find("thumb");
    if (NULL==font)
        return;

    render.font = font;
    render.w = 32;
    render.h = 7;
    render.align = ALIGN_LEFT;
    render.wrap = 0;
    render.direction = T_ROTATE;
    os_sDEBUG(tscore,"%d",score);
    drawText(gfx,&render, 0, 0, tscore, CRGB(0xff,0xff,0xff), 0x00);
#endif
}

static void board_draw(ScreenDrawer *gfx)
{
#if 1
    int x,y;

    //draw_hline(gfx, 0, board_y0-1, gfx->height, 0xffffff);

    for (x=0;x<BLOCKS_X;x++) {
        for (y=0;y<BLOCKS_Y;y++) {
            if (area[x][y]) {
                draw_block(gfx,
                           board_x0+x*BLOCKSIZE,
                           board_y0+y*BLOCKSIZE,
                           area[x][y]);
            }
        }
    }
#endif
}

void Tetris::setup_game()
{
    currentpiece = NULL;
    nextpiece=getRandomPiece();
    score = 0;
}

static piecedef * get_current_piecedef()
{
    return currentpiece->layout[currentrotate];
}


static void rotatepiece(ScreenDrawer*gfx)
{
    uint8_t nextrotate = (currentrotate+1)&0x3;
    DEBUG("Current rotate %d next %d\n", currentrotate, nextrotate);
    if (can_place(area, px,py,currentpiece, nextrotate)) {
        draw_piece( gfx,px, py, currentpiece->size, get_current_piecedef(), OPERATION_CLEAR);
        currentrotate = nextrotate;
        draw_piece( gfx,px, py, currentpiece->size, get_current_piecedef(), OPERATION_DRAW);
    } else {
        DEBUG("Cannot rotate\n");
    }
}

static int did_make_line(int start)
{
	int x,y;
	for (y=start; y>=0;y--) {
		int count = 0;

		for (x=0;x<BLOCKS_X;x++) {
			if (area[x][y])
				count++;
		}
		if (count==BLOCKS_X) {
                    return y;
		}
	}
	return -1;
}

static int count_line(uint8_t xarea[BLOCKS_X][BLOCKS_Y], int y)
{
    int x;
    int count = 0;
    for (x=0;x<BLOCKS_X;x++) {
        if (xarea[x][y])
            count++;
    }
    return count;
}


static void draw_rect(ScreenDrawer*gfx, int x, int y, int w, int h, uint32_t color)
{
    int i,j;
    for (i=x; i<x+w; i++) {
        for (j=y;j<y+h;j++) {
            gfx->drawPixel(j,i,color);
        }
    }
}

static void draw_hline(ScreenDrawer*gfx, int x, int y, int w, uint32_t color)
{
    int i;
    for (i=x; i<x+w; i++) {
        gfx->drawPixel(y,i,color);
    }
}
unsigned sfxtick=0;
static void special_effects(ScreenDrawer*gfx)
{
    int8_t *line = &completed_lines[0];
    uint32_t color;
    while (*line>0) {
        sfxtick++;
        printf("SFX!!!! %d\n", sfxtick);
        unsigned offsety = board_y0 + ((*line) * BLOCKSIZE);
        if (linedone_counter & LINEDONE_SELECTOR) {
            color = CRGB(0xFF,0xFF,0x00);
        } else {
            color = CRGB(0xFF,0x00,0x00);
        }
        draw_rect( gfx, board_x0, offsety, BLOCKS_X*BLOCKSIZE, BLOCKSIZE, color);
        line++;
    }
}

static void draw_block(ScreenDrawer *gfx, int x, int y, uint8_t color_index)
{

    uint32_t color = colors[color_index & 0xf];

#ifdef SMALL_PIECES
    drawPixel(gfx, y, x, color);
    drawPixel(gfx, y+1, x, color);
    drawPixel(gfx, y, x+1, color);
    drawPixel(gfx, y+1, x+1, color);
#else
    gfx->drawPixel( y,   x, color);
    gfx->drawPixel( y+1, x, color);
    gfx->drawPixel( y+2, x, color);
    gfx->drawPixel( y,   x+1, color);
    gfx->drawPixel( y+1, x+1, color);
    gfx->drawPixel( y+2, x+1, color);
    gfx->drawPixel( y,   x+2, color);
    gfx->drawPixel(y+1, x+2, color);
    gfx->drawPixel(y+2, x+2, color);

#endif

}

static void draw_block_small(ScreenDrawer *gfx, int x, int y, uint8_t color_index)
{
    uint32_t color = colors[color_index & 0xf];
#ifdef SMALL_PIECES
    drawPixel(gfx, y, x, color);
#else
    gfx->drawPixel(y,   x, color);
    gfx->drawPixel(y+1, x, color);
    gfx->drawPixel(y,   x+1, color);
    gfx->drawPixel(y+1, x+1, color);
#endif
}


static void draw_table(ScreenDrawer *gfx)
{
    board_draw(gfx);
    update_score(gfx);
#if 0
    int x, y;
    for (x=0; x<BLOCKS_X; x++) {

        for (y=0; y<BLOCKS_X; y++) {

            int piece = area[x][y];

            if (piece==0)
                continue;

            draw_block( gfx, x, y, piece);

        }
    }
#endif
}

static void checklines()
{
    int y = BLOCKS_Y;
    int s = 30;
    int l = 0;
    while ((y=did_make_line(y))!=-1) {
        score+=s;
        s=s*2;
        //line_done(y);
        completed_lines[l] = y;
        printf("Line completed at %d\n",y);
        l++;
        y--;
        if (y<0)
            break;
    }
    completed_lines[l] = -1;
    lines_made = l;
    printf("Total lines: %d\n", l);

    if ( l > 0 ) {
        printf("Lines completed: %d\n",l);
        linedone_counter = LINEDONE_COUNTER_MAX;
        game_state = LINEDONE;
    }
}

static void draw_next_piece(ScreenDrawer *gfx)
{
    draw_piece_impl(gfx, 11, -3, nextpiece->size, nextpiece->layout[0], OPERATION_DRAW, 2, &draw_block_small);
}

static void processEvent( ScreenDrawer *gfx, enum event_t ev )
{
    int nextx,nexty;
    bool checkcollision;

    if (currentpiece==NULL) {
//        DEBUG("K");
        return;
    }

    checkcollision=false;

    nextx = px;
    nexty = py;
    if (ev==event_rotate) {
        rotatepiece(gfx);
    }
    if (ev==event_left) {
        nextx=px-1;
    }
    if (ev==event_right) {
        nextx=px+1;
    }
    if (ev==event_down || tick==tickmax) {
        nexty=py+1;
        checkcollision=true;
    }

    if (can_place(area, nextx,nexty, currentpiece, currentrotate)) {
        draw_piece( gfx,px, py, currentpiece->size, get_current_piecedef(), OPERATION_CLEAR);
        py=nexty;
        px=nextx;
        draw_piece( gfx,px, py, currentpiece->size, get_current_piecedef(), OPERATION_DRAW);
    } else {
        if (checkcollision) {
            DEBUG("Piece is no longer\n");
            score+=7;
            do_place(area, px,py, currentpiece->size, get_current_piecedef());
            checklines();
            currentpiecevalid=false;
            py=0;
        }
    }

}

static void end_of_game(ScreenDrawer*gfx)
{
    DEBUG("End of game\n");
    game_state = START;
}

static void pre_game_play(ScreenDrawer*gfx)
{
    currentpiecevalid=false;

    area_init();
    board_draw(gfx);
    DEBUG("Pre game\n");
    score=0;
    lines=0;
    lines_total=0;
}


static void game_linedone(ScreenDrawer*gfx)
{
    DEBUG("Doing SFX %u\n", linedone_counter);
    special_effects(gfx);
    linedone_counter--;
    if (linedone_counter==0) {
        int y = BLOCKS_Y;
        while ((y=did_make_line(y))!=-1) {
            line_done(y);
        }
        completed_lines[0] = -1;
        game_state = PLAY;
    }
}

static void game_start(ScreenDrawer*gfx)
{

//    enum event_t ev = hasEvent(gfx);
    //    if (1 || ev==event_rotate) {
    srand(time(NULL));
    if (1) {
        DEBUG("Starting game\n");
        game_state = PLAY;
        pre_game_play(gfx);
    }
}

static void nextpieceevent(void);

static void game_play(ScreenDrawer*gfx)
{
    if (!currentpiecevalid) {

        py=0;
        currentpiece=nextpiece;
        currentrotate=0;

        nextpieceevent();

        nextpiece=getRandomPiece();
        currentpiecevalid=true;

        px=(BLOCKS_X/2)-2 + nextpiece->x_offset;

        if (!can_place(area, px,py,currentpiece,currentrotate)) {
            // End of game
            end_of_game(gfx);
            return;
        }
        draw_piece( gfx, px, py, currentpiece->size, get_current_piecedef(), OPERATION_DRAW);
    } else {
        draw_piece( gfx, px, py, currentpiece->size, get_current_piecedef(), OPERATION_DRAW);
    }
    draw_next_piece(gfx);
}

static void draw_pallete(ScreenDrawer*gfx)
{
    int x, y;
    uint32_t color = 0;
    for (x=0; x<32;x+=2) {
        for (y=0; y<32; y+=2) {
            gfx->drawPixel(board_y0+y,  x, color);
            gfx->drawPixel(board_y0+y+1, x, color);
            gfx->drawPixel(board_y0+y,   x+1, color);
            gfx->drawPixel(board_y0+y+1,x+1, color);
            color++;
        }
    }
    enum event_t ev = hasEvent();
    //if (ev != event_none) {
        game_state = START;
   // }
}


static loopfunc loop_functions[] =
{
    &draw_pallete,
    &game_start,
    &game_play,
    &game_linedone
};

void Tetris::game_render(ScreenDrawer*gfx)
{
    draw_table(gfx);

}

void Tetris::game_loop(ScreenDrawer*gfx)
{
    //gfx_clear(gfx);
    draw_table(gfx);
    special_effects(gfx);
}

int isDown()
{
    return 0;
}

int isRotate()
{
    return (random()&0xff) > 250;
}

int isLeft()
{
    return (random()&0xff) > 250;
}
int isRight()
{
    return (random()&0xff) > 250;
}

void Tetris::game_event_check(ScreenDrawer*gfx)
{
    loop_functions[game_state](gfx);

    if (game_state==PLAY) {
        enum event_t ev = ai_play();
        //enum event_t ev = hasEvent();
        tick++;

        if (tick==tickmax) {
            tick=0;
            processEvent(gfx,event_down);
        } else {

            if (ev!=event_none) {
                processEvent(gfx,ev);
            }
        }
    }
}

static bool evaluated = false;

static void nextpieceevent(void)
{
    evaluated = false;
}

static int bestscore;
static int bestx;
static int bestrot;

void score_pushdown(const int x, int y, int rot)
{
    uint8_t parea[BLOCKS_X][BLOCKS_Y];

    while (can_place(area, x,y+1, currentpiece, rot)) {
        y++;
    }
    DEBUG("Stop at %d, %d\n", x, y);
    // Place and evaluate
    memcpy(&parea, area, sizeof(area));
    do_place(parea, x,y, currentpiece->size,
             currentpiece->layout[rot]
            );
    // Evaluate score
    unsigned lines_done = 0;
    unsigned top_line;

    int yy,xx;

    for (yy=BLOCKS_Y-1;yy>0;yy--) {
        int r =count_line(parea, yy);
        if (r==0) {
          //  DEBUG("Line at %d empty\n", y);
            top_line=yy;
            break;
        }
        if (r==BLOCKS_X)
            lines_done++;
        //DEBUG("Line %d: %d\n", y, r);
    }
    int score = -10 * (BLOCKS_Y-top_line);

    DEBUG("BLOCK LINE %d score %d\n",top_line, score);

    score += 20*((int)lines_done);
    // Prefer pieces on edges.
    score += abs( x - (BLOCKS_X/2));

    // Add negatives for each empty with somethinf on top
    for (yy=BLOCKS_Y-1;yy>1;yy--) {
        if (count_line(parea,yy)==0)
            break;
        for (xx=0;xx<BLOCKS_X;xx++) {
            if (parea[xx][yy]==0 && (parea[xx][yy-1]!=0)) {
                DEBUG("Negative add due to empty area at %d %d\n",xx,yy);
                score-=10;
            }
        }
    }
    DEBUG("Current score: %d\n", score);

    //
    // ASCII TABLE
    {
        int ax, ay;
        for (ay=0;ay<BLOCKS_Y;ay++) {
            DEBUG("|");
            for (ax=0;ax<BLOCKS_X;ax++) {
                if (parea[ax][ay]!=0) {
                    DEBUG("X");
                } else {
                    DEBUG(" ");
                }
            }
            DEBUG("|\n");
        }
        for (ax=0;ax<BLOCKS_X+2;ax++) {
            DEBUG("=");
        }
        DEBUG("\n");
    }



    if (score>bestscore) {
        bestx = x;
        bestrot = rot;
        bestscore = score;
    }
}



int check_score_if_move(int deltax, int x, int y, int rot, std::vector<std::pair<int,int>> &validpos)
{

    if (can_place(area, x+deltax, y, currentpiece, rot)) {
        DEBUG("Valid pos %d %d\n", x+deltax, y);
        validpos.push_back(std::pair<int,int>(x+deltax, rot));
        check_score_if_move(deltax, x+deltax, y, rot, validpos);
    } else {
        DEBUG("CANNOT pos %d %d\n", x+deltax, y);
    }
}

void check_scores(std::vector<std::pair<int,int> > &validpos, int y)
{
    for (auto t: validpos) {
        DEBUG("BEGIN EVAL %d %d rot %d\n", t.first,y, t.second);
        score_pushdown(t.first, y, t.second);
    }
}

Tetris::Tetris()
{
    ai_busy=false;
}
event_t Tetris::ai_play()
{
    int x = px;
    int y = py;
    int rot = currentrotate;
    do {
        if (currentpiece!=NULL && evaluated==false && ai_busy==false) {
            // Check if we can rotate fully
            if (can_place(area, x, y, currentpiece, 1) &&
                can_place(area, x, y, currentpiece,  2) &&
                can_place(area, x, y, currentpiece,  3)) {

                DEBUG("FULL ROTATE***********\n");
            } else {
                break;
            }
            ai_busy=true;


            DEBUG("Evaluating\n");
            int r;
            std::vector<std::pair<int,int>> validpos;

            for (r=0; r<4; r++) {
                validpos.push_back(std::pair<int,int>(x,r));
                DEBUG("Check <<<\n");
                check_score_if_move(-1, x, y, r, validpos);
                DEBUG("Check >>>\n");
                check_score_if_move(+1, x, y, r, validpos);
            }

            DEBUG("Valid positions: %d\n", validpos.size());
            bestscore = INT_MIN;
            check_scores(validpos, y);
            evaluated=true;
            ai_busy=false;
        }
        if (currentpiece && evaluated) {
            DEBUG("Target X %d px %d\n", bestx, px);
            if (bestrot!=rot) {
                return event_rotate;
            }
            if (bestx>px)
                return event_right;
            if (bestx<px)
                return event_left;
            return event_down;
        }
    } while(0);
    return event_none;
}