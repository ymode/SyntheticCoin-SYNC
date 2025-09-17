# SyntheticCoin (SYNC) - Quick Start Guide

## 🚀 Running the Demo

The easiest way to see SYNC in action:

```bash
# Build the demo
./build.sh

# Run full demo
./sync-demo

# Or run specific demos:
./sync-demo podd     # Proof-of-Device-Distribution
./sync-demo rewards  # Reward calculations
./sync-demo squad    # Squad mining
./sync-demo stats    # Network statistics
```

## 💡 Key Innovation: Proof-of-Device-Distribution (PoDD)

SYNC solves the fundamental problem:
> "How do you prove 10 Bitaxes aren't just 1 Antminer pretending to be 10 devices?"

Our multi-layer verification system checks:
- **Timing Signatures**: Each device has unique crystal oscillator variations
- **Network Behavior**: Different devices have different network paths
- **Nonce Patterns**: Small devices produce distinctive patterns
- **Hardware Fingerprints**: Power, temperature, and performance characteristics

## 💰 Reward System

| Hashrate | Multiplier | Example (50 SYNC base) |
|----------|------------|------------------------|
| < 1 TH/s | 2.0x | 100 SYNC |
| 1-10 TH/s | 1.5x | 75 SYNC |
| 10-100 TH/s | 1.2x | 60 SYNC |
| > 100 TH/s | 1.0x | 50 SYNC |

**Per TH/s Advantage**: A Bitaxe (0.5 TH/s) earns **600x more per TH/s** than a 150 TH/s farm!

## 🏗️ Project Structure

```
sync/
├── src/
│   ├── consensus/      # Consensus rules & parameters
│   ├── podd/          # Proof-of-Device-Distribution
│   ├── mining/        # Reward calculation
│   ├── syncd.cpp      # Main daemon
│   ├── sync-cli.cpp   # CLI interface
│   └── sync-demo.cpp  # Working demo
├── plan.md            # Full development plan
├── build.sh          # Build script
└── sync-demo         # Compiled demo
```

## 🛠️ Building from Source

### Simple Build (Demo Only)
```bash
./build.sh
```

### Full Build (requires dependencies)
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install build-essential libssl-dev libboost-all-dev

# Build with Make
make

# Or with CMake
mkdir build && cd build
cmake .. && make
```

## 📋 Core Components

### 1. Device Verifier (`src/podd/device_verifier.h`)
- Registers mining devices
- Verifies device distribution
- Detects spoofing attempts
- Manages mining squads

### 2. Reward Calculator (`src/mining/reward_calculator.h`)
- Calculates base rewards
- Applies small miner bonuses
- Handles PoDD bonuses
- Manages squad rewards

### 3. Consensus Parameters (`src/consensus/params.h`)
- 5 minute block time
- 50 SYNC base reward
- 84M total supply
- Tier-based reward multipliers

## 🎯 Next Steps

1. **Test PoDD with Real Hardware**
   - Need 10+ Bitaxe devices
   - Collect timing data
   - Refine detection algorithms

2. **Implement P2P Network**
   - Fork Bitcoin P2P protocol
   - Add PoDD message types
   - Create device registration protocol

3. **Build Mining Pool**
   - Squad formation protocol
   - Share verification
   - Reward distribution

4. **Launch Testnet**
   - Low difficulty for Bitaxe testing
   - Faucet for test coins
   - Block explorer

## 📊 Demo Output Example

```
=== Mining Reward Calculation Demo ===
Miner Type        | Hashrate | Base  | Bonus | Total | Multi |
Single Bitaxe     |   0.50 TH | 50.00 | 50.00 | 100.00 | 2.00x |
Large Farm        | 150.00 TH | 50.00 |  0.00 | 50.00 | 1.00x |

Per TH/s Earnings:
  Bitaxe: 200.00 SYNC per TH/s
  Large farm: 0.33 SYNC per TH/s
  Advantage: 600x
```

## 🤝 Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for development guidelines.

Key areas needing help:
- Hardware testing with real Bitaxes
- P2P network implementation
- Mining pool software
- Mobile app development

## 📚 Documentation

- [Full Plan](plan.md) - Complete project roadmap
- [Pivot Rationale](PIVOT_RATIONALE.md) - Why we focus on small miners
- [GitHub Issues](https://github.com/ymode/SyntheticCoin-SYNC/issues) - Development tasks

## 🚦 Status

✅ **Completed**:
- Core consensus parameters
- PoDD verification system
- Reward calculation engine
- Squad mining support
- Working demo

🚧 **In Progress**:
- P2P network layer
- Real hardware testing
- Mining pool implementation

📋 **Planned**:
- Testnet launch
- Mobile wallet
- Exchange integration
- Mainnet launch

---

**Making mining profitable for small miners again! 🚀**
