// Provide TextToFloat shim for raygui (removed from raylib 5.0)
#include <stdlib.h>

static float TextToFloat(const char *text) { return strtof(text, NULL); }
static int TextToInteger(const char *text) { return (int)strtol(text, NULL, 10); }

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
