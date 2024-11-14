#ifndef _OTHELLO_PLAYER_H_
#define _OTHELLO_PLAYER_H_

#include <iostream>
#include <string>

class Player {
public:
    enum { BLACK = 1, WHITE = -1 };

    Player() {
    }

    explicit Player(int value)
        : value(value) {
    }

    int getValue() const {
        return value;
    }

    bool isBlack() const {
        return value == BLACK;
    }

    bool isWhite() const {
        return value == WHITE;
    }

    bool operator==(const Player &other) const {
        return value == other.value;
    }

    bool operator!=(const Player &other) const {
        return value != other.value;
    }

    Player operator-() const {
        return Player(-value);
    }

    std::string str() const {
        return value == Player::BLACK ? "BLACK" : "WHITE";
    }

    std::string repr() const {
        return str();
    }

private:
    int value;
};

#endif  // _OTHELLO_PLAYER_H_
