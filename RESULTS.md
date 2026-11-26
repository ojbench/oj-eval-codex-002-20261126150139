# Implementation and Validation

The implementation stores signed magnitudes in little-endian base-10000 limbs.
Zero has an empty magnitude and a positive sign. Addition and subtraction are
linear; multiplication uses school arithmetic for small inputs and a packed
complex FFT for large inputs. FFT roots are computed directly to avoid cumulative
phase error on large transforms.

Division normalizes the divisor, recursively computes an integer reciprocal
with Newton refinement, and divides the dividend in blocks. Exact corrections
produce quotient and remainder magnitudes. Signed division rounds down, and
the remainder has the divisor's sign, as required. Small divisions use long
division. No third-party arithmetic libraries are used.

## Reproduce Local Checks

```sh
mkdir -p build
g++ -std=c++20 -O2 -Wall -Wextra -Isrc/include tests/driver.cpp src/int2048.cpp -o build/driver
python3 tests/differential.py build/driver
python3 tools/amalgamate.py
```

Validation completed:

- 7,392 deterministic comparisons against Python arbitrary precision integers.
- Additional random and all-9 arithmetic checks at 200,000 and 500,000 digits,
  comparing multiplication, division, and remainder against Python.
- All seven supplied Corner programs compiled and executed successfully.
- Supplied 500,000-digit division benchmark completed in approximately 2 seconds.

The maintained interface is `src/include/int2048.h`; implementations are in
`src/int2048.cpp`. `tools/amalgamate.py` produces the submitted `code.cpp`.
The provided ACMOJ client was repaired by adding its missing `submit_code` method.

## Online Judge Results

All submissions used the implementation committed in `978b72f`, pushed and
verified remotely before submission. No repository changes or pushes were made
while judging was in progress.

| Problem | Submission ID | Result | Raw Score |
| --- | --- | --- | --- |
| 2014 | 879347 | Accepted | 5/5 |
| 2015 | 879349 | Accepted | 20/20 |
| 2016 | 879350 | Accepted | 16/16 |
| 2017 | 879351 | Accepted | 5/5 |
| 2018 | 879352 | Accepted | 5/5 |
| 2019 | 879353 | Accepted | 19/19 |

Six of twelve shared attempts used; six remain. No submissions were aborted.
All automated test points passed. The separate code-review component is not
reported by the online judge.
