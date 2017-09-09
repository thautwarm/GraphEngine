import GEKV
from ctypes import *
GEKV.init_gekv("GEKV_Storage", 64)
cellId = 1
size = 1024
buff = 'Hello'
cellType = 5
GEKV.save_cell(cellId, buff, size, cellType)
GEKV.save_storage()
GEKV.load_storage()
buff = 'World'
GEKV.update_cell(cellId, buff, size)
cell = GEKV.load_cell(cellId)
print(cell)
