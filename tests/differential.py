"""Deterministic comparisons with Python arbitrary precision arithmetic."""
import random
import subprocess
import sys
sys.set_int_max_str_digits(0)
rng = random.Random(2048)
cases, expected = [], []
def check(op,a,b):
    cases.append(f"{op} {a} {b}\n")
    if op == "cmp":
        expected.append(f"{int(a<b)}{int(a==b)}{int(a>b)}")
    else:
        expected.append(str({"+":lambda:a+b,"-":lambda:a-b,"*":lambda:a*b,
                             "/":lambda:a//b,"%":lambda:a%b}[op]()))
for _ in range(1200):
    a = rng.randrange(-10**rng.randrange(1,1200),10**rng.randrange(1,1200))
    b = rng.randrange(1,10**rng.randrange(1,700))*rng.choice([-1,1])
    for op in ["+","-","*","/","%","cmp"]: check(op,a,b)
for length in [256,257,512,1000,3000,12000,50000]:
    for _ in range(3):
        b = rng.randrange(10**(length-1),10**length)
        a = rng.randrange(10**(2*length-1),10**(2*length))
        for op in ["*","/","%"]: check(op,a,b)
        for delta in [-1,0,1]:
            check("/",b*b+delta,b); check("%",-b*b+delta,b)
cases += ["self 123456 1\n","min 0 1\n","+ -0000 +00000\n"]
expected += ["1",str(-2**63),"0"]
result = subprocess.run([sys.argv[1]],input="".join(cases),text=True,capture_output=True,check=True)
actual = result.stdout.splitlines()
assert len(actual) == len(expected), (len(actual),len(expected),result.stderr)
for i,(a,e) in enumerate(zip(actual,expected)):
    assert a == e, (i,cases[i][:200],a[:200],e[:200])
print(f"Passed {len(cases)} differential checks")
