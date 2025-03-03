# Watchdog Project

## Overview

The **Watchdog Project** is a tool designed to protect processes in critical sections of code that may fail under certain circumstances. It provides a mechanism to ensure that a process is restarted if it fails or crashes. The project includes two key functions:

- **MMI (Make Me Immortal)**: This function starts the watchdog service. If the process fails or crashes, the watchdog will automatically reactivate it.
- **DNR (Do Not Resuscitate)**: This function disables the watchdog service, preventing the watchdog from restarting the process.

The primary goal of this project is to provide protection for critical code sections, ensuring that processes can recover automatically if they fail unexpectedly.

## How It Works

1. **MMI (Make Me Immortal)**:
   - When called, this function begins the watchdog service.
   - If the process crashes or fails for any reason, the watchdog will detect this failure and automatically restart the process.

2. **DNR (Do Not Resuscitate)**:
   - This function disables the watchdog service.
   - Once called, the watchdog will no longer monitor or restart the process, even if it fails.

This mechanism ensures that your critical sections of code are protected from failure, and the process can be restarted automatically without manual intervention.

## Features

- **Automatic Restart**: The watchdog ensures that a failed process is restarted automatically.
- **Controlled Shutdown**: You can disable the watchdog service at any time by calling `DNR`.
- **Easy to Integrate**: Simply call `MMI` and `DNR` in your application code to enable or disable the watchdog service.

## Compilation and Usage

### 1. **Compilation**:

To compile the project, you'll need to compile both the source files in the `src` directory and the ones in the `/dependencies/src` directory. The source files can be compiled using the provided `Makefile`.

1. **Makefile**: The project includes a `Makefile` to simplify compilation. You can compile the project by running the following command:

   ```bash
   make
   ```

This will compile all the necessary files and create an executable named watchdog.


### 2. **Running the Program**:
To run the program that prints output every few seconds, and tests how the watchdog service reacts to a process failure:


	```bash
   ./watchdog
	```
	
### 3. **Integrating with Your Code**:
To integrate the watchdog functionality into your own code:

1. **Call MMI()**: Start the watchdog service by calling the MMI function in your code. This will ensure that the process is protected by the watchdog.

	```bash
    MMI(interval_in_seconds, repetition_count, arguments);
	```

1. **Call DNR()**: To stop the watchdog service and prevent it from restarting your process, call DNR.

	```bash
    DNR();
	```








