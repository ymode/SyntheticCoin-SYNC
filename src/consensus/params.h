// Copyright (c) 2024 SyntheticCoin Developers
// Distributed under the MIT software license

#ifndef SYNC_CONSENSUS_PARAMS_H
#define SYNC_CONSENSUS_PARAMS_H

#include <stdint.h>
#include <limits>
#include <map>
#include <string>

namespace Consensus {

/**
 * Parameters that influence chain consensus.
 * Optimized for small-scale miners (Bitaxe, etc.)
 */
struct Params {
    /** Block time target (5 minutes for SYNC) */
    int64_t nPowTargetSpacing = 5 * 60; // 300 seconds
    
    /** Difficulty adjustment interval (24 hours for faster response) */
    int64_t DifficultyAdjustmentInterval() const {
        return 288; // 288 blocks = ~24 hours at 5 min/block
    }
    
    /** Initial block subsidy (50 SYNC to support small miners) */
    int64_t nInitialSubsidy = 50 * 100000000; // 50 SYNC in satoshis
    
    /** Subsidy halving interval */
    int32_t nSubsidyHalvingInterval = 210000;
    
    /** Maximum supply (84 million SYNC) */
    int64_t nMaxMoneySupply = 84000000 * 100000000LL; // 84M SYNC
    
    /** Minimum difficulty for small miners */
    uint32_t nMinimumDifficulty = 1; // Very low for Bitaxe testing
    
    /** Maximum block size */
    uint32_t nMaxBlockSize = 2000000; // 2MB blocks
    
    /** 
     * Small Miner Boost Parameters 
     * Rewards multipliers based on hashrate
     */
    struct MinerBoost {
        double tier1_hashrate = 1.0;      // < 1 TH/s
        double tier1_multiplier = 2.0;    // 2x rewards
        
        double tier2_hashrate = 10.0;     // 1-10 TH/s  
        double tier2_multiplier = 1.5;    // 1.5x rewards
        
        double tier3_hashrate = 100.0;    // 10-100 TH/s
        double tier3_multiplier = 1.2;    // 1.2x rewards
        
        double tier4_multiplier = 1.0;    // >100 TH/s (standard)
    } minerBoost;
    
    /**
     * Proof-of-Device-Distribution (PoDD) Parameters
     */
    struct PoDD {
        /** Minimum devices for squad formation */
        uint32_t nMinSquadSize = 2;
        
        /** Maximum devices per squad */
        uint32_t nMaxSquadSize = 10;
        
        /** Device verification interval (blocks) */
        uint32_t nVerificationInterval = 1000;
        
        /** Maximum devices per IP address */
        uint32_t nMaxDevicesPerIP = 5;
        
        /** Timing variance threshold (microseconds) */
        uint64_t nTimingVarianceThreshold = 1000; // 1ms
        
        /** Device registration fee (in SYNC satoshis) */
        int64_t nRegistrationFee = 10000000; // 0.1 SYNC
        
        /** PoDD verification bonus multiplier */
        double nVerifiedDeviceBonus = 1.1; // 10% bonus for verified devices
        
        /** Anti-gaming slashing percentage */
        double nSlashingPercentage = 0.5; // 50% penalty for cheating
    } podd;
    
    /**
     * Network timing parameters for latency verification
     */
    struct NetworkTiming {
        /** Minimum ping interval (seconds) */
        uint32_t nMinPingInterval = 60;
        
        /** Maximum acceptable latency variance (ms) */
        uint32_t nMaxLatencyVariance = 500;
        
        /** Geographic distribution requirement (km) */
        uint32_t nMinGeographicDistance = 10;
    } networkTiming;
    
    /** Chain ID to prevent replay attacks */
    uint32_t nChainId = 0x53594E43; // "SYNC" in hex
    
    /** Activation heights for features */
    std::map<std::string, int32_t> activationHeights = {
        {"podd", 1000},           // PoDD activation
        {"small_miner_boost", 1},  // Active from genesis
        {"squad_mining", 2000},    // Squad mining activation
        {"device_registry", 500}   // Device registration activation
    };
    
    /**
     * Get reward multiplier based on hashrate
     * @param hashrate_ths Hashrate in TH/s
     * @return Multiplier for block reward
     */
    double GetMinerBoostMultiplier(double hashrate_ths) const {
        if (hashrate_ths < minerBoost.tier1_hashrate) {
            return minerBoost.tier1_multiplier;
        } else if (hashrate_ths < minerBoost.tier2_hashrate) {
            return minerBoost.tier2_multiplier;
        } else if (hashrate_ths < minerBoost.tier3_hashrate) {
            return minerBoost.tier3_multiplier;
        }
        return minerBoost.tier4_multiplier;
    }
    
    /**
     * Check if a feature is active at given height
     */
    bool IsFeatureActive(const std::string& feature, int32_t height) const {
        auto it = activationHeights.find(feature);
        if (it != activationHeights.end()) {
            return height >= it->second;
        }
        return false;
    }
};

/**
 * Testnet parameters - even lower difficulty for Bitaxe testing
 */
struct TestNetParams : public Params {
    TestNetParams() {
        nMinimumDifficulty = 1;
        nPowTargetSpacing = 60; // 1 minute blocks on testnet
        minerBoost.tier1_hashrate = 0.1; // 100 GH/s for testnet
        nChainId = 0x54455354; // "TEST" in hex
        
        // Activate all features immediately on testnet
        activationHeights = {
            {"podd", 1},
            {"small_miner_boost", 1},
            {"squad_mining", 1},
            {"device_registry", 1}
        };
    }
};

/**
 * Regtest parameters - for development
 */
struct RegTestParams : public Params {
    RegTestParams() {
        nMinimumDifficulty = 1;
        nPowTargetSpacing = 1; // 1 second blocks
        minerBoost.tier1_hashrate = 0.001; // 1 GH/s
        nChainId = 0x52454754; // "REGT" in hex
    }
};

} // namespace Consensus

#endif // SYNC_CONSENSUS_PARAMS_H
