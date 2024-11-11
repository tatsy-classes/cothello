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

    bool isPass() const {
        return x < 0 && y < 0;
    }

    static Action makePass(const Player &player) {
        Action action(player, 0, 0);
        action.x = -1;
        action.y = -1;
        return action;
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
        oss << player.str() << " ";
        if (this->isPass()) {
            oss << "PASS";
        } else {
            oss << x + 1 << " " << y + 1;
        }
        return oss.str();
    }

    std::string repr() const {
        return str();
    }

    std::string toSymbol() const {
        if (isPass()) {
            return "";
        }
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
