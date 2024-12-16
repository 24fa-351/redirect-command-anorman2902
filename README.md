# 351-Redirect Streams (`redir`)

## Overview
The `redir` program redirects input and output streams for a specified command by forking a child process and using `execvp` to execute the command. Input and output redirection is based on specified filenames or defaults to `stdin`/`stdout` if `-` is provided.

### Key Features
1. **Input and Output Redirection**:
   - Redirects input and output streams based on user-specified file names.
   - Handles cases where `-` is used to keep `stdin` and `stdout`.

2. **Command Parsing**:
   - Splits the `cmd` string into separate arguments (e.g., `wc -l` → `wc` and `-l`).
   - Executes commands even if they are not absolute paths.

3. **Path Resolution**:
   - Resolves non-absolute paths for commands using the `PATH` environment variable.

4. **Error Handling**:
   - Detects and reports errors such as missing files, invalid commands, or file permission issues.

---

## Usage
```bash
./stream <inp> <cmd> <out>

