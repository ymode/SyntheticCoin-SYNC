// Copyright (c) 2024 SyntheticCoin Developers
// Distributed under the MIT software license

#include <iostream>
#include <string>
#include <thread>
#include <csignal>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "consensus/params.h"
#include "podd/device_verifier.h"
#include "mining/reward_calculator.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

// Global shutdown flag
std::atomic<bool> g_shutdown(false);

void SignalHandler(int signal) {
    std::cout << "\nShutdown signal received..." << std::endl;
    g_shutdown = true;
}

class SyncNode {
public:
    SyncNode(const Consensus::Params& params) 
        : m_params(params), 
          m_device_verifier(),
          m_reward_calculator(params) {
    }
    
    bool Initialize(const po::variables_map& vm) {
        std::cout << "==================================" << std::endl;
        std::cout << "SyntheticCoin (SYNC) Node v0.1.0" << std::endl;
        std::cout << "The Small Miner's Cryptocurrency" << std::endl;
        std::cout << "==================================" << std::endl;
        
        // Set data directory
        if (vm.count("datadir")) {
            m_datadir = vm["datadir"].as<std::string>();
        } else {
            m_datadir = fs::path(getenv("HOME")) / ".sync";
        }
        
        // Create data directory if it doesn't exist
        if (!fs::exists(m_datadir)) {
            std::cout << "Creating data directory: " << m_datadir << std::endl;
            fs::create_directories(m_datadir);
        }
        
        // Initialize network
        if (vm.count("testnet")) {
            std::cout << "Running on TESTNET" << std::endl;
            m_is_testnet = true;
        } else if (vm.count("regtest")) {
            std::cout << "Running on REGTEST" << std::endl;
            m_is_regtest = true;
        } else {
            std::cout << "Running on MAINNET" << std::endl;
        }
        
        // Set mining address if provided
        if (vm.count("mineraddress")) {
            m_miner_address = vm["mineraddress"].as<std::string>();
            std::cout << "Mining to address: " << m_miner_address << std::endl;
        }
        
        // Check for Bitaxe mode
        if (vm.count("bitaxe")) {
            m_bitaxe_mode = true;
            std::cout << "BITAXE MODE ENABLED - Optimized for small miners!" << std::endl;
            std::cout << "  • Minimum difficulty: " << m_params.nMinimumDifficulty << std::endl;
            std::cout << "  • Block time: " << m_params.nPowTargetSpacing << " seconds" << std::endl;
            std::cout << "  • Small miner bonuses active" << std::endl;
        }
        
        // Initialize PoDD if devices provided
        if (vm.count("devices")) {
            auto devices = vm["devices"].as<std::vector<std::string>>();
            std::cout << "Registering " << devices.size() << " mining devices:" << std::endl;
            
            for (const auto& device_id : devices) {
                PoDD::DeviceFingerprint fp;
                fp.device_id = device_id;
                fp.firmware_version = "1.0.0"; // Would get from device
                fp.chip_count = 1; // Bitaxe typically has 1 chip
                
                if (m_device_verifier.RegisterDevice(device_id, fp)) {
                    std::cout << "  ✓ Registered device: " << device_id << std::endl;
                } else {
                    std::cout << "  ✗ Failed to register: " << device_id << std::endl;
                }
            }
        }
        
        return true;
    }
    
    void Run() {
        std::cout << "\nNode started successfully!" << std::endl;
        std::cout << "Features enabled:" << std::endl;
        std::cout << "  • Proof-of-Device-Distribution (PoDD)" << std::endl;
        std::cout << "  • Small Miner Boost Protocol" << std::endl;
        std::cout << "  • Squad Mining Support" << std::endl;
        std::cout << "  • Anti-ASIC-Farm Protection" << std::endl;
        
        // Display reward tiers
        std::cout << "\nReward Multipliers:" << std::endl;
        std::cout << "  • <1 TH/s:     " << m_params.minerBoost.tier1_multiplier << "x" << std::endl;
        std::cout << "  • 1-10 TH/s:   " << m_params.minerBoost.tier2_multiplier << "x" << std::endl;
        std::cout << "  • 10-100 TH/s: " << m_params.minerBoost.tier3_multiplier << "x" << std::endl;
        std::cout << "  • >100 TH/s:   " << m_params.minerBoost.tier4_multiplier << "x" << std::endl;
        
        // Main loop
        while (!g_shutdown) {
            // Simulate node operations
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            // In real implementation:
            // - Process network messages
            // - Validate blocks
            // - Update PoDD verifications
            // - Handle mining if enabled
        }
        
        std::cout << "Node shutting down..." << std::endl;
    }
    
