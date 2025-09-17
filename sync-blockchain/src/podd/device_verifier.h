// Copyright (c) 2024 SyntheticCoin Developers
// Distributed under the MIT software license

#ifndef SYNC_PODD_DEVICE_VERIFIER_H
#define SYNC_PODD_DEVICE_VERIFIER_H

#include <vector>
#include <map>
#include <chrono>
#include <string>
#include <array>
#include <memory>

namespace PoDD {

/**
 * Device fingerprint containing unique hardware characteristics
 */
struct DeviceFingerprint {
    // Timing characteristics
    uint64_t avg_nonce_time_us;        // Average time to find nonce (microseconds)
    uint64_t timing_variance_us;       // Variance in timing
    std::array<uint64_t, 10> timing_samples; // Recent timing samples
    
    // Network characteristics  
    std::string ip_address;
    uint32_t avg_latency_ms;
    uint32_t latency_variance_ms;
    std::vector<uint32_t> traceroute_hops;
    
    // Hardware characteristics
    std::string device_id;              // Unique device identifier
    std::string firmware_version;
    uint32_t memory_size_mb;
    uint32_t chip_count;                // Number of mining chips
    double power_consumption_watts;
    double temperature_celsius;
    
    // Nonce pattern characteristics
    uint64_t nonce_search_space;        // How device searches nonce space
    uint32_t nonce_increment_pattern;   // Pattern in nonce increments
    std::vector<uint64_t> recent_nonces; // Recent nonces found
    
    // Behavioral characteristics
    std::chrono::steady_clock::time_point last_seen;
    uint32_t uptime_seconds;
    uint32_t restart_count;
    double average_hashrate;
    
    /**
     * Calculate similarity score with another device
     * @return Score from 0.0 (completely different) to 1.0 (identical)
     */
    double CalculateSimilarity(const DeviceFingerprint& other) const;
    
    /**
     * Generate a hash of the fingerprint for quick comparison
     */
    std::string GetFingerprintHash() const;
};

/**
 * Mining squad - group of small miners working together
 */
struct MiningSquad {
    std::string squad_id;
    std::vector<std::string> member_devices;
    std::chrono::steady_clock::time_point created_at;
    uint64_t total_hashrate;
    uint64_t blocks_found;
    
    bool AddDevice(const std::string& device_id);
    bool RemoveDevice(const std::string& device_id);
    double GetRewardShare(const std::string& device_id) const;
};

/**
 * Main device verification system for Proof-of-Device-Distribution
 */
class DeviceVerifier {
public:
    DeviceVerifier();
    ~DeviceVerifier();
    
    /**
     * Register a new mining device
     * @param device_id Unique device identifier (e.g., Bitaxe serial)
     * @param initial_fingerprint Initial device fingerprint
     * @return True if registration successful
     */
    bool RegisterDevice(const std::string& device_id, 
                       const DeviceFingerprint& initial_fingerprint);
    
    /**
     * Update device fingerprint with new data
     * @param device_id Device to update
     * @param share_data Data from recent mining share
     */
    void UpdateDeviceFingerprint(const std::string& device_id,
                                const ShareData& share_data);
    
    /**
     * Verify that multiple devices are genuinely different
     * @param device_ids List of devices claiming to be separate
     * @return Verification result with confidence score
     */
    struct VerificationResult {
        bool is_valid;
        double confidence;      // 0.0 to 1.0
        std::string reason;
        std::vector<std::pair<std::string, std::string>> suspicious_pairs;
    };
    
    VerificationResult VerifyDeviceDistribution(const std::vector<std::string>& device_ids);
    
    /**
     * Detect if one large miner is pretending to be multiple small miners
     * @param device_ids Devices to check
     * @return True if spoofing detected
     */
    bool DetectSpoofing(const std::vector<std::string>& device_ids);
    
    /**
     * Calculate reward multiplier based on device verification status
     * @param device_id Device requesting reward
     * @return Multiplier (1.0 = standard, >1.0 = bonus)
     */
    double GetDeviceRewardMultiplier(const std::string& device_id) const;
    
