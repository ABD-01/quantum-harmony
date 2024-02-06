from random import randint, randbytes
import sys
import os.path as osp

def addBytes(filepath):
    print("\033[94m[Before] File Size is:", osp.getsize(filepath), "\033[0m")
    numBytes = randint(10,20)
    print(f"Adding \033[96m\033[1m{numBytes}\033[0m\033[0m random bytes to the end of file")
    rbytes = randbytes(numBytes)
    print("Random Bytes Added:\n\033[92m{}\033[0m".format(rbytes.hex(' ').upper()))
    with open(filepath, 'ab') as f:
        f.write(rbytes)
    print("\033[94m[After ] File Size is:", osp.getsize(filepath), "\033[0m")


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <filepath>")
        sys.exit(1)
    filepath = sys.argv[1]
    if not osp.exists(filepath):
        print(f"File: {filepath} does not exist.")
        sys.exit(0)
    addBytes(filepath)