// Copyright (c) 2024 SyntheticCoin Developers  
// Distributed under the MIT software license

#include "reward_calculator.h"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace Mining {

RewardCalculator::RewardCalculator(const Consensus::Params& params) 
    : m_params(params) {
}

int64_t RewardCalculator::GetBaseSubsidy(int32_t height) const {
    // Check cache first
    auto it = m_subsidy_cache.find(height);
    if (it != m_subsidy_cache.end()) {
        return it->second;
    }
    
    // Calculate halving epoch
    int32_t halvings = height / m_params.nSubsidyHalvingInterval;
    
    // Subsidy is cut in half every halving interval
    int64_t subsidy = m_params.nInitialSubsidy;
    
    // Shift right by number of halvings (equivalent to dividing by 2^halvings)
    subsidy >>= halvings;
    
    // Cache the result
    m_subsidy_cache[height] = subsidy;
    
    return subsidy;
}

RewardBreakdown RewardCalculator::CalculateReward(int32_t height,
                                                 const MinerInfo& miner,
                                                 int64_t tx_fees) {
    RewardBreakdown breakdown;
    
    // Get base subsidy
    breakdown.base_reward = GetBaseSubsidy(height);
    
    // Calculate small miner bonus
    if (IsFeatureActive("small_miner_boost", height)) {
        breakdown.small_miner_bonus = CalculateSmallMinerBonus(
            miner.hashrate_ths, breakdown.base_reward);
    }
    
    // Calculate PoDD bonus
    if (IsFeatureActive("podd", height) && miner.is_podd_verified) {
        breakdown.podd_bonus = CalculatePoDDBonus(
            miner.is_podd_verified, 
            miner.device_ids.size(),
            breakdown.base_reward);
    }
    
    // Calculate efficiency bonus
    breakdown.efficiency_bonus = CalculateEfficiencyBonus(
        miner.efficiency_score, breakdown.base_reward);
    
    // Calculate squad bonus
    if (IsFeatureActive("squad_mining", height) && miner.is_squad_member) {
        // Get squad size (would need actual squad lookup)
        size_t squad_size = 5; // Example
        breakdown.squad_bonus = CalculateSquadBonus(
            miner.is_squad_member, squad_size, breakdown.base_reward);
    }
    
    // Apply anti-whale penalty if needed
    double penalty_multiplier = CalculateAntiWhalePenalty(miner.consecutive_blocks);
    
    // Calculate total before penalties
    int64_t total_before_penalty = breakdown.base_reward +
                                   breakdown.small_miner_bonus +
                                   breakdown.podd_bonus +
                                   breakdown.efficiency_bonus +
                                   breakdown.squad_bonus;
    
    // Apply penalty
    breakdown.total_reward = static_cast<int64_t>(total_before_penalty * penalty_multiplier);
    
    // Add transaction fees (not subject to penalty)
    breakdown.miner_fees = tx_fees;
    breakdown.total_reward += tx_fees;
    
    // Calculate fund allocations (10% community, 5% development)
    breakdown.community_fund = breakdown.total_reward * 0.10;
    breakdown.development_fund = breakdown.total_reward * 0.05;
    
    // Adjust miner reward
    breakdown.total_reward -= (breakdown.community_fund + breakdown.development_fund);
    
    return breakdown;
}

int64_t RewardCalculator::CalculateSmallMinerBonus(double hashrate_ths, 
                                                  int64_t base_reward) const {
    double multiplier = m_params.GetMinerBoostMultiplier(hashrate_ths);
    
    // Bonus is the additional reward beyond base
    double bonus_multiplier = multiplier - 1.0;
    
    return static_cast<int64_t>(base_reward * bonus_multiplier);
}

int64_t RewardCalculator::CalculatePoDDBonus(bool is_verified,
                                            size_t device_count,
                                            int64_t base_reward) const {
    if (!is_verified || device_count == 0) {
        return 0;
    }
    
    // Base PoDD bonus (10%)
    double bonus_percentage = m_params.podd.nVerifiedDeviceBonus - 1.0;
    
    // Additional bonus for multiple devices (1% per device, max 10%)
    double device_bonus = std::min(0.10, device_count * 0.01);
    
    double total_bonus = bonus_percentage + device_bonus;
    
    return static_cast<int64_t>(base_reward * total_bonus);
}

