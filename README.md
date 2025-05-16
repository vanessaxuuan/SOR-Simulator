# 🧠 Smart Order Router (SOR) Simulator

A simple C-based Smart Order Router simulator that mimics order routing logic used by investment banks and electronic trading platforms. 

It evaluates multiple simulated exchanges to route client orders optimally based on price, volume, and latency.

---

## 📘 Context

This SOR simulates a **low-latency, high-throughput** order management system, focusing on routing logic for **smart order routers (SORs)**.  

The goal is to:
- Deepen understanding of SOR decision-making.
- Practice system design in C within a Linux environment.
- Showcase key concepts in financial infrastructure (e.g., price discovery, multi-venue trading, latency arbitration). (Future additions)

---

## 📌 Project Status

### ✅ Current Functionality
- [x] Read and parse orders from input
- [x] Simulate multiple exchanges with different ask prices, volume, and latency
- [x] Route orders to best exchange(s) based on:
  - Best Ask Price
  - Available Volume
  - Latency Scoring
- [x] Split orders across exchanges if required
- [x] Console-based log output of routing decisions
- [x] Modular C codebase with headers and implementation separation
- [x] Perform parallel calculations with multithreading
- [x] Integrate real-time market feed simulation (yahoo finance)

---

### 🧩 Future Enhancements (Planned)
- [ ] Linux-compatible build system
- [ ] Add unit tests and input validation
- [ ] Simulate trading session over a timer or tick-based loop
- [ ] Add GUI or web dashboard to visualize routing results
- [ ] Enable CSV logging of execution history

---

## 🛠️ How to Build & Run the SOR Simulator

This project is written in **C** and built using `make`. 

---

### 1. ✅ Install Dependencies

Open a terminal and ensure the required tools are installed:

```bash
sudo apt update
sudo apt install build-essential
```

### 2. ✅ Build project

Inside the project directory, simply run:

```bash
make
```

### 3. ✅ Run the SOR Simulator

Simply run:

```bash
./bin/sor_simulator
```

### 4. ✅ Key in input

Enter the stock ticker symbol (e.g., AAPL, GOOGL) and the desired quantity of shares to purchase!

### 5. ✅ Expected Output

The simulator will display:
- Exchange data and rankings based on price, volume, and latency
- Order allocation across exchanges
- Execution status and remaining quantity (if any)
- Performance metrics (execution time and resource usage)
