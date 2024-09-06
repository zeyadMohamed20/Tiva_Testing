import os
import subprocess
import serial.tools.list_ports
import sys
import platform
import urllib.request

# Constants
HEX_FILE_PATH = "testing.hex"  # Replace with your actual hex file path
OPENOCD_PATH = "openocd"  # Path to OpenOCD binary (or install OpenOCD)
ICDI_DRIVER_URL = "https://www.ti.com/lit/ml/spmu287/spmu287.zip"  # Link to ICDI driver

# Function to check if ICDI driver is installed
def is_icdi_installed():
    if platform.system() == "Windows":
        driver_installed = os.system("driverquery | findstr /i icdi") == 0
        return driver_installed
    # Assume pre-installed drivers on Linux/macOS
    return True

# Function to download and install ICDI driver if not installed
def install_icdi_driver():
    if not is_icdi_installed():
        print("ICDI Driver not found, downloading and installing...")
        driver_zip = "icdi_driver.zip"
        urllib.request.urlretrieve(ICDI_DRIVER_URL, driver_zip)
        if platform.system() == "Windows":
            # Extract and install on Windows (simple process for demonstration)
            subprocess.run(["tar", "-xvf", driver_zip, "-C", "icdi_driver"])
            subprocess.run(["icdi_driver/installer.exe"], shell=True)
        else:
            print("Please manually install the ICDI driver for your system.")
            sys.exit(1)

# Function to detect Tiva C connected to the PC
def detect_tiva_c():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        print(port.description)
    tiva_ports = [port for port in ports if "Stellaris" in port.description or "Tiva" in port.description]
    if len(tiva_ports) > 0:
        print(f"Tiva C board detected at {tiva_ports[0].device}")
        return tiva_ports[0].device
    else:
        print("No Tiva C board detected. Please connect the board and try again.")
        sys.exit(1)

# Function to program the Tiva C using OpenOCD
def program_tiva_c_with_openocd(tiva_port, hex_file):
    openocd_cmd = [
        OPENOCD_PATH,
        "-f", "./tcl/board/ti_ek-tm4c123gxl.cfg",  # Replace with your board configuration
        "-c", f"program {hex_file} verify reset exit"
    ]
    try:
        subprocess.run(openocd_cmd, check=True)
        print(f"Successfully programmed the Tiva C board with {hex_file}")
    except subprocess.CalledProcessError as e:
        print(f"Error while programming: {e}")
        sys.exit(1)


# Main function to orchestrate the programming process
def main():
    # Install ICDI driver if not installed
    install_icdi_driver()

    # Detect the Tiva C board
    tiva_port = detect_tiva_c()

    # Program the Tiva C board with the provided hex file
    program_tiva_c_with_openocd(tiva_port, HEX_FILE_PATH)

if __name__ == "__main__":
    main()
