#ifndef _OTHELLO_ACTION_H_
#define _OTHELLO_ACTION_H_

#include "player.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

class Action {
public:
    Action() {
    }

    Action(Player player, int x, int y)
        : player{ player }
        , x{ x }
        , y{ y } {
        if (x < 0 || x >= 8 || y < 0 || y >= 8) {
            throw std::runtime_error("Invalid coordinates");
        }
    }

    int getX() const {
        return x;
    }

    int getY() const {
        return y;
    }

    int getPlayer() const {
        return player.getValue();
    }

    bool operator==(const Action &other) const {
        if (player != other.player) {
            return false;
        }
        if (x != other.x) {
            return false;
        }
        return y == other.y;
    }

    std::string str() const {
        std::ostringstream oss;
        const char c[1] = { (char)('A' + y) };
        oss << player.str() << " ";
        oss << (int)(x + 1) << std::string(c);
        return oss.str();
    }

    std::string repr() const {
        return str();
    }

    std::string toSymbol() const {
        std::ostringstream oss;
        oss << char('A' + x) << char('1' + y);
        return oss.str();
    }

private:
    Player player;
    int x;
    int y;
};

#endif  // _OTHELLO_ACTION_H_
