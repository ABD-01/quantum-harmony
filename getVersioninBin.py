import re
import sys

def getVersionFromBin(filepath):
    # similar to grep -a <input_file> "FIRMWARE_VER"
    pattern = re.compile(br'\b\w+_FIRMWARE_VER (?P<version>\d+\.\d+\.\d+)') 
    with open(filepath, 'rb') as binary_file:
        for line in binary_file:
            match = pattern.search(line)
            if match:
                print(line)
                version = match.group('version').decode('utf-8')
                return str(version)
    return ''

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python3 getVersioninBin.py <filepath>")
        sys.exit(1)
    filepath = sys.argv[1]
    version = getVersionFromBin(filepath)
    print(version)