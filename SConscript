from building import *

cwd = GetCurrentDir()
src	= Glob('*.c')

CPPPATH = [cwd]

group = DefineGroup('uC-CRC', src, depend = ['PKG_USING_UC_CRC'], CPPPATH = CPPPATH)

Return('group')