    /**
     * Form a mining squad from verified devices
     * @param device_ids Devices wanting to form squad
     * @return Squad ID if successful, empty string otherwise
     */
    std::string FormMiningSquad(const std::vector<std::string>& device_ids);
    
    /**
     * Get device's current hashrate estimate
     */
    double GetDeviceHashrate(const std::string& device_id) const;
    
    /**
     * Timing analysis to detect hardware variations
     */
    struct TimingAnalysis {
        bool AnalyzeTimingPatterns(const std::vector<uint64_t>& timestamps);
        double CalculateTimingEntropy(const DeviceFingerprint& fp);
        bool DetectSynchronizedTiming(const std::vector<DeviceFingerprint>& devices);
    };
    
    /**
     * Network analysis to verify geographic distribution
     */
    struct NetworkAnalysis {
        bool VerifyNetworkDiversity(const std::vector<DeviceFingerprint>& devices);
        double EstimateGeographicDistance(const DeviceFingerprint& d1, 
                                         const DeviceFingerprint& d2);
        bool DetectNATSpoofing(const std::vector<std::string>& ip_addresses);
    };
    
    /**
     * Nonce pattern analysis to detect device-specific patterns
     */
    struct NonceAnalysis {
        bool AnalyzeNoncePatterns(const std::vector<uint64_t>& nonces);
        double CalculateNonceEntropy(const std::vector<uint64_t>& nonces);
        bool DetectArtificialNonceGeneration(const DeviceFingerprint& fp);
    };
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Device registry
    std::map<std::string, DeviceFingerprint> m_devices;
    std::map<std::string, std::chrono::steady_clock::time_point> m_registration_times;
    
    // Squad registry
    std::map<std::string, MiningSquad> m_squads;
    
    // Verification cache
    struct VerificationCache {
        std::chrono::steady_clock::time_point timestamp;
        VerificationResult result;
    };
    std::map<std::string, VerificationCache> m_verification_cache;
    
    // Anti-spoofing detection
    bool CheckTimingConsistency(const DeviceFingerprint& fp);
    bool CheckNetworkConsistency(const DeviceFingerprint& fp);
    bool CheckHardwareConsistency(const DeviceFingerprint& fp);
    
    // Statistical analysis
    double CalculateKolmogorovSmirnovStatistic(const std::vector<double>& sample1,
                                               const std::vector<double>& sample2);
    double CalculateMutualInformation(const std::vector<uint64_t>& data1,
                                     const std::vector<uint64_t>& data2);
};

/**
 * Share data from mining operation
 */
struct ShareData {
    std::string device_id;
    uint64_t nonce;
    uint64_t timestamp_us;
    uint32_t difficulty;
    std::string block_hash;
    double hashrate;
    double temperature;
    double power_watts;
    std::string ip_address;
    uint32_t latency_ms;
};

/**
 * Device registration data
 */
struct DeviceRegistration {
    std::string device_id;
    std::string manufacturer;
    std::string model;
    std::string serial_number;
    std::string firmware_version;
    uint32_t chip_count;
    double max_hashrate_ghs;
    std::chrono::system_clock::time_point manufacture_date;
    std::string owner_address; // SYNC address of owner
    std::vector<uint8_t> signature; // Cryptographic proof of ownership
};

/**
 * Global device registry interface
 */
class DeviceRegistry {
public:
    static DeviceRegistry& GetInstance();
    
    bool RegisterDevice(const DeviceRegistration& registration);
    bool VerifyDeviceOwnership(const std::string& device_id, const std::string& owner_address);
    std::vector<std::string> GetOwnerDevices(const std::string& owner_address);
    bool TransferDevice(const std::string& device_id, 
                       const std::string& from_address,
                       const std::string& to_address);
    
    // Statistics
    uint32_t GetTotalRegisteredDevices() const;
    uint32_t GetActiveDevices(uint32_t last_n_blocks) const;
    double GetNetworkHashrateDistribution() const;
    
private:
    DeviceRegistry() = default;
    std::map<std::string, DeviceRegistration> m_registrations;
};

} // namespace PoDD

#endif // SYNC_PODD_DEVICE_VERIFIER_H
