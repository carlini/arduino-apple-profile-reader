## Copyright (C) 2023 Nicholas Carlini.
## 
## This program is free software: you can redistribute it and/or modify  
## it under the terms of the GNU General Public License as published by  
## the Free Software Foundation, version 3.
##
## This program is distributed in the hope that it will be useful, but 
## WITHOUT ANY WARRANTY; without even the implied warranty of 
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License 
## along with this program. If not, see <http://www.gnu.org/licenses/>.
##

import numpy as np

def popcnt(x):
    return bin(x).count('1')

disk_img = []
for line in open("profile.bin"):
    if line[0] == '<':
        got = line[1:-2]
        got = [(ord(a)-0x40) + ((ord(b)-0x40)<<4)for a,b in zip(got[::2], got[1::2])]

        # verify the parity bits
        assert np.all((np.array(list(map(popcnt, got))) & 1) == 1)
        got = np.array(got)&0xFF

        # verify no errors
        assert np.all(got[:4] == 0)
        got = got[4:]
        
        got = np.array(got, dtype=np.uint8)
        disk_img.append(got)

disk_img = np.concatenate(disk_img)
print(disk_img.shape)
open("disk.img", "wb").write(disk_img.tobytes())
