import sys
from rich import print
from rich.console import Console
from rich.progress import Progress
import os

console = Console()

def crc32_calc(running_crc, data):
    for byte in data:
        running_crc ^= (byte << 24)
        for _ in range(8):
            msb = running_crc >> 31
            running_crc = (running_crc << 1) & 0xFFFFFFFF
            running_crc ^= (0 - msb) & 0x04C11DB7
    return running_crc

def get_file_crc(file_path, buffer_size=4096):
    running_crc = 0xFFFFFFFF 

    with open(file_path, "rb") as file:
        while (data := file.read(buffer_size)):
            running_crc = crc32_calc(running_crc, data)

    running_crc = running_crc & 0xFFFFFFFF
    return running_crc

def verify_crc(file_path):
    contents = open(file_path, "rb").read()
    file_crc = contents[-4:]
    console.print(f"Extracted File CRC: {file_crc.hex(' ').upper()}", style="blue")

    running_crc = 0xFFFFFFFF
    running_crc = crc32_calc(running_crc, contents[:-4])
    running_crc = running_crc & 0xFFFFFFFF
    running_crc = running_crc.to_bytes(4, byteorder='big')
    if running_crc == file_crc:
        console.print(f"File CRC verified: {running_crc.hex(' ').upper()}", style="green")
        console.print(f"Decimal Value: {int.from_bytes(running_crc, byteorder='big')}", style="green")
    else:
        console.print(f"File CRC verification failed: {running_crc.hex(' ').upper()}", style="red")


def appendCRC(file_path):
    try:
        console.print(f"File Size (Before): {os.path.getsize(file_path)}", style="bold magenta")
        with console.status("[bold blue] Calculating CRC"):
            crc = get_file_crc(file_path)
            console.print(f"File CRC: {crc.to_bytes(4, byteorder='big').hex(' ').upper()}", style="green")

            if crc == 0:
                console.print(f"File CRC already exists as the trailing 4 bytes", style="bold red")
            else:
                with open(file_path, "ab") as file:
                    file.write(crc.to_bytes(4, byteorder='big'))

        console.print(f"File Size (After): {os.path.getsize(file_path)}", style="bold magenta")

        with console.status("[bold blue] Verifying CRC"):
            verify_crc(file_path)

    except FileNotFoundError:
        console.print(f"File '{file_path}' does not exist.", style="bold red")
    except Exception as e:
        console.print_exception()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("[bold magenta] Usage: {0} <file_path>".format(sys.argv[0]))
    else:
        file_path = sys.argv[1]
        appendCRC(file_path)

    input("\nPress any key to exit...")
