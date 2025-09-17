// SyntheticCoin (SYNC) Demo
// Demonstrates core concepts without external dependencies

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <iomanip>
#include <random>
#include <chrono>
#include <algorithm>

// Simplified PoDD demonstration
class SimplePoDDVerifier {
public:
    struct DeviceInfo {
        std::string id;
        double hashrate_ghs;
        double timing_variance;
        std::string ip_address;
        int nonce_pattern;
    };
    
    bool VerifyDevices(const std::vector<DeviceInfo>& devices) {
        if (devices.size() < 2) return true;
        
        // Check timing variance - real devices have different timing
        double min_variance = devices[0].timing_variance;
        double max_variance = devices[0].timing_variance;
        
        for (const auto& dev : devices) {
            min_variance = std::min(min_variance, dev.timing_variance);
            max_variance = std::max(max_variance, dev.timing_variance);
        }
        
        double variance_spread = max_variance - min_variance;
        bool timing_ok = variance_spread > 0.1; // Need variation
        
        // Check IP diversity
        std::map<std::string, int> ip_count;
        for (const auto& dev : devices) {
            ip_count[dev.ip_address]++;
        }
        
        bool ip_ok = ip_count.size() > devices.size() / 2; // At least 50% unique IPs
        
        // Check nonce patterns
        std::map<int, int> pattern_count;
        for (const auto& dev : devices) {
            pattern_count[dev.nonce_pattern]++;
        }
        
        bool pattern_ok = pattern_count.size() > 1; // Different patterns
        
        std::cout << "  Timing Variance Check: " << (timing_ok ? "PASS" : "FAIL") 
                  << " (spread: " << variance_spread << ")" << std::endl;
        std::cout << "  IP Diversity Check: " << (ip_ok ? "PASS" : "FAIL")
                  << " (" << ip_count.size() << "/" << devices.size() << " unique)" << std::endl;
        std::cout << "  Nonce Pattern Check: " << (pattern_ok ? "PASS" : "FAIL")
                  << " (" << pattern_count.size() << " patterns)" << std::endl;
        
        return timing_ok && ip_ok && pattern_ok;
    }
};

// Simplified reward calculator
class SimpleRewardCalculator {
public:
    struct RewardInfo {
        double base_reward;
        double small_miner_bonus;
        double podd_bonus;
        double total_reward;
        double multiplier;
    };
    
    RewardInfo CalculateReward(double hashrate_ths, bool podd_verified, int device_count) {
        RewardInfo info;
        info.base_reward = 50.0; // 50 SYNC base
        
        // Small miner bonus
        double boost_multiplier = 1.0;
        if (hashrate_ths < 1.0) {
            boost_multiplier = 2.0; // 2x for < 1 TH/s
        } else if (hashrate_ths < 10.0) {
            boost_multiplier = 1.5; // 1.5x for 1-10 TH/s
        } else if (hashrate_ths < 100.0) {
            boost_multiplier = 1.2; // 1.2x for 10-100 TH/s
        }
        
        info.small_miner_bonus = info.base_reward * (boost_multiplier - 1.0);
        
        // PoDD bonus
        if (podd_verified && device_count > 1) {
            info.podd_bonus = info.base_reward * 0.1; // 10% base
            info.podd_bonus += info.base_reward * (device_count * 0.01); // 1% per device
            if (info.podd_bonus > info.base_reward * 0.2) {
                info.podd_bonus = info.base_reward * 0.2; // Cap at 20%
            }
        } else {
            info.podd_bonus = 0;
        }
        
        info.total_reward = info.base_reward + info.small_miner_bonus + info.podd_bonus;
        info.multiplier = info.total_reward / info.base_reward;
        
        return info;
    }
};

void ShowBanner() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════╗
║            SyntheticCoin (SYNC) Demo v0.1.0             ║
║         The Small Miner's Cryptocurrency                 ║
║                                                          ║
║  Featuring: Proof-of-Device-Distribution (PoDD)         ║
║            Small Miner Rewards & Squad Mining           ║
╚══════════════════════════════════════════════════════════╝
)" << std::endl;
}

