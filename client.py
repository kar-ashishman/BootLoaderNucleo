import os
import time
import serial, serial.tools.list_ports
WARN = '\033[92m'
ALERT = '\033[91m'
RESET = '\033[0m'
RETRY_COUNT = 6
STATUS_BAD = -1
STATUS_OK = 0
STATUS_AUTH_FAIL = 2
PAD_BYTE = b'\xFF'


class ClientApp:

    def __init__(self):
        self.__authorization = False
        self.__authMask = 0xABCD1234
        self.__checksum_mask = 0xAB
        self._eot_bytes = b'\x44\x4F\x4E\x45'
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
        try:            
            self.__connection = serial.Serial(port=f"COM{port}", baudrate=int(baudrate), timeout=1)  # Adjust port and baudrate as needed
        except:
            print(f"{ALERT}Failed to open serial port COM{port} at {baudrate} baud. Please check the port and try again.{RESET}")
            exit(1)
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
        status = self._getTargetResponse(b'\x03')  # 0x03 : Request Seed Command
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
            if res == 2:
                print(f"{ALERT}Failed to receive seed from target.{RESET}")
                return False
            seed.append(res)
        print(f"\n{WARN}Received seed{RESET}")

        # checksum good, compute key
        computedKey = self._computeKey(int.from_bytes(seed, byteorder='little'))
        print(f"\n{WARN}Computed key{RESET}")

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
            self._sendFlashBytes(self._parse_flashbyes())
        else:
            print("\rInvalid choice. Please select a valid option.")
    
    def _getTargetResponse(self, command, timeout=6, waitChar:str = '='):
        if command:
            try:
                self.__connection.write(command)
            except:
                print(f"{ALERT}Error occurred while sending command to target.{RESET}")
                return -1
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
        response = self._getTargetResponse(b'\x01') # 0x01 : Test Target started
        if response == 0:
            print(f"\n{WARN}Target test started.{RESET}")
        else:
            print(f"\n{ALERT}Target test failed. Received unexpected response: {response}{RESET}")
        response = self._getTargetResponse(None, timeout=10)
        if response == 0:
            print(f"\n{WARN}Target test completed successfully.{RESET}")
        else:
            print(f"\n{ALERT}Target test failed. Received unexpected response: {response}{RESET}")


    def _sendFlashBytes(self,flashBytes):
        print("\rFlashing target...")
        # expect a positive response
        try:
            response = self._getTargetResponse(b'\x04',timeout=6) # 0x04 : Flash Target Command
        except:
            print(f"{ALERT}Error occurred while sending flash command to target.{RESET}")
            return
        if response == STATUS_AUTH_FAIL:
            print("Flashing Aborted....\n")
            print("Autorization failed...authorize again\n")
            return

        # pad , if total bytes not in the multiples of 3
        pad_bytes_count = 3 - (len(flashBytes) % 3)
        flashBytes += PAD_BYTE * pad_bytes_count
        retry = 0
        for i in range(0,len(flashBytes),3):            
            self._TransmitBytes(flashBytes[i:i+3])
            retry = 1
            #Waiting for confirmation from target
            try:
                response = self._getTargetResponse(None, timeout=6) 
            except:
                print(f"{ALERT}Error occurred while waiting for target response.{RESET}")
                return         
            if response == STATUS_OK:
                print(f"\n{WARN}Checksum matched...{RESET}")
            elif response == 3:
                print(f"Message timeout at server..\n")
                return
            elif response == STATUS_BAD:
                return
            else:                
                while retry <= 6 and response != STATUS_OK:
                    print(f"Checksum failed...retry {retry}\n")
                    self._TransmitBytes(flashBytes[i:i+3])
                    #Waiting for confirmation from target
                    try:
                        response = self._getTargetResponse(None, timeout=6)
                    except:
                        print(f"{ALERT}Error occurred while waiting for target response.{RESET}")
                        return
                    if response == STATUS_OK:
                        retry = 1
                        break
                    retry += 1
                
            if retry >= RETRY_COUNT:
                print(f"\n{ALERT}Flashing failed. Received unexpected response: {response}{RESET}")
                print("Flashing Aborted....\n")
                retry = STATUS_BAD
                break
        # Send DONE if all messages sent successfully
        if retry != STATUS_BAD:
            print(f"Sending flash completion....\n")
            try:
                self.__connection.write(self._eot_bytes[0].to_bytes(1, byteorder='big'))
                self.__connection.write(self._eot_bytes[1].to_bytes(1, byteorder='big'))
                self.__connection.write(self._eot_bytes[2].to_bytes(1, byteorder='big'))
                self.__connection.write(self._eot_bytes[3].to_bytes(1, byteorder='big'))
            except:
                print(f"{ALERT}Error occurred while sending flash completion to target.{RESET}")
                return

            #Waiting for confirmation from target
            try:
                response = self._getTargetResponse(None, timeout=6)
            except:
                print(f"{ALERT}Error occurred while waiting for target response.{RESET}")
                return
            if response == STATUS_OK:
                print(f"\n{WARN}Flashing completed successfully.{RESET}")
            else:
                print(f"response received {response}")

                
    def _TransmitBytes(self,flashBytes):
        try:
            for i in range(0,len(flashBytes),3):
                print(f"Transmitting byte {flashBytes[0]}...\n")
                self.__connection.write(flashBytes[0].to_bytes(1, byteorder='big'))
                print(f"Transmitting byte {flashBytes[1]}...\n")
                self.__connection.write(flashBytes[1].to_bytes(1, byteorder='big'))
                print(f"Transmitting byte {flashBytes[2]}...\n")
                self.__connection.write(flashBytes[2].to_bytes(1, byteorder='big'))

                checksum = self._calculate_checksum(flashBytes)     
                print(f"Transmitting checksum byte {checksum}...\n")
                self.__connection.write(checksum.to_bytes(1, byteorder='big'))  
        except:
            print(f"{ALERT}Error occurred while transmitting bytes to target.{RESET}")
            return STATUS_BAD  


    def _calculate_checksum(self,bytes):
        total = 0
        for byte in bytes:
            total = total ^ byte
        
        total ^=self. __checksum_mask
        return total

    def _parse_flashbyes(self):
        flashBytes= b'\xAA\xBB\xCC\xDD\xEE\xFF\x11\x22\x33\x44\x55\x66\x77\x88\x99'
        return flashBytes
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
