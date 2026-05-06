#ifndef SGFWRITER_H
#define SGFWRITER_H

void write_move(const char* filename, int pos, int turn);
void write_game(const char* filename, const Game* game);
void write_input_from_sgf(const char* in, const char* out);

#endif  // SGFWRITER_H