int64_t RewardCalculator::CalculateEfficiencyBonus(double efficiency_score,
                                                  int64_t base_reward) const {
    // Efficiency score should be between 0.0 and 1.0
    efficiency_score = std::max(0.0, std::min(1.0, efficiency_score));
    
    // Max 5% bonus for perfect efficiency
    double bonus_percentage = efficiency_score * 0.05;
    
    return static_cast<int64_t>(base_reward * bonus_percentage);
}

int64_t RewardCalculator::CalculateSquadBonus(bool is_squad_member,
                                             size_t squad_size,
                                             int64_t base_reward) const {
    if (!is_squad_member || squad_size < 2) {
        return 0;
    }
    
    // 2% bonus per squad member, max 20% for full squad
    double bonus_percentage = std::min(0.20, squad_size * 0.02);
    
    return static_cast<int64_t>(base_reward * bonus_percentage);
}

uint32_t RewardCalculator::GetMinerTier(double hashrate_ths) const {
    if (hashrate_ths < m_params.minerBoost.tier1_hashrate) {
        return 1; // < 1 TH/s
    } else if (hashrate_ths < m_params.minerBoost.tier2_hashrate) {
        return 2; // 1-10 TH/s
    } else if (hashrate_ths < m_params.minerBoost.tier3_hashrate) {
        return 3; // 10-100 TH/s
    }
    return 4; // > 100 TH/s
}

double RewardCalculator::CalculateAntiWhalePenalty(uint32_t consecutive_blocks) const {
    if (consecutive_blocks <= 3) {
        return 1.0; // No penalty for up to 3 consecutive blocks
    }
    
    // Progressive penalty: 5% per additional block
    double penalty = (consecutive_blocks - 3) * 0.05;
    
    // Max 50% penalty
    penalty = std::min(0.5, penalty);
    
    return 1.0 - penalty;
}

bool RewardCalculator::ValidateRewardBreakdown(const RewardBreakdown& breakdown) const {
    // Check that total doesn't exceed maximum
    if (breakdown.total_reward < 0) {
        return false;
    }
    
    // Check that bonuses are reasonable
    int64_t total_bonuses = breakdown.small_miner_bonus +
                           breakdown.podd_bonus +
                           breakdown.efficiency_bonus +
                           breakdown.squad_bonus;
    
    // Bonuses shouldn't exceed 3x base reward
    if (total_bonuses > breakdown.base_reward * 3) {
        return false;
    }
    
    // Check fund allocations
    if (breakdown.community_fund < 0 || breakdown.development_fund < 0) {
        return false;
    }
    
    return true;
}

int64_t RewardCalculator::GetMaxPossibleReward(int32_t height) const {
    int64_t base = GetBaseSubsidy(height);
    
    // Maximum multiplier: 2x (tier 1) + 20% (PoDD) + 5% (efficiency) + 20% (squad)
    double max_multiplier = 2.0 + 0.2 + 0.05 + 0.2;
    
    return static_cast<int64_t>(base * max_multiplier);
}

double RewardCalculator::CalculateDecentralizationScore() const {
    // This would need access to actual network data
    // For now, return a placeholder
    return 0.75; // Example: 75% decentralized
}

bool RewardCalculator::IsFeatureActive(const std::string& feature, int32_t height) const {
    return m_params.IsFeatureActive(feature, height);
}

int32_t RewardCalculator::GetHalvingEpoch(int32_t height) const {
    return height / m_params.nSubsidyHalvingInterval;
}

// RewardStatistics implementation
void RewardStatistics::RecordReward(const MinerInfo& miner, const RewardBreakdown& reward) {
    RewardRecord record;
    record.height = 0; // Would need actual height
    record.miner_address = miner.address;
    record.amount = reward.total_reward;
    record.hashrate = miner.hashrate_ths;
    record.is_small_miner = miner.hashrate_ths < 10.0;
    record.is_podd_verified = miner.is_podd_verified;
    record.timestamp = std::chrono::system_clock::now();
    
    m_records.push_back(record);
    m_miner_totals[miner.address] += reward.total_reward;
}