void DemoPoDD() {
    std::cout << "\n=== Proof-of-Device-Distribution Demo ===" << std::endl;
    std::cout << "Testing whether mining comes from multiple Bitaxes or one Antminer...\n" << std::endl;
    
    SimplePoDDVerifier verifier;
    
    // Scenario 1: Real multiple Bitaxes
    std::cout << "Scenario 1: 5 Real Bitaxe Devices" << std::endl;
    std::vector<SimplePoDDVerifier::DeviceInfo> real_devices = {
        {"BITAXE_001", 500, 0.15, "192.168.1.101", 1},
        {"BITAXE_002", 480, 0.22, "192.168.1.102", 2},
        {"BITAXE_003", 510, 0.18, "10.0.0.5", 1},
        {"BITAXE_004", 495, 0.25, "172.16.0.10", 3},
        {"BITAXE_005", 505, 0.20, "192.168.2.50", 2}
    };
    
    bool real_valid = verifier.VerifyDevices(real_devices);
    std::cout << "Result: " << (real_valid ? "✓ VERIFIED - Real distributed devices!" 
                                           : "✗ FAILED - Possible spoofing") << std::endl;
    
    // Scenario 2: Spoofed devices (Antminer pretending)
    std::cout << "\nScenario 2: Antminer Pretending to be 5 Bitaxes" << std::endl;
    std::vector<SimplePoDDVerifier::DeviceInfo> fake_devices = {
        {"FAKE_001", 500, 0.10, "192.168.1.100", 1},
        {"FAKE_002", 500, 0.11, "192.168.1.100", 1},
        {"FAKE_003", 500, 0.10, "192.168.1.100", 1},
        {"FAKE_004", 500, 0.11, "192.168.1.100", 1},
        {"FAKE_005", 500, 0.10, "192.168.1.100", 1}
    };
    
    bool fake_valid = verifier.VerifyDevices(fake_devices);
    std::cout << "Result: " << (fake_valid ? "✓ VERIFIED - Real distributed devices!" 
                                           : "✗ FAILED - Spoofing detected!") << std::endl;
}

void DemoRewards() {
    std::cout << "\n=== Mining Reward Calculation Demo ===" << std::endl;
    std::cout << "Showing rewards for different miner sizes:\n" << std::endl;
    
    SimpleRewardCalculator calc;
    
    struct TestCase {
        std::string name;
        double hashrate_ths;
        bool podd_verified;
        int devices;
    };
    
    std::vector<TestCase> cases = {
        {"Single Bitaxe", 0.5, true, 1},
        {"5 Bitaxes (Squad)", 2.5, true, 5},
        {"Small Farm", 25, false, 0},
        {"Large Farm", 150, false, 0}
    };
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Miner Type        | Hashrate | Base  | Bonus | PoDD  | Total | Multi |" << std::endl;
    std::cout << "------------------|----------|-------|-------|-------|-------|-------|" << std::endl;
    
    for (const auto& test : cases) {
        auto reward = calc.CalculateReward(test.hashrate_ths, test.podd_verified, test.devices);
        
        std::cout << std::left << std::setw(17) << test.name << " | "
                  << std::right << std::setw(6) << test.hashrate_ths << " TH | "
                  << std::setw(5) << reward.base_reward << " | "
                  << std::setw(5) << reward.small_miner_bonus << " | "
                  << std::setw(5) << reward.podd_bonus << " | "
                  << std::setw(5) << reward.total_reward << " | "
                  << std::setw(4) << reward.multiplier << "x |" << std::endl;
    }
    
    std::cout << "\nPer TH/s Earnings Comparison:" << std::endl;
    auto bitaxe = calc.CalculateReward(0.5, true, 1);
    auto large = calc.CalculateReward(150, false, 0);
    
    double bitaxe_per_ths = bitaxe.total_reward / 0.5;
    double large_per_ths = large.total_reward / 150;
    
    std::cout << "  Bitaxe earns: " << bitaxe_per_ths << " SYNC per TH/s" << std::endl;
    std::cout << "  Large farm earns: " << large_per_ths << " SYNC per TH/s" << std::endl;
    std::cout << "  Advantage: " << (bitaxe_per_ths / large_per_ths) << "x" << std::endl;
}

void DemoSquadMining() {
    std::cout << "\n=== Squad Mining Demo ===" << std::endl;
    std::cout << "Forming a squad of small miners for better rewards:\n" << std::endl;
    
    struct SquadMember {
        std::string id;
        double hashrate_ghs;
        double contribution_percent;
    };
    
    std::vector<SquadMember> squad = {
        {"Alice_Bitaxe", 500, 0},
        {"Bob_Bitaxe_1", 480, 0},
        {"Bob_Bitaxe_2", 520, 0},
        {"Carol_Mini", 250, 0},
        {"Dave_Bitaxe", 510, 0}
    };
    
    double total_hashrate = 0;
    for (const auto& member : squad) {
        total_hashrate += member.hashrate_ghs;
    }
    
    std::cout << "Squad Members:" << std::endl;
    for (auto& member : squad) {
        member.contribution_percent = (member.hashrate_ghs / total_hashrate) * 100;
        std::cout << "  " << std::setw(15) << member.id 
                  << ": " << std::setw(4) << member.hashrate_ghs << " GH/s"
                  << " (" << std::fixed << std::setprecision(1) 
                  << member.contribution_percent << "%)" << std::endl;
    }
    
    std::cout << "\nSquad Statistics:" << std::endl;
    std::cout << "  Total Hashrate: " << (total_hashrate / 1000) << " TH/s" << std::endl;
    std::cout << "  Squad Size: " << squad.size() << " devices" << std::endl;
    std::cout << "  Squad Bonus: +" << (squad.size() * 2) << "%" << std::endl;
    
    SimpleRewardCalculator calc;
    auto solo_reward = calc.CalculateReward(0.5, false, 1);
    auto squad_reward = calc.CalculateReward(total_hashrate / 1000, true, squad.size());
    
    std::cout << "\nReward Comparison (per block found):" << std::endl;
    std::cout << "  Solo mining: " << solo_reward.total_reward << " SYNC (if you find a block)" << std::endl;
    std::cout << "  Squad total: " << squad_reward.total_reward << " SYNC" << std::endl;
    std::cout << "  Your share (25%): " << (squad_reward.total_reward * 0.25) << " SYNC" << std::endl;
    std::cout << "\n  ✓ More consistent rewards!" << std::endl;
    std::cout << "  ✓ Lower variance!" << std::endl;
    std::cout << "  ✓ Community support!" << std::endl;
}

