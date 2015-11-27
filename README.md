# shellcode

"normal" use case                         stack overflow attack

|                |                        |                |
|-stack frame A--|                        |-stack frame A--|
| ...            |                        | ...            |
|                |                        |                |
|return addr     |                       /|&buffer         |
|-stack frame B--| <-- $rbp             e |xxxxxxxxxxxxxxxx|
|                |                      x |xxxxxxxxxxxxxxxx|
|                |                      e |^%?/bin/shxxxxxx|
|                |                      c |)$%&@!~)*&&%~_#!|
|                |                      v |xx&!#@#(~~*@(^^^|
|                |                      e |xxxxxxxxxxxxxxxx|
|hello world     | <-- buffer            \|xxxxxxxxxxxxxxxx| <-- return address
|----------------| <-- $rsp               |----------------|
|                |                        |                |
|                |                        |                |
|                |                        |                |
| .text          |                        | .text          |
|                |                        |                |
|----func B------|                        |----func B------|
| ...            | <-- $rip               | ...            |
|                |                        |                |
| return         |                        | return         |
|----------------|                        |----------------|
|                |                        |                |
|                |                        |                |
|----func A------|                        |----func A------|
| ...            |                        | ...            |
|                |                        |                |
| @call func B   |                        | @call func B   |
| ...            | <-- return address --> | ...            |
|                |                        |                |
|----------------|                        |                |
|                |                        |                |
+----------------+                        +----------------+
