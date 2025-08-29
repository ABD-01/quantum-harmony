# README #

---

### ASL CBUF Example ###

#### What
- Example that makes use of ASL CBUF component.

#### Why
- Example will show end-users a reference implementation.

#### How
- The user provides the size of CBUF buffer through command line argument.
- The user input is validated before proceeding.
- The memory will be allocated (malloc) and CBUF will be created.
- The test uses producer-consumer threading pattern to validate CBUF operations.
- The test covers enqueue/dequeue from single byte up to maximum capacity possible.
- The test validates data integrity and thread-safe circular buffer operations.
- The test terminates successfully after all tests are completed (or terminates on first error).

