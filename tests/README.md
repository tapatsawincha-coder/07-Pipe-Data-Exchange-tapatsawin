# Visible Checks

Run the local visible check with:

```bash
./scripts/test.sh
```

The script builds the project, runs `./bin/pipe_lab`, and performs an **exact**
string comparison against the expected output:

```
child received: ping
parent: sent ping
child computed: 42
parent received: 42
child: got EOF after 5 bytes
parent: broken pipe detected
all tasks done
```

The test exits with status 0 only when the program output matches exactly.
Any extra or missing line, or a line with different content, will fail the check.