RewardStatistics::Stats RewardStatistics::GetStatistics(uint32_t last_n_blocks) const {
    Stats stats = {};
    
    // Calculate from records
    for (const auto& record : m_records) {
        stats.total_rewards_paid += record.amount;
        
        if (record.is_small_miner) {
            stats.rewards_to_small_miners += record.amount;
        } else {
            stats.rewards_to_large_miners += record.amount;
        }
        
        if (record.is_podd_verified) {
            stats.verified_devices++;
        }
    }
    
    // Calculate unique miners
    std::set<std::string> unique_addresses;
    for (const auto& record : m_records) {
        unique_addresses.insert(record.miner_address);
    }
    stats.unique_miners = unique_addresses.size();
    
    // Calculate Gini coefficient (income inequality)
    if (!m_miner_totals.empty()) {
        std::vector<int64_t> amounts;
        for (const auto& [addr, amount] : m_miner_totals) {
            amounts.push_back(amount);
        }
        std::sort(amounts.begin(), amounts.end());
        
        double sum_of_differences = 0;
        double sum_of_values = 0;
        
        for (size_t i = 0; i < amounts.size(); ++i) {
            sum_of_values += amounts[i];
            sum_of_differences += (2 * (i + 1) - amounts.size() - 1) * amounts[i];
        }
        
        if (sum_of_values > 0) {
            stats.gini_coefficient = sum_of_differences / (amounts.size() * sum_of_values);
        }
    }
    
    // Calculate Herfindahl index (market concentration)
    if (stats.total_rewards_paid > 0) {
        double hhi = 0;
        for (const auto& [addr, amount] : m_miner_totals) {
            double market_share = static_cast<double>(amount) / stats.total_rewards_paid;
            hhi += market_share * market_share;
        }
        stats.herfindahl_index = hhi;
    }
    
    // Calculate Nakamoto coefficient
    if (!m_miner_totals.empty()) {
        std::vector<std::pair<std::string, int64_t>> sorted_miners(
            m_miner_totals.begin(), m_miner_totals.end());
        
        std::sort(sorted_miners.begin(), sorted_miners.end(),
                 [](const auto& a, const auto& b) { return a.second > b.second; });
        
        int64_t cumulative = 0;
        int64_t half_total = stats.total_rewards_paid / 2;
        uint32_t nakamoto = 0;
        
        for (const auto& [addr, amount] : sorted_miners) {
            cumulative += amount;
            nakamoto++;
            if (cumulative > half_total) {
                break;
            }
        }
        stats.nakamoto_coefficient = nakamoto;
    }
    
    return stats;
}

double RewardStatistics::GetSmallMinerPercentage() const {
    if (m_records.empty()) return 0.0;
    
    size_t small_miners = std::count_if(m_records.begin(), m_records.end(),
                                       [](const auto& r) { return r.is_small_miner; });
    
    return static_cast<double>(small_miners) / m_records.size();
}

// DynamicRewardAdjuster implementation
double DynamicRewardAdjuster::GetDecentralizationAdjustment(double current_score, 
                                                           double target_score) const {
    // If we're below target, increase small miner rewards
    if (current_score < target_score) {
        double deficit = target_score - current_score;
        return 1.0 + (deficit * 0.5); // Up to 50% bonus
    }
    
    return 1.0; // No adjustment if at or above target
}

double DynamicRewardAdjuster::GetEmergencyAdjustment(double top_miner_percentage) const {
    // If top miner has > 30%, apply emergency measures
    if (top_miner_percentage > 0.30) {
        // Reduce their rewards significantly
        return 0.5; // 50% penalty
    } else if (top_miner_percentage > 0.20) {
        // Mild penalty
        return 0.8; // 20% penalty
    }
    
    return 1.0; // No emergency adjustment
}

} // namespace Mining
