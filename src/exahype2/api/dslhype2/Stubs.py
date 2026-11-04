from __future__ import annotations

class Integer:
    def __init__(self, value: int | str):
        pass

class DataBlock:
    def __init__(self, dimensions: list[list[int | Integer]], source: str):
        pass

    def __getitem__(self, index: tuple[slice | int, ...]) -> DataBlock:
        return DataBlock([[]], "")
