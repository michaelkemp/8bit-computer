# 8bit-computer

Instructions are made up of 4 bits of *Op-Code* and 4 bits of *Data*

### NOP - 0000
No Operation
```
NOP
```
Instruction: 0000 0000

### LDA - 0001
Load A-Register with Value from RAM Address
```
LDA 15
```
Instruction: 0001 1111

### LDI - 0010
Load A-Register with a Value
```
LDI #6
```
Instruction: 0010 0110

### ADD - 0011
Add Value from RAM Address to A-Register
```
ADD 15
```
Instruction: 0011 1111

### SUB - 0100
Subtract Value from RAM Address from A-Register
```
SUB 15
```
Instruction: 0100 1111

### STA - 0101
Store A-Register at RAM Address
```
STA 15
```
Instruction: 0101 1111

### INC - 0110
Add Value to A-Register
```
INC #1
```
Instruction: 0110 0001

### DEC - 0111
Subtract Value from A-Register
```
DEC #1
```
Instruction: 0111 0001

### OUT - 1000
Display Contents of A-Register
```
OUT
```
Instruction: 1000 0000

### JNC - 1001
Jump [Set Program Counter] to Address on Carry Flag NOT Set
```
JNC 2
```
Instruction: 1001 0010

### JNZ - 1010
Jump [Set Program Counter] to Address on Zero Flag NOT Set
```
JNZ 2
```
Instruction: 1010 0010

### DSP - 1011
Display Contents of RAM Address
```
DSP 15
```
Instruction: 1011 1111

### JMP - 1100
Jump [Set Program Counter] to Address
```
JMP 7
```
Instruction: 1100 0111

### JC  - 1101
Jump [Set Program Counter] to Address on Carry Flag Set
```
JC 5
```
Instruction: 1101 0101

### JZ  - 1110
Jump [Set Program Counter] to Address on Zero Flag Set
```
JZ 3
```
Instruction: 1110 0011

### HLT - 1111
Stop Clock
```
HLT
```
Instruction: 1111 0000
