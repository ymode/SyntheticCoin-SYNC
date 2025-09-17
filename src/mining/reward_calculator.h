// Copyright (c) 2024 SyntheticCoin Developers
// Distributed under the MIT software license

#ifndef SYNC_MINING_REWARD_CALCULATOR_H
#define SYNC_MINING_REWARD_CALCULATOR_H

#include <stdint.h>
#include <string>
#include <vector>
#include "../consensus/params.h"
#include "../podd/device_verifier.h"

namespace Mining {

/**
 * Miner information for reward calculation
 */
struct MinerInfo {
    std::string address;              // SYNC address
    double hashrate_ths;              // Hashrate in TH/s
    std::vector<std::string> device_ids; // Registered device IDs
    bool is_squad_member;             // Part of a mining squad
    std::string squad_id;             // Squad ID if applicable
    bool is_podd_verified;            // Passed PoDD verification
    double efficiency_score;          // Power efficiency (0.0 to 1.0)
    uint32_t consecutive_blocks;      // Consecutive blocks mined
    uint64_t total_shares_submitted;  // Total shares in current period
};

/**
 * Block reward breakdown
 */
struct RewardBreakdown {
    int64_t base_reward;              // Base block reward
    int64_t small_miner_bonus;        // Bonus for small miners
    int64_t podd_bonus;               // PoDD verification bonus
    int64_t efficiency_bonus;         // Efficiency bonus
    int64_t squad_bonus;              // Squad participation bonus
    int64_t total_reward;             // Total reward
    
    // Fee distribution
    int64_t community_fund;           // To community fund
    int64_t development_fund;         // To development fund
    int64_t miner_fees;               // Transaction fees to miner
    
    double GetMultiplier() const {
        if (base_reward == 0) return 1.0;
        return static_cast<double>(total_reward) / static_cast<double>(base_reward);
    }
};

/**
 * Main reward calculator for SYNC mining
 */
class RewardCalculator {
public:
    RewardCalculator(const Consensus::Params& params);
    
    /**
     * Calculate total reward for a miner
     * @param height Block height
     * @param miner Miner information
     * @param tx_fees Transaction fees in block
     * @return Detailed reward breakdown
     */
    RewardBreakdown CalculateReward(int32_t height, 
                                   const MinerInfo& miner,
                                   int64_t tx_fees);
    
    /**
     * Get base subsidy at given height
     * @param height Block height
     * @return Base subsidy in satoshis
     */
    int64_t GetBaseSubsidy(int32_t height) const;
    
    /**
     * Calculate small miner bonus
     * @param hashrate_ths Miner's hashrate in TH/s
     * @param base_reward Base block reward
     * @return Bonus amount in satoshis
     */
    int64_t CalculateSmallMinerBonus(double hashrate_ths, int64_t base_reward) const;
    
    /**
     * Calculate PoDD verification bonus
     * @param is_verified Whether miner passed PoDD verification
     * @param device_count Number of verified devices
     * @param base_reward Base block reward
     * @return Bonus amount in satoshis
     */
    int64_t CalculatePoDDBonus(bool is_verified, size_t device_count, int64_t base_reward) const;
    
    /**
     * Calculate efficiency bonus
     * @param efficiency_score Efficiency score (0.0 to 1.0)
     * @param base_reward Base block reward
     * @return Bonus amount in satoshis
     */
    int64_t CalculateEfficiencyBonus(double efficiency_score, int64_t base_reward) const;
    
    /**
     * Calculate squad mining bonus
     * @param is_squad_member Whether miner is in a squad
     * @param squad_size Size of the squad
     * @param base_reward Base block reward
     * @return Bonus amount in satoshis
     */
    int64_t CalculateSquadBonus(bool is_squad_member, size_t squad_size, int64_t base_reward) const;
    
    /**
     * Distribute reward among squad members
     * @param total_reward Total reward for squad
     * @param squad_id Squad identifier
     * @return Vector of (address, amount) pairs
     */
    std::vector<std::pair<std::string, int64_t>> DistributeSquadReward(
        int64_t total_reward, const std::string& squad_id) const;
    
