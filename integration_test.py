#!/usr/bin/env python3
"""
SmvIT Integration Test Runner
Automated testing for USB-TTL Bridge device

This script sends commands to the device and validates responses.
Run this after flashing firmware to verify basic functionality.

Usage:
    python3 integration_test.py /dev/ttyUSB0
"""

import sys
import time
import serial
import argparse
from datetime import datetime

class SmvITTester:
    def __init__(self, port, baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.ser = None
        self.test_results = []
        
    def connect(self):
        """Open serial connection to device"""
        try:
            self.ser = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                timeout=2.0,
                write_timeout=2.0
            )
            time.sleep(2)  # Wait for device to reset
            print(f"✓ Connected to {self.port} at {self.baudrate} baud")
            return True
        except Exception as e:
            print(f"✗ Failed to connect: {e}")
            return False
    
    def disconnect(self):
        """Close serial connection"""
        if self.ser and self.ser.is_open:
            self.ser.close()
            print("✓ Disconnected")
    
    def send_command(self, cmd):
        """Send command to device"""
        if not self.ser:
            return False
        try:
            self.ser.write(cmd.encode())
            time.sleep(0.1)
            return True
        except Exception as e:
            print(f"✗ Failed to send command: {e}")
            return False
    
    def read_output(self, timeout=2.0):
        """Read output from device"""
        if not self.ser:
            return ""
        
        output = ""
        start_time = time.time()
        
        while time.time() - start_time < timeout:
            if self.ser.in_waiting:
                data = self.ser.read(self.ser.in_waiting)
                output += data.decode('utf-8', errors='replace')
                time.sleep(0.1)
            else:
                time.sleep(0.1)
        
        return output
    
    def wait_for_menu(self, timeout=5.0):
        """Wait for main menu to appear"""
        print("  Waiting for main menu...")
        output = self.read_output(timeout)
        
        if "Main Menu" in output or "SmvIT" in output:
            print("  ✓ Menu detected")
            return True
        else:
            print("  ✗ Menu not detected")
            return False
    
    def test_menu_navigation(self):
        """Test Case: Menu Navigation"""
        print("\n[TC2] Testing Menu Navigation...")
        
        # Test help menu
        print("  Testing Help menu (H)...")
        self.send_command('H')
        output = self.read_output(2.0)
        
        if "Help" in output or "Commands" in output:
            print("  ✓ Help menu displayed")
            result = "PASS"
        else:
            print("  ✗ Help menu not displayed")
            result = "FAIL"
        
        self.test_results.append(("Menu Navigation - Help", result))
        
        # Return to main menu
        time.sleep(1)
        self.send_command('M')
        time.sleep(1)
        
        # Test info menu
        print("  Testing Info menu (I)...")
        self.send_command('I')
        output = self.read_output(2.0)
        
        if "Hardware" in output or "Info" in output:
            print("  ✓ Info menu displayed")
            result = "PASS"
        else:
            print("  ✗ Info menu not displayed")
            result = "FAIL"
        
        self.test_results.append(("Menu Navigation - Info", result))
        
        # Return to main menu
        time.sleep(1)
        self.send_command('M')
        time.sleep(1)
    
    def test_settings_menu(self):
        """Test Case: Settings Menu"""
        print("\n[TC2.3] Testing Settings Menu...")
        
        self.send_command('S')
        output = self.read_output(2.0)
        
        if "Settings" in output or "Baud" in output:
            print("  ✓ Settings menu displayed")
            result = "PASS"
        else:
            print("  ✗ Settings menu not displayed")
            result = "FAIL"
        
        self.test_results.append(("Settings Menu", result))
        
        # Return to main menu
        time.sleep(1)
        self.send_command('M')
        time.sleep(1)
    
    def test_bridge_mode_entry(self):
        """Test Case: Bridge Mode Entry"""
        print("\n[TC4.1] Testing Bridge Mode Entry...")
        
        self.send_command('B')
        output = self.read_output(3.0)
        
        if "BRIDGE MODE" in output and "~~~" in output:
            print("  ✓ Bridge mode entered")
            result = "PASS"
            
            # Test escape sequence
            print("  Testing escape sequence...")
            time.sleep(0.5)
            self.send_command('~~~')
            output = self.read_output(2.0)
            
            if "EXITED" in output or "menu" in output.lower():
                print("  ✓ Escape sequence works")
                self.test_results.append(("Bridge Mode - Escape", "PASS"))
            else:
                print("  ✗ Escape sequence failed")
                self.test_results.append(("Bridge Mode - Escape", "FAIL"))
        else:
            print("  ✗ Bridge mode not entered")
            result = "FAIL"
        
        self.test_results.append(("Bridge Mode Entry", result))
        
        # Ensure we're back at menu
        time.sleep(1)
    
    def test_hardware_detection(self):
        """Test Case: Hardware Detection"""
        print("\n[TC1] Testing Hardware Detection...")
        
        # Read initial output to see hardware detection
        output = self.read_output(2.0)
        
        # Check for display detection
        if "Display" in output:
            if "not detected" in output.lower() or "failed" in output.lower():
                print("  ℹ Display not detected (OK if not connected)")
                display_result = "PASS (Not Present)"
            else:
                print("  ✓ Display detected")
                display_result = "PASS (Present)"
        else:
            display_result = "UNKNOWN"
        
        # Check for SD card detection
        if "SD" in output:
            if "not detected" in output.lower() or "failed" in output.lower():
                print("  ℹ SD card not detected (OK if not inserted)")
                sd_result = "PASS (Not Present)"
            else:
                print("  ✓ SD card detected")
                sd_result = "PASS (Present)"
        else:
            sd_result = "UNKNOWN"
        
        self.test_results.append(("Hardware Detection - Display", display_result))
        self.test_results.append(("Hardware Detection - SD Card", sd_result))
    
    def test_state_machine(self):
        """Test Case: State Machine Transitions"""
        print("\n[TC7] Testing State Machine...")
        
        # Navigate through states rapidly
        commands = ['H', 'M', 'I', 'M', 'S', 'M', 'B', '~~~']
        
        print("  Rapid state transitions...")
        for cmd in commands:
            self.send_command(cmd)
            time.sleep(0.5)
        
        # Check if we're back at menu
        output = self.read_output(2.0)
        
        if "Menu" in output:
            print("  ✓ State machine stable")
            result = "PASS"
        else:
            print("  ✗ State machine unstable")
            result = "FAIL"
        
        self.test_results.append(("State Machine Stability", result))
    
    def print_results(self):
        """Print test results summary"""
        print("\n" + "="*60)
        print("TEST RESULTS SUMMARY")
        print("="*60)
        
        passed = 0
        failed = 0
        
        for test_name, result in self.test_results:
            status = "✓" if "PASS" in result else "✗"
            print(f"{status} {test_name:.<45} {result}")
            
            if "PASS" in result:
                passed += 1
            elif "FAIL" in result:
                failed += 1
        
        print("="*60)
        print(f"Total: {len(self.test_results)} | Passed: {passed} | Failed: {failed}")
        print("="*60)
        
        # Save results to file
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        filename = f"test_results_{timestamp}.txt"
        
        with open(filename, 'w') as f:
            f.write(f"SmvIT Integration Test Results\n")
            f.write(f"Date: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write(f"Port: {self.port}\n")
            f.write(f"Baudrate: {self.baudrate}\n\n")
            
            for test_name, result in self.test_results:
                f.write(f"{test_name}: {result}\n")
            
            f.write(f"\nTotal: {len(self.test_results)} | Passed: {passed} | Failed: {failed}\n")
        
        print(f"\nResults saved to: {filename}")
        
        return failed == 0
    
    def run_all_tests(self):
        """Run all integration tests"""
        print("\n" + "="*60)
        print("SmvIT INTEGRATION TEST SUITE")
        print("="*60)
        print(f"Port: {self.port}")
        print(f"Baudrate: {self.baudrate}")
        print(f"Start Time: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        print("="*60)
        
        if not self.connect():
            return False
        
        try:
            # Wait for device to boot
            print("\nWaiting for device boot...")
            if not self.wait_for_menu(10.0):
                print("✗ Device did not boot to menu")
                return False
            
            # Run test cases
            self.test_hardware_detection()
            self.test_menu_navigation()
            self.test_settings_menu()
            self.test_bridge_mode_entry()
            self.test_state_machine()
            
            # Print results
            success = self.print_results()
            
            return success
            
        except KeyboardInterrupt:
            print("\n\nTest interrupted by user")
            return False
        
        finally:
            self.disconnect()

def main():
    parser = argparse.ArgumentParser(description='SmvIT Integration Test Runner')
    parser.add_argument('port', help='Serial port (e.g., /dev/ttyUSB0 or COM3)')
    parser.add_argument('--baud', type=int, default=115200, help='Baud rate (default: 115200)')
    
    args = parser.parse_args()
    
    tester = SmvITTester(args.port, args.baud)
    success = tester.run_all_tests()
    
    sys.exit(0 if success else 1)

if __name__ == '__main__':
    main()
