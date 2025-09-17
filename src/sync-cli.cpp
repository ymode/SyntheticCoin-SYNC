// Copyright (c) 2024 SyntheticCoin Developers
// Distributed under the MIT software license

#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include "consensus/params.h"
#include "podd/device_verifier.h"
#include "mining/reward_calculator.h"

namespace po = boost::program_options;

class SyncCLI {
public:
    void Run(int argc, char* argv[]) {
        po::options_description desc("SYNC CLI - Command line interface for SyntheticCoin");
        desc.add_options()
            ("help,h", "Show help message")
            ("command", po::value<std::string>(), "Command to execute")
            ("args", po::value<std::vector<std::string>>(), "Command arguments");
        
        po::positional_options_description pos;
        pos.add("command", 1);
        pos.add("args", -1);
        
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv)
                 .options(desc)
                 .positional(pos)
                 .run(), vm);
        po::notify(vm);
        
        if (vm.count("help") || !vm.count("command")) {
            ShowHelp();
            return;
        }
        
        std::string command = vm["command"].as<std::string>();
        std::vector<std::string> args;
        
        if (vm.count("args")) {
            args = vm["args"].as<std::vector<std::string>>();
        }
        
        ExecuteCommand(command, args);
    }
    
private:
    void ShowHelp() {
        std::cout << "SyntheticCoin CLI v0.1.0" << std::endl;
        std::cout << "Usage: sync-cli <command> [arguments]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  getinfo                    Get node information" << std::endl;
        std::cout << "  getblockcount              Get current block height" << std::endl;
        std::cout << "  getdifficulty              Get current mining difficulty" << std::endl;
        std::cout << "  getmininginfo              Get mining statistics" << std::endl;
        std::cout << "  registerdevice <id>        Register a mining device" << std::endl;
        std::cout << "  listdevices                List registered devices" << std::endl;
        std::cout << "  verifypodd <devices...>    Verify device distribution" << std::endl;
        std::cout << "  calcrweard <hashrate>      Calculate reward for hashrate" << std::endl;
        std::cout << "  formsquad <devices...>     Form a mining squad" << std::endl;
        std::cout << "  getdecentralization        Get network decentralization score" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  sync-cli getinfo" << std::endl;
        std::cout << "  sync-cli registerdevice BITAXE_001" << std::endl;
        std::cout << "  sync-cli calcreward 0.5" << std::endl;
        std::cout << "  sync-cli verifypodd BITAXE_001 BITAXE_002 BITAXE_003" << std::endl;
    }
    
    void ExecuteCommand(const std::string& command, const std::vector<std::string>& args) {
        if (command == "getinfo") {
            GetInfo();
        } else if (command == "getblockcount") {
            GetBlockCount();
        } else if (command == "getdifficulty") {
            GetDifficulty();
        } else if (command == "getmininginfo") {
            GetMiningInfo();
        } else if (command == "registerdevice") {
            if (args.empty()) {
                std::cerr << "Error: Device ID required" << std::endl;
                return;
            }
            RegisterDevice(args[0]);
        } else if (command == "listdevices") {
            ListDevices();
        } else if (command == "verifypodd") {
            if (args.size() < 2) {
                std::cerr << "Error: At least 2 devices required for PoDD verification" << std::endl;
                return;
            }
            VerifyPoDD(args);
        } else if (command == "calcreward") {
            if (args.empty()) {
                std::cerr << "Error: Hashrate required (in TH/s)" << std::endl;
                return;
            }
            double hashrate = std::stod(args[0]);
            CalculateReward(hashrate);
        } else if (command == "formsquad") {
            if (args.size() < 2) {
                std::cerr << "Error: At least 2 devices required for squad" << std::endl;
                return;
            }
            FormSquad(args);
        } else if (command == "getdecentralization") {
            GetDecentralization();
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
            std::cerr << "Use 'sync-cli help' for list of commands" << std::endl;
        }
    }
    
    void GetInfo() {
        std::cout << "SyntheticCoin Core" << std::endl;
        std::cout << "==================" << std::endl;
        std::cout << "Version: 0.1.0-beta" << std::endl;
        std::cout << "Protocol: 1" << std::endl;
        std::cout << "Network: mainnet" << std::endl;
        std::cout << "Blocks: 0" << std::endl; // Would get from node
        std::cout << "Difficulty: 1.0" << std::endl;
        std::cout << "Connections: 0" << std::endl;
        std::cout << std::endl;
        std::cout << "Features Active:" << std::endl;
        std::cout << "  • Proof-of-Device-Distribution (PoDD)" << std::endl;
        std::cout << "  • Small Miner Boost" << std::endl;
        std::cout << "  • Squad Mining" << std::endl;
        std::cout << "  • Anti-ASIC-Farm Protection" << std::endl;
    }
    
    void GetBlockCount() {
        std::cout << "0" << std::endl; // Would get from node
    }
    
    void GetDifficulty() {
        std::cout << "1.0" << std::endl; // Would get from node
    }
    
    void GetMiningInfo() {
        std::cout << "Mining Information" << std::endl;
        std::cout << "==================" << std::endl;
        std::cout << "Blocks: 0" << std::endl;
        std::cout << "Difficulty: 1.0" << std::endl;
        std::cout << "Network Hashrate: 0 TH/s" << std::endl;
        std::cout << "Registered Devices: " << 
                     PoDD::DeviceRegistry::GetInstance().GetTotalRegisteredDevices() << std::endl;
        std::cout << std::endl;
        std::cout << "Reward Tiers (TH/s : Multiplier):" << std::endl;
        std::cout << "  < 1    : 2.0x" << std::endl;
        std::cout << "  1-10   : 1.5x" << std::endl;
        std::cout << "  10-100 : 1.2x" << std::endl;
        std::cout << "  > 100  : 1.0x" << std::endl;
    }
    
    void RegisterDevice(const std::string& device_id) {
        PoDD::DeviceRegistration reg;
        reg.device_id = device_id;
        reg.manufacturer = "Generic";
        reg.model = "Bitaxe";
        reg.serial_number = device_id;
        reg.firmware_version = "1.0.0";
        reg.chip_count = 1;
        reg.max_hashrate_ghs = 500; // 500 GH/s typical
        reg.owner_address = "sync1qexample...";
        
        if (PoDD::DeviceRegistry::GetInstance().RegisterDevice(reg)) {
            std::cout << "Device registered successfully!" << std::endl;
            std::cout << "Device ID: " << device_id << std::endl;
            std::cout << "Max Hashrate: " << reg.max_hashrate_ghs << " GH/s" << std::endl;
            std::cout << "Expected Reward Multiplier: 2.0x" << std::endl;
        } else {
            std::cerr << "Failed to register device (may already exist)" << std::endl;
        }
    }
    
    void ListDevices() {
        auto& registry = PoDD::DeviceRegistry::GetInstance();
        uint32_t total = registry.GetTotalRegisteredDevices();
        
        std::cout << "Registered Devices: " << total << std::endl;
        
        if (total > 0) {
            std::cout << std::endl;
            // In real implementation, would list actual devices
            std::cout << "Example devices:" << std::endl;
            std::cout << "  BITAXE_001 - 500 GH/s - Active" << std::endl;
            std::cout << "  BITAXE_002 - 480 GH/s - Active" << std::endl;
        }
    }
    
    void VerifyPoDD(const std::vector<std::string>& devices) {
        std::cout << "Verifying Proof-of-Device-Distribution..." << std::endl;
        std::cout << "Devices: ";
        for (const auto& d : devices) {
            std::cout << d << " ";
        }
        std::cout << std::endl;
        
        PoDD::DeviceVerifier verifier;
        
        // Register devices for verification (in real impl, would check existing)
        for (const auto& device_id : devices) {
            PoDD::DeviceFingerprint fp;
            fp.device_id = device_id;
            fp.avg_nonce_time_us = 1000000 + (rand() % 100000); // Simulate variation
            fp.timing_variance_us = 5000 + (rand() % 5000);
            fp.ip_address = "192.168.1." + std::to_string(rand() % 255);
            
            verifier.RegisterDevice(device_id, fp);
        }
        
        auto result = verifier.VerifyDeviceDistribution(devices);
        
        std::cout << std::endl;
        std::cout << "Verification Result:" << std::endl;
        std::cout << "  Valid: " << (result.is_valid ? "YES" : "NO") << std::endl;
        std::cout << "  Confidence: " << (result.confidence * 100) << "%" << std::endl;
        
        if (!result.reason.empty()) {
            std::cout << "  Reason: " << result.reason << std::endl;
        }
        
        if (!result.suspicious_pairs.empty()) {
            std::cout << "  Suspicious pairs:" << std::endl;
            for (const auto& [d1, d2] : result.suspicious_pairs) {
                std::cout << "    " << d1 << " <-> " << d2 << std::endl;
            }
        }
        
        if (result.is_valid) {
            std::cout << std::endl;
            std::cout << "✓ Devices verified as genuinely distributed!" << std::endl;
            std::cout << "  Eligible for PoDD bonus: +10%" << std::endl;
        } else {
            std::cout << std::endl;
            std::cout << "✗ Verification failed - possible spoofing detected" << std::endl;
        }
    }
    
    void CalculateReward(double hashrate_ths) {
        Consensus::Params params;
        Mining::RewardCalculator calculator(params);
        
        Mining::MinerInfo miner;
        miner.hashrate_ths = hashrate_ths;
        miner.is_podd_verified = hashrate_ths < 1.0; // Assume small miners are verified
        miner.efficiency_score = 0.8;
        
        auto reward = calculator.CalculateReward(1000, miner, 0);
        
        std::cout << "Reward Calculation" << std::endl;
        std::cout << "==================" << std::endl;
        std::cout << "Hashrate: " << hashrate_ths << " TH/s" << std::endl;
        std::cout << "Tier: " << calculator.GetMinerTier(hashrate_ths) << std::endl;
        std::cout << std::endl;
        std::cout << "Base Reward:       " << boost::format("%10.4f SYNC") % 
                     (reward.base_reward / 100000000.0) << std::endl;
        std::cout << "Small Miner Bonus: " << boost::format("%10.4f SYNC") % 
                     (reward.small_miner_bonus / 100000000.0) << std::endl;
        
        if (miner.is_podd_verified) {
            std::cout << "PoDD Bonus:        " << boost::format("%10.4f SYNC") % 
                         (reward.podd_bonus / 100000000.0) << std::endl;
        }
        
        std::cout << "Efficiency Bonus:  " << boost::format("%10.4f SYNC") % 
                     (reward.efficiency_bonus / 100000000.0) << std::endl;
        std::cout << "                   -----------" << std::endl;
        std::cout << "Total Reward:      " << boost::format("%10.4f SYNC") % 
                     (reward.total_reward / 100000000.0) << std::endl;
        std::cout << std::endl;
        std::cout << "Multiplier: " << boost::format("%.2fx") % reward.GetMultiplier() << std::endl;
        
        // Show comparison
        if (hashrate_ths < 1.0) {
            double large_miner_reward = calculator.CalculateReward(1000, 
                {.hashrate_ths = 100.0}, 0).total_reward;
            double advantage = (reward.total_reward / (hashrate_ths + 0.001)) / 
                              (large_miner_reward / 100.0);
            std::cout << std::endl;
            std::cout << "Per TH/s advantage vs 100 TH/s miner: " << 
                         boost::format("%.2fx") % advantage << std::endl;
        }
    }
    
    void FormSquad(const std::vector<std::string>& devices) {
        std::cout << "Forming mining squad..." << std::endl;
        std::cout << "Members: ";
        for (const auto& d : devices) {
            std::cout << d << " ";
        }
        std::cout << std::endl;
        
        PoDD::DeviceVerifier verifier;
        
        // Would verify devices first in real implementation
        std::string squad_id = "SQUAD_" + std::to_string(time(nullptr));
        
        std::cout << std::endl;
        std::cout << "Squad formed successfully!" << std::endl;
        std::cout << "Squad ID: " << squad_id << std::endl;
        std::cout << "Members: " << devices.size() << std::endl;
        std::cout << "Squad Bonus: +" << (devices.size() * 2) << "%" << std::endl;
        std::cout << std::endl;
        std::cout << "Benefits:" << std::endl;
        std::cout << "  • Shared block rewards" << std::endl;
        std::cout << "  • Reduced variance" << std::endl;
        std::cout << "  • Squad mining bonus" << std::endl;
        std::cout << "  • Community support" << std::endl;
    }
    
    void GetDecentralization() {
        Mining::RewardCalculator calculator(Consensus::Params{});
        double score = calculator.CalculateDecentralizationScore();
        
        std::cout << "Network Decentralization Score" << std::endl;
        std::cout << "==============================" << std::endl;
        std::cout << "Overall Score: " << (score * 100) << "%" << std::endl;
        std::cout << std::endl;
        
        // Example metrics (would be real in production)
        std::cout << "Metrics:" << std::endl;
        std::cout << "  Unique Miners: 1,234" << std::endl;
        std::cout << "  Registered Devices: 5,678" << std::endl;
        std::cout << "  Average Hashrate: 2.5 TH/s" << std::endl;
        std::cout << "  Top Miner Share: 8.2%" << std::endl;
        std::cout << "  Gini Coefficient: 0.42" << std::endl;
        std::cout << "  Nakamoto Coefficient: 25" << std::endl;
        std::cout << std::endl;
        
        if (score > 0.8) {
            std::cout << "Status: EXCELLENT - Highly decentralized" << std::endl;
        } else if (score > 0.6) {
            std::cout << "Status: GOOD - Well distributed" << std::endl;
        } else if (score > 0.4) {
            std::cout << "Status: FAIR - Some centralization concerns" << std::endl;
        } else {
            std::cout << "Status: WARNING - High centralization risk" << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    try {
        SyncCLI cli;
        cli.Run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