    /**
     * Check if miner qualifies for small miner tier
     * @param hashrate_ths Hashrate in TH/s
     * @return Tier number (1-4) or 0 if not qualified
     */
    uint32_t GetMinerTier(double hashrate_ths) const;
    
    /**
     * Calculate anti-whale penalty
     * @param consecutive_blocks Number of consecutive blocks by same miner
     * @return Penalty multiplier (0.0 to 1.0, where 1.0 = no penalty)
     */
    double CalculateAntiWhalePenalty(uint32_t consecutive_blocks) const;
    
    /**
     * Validate reward calculation
     * @param breakdown Reward breakdown to validate
     * @return True if valid, false otherwise
     */
    bool ValidateRewardBreakdown(const RewardBreakdown& breakdown) const;
    
    /**
     * Get maximum possible reward at height
     * @param height Block height
     * @return Maximum reward in satoshis
     */
    int64_t GetMaxPossibleReward(int32_t height) const;
    
    /**
     * Calculate network decentralization score
     * @return Score from 0.0 (centralized) to 1.0 (perfectly decentralized)
     */
    double CalculateDecentralizationScore() const;
    
private:
    Consensus::Params m_params;
    PoDD::DeviceVerifier m_device_verifier;
    
    // Helper functions
    int32_t GetHalvingEpoch(int32_t height) const;
    bool IsFeatureActive(const std::string& feature, int32_t height) const;
    
    // Caching for efficiency
    mutable std::map<int32_t, int64_t> m_subsidy_cache;
};

/**
 * Statistics tracker for mining rewards
 */
class RewardStatistics {
public:
    struct Stats {
        uint64_t total_rewards_paid;
        uint64_t total_bonuses_paid;
        uint64_t rewards_to_small_miners;
        uint64_t rewards_to_large_miners;
        uint32_t unique_miners;
        uint32_t verified_devices;
        double average_hashrate;
        double network_decentralization_score;
        
        // Distribution metrics
        double gini_coefficient;      // Income inequality measure
        double herfindahl_index;      // Market concentration
        double nakamoto_coefficient;  // Minimum entities controlling 51%
    };
    
    void RecordReward(const MinerInfo& miner, const RewardBreakdown& reward);
    Stats GetStatistics(uint32_t last_n_blocks = 0) const;
    void PruneOldData(int32_t current_height);
    
    // Analysis functions
    std::vector<std::pair<std::string, int64_t>> GetTopMiners(size_t count) const;
    std::map<uint32_t, uint32_t> GetTierDistribution() const;
    double GetSmallMinerPercentage() const;
    
private:
    struct RewardRecord {
        int32_t height;
        std::string miner_address;
        int64_t amount;
        double hashrate;
        bool is_small_miner;
        bool is_podd_verified;
        std::chrono::system_clock::time_point timestamp;
    };
    
    std::vector<RewardRecord> m_records;
    std::map<std::string, int64_t> m_miner_totals;
};

/**
 * Dynamic reward adjuster based on network conditions
 */
class DynamicRewardAdjuster {
public:
    /**
     * Adjust rewards based on network decentralization
     * @param current_score Current decentralization score
     * @param target_score Target decentralization score
     * @return Adjustment multiplier
     */
    double GetDecentralizationAdjustment(double current_score, double target_score) const;
    
    /**
     * Emergency adjustment for extreme centralization
     * @param top_miner_percentage Percentage of blocks by top miner
     * @return Emergency multiplier (can be < 1.0 for penalties)
     */
    double GetEmergencyAdjustment(double top_miner_percentage) const;
    
    /**
     * Calculate optimal reward distribution
     * @param network_hashrate Total network hashrate
     * @param device_count Number of active devices
     * @return Suggested parameter adjustments
     */
    struct OptimalParameters {
        double tier1_multiplier;
        double tier2_multiplier;
        double tier3_multiplier;
        double podd_bonus_percentage;
        double efficiency_bonus_percentage;
    };
    
    OptimalParameters CalculateOptimalParameters(double network_hashrate, 
                                                 uint32_t device_count) const;
};

} // namespace Mining

#endif // SYNC_MINING_REWARD_CALCULATOR_H
