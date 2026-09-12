import os
import time
import serial, serial.tools.list_ports
WARN = '\033[92m'
ALERT = '\033[91m'
RESET = '\033[0m'
class ClientApp:
   def __init__(self):
       self.__authorization = False
       self.__authMask = 0xABCD1234
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
       if status == -1:
           print(f"{ALERT}Failed to request seed from target.{RESET}")
           return False
       else:
            # get 4 bytes of seed from target
            seed = 0
            for _ in range(4):
                res = self._getTargetResponse(None, timeout=6)
                if res == -1:
                    print(f"{ALERT}Failed to receive seed from target.{RESET}")
                    return False
                else:
                    seed = (seed << 8) | int.from_bytes(res, byteorder='big')
           # compute key using the received seed
            key = self._computeKey(seed)
            # send the computed key to the target for authorization
            print("Sending computed key to target...")
            response = self._getTargetResponse(key.to_bytes(4, byteorder='big'))
            if response == b'\x00':
                self.__authorization = True
                print(f"{WARN}Client authorized successfully.{RESET}")
                return True
            else:
                print(f"{ALERT}Authorization failed. Received unexpected response: {response}{RESET}")
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
                time.sleep(1)
       return self.__connection.read(self.__connection.in_waiting)
   def _fetchBootloaderVersion(self):
       print("\rFetching bootloader version...")
       # expect a positive response
       response = self._getTargetResponse(b'\x02', waitChar='|') # 0x02 : Fetch Bootloader Version Command
       if response == -1:
           return
       else:
           # positive response received. wait for version data
           version = self._getTargetResponse(None, timeout=6)
           if version == -1:
               return
           else:
               print(f"\n{WARN}Bootloader version: {version.decode('utf-8')}{RESET}")
   def _testTarget(self):
       print("\rTesting target...")
       print(f"""{WARN}Target test occurs by blinking the on-board LED for 5 seconds.
During this time, target doen't respond to any other commands. Wait for the test sequence to complete.
{RESET}""")
       response = self._getTargetResponse(b'\x01') # 0x01 : Test Target Command
       if response == b'\x00':
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
