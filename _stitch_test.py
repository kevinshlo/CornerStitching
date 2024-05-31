#!/usr/bin/env python3

import unittest
from typing import List, Tuple, overload

from _stitch_pytest import *


class StitchTest(unittest.TestCase):
    def check_all(self, s: Stitch):
        self.assertEqual(0, pytest_check_neighbors(s))
        self.assertEqual(0, pytest_check_tiles(s))
        self.assertEqual(0, pytest_check_strip(s))

    def __init__(self, methodName: str = "runTest") -> None:
        super().__init__(methodName)
        self.tiles = [
            (Pt(coord), Pt(size))
            for coord, size in [
                ((15, 2), (5, 5)),
                ((20, 2), (8, 2)),
                ((4, 5), (5, 8)),
                ((11, 11), (8, 4)),
                ((19, 14), (6, 4)),
                ((7, 18), (6, 4)),
            ]
        ]

    def init(self) -> Stitch:
        s = Stitch(Pt(0, 0), Pt(30, 24))
        self.assertEqual(1, len(pytest_tiles(s)))
        self.check_all(s)
        return s

    def test_init(self):
        self.init()

    def insert(self) -> Tuple[Stitch, List[Tile | None]]:
        s = self.init()
        ts = []
        for coord, size in self.tiles:
            ts.append(s.insert(coord, size))
            self.check_all(s)
        return s, ts

    def test_insert(self):
        self.insert()

    def delete(self) -> Stitch:
        s, ts = self.insert()
        for t in ts:
            if t is not None:
                s.delete(t)
            self.check_all(s)
        return s

    def test_delete(self):
        self.delete()


if __name__ == "__main__":
    unittest.main()
