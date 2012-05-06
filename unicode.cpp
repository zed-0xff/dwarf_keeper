#ifndef UNICODE_H
#define UNICODE_H

#include <string>

// http://dwarffortresswiki.org/index.php/Character_table
const char* CP437_TO_UTF8[] = {
        " ", "☺", "☻", "♥", "♦", "♣", "♠", "•", "◘", "○", "◙", "♂", "♀", "♪", "♫", "☼",
        "►", "◄", "↕", "‼", "¶", "§", "▬", "↨", "↑", "↓", "→", "←", "∟", "↔", "▲", "▼",
        " ", "!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/",
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?",
        "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O",
        "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[", "\\", "]", "^", "_",
        "`", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o",
        "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "|", "}", "~", "⌂",
        "Ç", "ü", "é", "â", "ä", "à", "å", "ç", "ê", "ë", "è", "ï", "î", "ì", "Ä", "Å",
        "É", "æ", "Æ", "ô", "ö", "ò", "û", "ù", "ÿ", "Ö", "Ü", "¢", "£", "¥", "₧", "ƒ",
        "á", "í", "ó", "ú", "ñ", "Ñ", "ª", "º", "¿", "⌐", "¬", "½", "¼", "¡", "«", "»",
        "░", "▒", "▓", "│", "┤", "╡", "╢", "╖", "╕", "╣", "║", "╗", "╝", "╜", "╛", "┐",
        "└", "┴", "┬", "├", "─", "┼", "╞", "╟", "╚", "╔", "╩", "╦", "╠", "═", "╬", "╧",
        "╨", "╤", "╥", "╙", "╘", "╒", "╓", "╫", "╪", "┘", "┌", "█", "▄", "▌", "▐", "▀",
        "α", "ß", "Γ", "π", "Σ", "σ", "µ", "τ", "Φ", "Θ", "Ω", "δ", "∞", "φ", "ε", "∩",
        "≡", "±", "≥", "≤", "⌠", "⌡", "÷", "≈", "°", "∙", "·", "√", "ⁿ", "²", "■", " ",
};

using namespace std;

string cp437_to_utf8(string src){
    string dst;
    const char*c;
    int l=src.size();
    dst.reserve(l + l/10);
    const unsigned char*p = (const unsigned char*) src.data();

    for(int i=0;i<l;i++){
        dst += CP437_TO_UTF8[p[i]];
    }
    return dst;
}

string url_escape(string src){
    string dst;
    const char*c;
    int l=src.size();
    dst.reserve(l + l/10);
    const unsigned char*p = (const unsigned char*) src.data();

    for(int i=0;i<l;i++){
        switch(p[i]){
            case ' ':
                dst += "+";
                break;
            default:
                dst += p[i];
                break;
        }
    }
    return dst;
}

string html_escape(string src){
    string dst;
    const char*c;
    int l=src.size();
    dst.reserve(l + l/10);
    const unsigned char*p = (const unsigned char*) src.data();

    for(int i=0;i<l;i++){
        switch(p[i]){
            case '<':
                dst += "&lt;";
                break;
            case '>':
                dst += "&gt;";
                break;
            case '&':
                dst += "&amp;";
                break;
            default:
                dst += p[i];
                break;
        }
    }
    return dst;
}

#endif
