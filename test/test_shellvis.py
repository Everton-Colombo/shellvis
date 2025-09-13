import subprocess
import os
import tempfile
import shutil
import time
import sys
from pathlib import Path

class Colors:
    RED = '\033[91m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    MAGENTA = '\033[95m'
    CYAN = '\033[96m'
    WHITE = '\033[97m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    END = '\033[0m'

class ShellvisTest:
    def __init__(self, shell_path="./shellvis"):
        self.shell_path = shell_path
        self.test_dir = tempfile.mkdtemp()
        self.passed = 0
        self.failed = 0
        self.original_cwd = os.getcwd()
        
    def __enter__(self):
        os.chdir(self.test_dir)
        return self
        
    def __exit__(self, exc_type, exc_val, exc_tb):
        os.chdir(self.original_cwd)
        shutil.rmtree(self.test_dir)
        
    def run_shell_command(self, command, timeout=5, input_text=None):
        """Run a command in shellvis and return stdout, stderr, returncode"""
        try:
            if input_text is None:
                input_text = f"{command}\nexit\n"
            else:
                input_text = f"{input_text}\nexit\n"
                
            process = subprocess.Popen(
                [self.shell_path],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                cwd=self.test_dir
            )
            
            stdout, stderr = process.communicate(input=input_text, timeout=timeout)
            return stdout, stderr, process.returncode
            
        except subprocess.TimeoutExpired:
            process.kill()
            return "", "Command timed out", -1
        except Exception as e:
            return "", str(e), -1
            
    def run_batch_mode(self, batch_file_content, timeout=5):
        """Run shellvis in batch mode"""
        batch_file = os.path.join(self.test_dir, "batch.txt")
        with open(batch_file, 'w') as f:
            f.write(batch_file_content)
            
        try:
            process = subprocess.run(
                [self.shell_path, batch_file],
                capture_output=True,
                text=True,
                timeout=timeout,
                cwd=self.test_dir
            )
            return process.stdout, process.stderr, process.returncode
        except subprocess.TimeoutExpired:
            return "", "Batch mode timed out", -1
        except Exception as e:
            return "", str(e), -1
    
    def assert_output_contains(self, test_name, command, expected_substring, input_text=None):
        """Test that command output contains expected substring"""
        stdout, stderr, returncode = self.run_shell_command(command, input_text=input_text)
        
        if expected_substring in stdout or expected_substring in stderr:
            print(f"{Colors.GREEN}PASS{Colors.END} {test_name}")
            self.passed += 1
            return True
        else:
            print(f"{Colors.RED}FAIL{Colors.END} {test_name}")
            print(f"  {Colors.YELLOW}Expected substring:{Colors.END} '{expected_substring}'")
            print(f"  {Colors.YELLOW}Stdout:{Colors.END} '{stdout.strip()}'")
            print(f"  {Colors.YELLOW}Stderr:{Colors.END} '{stderr.strip()}'")
            self.failed += 1
            return False
            
    def assert_file_exists(self, test_name, filename, should_exist=True):
        """Test that a file exists or doesn't exist"""
        exists = os.path.exists(os.path.join(self.test_dir, filename))
        
        if exists == should_exist:
            print(f"{Colors.GREEN}PASS{Colors.END} {test_name}")
            self.passed += 1
            return True
        else:
            status = "exists" if exists else "doesn't exist"
            expected = "should exist" if should_exist else "should not exist"
            print(f"{Colors.RED}FAIL{Colors.END} {test_name}")
            print(f"  {Colors.YELLOW}File {filename} {status} but {expected}{Colors.END}")
            self.failed += 1
            return False
            
    def assert_file_content(self, test_name, filename, expected_content):
        """Test that file contains expected content"""
        try:
            with open(os.path.join(self.test_dir, filename), 'r') as f:
                content = f.read()
                
            if expected_content in content:
                print(f"{Colors.GREEN}PASS{Colors.END} {test_name}")
                self.passed += 1
                return True
            else:
                print(f"{Colors.RED}FAIL{Colors.END} {test_name}")
                print(f"  {Colors.YELLOW}Expected content:{Colors.END} '{expected_content}'")
                print(f"  {Colors.YELLOW}Actual content:{Colors.END} '{content}'")
                self.failed += 1
                return False
        except Exception as e:
            print(f"{Colors.RED}FAIL{Colors.END} {test_name}")
            print(f"  {Colors.YELLOW}Error reading file:{Colors.END} {e}")
            self.failed += 1
            return False

def main():
    if len(sys.argv) > 1:
        shell_path = sys.argv[1]
    else:
        shell_path = "./shellvis"
        
    if not os.path.exists(shell_path):
        print(f"{Colors.RED}ERROR:{Colors.END} Shell binary '{shell_path}' not found!")
        print("Please compile shellvis first or provide the correct path")
        sys.exit(1)
        
    with ShellvisTest(shell_path) as test:
        print(f"{Colors.BOLD}{Colors.CYAN}TESTING SHELLVIS SHELL{Colors.END}")
        print("=" * 50)
        
        # Test 1: Basic shell startup and exit
        print(f"\n{Colors.BLUE}[SECTION]{Colors.END} Testing Basic Functionality")
        test.assert_output_contains("Shell prompt display", "", "shellvis>")
        
        # Test 2: Built-in commands
        print(f"\n{Colors.BLUE}[SECTION]{Colors.END} Testing Built-in Commands")
        
        # Test pwd
        test.assert_output_contains("pwd command", "pwd", test.test_dir)
        
        # Test cd
        os.makedirs("test_subdir", exist_ok=True)
        test.assert_output_contains("cd command", "cd test_subdir\npwd", "test_subdir")
        
        # Test path command (basic test)
        test.assert_output_contains("path command", "path /bin /usr/bin", "")
        
        # Test 3: External programs (create test files first)
        print(f"\n{Colors.BLUE}[SECTION]{Colors.END} Testing External Programs")
        
        # Create test files for external programs
        with open("test_file.txt", "w") as f:
            f.write("line 1\nline 2 with test\nline 3\n")
            
        with open("source.txt", "w") as f:
            f.write("source content")
            
        # Test ls command
        test.assert_output_contains("ls command", "ls", "test_file.txt")
        test.assert_output_contains("ls -l command", "ls -l", "test_file.txt")
        test.assert_output_contains("ls -a command", "ls -a", ".")
        
        # Test cat command
        test.assert_output_contains("cat command", "cat test_file.txt", "line 1")
        
        # Test grep command
        test.assert_output_contains("grep command", "grep test test_file.txt", "line 2 with test")
        test.assert_output_contains("grep -n command", "grep -n test test_file.txt", "2:")
        
        # Test touch command
        test.run_shell_command("touch new_file.txt")
        test.assert_file_exists("touch command creates file", "new_file.txt")
        
        # Test cp command
        test.run_shell_command("cp source.txt dest.txt")
        test.assert_file_exists("cp command creates destination", "dest.txt")
        test.assert_file_content("cp command copies content", "dest.txt", "source content")
        
        # Test rm command
        test.run_shell_command("rm dest.txt")
        test.assert_file_exists("rm command removes file", "dest.txt", should_exist=False)
        
        # Test 4: Output redirection
        print(f"\n{Colors.BLUE}[SECTION]{Colors.END} Testing Output Redirection")
        
        test.run_shell_command("ls > output.txt")
        test.assert_file_exists("output redirection creates file", "output.txt")
        test.assert_file_content("output redirection content", "output.txt", "test_file.txt")
        
        # Test 5: Input redirection
        print(f"\n{Colors.BLUE}[SECTION]{Colors.END} Testing Input Redirection")
        
        with open("input.txt", "w") as f:
            f.write("input line 1\ninput line 2\n")
            
        test.assert_output_contains("input redirection", "cat < input.txt", "input line 1")
        
        # Test 6: Parallel execution
        print(f"\n{Colors.BLUE}[SECTION]{Colors.END} Testing Parallel Execution")
        
        # Create a slow command for parallel testing
        test.run_shell_command("touch file1.txt & touch file2.txt & touch file3.txt")
        time.sleep(1)  # Give time for parallel execution
        test.assert_file_exists("parallel execution file1", "file1.txt")
        test.assert_file_exists("parallel execution file2", "file2.txt")
        test.assert_file_exists("parallel execution file3", "file3.txt")
        
        # Test 7: Batch mode
        print(f"\n{Colors.BLUE}[SECTION]{Colors.END} Testing Batch Mode")
        
        batch_content = """pwd
touch batch_file.txt
ls
"""
        stdout, stderr, returncode = test.run_batch_mode(batch_content)
        test.assert_file_exists("batch mode creates file", "batch_file.txt")
        
        # Test 8: Error handling
        print(f"\n{Colors.BLUE}[SECTION]{Colors.END} Testing Error Handling")
        
        # Test command not found
        test.assert_output_contains("command not found error", "nonexistent_command", "not found")
        
        # Test invalid directory for cd
        test.assert_output_contains("cd invalid directory", "cd nonexistent_dir", "")
        
        # Test file not found for cat
        test.assert_output_contains("cat file not found", "cat nonexistent.txt", "")
        
        # Test 9: Complex command combinations
        print(f"\n{Colors.BLUE}[SECTION]{Colors.END} Testing Complex Commands")
        
        # Test redirection with external programs
        test.run_shell_command("grep test test_file.txt > grep_output.txt")
        test.assert_file_content("grep with redirection", "grep_output.txt", "line 2 with test")
        
        # Test multiple redirections and parallel
        test.run_shell_command("ls > list1.txt & pwd > list2.txt")
        time.sleep(1)
        test.assert_file_exists("parallel redirection file1", "list1.txt")
        test.assert_file_exists("parallel redirection file2", "list2.txt")
        
        # Final results
        print("\n" + "=" * 50)
        total_tests = test.passed + test.failed
        
        if test.failed == 0:
            print(f"{Colors.BOLD}{Colors.GREEN}SUCCESS: All {test.passed} tests passed!{Colors.END}")
            return 0
        else:
            print(f"{Colors.BOLD}RESULTS: {Colors.GREEN}{test.passed} passed{Colors.END}, {Colors.RED}{test.failed} failed{Colors.END} (out of {total_tests} total)")
            print(f"{Colors.RED}FAILURE: {test.failed} test(s) failed{Colors.END}")
            return 1

if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)