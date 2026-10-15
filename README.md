*This project has been created as part of the 42 curriculum by digglega.*

# Codexion

## Description

Coders sit in a circle. Each needs two USB dongles to compile.
Then they debug, refactor, and compile again. If a coder does not
start compiling within `time_to_burnout`, they burn out. The game
stops on burnout or when everyone compiled enough times.

Idea: one lock plus one bell decides everything. Dongles are given
only in pairs, so deadlock is impossible. A hand-written min-heap
picks who is next: fifo by hunger time, edf by burnout deadline.

## Instructions

```sh
make
./codexion n t_burn t_comp t_deb t_ref need cool fifo|edf
./codexion 5 800 200 200 200 3 50 fifo
make clean / make fclean / make re
```

## Resources

- `man pthread_create`, `man pthread_mutex_init`, `man pthread_cond_wait`
- Dining Philosophers problem, Coffman conditions, min-heap (CLRS ch. 6)
- AI was used to brainstorm the pair-giving trick and to draft this file.

## Blocking cases handled

- **Deadlock:** we never hold one dongle while waiting for the second.
  Both are taken at once under one lock. No circle can form.
- **Starvation:** every hungry coder waits in a heap. Fifo serves the
  longest hunger, edf the earliest deadline. One coder needs one dongle.
- **Cooldown:** each dongle stores `ready = release + cool` and is skipped
  while `ready` is in the future.
- **Burnout:** the monitor checks every 1 ms and prints within 10 ms.
  Compiling coders are safe.
- **Logs:** every line is one `write()` call under a print mutex.

## Thread synchronization mechanisms

- `lock` guards states, hunger times, counters, stop flag.
- `bell` wakes waiting coders; planner rings it after each give.
- Each dongle has its own `mutex`, taken in address order, briefly.
- `pen` guards `write()` so lines never mix.
- `CLOCK_MONOTONIC` for honest time. N coder threads + planner + monitor.