void ShowNetworkStats() {
    std::cout << "\n=== Simulated Network Statistics ===" << std::endl;
    
    // Simulate network data
    std::mt19937 gen(42); // Fixed seed for consistent demo
    std::normal_distribution<> hashrate_dist(2.5, 3.0);
    
    int total_devices = 5678;
    int active_devices = 4321;
    double network_hashrate = 0;
    
    // Calculate distribution
    std::map<std::string, int> tier_distribution = {
        {"<1 TH/s", 3456},
        {"1-10 TH/s", 765},
        {"10-100 TH/s", 89},
        {">100 TH/s", 11}
    };
    
    std::cout << "\nNetwork Overview:" << std::endl;
    std::cout << "  Registered Devices: " << total_devices << std::endl;
    std::cout << "  Active Devices: " << active_devices << std::endl;
    std::cout << "  Network Hashrate: " << "12.5 PH/s" << std::endl;
    std::cout << "  Average per Device: " << "2.9 TH/s" << std::endl;
    
    std::cout << "\nHashrate Distribution:" << std::endl;
    for (const auto& [tier, count] : tier_distribution) {
        double percent = (static_cast<double>(count) / active_devices) * 100;
        std::cout << "  " << std::setw(12) << tier << ": " 
                  << std::setw(4) << count << " devices ("
                  << std::fixed << std::setprecision(1) << percent << "%)" << std::endl;
    }
    
    // Calculate decentralization score
    double small_miner_percent = 80.0;
    double largest_miner_percent = 8.5;
    double nakamoto_coefficient = 25;
    
    std::cout << "\nDecentralization Metrics:" << std::endl;
    std::cout << "  Small Miners (<10 TH/s): " << small_miner_percent << "%" << std::endl;
    std::cout << "  Largest Miner: " << largest_miner_percent << "%" << std::endl;
    std::cout << "  Nakamoto Coefficient: " << nakamoto_coefficient << std::endl;
    
    double score = (small_miner_percent / 100) * 0.4 + 
                   (1.0 - largest_miner_percent / 100) * 0.3 +
                   std::min(1.0, nakamoto_coefficient / 50.0) * 0.3;
    
    std::cout << "  Overall Score: " << (score * 100) << "%" << std::endl;
    std::cout << "  Status: " << (score > 0.7 ? "✓ HEALTHY - Well decentralized" 
                                              : "⚠ WARNING - Needs improvement") << std::endl;
}

int main(int argc, char* argv[]) {
    ShowBanner();
    
    if (argc > 1) {
        std::string cmd = argv[1];
        
        if (cmd == "podd") {
            DemoPoDD();
        } else if (cmd == "rewards") {
            DemoRewards();
        } else if (cmd == "squad") {
            DemoSquadMining();
        } else if (cmd == "stats") {
            ShowNetworkStats();
        } else if (cmd == "--help" || cmd == "-h") {
            std::cout << "Usage: " << argv[0] << " [command]" << std::endl;
            std::cout << "\nCommands:" << std::endl;
            std::cout << "  podd     - Demonstrate Proof-of-Device-Distribution" << std::endl;
            std::cout << "  rewards  - Show reward calculations" << std::endl;
            std::cout << "  squad    - Demonstrate squad mining" << std::endl;
            std::cout << "  stats    - Show network statistics" << std::endl;
            std::cout << "  (none)   - Run all demos" << std::endl;
        } else {
            std::cout << "Unknown command. Use --help for options." << std::endl;
        }
    } else {
        // Run all demos
        DemoPoDD();
        DemoRewards();
        DemoSquadMining();
        ShowNetworkStats();
        
        std::cout << "\n=== Summary ===" << std::endl;
        std::cout << "\nSyntheticCoin (SYNC) solves the fundamental problem:" << std::endl;
        std::cout << "  'How do you prove 10 Bitaxes aren't 1 Antminer in disguise?'" << std::endl;
        std::cout << "\nOur solution:" << std::endl;
        std::cout << "  • Proof-of-Device-Distribution (PoDD) verification" << std::endl;
        std::cout << "  • 2x rewards for miners under 1 TH/s" << std::endl;
        std::cout << "  • Squad mining for consistent earnings" << std::endl;
        std::cout << "  • True decentralization, cryptographically enforced" << std::endl;
        std::cout << "\nMaking mining profitable for small miners again! 🚀" << std::endl;
    }
    
    return 0;
}