    void ShowExampleReward() {
        std::cout << "\n=== Example Reward Calculation ===" << std::endl;
        
        Mining::MinerInfo miner;
        miner.address = "sync1qexample...";
        miner.hashrate_ths = 0.5; // 500 GH/s (typical Bitaxe)
        miner.device_ids = {"BITAXE_001"};
        miner.is_podd_verified = true;
        miner.efficiency_score = 0.8;
        
        auto reward = m_reward_calculator.CalculateReward(1000, miner, 0);
        
        std::cout << "Miner: " << miner.address << std::endl;
        std::cout << "Hashrate: " << miner.hashrate_ths << " TH/s" << std::endl;
        std::cout << "Base Reward: " << reward.base_reward / 100000000.0 << " SYNC" << std::endl;
        std::cout << "Small Miner Bonus: " << reward.small_miner_bonus / 100000000.0 << " SYNC" << std::endl;
        std::cout << "PoDD Bonus: " << reward.podd_bonus / 100000000.0 << " SYNC" << std::endl;
        std::cout << "Total Reward: " << reward.total_reward / 100000000.0 << " SYNC" << std::endl;
        std::cout << "Multiplier: " << reward.GetMultiplier() << "x" << std::endl;
    }
    
private:
    Consensus::Params m_params;
    PoDD::DeviceVerifier m_device_verifier;
    Mining::RewardCalculator m_reward_calculator;
    
    fs::path m_datadir;
    std::string m_miner_address;
    bool m_is_testnet = false;
    bool m_is_regtest = false;
    bool m_bitaxe_mode = false;
};

int main(int argc, char* argv[]) {
    try {
        // Parse command line options
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "Show help message")
            ("version,v", "Show version information")
            ("datadir", po::value<std::string>(), "Data directory path")
            ("testnet", "Use testnet")
            ("regtest", "Use regtest mode")
            ("bitaxe", "Enable Bitaxe optimization mode")
            ("mineraddress", po::value<std::string>(), "Address to mine to")
            ("devices", po::value<std::vector<std::string>>()->multitoken(), 
             "Device IDs to register for PoDD")
            ("showreward", "Show example reward calculation")
            ("daemon", "Run in background")
            ("rpcport", po::value<int>()->default_value(8332), "RPC port")
            ("p2pport", po::value<int>()->default_value(8333), "P2P port");
        
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        
        if (vm.count("help")) {
            std::cout << desc << std::endl;
            return 0;
        }
        
        if (vm.count("version")) {
            std::cout << "SyntheticCoin Core Daemon version v0.1.0-beta" << std::endl;
            std::cout << "Copyright (C) 2024 The SyntheticCoin Developers" << std::endl;
            std::cout << "Optimized for Bitaxe and small-scale miners" << std::endl;
            return 0;
        }
        
        // Set up signal handlers
        std::signal(SIGINT, SignalHandler);
        std::signal(SIGTERM, SignalHandler);
        
        // Determine network parameters
        Consensus::Params params;
        if (vm.count("testnet")) {
            Consensus::TestNetParams testnet_params;
            params = testnet_params;
        } else if (vm.count("regtest")) {
            Consensus::RegTestParams regtest_params;
            params = regtest_params;
        }
        
        // Create and initialize node
        SyncNode node(params);
        
        if (!node.Initialize(vm)) {
            std::cerr << "Failed to initialize node" << std::endl;
            return 1;
        }
        
        // Show example reward if requested
        if (vm.count("showreward")) {
            node.ShowExampleReward();
            return 0;
        }
        
        // Run node
        node.Run();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
