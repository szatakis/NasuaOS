#pragma once

// Dodaje komendę do historii
void history_add(const char* cmd);

// Resetuje nawigację (wywoływane po wciśnięciu Enter)
void history_reset_nav();

// Zwraca starszą komendę (Shift + Up)
// Jeśli nie ma starszej, zwraca nullptr.
const char* history_navigate_up();

// Zwraca nowszą komendę (Shift + Down)
// Jeśli wróciliśmy na sam dół (do pustej linii), zwraca puste słowo "".
// Jeśli nie ma nic do zrobienia, zwraca nullptr.
const char* history_navigate_down();