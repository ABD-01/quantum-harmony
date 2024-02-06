import sys
import random
import os.path as osp

def delete_bytes(file_path):
    with open(file_path, 'rb') as file:
        content = bytearray(file.read())

    file_size = len(content)

    num_places_to_delete_from = random.randint(2,8)
    positions_to_delete = random.sample(range(file_size-2047), num_places_to_delete_from)

    num_bytes_deleted = 0
    for position in sorted(positions_to_delete, reverse=True):
        num_bytes_to_delete = random.randint(256,2046)
        if num_bytes_to_delete > len(content):
            print("\033[91mNot enough bytes available.\033[0m")
            continue
        num_bytes_deleted += num_bytes_to_delete
        del content[position:position + num_bytes_to_delete]
        print("Deleting \033[96m\033[1m{1}\033[0m\033[0m bytes from offset \033[96m\033[1m{0}\033[0m\033[0m.".format(position, num_bytes_to_delete))

    prompt = None
    while (prompt := input(f"Delete {num_bytes_deleted} bytes?(y/n)\n")) not in ("yes", "y", "n", "no"):
        continue
    if prompt in ("no", "n"):
        print("\033[93mAborting.\033[0m")
        return 0


    with open(file_path, 'wb') as file:
        file.write(content)

    return num_bytes_deleted

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <filepath>")
        sys.exit(1)
    filepath = sys.argv[1]
    if not osp.exists(filepath):
        print(f"File: {filepath} does not exist.")
        sys.exit(0)
    print("\033[94m[Before] File Size is:", osp.getsize(filepath), "\033[0m")
    print("\033[92mDeleted \033[1m{1}\033[0m\033[92m Bytes from {0}\033[0m".format(filepath, delete_bytes(filepath)))
    print("\033[94m[After ] File Size is:", osp.getsize(filepath), "\033[0m")