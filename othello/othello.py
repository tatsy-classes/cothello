from __future__ import annotations

import os
import enum
from typing import List, Tuple

import numpy as np
import numpy.typing as npt
import matplotlib.pyplot as plt
from PIL import Image, ImageDraw
from IPython.core.pylabtools import print_figure

# isort: off
import othello.bitboard as bb

# isort: on

Board = npt.NDArray[np.int32]

__all__ = ["Player", "Move", "Env", "make"]


class Player(enum.IntEnum):
    BLACK = 1
    WHITE = -1

    def __neg__(self) -> Player:
        return Player(-self.value)

    def __str__(self) -> str:
        return self.name

    def __repr__(self) -> str:
        return self.name


class Move(object):
    def __init__(self, player: Player, x: int, y: int) -> None:
        assert 0 <= x < 8 and 0 <= y < 8
        self.player = player
        self.x = x
        self.y = y

    def is_pass(self):
        return self.x < 0 and self.y < 0

    @staticmethod
    def make_pass(player: Player) -> Move:
        move = Move(player, 0, 0)
        move.x, move.y = -1, -1
        return move

    def __eq__(self, other) -> bool:
        if self.player != other.player:
            return False
        if self.x != other.x:
            return False
        return self.y == other.y

    def __repr__(self) -> str:
        return str((self.player.name, self.x + 1, self.y + 1))

    def tosym(self) -> str:
        if self.is_pass():
            return ""
        return chr(ord("A") + self.x) + chr(ord("1") + self.y)


class Env(object):
    def __init__(self) -> None:
        self.player: Player = Player.BLACK
        self.b_board: np.uint64 = np.uint64(0x0000_0000_0000_0000)
        self.w_board: np.uint64 = np.uint64(0x0000_0000_0000_0000)
        self.board_history: List[Tuple[np.uint64, np.uint64]] = []
        self.move_history: List[Move] = []

    def copy(self) -> Env:
        env = Env()
        env.player = self.player
        env.b_board = self.b_board
        env.w_board = self.w_board
        return env

    def reset(self) -> None:
        self.player = Player.BLACK
        self.b_board = np.uint64(0x0000_0008_1000_0000)
        self.w_board = np.uint64(0x0000_0010_0800_0000)

    def is_done(self) -> bool:
        return bb.c_is_done(self.b_board, self.w_board)

    @property
    def board(self) -> Board:
        B = bb.c_bits_to_board(self.b_board)
        W = bb.c_bits_to_board(self.w_board)
        return B - W

    def count(self, player: Player) -> int:
        if player == Player.BLACK:
            return bb.c_bit_count(self.b_board)
        else:
            return bb.c_bit_count(self.w_board)

    def is_win(self, player: Player) -> bool:
        return self.count(player) > self.count(-player)

    def is_lose(self, player: Player) -> bool:
        return not self.is_win(player)

    def undo(self) -> None:
        self.player = -self.player
        self.b_board, self.w_board = self.board_history.pop()
        self.move_history.pop()

    def update(self, move: Move) -> None:
        """Update othello board by a move"""
        self.move_history.append(move)
        self.board_history.append((self.b_board, self.w_board))

        if self.player == Player.BLACK:
            b0, b1 = self.b_board, self.w_board
        else:
            b0, b1 = self.w_board, self.b_board

        sym = move.tosym()
        if sym != "":
            put = bb.c_coordinates_to_bits(sym)
            b0, b1 = bb.c_reverse(put, b0, b1)

        if self.player == Player.BLACK:
            self.player = Player.WHITE
            self.b_board, self.w_board = b0, b1
        else:
            self.player = Player.BLACK
            self.w_board, self.b_board = b0, b1

    def legal_moves(self) -> List[Move]:
        """List legal moves"""
        if self.player == Player.BLACK:
            b0, b1 = self.b_board, self.w_board
        else:
            b0, b1 = self.w_board, self.b_board

        bits = bb.c_make_legal_board(b0, b1)
        if bits == 0:
            return [Move.make_pass(self.player)]

        legal = bb.c_bits_to_board(bits)
        ys, xs = np.where(legal != 0)
        return [Move(self.player, x, y) for x, y in zip(xs, ys)]

    def render(self) -> npt.NDArray[np.uint8]:
        size = 1024  # size of board
        csize = size // 8  # size of cell
        margin = 10  # margin from disk to grid line

        # Initialize board with green color
        img = Image.new("RGB", (size, size), color="#007700")
        draw = ImageDraw.Draw(img)

        # If board history exists, highlight last move
        if len(self.move_history) > 0:
            last_move = self.move_history[-1]
            i, j = last_move.x, last_move.y
            if 0 <= i < 8 and 0 <= j < 8:
                x = i * csize
                y = j * csize
                draw.rectangle((x, y, x + csize, y + csize), fill="#00aa00")

        # Grid lines
        for i in range(1, 8):
            y = i * csize
            draw.line((0, y, size, y), "black", width=3)

        for j in range(1, 8):
            x = j * csize
            draw.line((x, 0, x, size), "black", width=2)

        # Stars
        stars = [
            (csize * 2 - 10, csize * 2 - 10, csize * 2 + 10, csize * 2 + 10),
            (csize * 2 - 10, csize * 6 - 10, csize * 2 + 10, csize * 6 + 10),
            (csize * 6 - 10, csize * 2 - 10, csize * 6 + 10, csize * 2 + 10),
            (csize * 6 - 10, csize * 6 - 10, csize * 6 + 10, csize * 6 + 10),
        ]
        for st in stars:
            draw.ellipse(st, fill="black")

        # Disks
        B = bb.c_bits_to_board(self.b_board)
        W = bb.c_bits_to_board(self.w_board)
        for i in range(8):
            for j in range(8):
                px = j * csize + margin
                py = i * csize + margin
                diameter = csize - margin * 2
                xy = (px, py, px + diameter, py + diameter)
                if B[i, j] != 0:
                    draw.ellipse(xy, fill="black")
                if W[i, j] != 0:
                    draw.ellipse(xy, fill="white")

        return np.array(img, dtype="uint8")

    def __repr__(self) -> str:
        B = bb.c_bits_to_board(self.b_board)
        W = bb.c_bits_to_board(self.w_board)

        sep = "+-" * 8 + "+" + os.linesep
        ret = ""
        ret += sep
        for i in range(self.board.shape[0]):
            ret += "|"
            for j in range(self.board.shape[1]):
                if B[i, j] != 0:
                    ret += "o"
                elif W[i, j] != 0:
                    ret += "x"
                else:
                    ret += " "
                ret += "|"
            ret += os.linesep
            ret += sep
        return ret

    def __dir__(self) -> List[str]:
        return [
            "copy",
            "reset",
            "update",
            "undo",
            "legal_moves",
            "render",
            "count",
            "is_done",
            "is_win",
            "is_lose",
        ]

    def _repr_png_(self):
        fig, ax = plt.subplots(figsize=(6, 6), dpi=100)
        img = self.render()
        size = img.shape[0]
        csize = size // 8

        ax.imshow(img)
        ax.xaxis.tick_top()
        ax.set(
            xticks=np.arange(csize // 2, size, csize),
            yticks=np.arange(csize // 2, size, csize),
            xticklabels=[chr(ord("A") + i) for i in range(8)],
            yticklabels=np.arange(1, 9),
        )
        data = print_figure(fig, "png")
        plt.close(fig)
        return data


def make() -> Env:
    return Env()
