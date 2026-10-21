import os
import time
import serial, serial.tools.list_ports
WARN = '\033[92m'
ALERT = '\033[91m'
RESET = '\033[0m'


class ClientApp:

    def __init__(self):
        self.__authorization = False
        self.__authMask = 0x12345678
        self.__validOptions = list(enumerate(
            ['Authorize', 
            'Test Target', 
            'Fetch Bootloader Version',
            'Flash Target'], start=1))
        # initialize serial connection
        availableSerialPorts = serial.tools.list_ports.comports()
        print("Available Serial Ports:")
        for port in availableSerialPorts:
            print(f" - {port.device}:{port.description}")
        print("Available baud rates: 9600, 19200, 38400, 57600, 115200")
        port = input("Select serial port number: ")
        baudrate = input("Select baud rate: ")
        if 'COM' in port.upper():
            port = port.upper().replace('COM', '')
        self.__connection = serial.Serial(port=f"COM{port}", baudrate=int(baudrate), timeout=1)  # Adjust port and baudrate as needed
        self._clearScreen()
        time.sleep(1)
        # check if the connection is open
        if self.__connection.is_open:
            print(f"Serial connection established on {port} at {baudrate} baud.\n")

    def _getAuthStatus(self):
        return self.__authorization
    
    def _computeKey(self, seed):
        # key = (seed ^ self.__authMask) & 0xFFFFFFFF
        return (seed ^ self.__authMask) & 0xFFFFFFFF
    
    def _authorizeClient(self):
        # get seed from target
        print("Requesting seed from target...")
        status = self._getTargetResponse(b'\x01')  # 0x01 : Request Seed Command
        print(f"\n{WARN}Received status: {status}{RESET}")
        if status == 0:
            print(f"\n{WARN}Target is ready to send seed.{RESET}")
        else:
            print(f"{ALERT}Failed to request seed from target.{RESET}")
            return False
        
        # get 4 bytes of seed and a checksum from target
        seed = []
        for _ in range(4):
            res = self._getTargetResponse(None, timeout=6)
            if res == -1:
                print(f"{ALERT}Failed to receive seed from target.{RESET}")
                return False
            seed.append(res)
        print(f"\n{WARN}Received seed{RESET}")

        # checksum good, compute key
        computedKey = self._computeKey(int.from_bytes(seed, byteorder='big'))
        print(f"\n{WARN}Computed key{RESET}, {seed} , {hex(computedKey)}")

        # send key to target
        print(f"\n{WARN}Sending key to target...{RESET}")
        self.__connection.write((computedKey & 0xFF).to_bytes(1, byteorder='big'))
        self.__connection.write((computedKey >> 8 & 0xFF).to_bytes(1, byteorder='big'))
        self.__connection.write((computedKey >> 16 & 0xFF).to_bytes(1, byteorder='big'))
        self.__connection.write((computedKey >> 24 & 0xFF).to_bytes(1, byteorder='big'))

        # wait for confirmation from target
        response = self._getTargetResponse(None, timeout=6)
        if response == 0:
            print(f"\n{WARN}Authorization successful.{RESET}")
            self.__authorization = True
            return True
        else:
            print(f"{ALERT}Authorization failed. Received response: {response}{RESET}")
            return False
        
    def parsehex(file: str, framechkseq: int = 0xAA) -> list:
        """ Parse a hex file and return the data as a list of integers"""

        with open("dummy1.hex", 'r') as f:
            lines = f.readlines()

        lines = lines[1:-2]
        array = []

        for line in lines:
            # Remove ':XXXXXXXX' first 9 characters - : bytecount, address, type also remove Checksum and \n
            line = line[9:-3]
            # Pick 2 characters at a time and form a byte
            for i in range(0, len(line)-1, 2):
                array.append(int('0x'+line[i:i+2], 16))
        return array
    
    def compute_crc(data: int, framechkseq: int)-> int:
        """ Compute 1 byte checksum for the given data.
        :param data: The input data 24 bit (3 byte) long.
        :param framechkseq: The frame check sequence, which is a 1 byte long value and should be >= 0x80.
        :return: 32 byte checksumed data as an integer.
        """   
        # When data is zero
        if data == 0:
            return(framechkseq & 0x7f)
        # Initialize the data with padding to form a 32-bit integer
        data = (data << 7)
        # Find the first non-zero bit in the data
        cnt = 31
        while (data & (1 << cnt) == 0):
            cnt  -= 1
        # Initialize bit position
        bitpos = cnt - 7
        num = (data & (0xFF << bitpos)) >> bitpos
        rem = framechkseq
        while(num >= (1 << 7)):
            rem = num ^ framechkseq
            while(bitpos > 0):
                bitpos -= 1
                rem = (rem << 1) | ((data & (1 << bitpos)) >> bitpos)
                if (rem  >= (1 << 7)):
                    break
            num = rem & 0xFF
        return(data | rem)

    def crc_append(array :list,framecheckseq : int = 0xAA) -> list:
        crc_array = []
        # The array length must be a multiple of three because we pack 3 bytes of data + CRC in one frame 
        if(len(array)%3 == 1):
            # Pad with two 0xFF bytes because in STM, the default value of a memory location is 0xFF
            array.append(0xFF)
            array.append(0xFF)
        elif(len(array)%3 == 2):
            array.append(0xFF)

        for i in range(0,len(array),3):
            data = (array[i+2] << 16) | (array[i+1] << 8) | array[i]
            crc = ClientApp.compute_crc(data, framecheckseq)
            crc_array.append(crc)
        return crc_array
    def transmit_data(self, frame: int):
        self.__connection.write(((frame & 0xFF000000) >> 24).to_bytes(1, byteorder='big'))
        self.__connection.write(((frame & 0x00FF0000) >> 16).to_bytes(1, byteorder='big'))
        self.__connection.write(((frame & 0x0000FF00) >> 8).to_bytes(1, byteorder='big'))
        self.__connection.write((frame & 0x000000FF).to_bytes(1, byteorder='big'))
    def _flashFirmware(self):
        flash_contents = []
        error_counter = 0
        count = 0
        if(self.__authorization == False):
            # Authorize the client before flashing
            self._authorizeClient()
        else:
            # parse the intel hex flash file and store flash contents in an array
            flash_org = ClientApp.parsehex("dummy1.hex")
            # Append the array with flash contents and the crc as well 3 bytes data + CRC ...
            flash_contents = ClientApp.crc_append(flash_org,0b10101010)
            # Request to flash firmware, receive ACK
            if(self._getTargetResponse(b'\x04') == 0):
                # send size of flash file to target and receive ACK
                if (self._getTargetResponse((len(flash_contents)).to_bytes(2,byteorder='big'),timeout=10) == 0):
                    #send data to target with crc
                    for frame in flash_contents:
                        while(error_counter < 5):
                            self.transmit_data(frame)
                            res = self._getTargetResponse(None)
                            if(res == 0):
                                print(f"{WARN}Frame {count} flashed successfully.{RESET}")
                                error_counter = 0
                                break
                            elif(res == 254):
                                print(f"Frame received but CRC failed - resend")
                                error_counter = error_counter + 1
                            else:
                                print(f"NACK recieved - resend")
                                error_counter = error_counter + 1
                        if error_counter != 0:
                            print(f"{ALERT}NACK from target , Can't flash software.{RESET}")
                            return
                else:
                    print(f"{ALERT}Flash file size bigger than expected. Can't flash target{RESET}") 
            else:
                #NACK from target , Can't flash software
                print(f"{ALERT}NACK from target , Can't flash software.{RESET}")

    def _clearScreen(self):
        os.system('cls' if os.name == 'nt' else 'clear')  # Clear the console

    def _showBootLoaderOptions(self):
        print("Bootloader Options:")
        for i, option in self.__validOptions:
            print(f"{i}. {option}")

    def _waitForUserInput(self):
        return input("Select an operation: ").strip()
    
    def _processUserChoice(self, choice):
        os.system('cls' if os.name == 'nt' else 'clear')  # Clear the console
        if choice == '1':
            self._authorizeClient()
        elif choice == '2':
            self._testTarget()
        elif choice == '3':
            self._fetchBootloaderVersion()
        elif choice == '4':
            self._flashFirmware()
        else:
            print("\rInvalid choice. Please select a valid option.")
    
    def _getTargetResponse(self, command, timeout=6, waitChar:str = '='):
        if command:
            self.__connection.write(command)
        start = time.time()
        while self.__connection.in_waiting == 0:
            delta = time.time() - start
            if delta > timeout:
                print(f"{ALERT}Timeout occurred while waiting for target response.{RESET}")
                return -1
            else:
                if waitChar == '=':
                    print("=", end='', flush=True)
                else:
                    if waitChar == '|':
                        waitChar = '—'
                    else:
                        waitChar = '|'
                    print(f"\r{waitChar}", end='')
                time.sleep(0.2)

        return int.from_bytes(self.__connection.read(1), byteorder='big')

    def _fetchBootloaderVersion(self):
        print("\rFetching bootloader version...")
        # expect a positive response
        response = self._getTargetResponse(b'\x02') # 0x02 : Fetch Bootloader Version Command
        if response == -1:
            return
        else:
            # positive response received. wait for version data
            version = self._getTargetResponse(None, timeout=6)
            if version == -1:
                return
            else:
                print(f"\n{WARN}Bootloader version: {version}{RESET}")

    def _testTarget(self):
        print("\rTesting target...")
        print(f"""{WARN}Target test occurs by blinking the on-board LED for 5 seconds.
During this time, target doen't respond to any other commands. Wait for the test sequence to complete.
{RESET}""")
        response = self._getTargetResponse(b'\x02') # 0x01 : Test Target started
        if response == 0:
            print(f"\n{WARN}Target test started.{RESET}")
        else:
            print(f"\n{ALERT}Target test failed. Received unexpected response: {response}{RESET}")
        response = self._getTargetResponse(None, timeout=10)
        if response == 0:
            print(f"\n{WARN}Target test completed successfully.{RESET}")
        else:
            print(f"\n{ALERT}Target test failed. Received unexpected response: {response}{RESET}")


# TESTING

if __name__ == "__main__":
   app = ClientApp()
   while True:
       app._showBootLoaderOptions()
       user_choice = app._waitForUserInput()
       app._processUserChoice(user_choice)
       cont = input("\nDo you want to perform another operation? (y/n): ").strip().lower()
       print(f"\n{WARN}You selected: {cont}{RESET}")
       if cont == 'y':
           app._clearScreen()
       else:
           break
