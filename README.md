### Usage

- /read all   - reads all commands data
- /read xx    - reads command data to send to Flthys for a given index from 00 to 99. Example: /read 01
- /write xx command milliseconds - writes command data to Flthys for a given index from 00 to 99. Example: /write 01 #A0971 10000
- /free xx    - frees command data for a given indes from 99 to 99. Example: /free 01
- /reset      - reset command table to factory

### Default Command Mapping

| Seq # | Flthy-Cmd | Time to Reset (ms) |
| --- | --- | ---:|
| 00 | A0971 | 0 |
| 01 | A00359 | 4000 |
| 02 | A0059 | 4000 |
| 03 | A00389 | 4000 |
| 04 | A0059 | 5000 |
| 05 | A00358 | 155000 |
| 06 | A0051 | 10000 |
| 07 | A00387 | 40500 |
| 08 | A001 | 35500 |
| 09 | A00387 | 245000 |
| 10 | A0971 | 0 |
| 11 | A0971 | 0 |
| 12 | A0971 | 0 |
| 13 | A0971 | 0 |
| 14 | A0971 | 0 |
| 15 | A0051 | 4000 |
| | | |
| 51 | A0051 | 4000 |
| 52 | A0059 | 4000 |
| 53 | A0059 | 4000 |
| 54 | A0059 | 6000 |
| 55 | A00387 | 15500 |
| 56 | A00387 | 10000 |
| 57 | A00387 | 40000 |

### Hints

To control BODY panels via serial, app or controllers, the ";" character is used as command prefix! (so use ;OP01 instead of :OP01)
To configure BODY panels via serial, app or controllers, the "_" character is used as command prefix! (so use _MD02 instead of #MD02)
