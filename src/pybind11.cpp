#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "player.h"
#include "action.h"
#include "env.h"

template <typename T, int Width, int Height>
py::array_t<T> to_numpy(const std::array<T, Width * Height> &data) {
    return py::array_t<T>({ Width, Height }, data.data());
}

PYBIND11_MODULE(libcpp, m_cpp) {
    // Player
    auto cls_player = py::class_<Player>(m_cpp, "Player");
    cls_player.def(py::init<int>())
        .def_property_readonly("name", &Player::str)
        .def_property_readonly_static("BLACK", [](py::object) { return (int)Player::BLACK; })
        .def_property_readonly_static("WHITE", [](py::object) { return (int)Player::WHITE; })
        .def_static("as_black", [](py::object) { return Player(Player::BLACK); })
        .def_static("as_white", [](py::object) { return Player(Player::WHITE); })
        .def("is_black", &Player::isBlack)
        .def("is_white", &Player::isWhite)
        .def("__eq__", &Player::operator==)
        .def("__ne__", &Player::operator!=)
        .def("__neg__", [](const Player &self) { return -self; })
        .def("__str__", &Player::str)
        .def("__repr__", &Player::repr);

    // Action
    auto cls_action = py::class_<Action>(m_cpp, "Action");
    cls_action.def(py::init<Player, int, int>())
        .def("is_pass", &Action::isPass)
        .def("to_symbol", &Action::toSymbol)
        .def_static("make_pass", &Action::makePass)
        .def_property_readonly("player", &Action::getPlayer)
        .def_property_readonly("x", &Action::getX)
        .def_property_readonly("y", &Action::getY)
        .def("__str__", &Action::str)
        .def("__repr__", &Action::repr);

    // Env
    auto cls_env = py::class_<Env>(m_cpp, "CEnv");
    cls_env.def(py::init<>())
        .def("copy", &Env::copy)
        .def("reset", &Env::reset)
        .def("is_done", &Env::isDone)
        .def_property_readonly("board", [](const Env &self) { return to_numpy<int, 8, 8>(self.board()); })
        .def("count", [](const Env &self, int player) { return self.count(Player(player)); })
        .def("is_win", &Env::isWin)
        .def("is_lose", &Env::isLose)
        .def("undo", &Env::undo)
        .def("update", &Env::update)
        .def("legal_actions", &Env::legalActions)
        .def_property_readonly("player", &Env::getPlayer)
        .def_property_readonly("history_size", &Env::historySize)
        .def_property_readonly("last_action", &Env::getLastAction);

    // Bitboard
    auto m_bb = m_cpp.def_submodule("bitboard", "Bitboard submodule");
    m_bb.def("c_coordinates_to_bits", &coordinatesToBits);
    m_bb.def("c_bits_to_board", &bitsToBoard);
    m_bb.def("c_make_legal_board", &makeLegalBoard);
    m_bb.def("c_can_put", &canPut);
    m_bb.def("c_reverse", &reverse);
    m_bb.def("c_is_pass", &isPass);
    m_bb.def("c_is_done", &isDone);
    m_bb.def("c_bit_count", &bitCount);
}
