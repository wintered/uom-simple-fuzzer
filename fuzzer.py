import os
import sys
import subprocess
import random


def usage():
    print("Usage:")
    print("python fuzzer.py <program> <num-tests>")
    print("")
    print("Example:")
    print("python fuzzer.py /usr/bin/wc 1000")
    sys.exit(1)

if len(sys.argv) != 3:
    usage()

exe = sys.argv[1]
n_iterations = int(sys.argv[2])

if not os.path.isfile(exe) and shutil.which(exe) is None:
    print(f"Error: program '{exe}' does not exist or is not found in PATH.")
    usage()


def run(cmd, capture=False):
    """
    Run a bash command.

    Parameters:
        cmd (str): The shell command to run.
        capture (bool): If True, return stdout instead of printing it.

    This function also prints the command being executed, so it is
    perfectly clear what is run.
    """
    print(f"{cmd}")

    if capture:
        result = subprocess.run(
            cmd,
            shell=True,
            text=True,
            capture_output=True,
        )

        # Check nonzero exit code
        if result.returncode != 0:
            print("Bug has been found")
            sys.exit(result.returncode)

        return result.stdout.strip()

    else:
        result = subprocess.run(cmd, shell=True)

        # Check nonzero exit code
        if result.returncode != 0 and result.returncode != 127:
            print("\033[91mBug has been found!\033[0m")
            sys.exit(result.returncode)
        if result.returncode == 127:
            print("\033[91mFuzzer crashed. Check commandline\033[0m")
            sys.exit(1)


# ----------------------------------------------------------------------

# Fuzzing loop
for _ in range(n_iterations):
    # 1. Determine input size at random
    low = 1
    high = 5    # HINT1
    inputsize = random.randint(low, high)

    # 2. Create input at random
    alphabet = ["a", "b", "z", "0", "1"]# HINT2 + HINT3
    fuzz_input = ''.join(
        random.choice(alphabet)
        for _ in range(inputsize)
    )

    # 3. Call program on stdin
    run(f"echo {fuzz_input}|{exe}")
