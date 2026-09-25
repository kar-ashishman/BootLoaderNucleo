def parsehex(file: str, framechkseq: int = 0xAA) -> list:
    """ Parse a hex file and return the data as a list of integers"""

    with open(file, 'r') as f:
        lines = f.readlines()

    lines = lines[1:-2]
    array = []

    for line in lines:
        # Remove ':XXXXXXXX' first 9 characters - : bytecount, address, type also remove Checksum and \n
        line = line[9:-3]
        # Pick 2 characters at a time and form a byte
        for i in range(0, len(line)-1, 2):
            array.append(int('0x'+line[i:i+2], 16))
        
    print ("Size of FLashdata: " + str(len(array)) + " bytes")

parsehex("debug/debug.hex")