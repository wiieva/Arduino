
#include "WiievaPlotter.h"
#include <math.h>

void WiievaPlotter::begin () {
    labelsFont = gdispOpenFont("fixed_5x8");
    viewWidth = gdispGetWidth () - 20;
}

WiievaPlotter::~WiievaPlotter () {
    end ();
}

void WiievaPlotter::end () {
    gdispCloseFont (labelsFont);
    labelsFont = nullptr;
    for (int g = 0; g < graphs; ++g) {
        delete [] vals[g];
        vals[g] = nullptr;
        delete [] prevVals[g];
        prevVals[g] = nullptr;

    }
    graphs = 0;
}

size_t WiievaPlotter::write(uint8_t c) {
    if (c == '\n') {
        lineBuf[linePos] = 0;
        linePos = 0;
        parseLine ();
    } else {
        lineBuf[linePos++] = c;
        if (linePos + 1 >= sizeof (lineBuf))
            linePos = 0;
    }
}

void WiievaPlotter::setMode (WiievaPlotter::Mode _mode) {
    mode = _mode;
}

void WiievaPlotter::parseLine () {

    char *p = lineBuf;
    for (int g = 0; g < maxGraphs; ++g) {
        char *pstart = p;
        int val = 0;
        while (*pstart == ' ' || *pstart == '\t') ++pstart; 
        float f = strtod (pstart,&p);
        if (p != pstart) {
            val = (int)(f * fixPointMul);
            if (g+1 > graphs) {
                graphs = g+1;
                vals[g] = new int [viewWidth+1];
                memset (vals[g],0,sizeof(int)*(viewWidth+1));
            }
        }
        if (*p != '\0')
            ++p;
        if (vals[g])
            vals[g][points] = val;
    }
    if (points < viewWidth) {
        points++;
    } else {
        if (mode == Plotter) {
            for (int g = 0; g < graphs; ++g)
                memmove (&vals[g][0],&vals[g][1],(points)*sizeof (vals[g][0]));
        } else {
            draw ();
            for (int g = 0; g < graphs; ++g)
                memmove (&vals[g][0],&vals[g][points],sizeof (vals[g][0]));
            points = 1;
        }
    }
    totalPoints++;
    if (mode == Plotter)
        draw ();
}

void WiievaPlotter::draw () {
    int h = gdispGetHeight (), w = gdispGetWidth ();
    const int m = 10,st = 40;

    int oldLowerBound = lowerBound;
    int oldHigherBound = higherBound;

    calcBounds ();
    bool boundsChanged = (oldLowerBound != lowerBound || oldHigherBound != higherBound);

    gdispDrawLine (m,m,m,h-m,HTML2COLOR(0xFFFFFF));
    gdispDrawLine (m,h-m,w-m,h-m,HTML2COLOR(0xFFFFFF));

    for (int y = m*2; y < h-m; y += st) {
        gdispDrawLine (m+1,h-y,w-m,h-y,HTML2COLOR(0x202020));
        char lbl[16];
        float v = map (h-y,h-m,m,lowerBound,higherBound)/fixPointMul;
        sprintf (lbl,"%.2f",v);
        if (boundsChanged)
            gdispFillArea (m+2,h-y-8,30,8,HTML2COLOR(0x000000));
        gdispDrawString (m+2,h-y-8,lbl,labelsFont,HTML2COLOR(0x800000));
    }

    int of = st-((mode==Plotter&&totalPoints>points)?(totalPoints%st):0);

    for (int x = 2; x < points; ++x) {
        if (x-1 >= of && ((x-1 - of) % st) == 0)
            gdispDrawLine (x+m,m,x+m,h-m-1,HTML2COLOR(0));

        for (int g = 0; g < graphs; ++g) {
            int *pVals = (mode==Plotter)?vals[g]-(totalPoints>points?1:0):prevVals[g];
            if (pVals) {
                int16_t _y1 = map (pVals[x-1],oldLowerBound,oldHigherBound,h-m-1,m);
                int16_t _y2 = map (pVals[x],oldLowerBound,oldHigherBound,h-m-1,m);
                gdispDrawLine (x-1+m,_y1,x+m-1,_y2,0);
            }
        }

        if (x >= of && ((x - of) % st) == 0)
            gdispDrawLine (x+m,m,x+m,h-m-1,HTML2COLOR(0x202020));

        for (int g = 0; g < graphs; ++g) {
            int16_t _y1 = map (vals[g][x-1],lowerBound,higherBound,h-m-1,m);
            int16_t _y2 = map (vals[g][x],lowerBound,higherBound,h-m-1,m);
            gdispDrawLine (x-1+m,_y1,x+m-1,_y2,HTML2COLOR(graphsColors[g]));
        }

        if (x >= of && ((x - of) % st) == 0) {
            char lbl[16];
            itoa (x-m+totalPoints-points,lbl,10);
            gdispFillStringBox (x+m-st/2,h-m+2,st,10,lbl,labelsFont,HTML2COLOR(0x800000),HTML2COLOR(0x000000),justifyCenter);
        }
    }

    if (mode == Oscilloscope) {
        for (int g = 0; g < graphs; ++g) {
            if (!prevVals[g])
                prevVals[g] = new int [viewWidth+3];
            memcpy (prevVals[g],vals[g],sizeof(int)*viewWidth+3);
        }
    }
    gfxYield ();
}

void WiievaPlotter::calcBounds () {
    for (int x = 0; x < points; ++x) {
        for (int g = 0; g < graphs; ++g) {
            if (vals[g][x] < lowerBound)
                lowerBound = vals[g][x];
            if (vals[g][x] > higherBound)
                higherBound = vals[g][x];
        }
    }
}
